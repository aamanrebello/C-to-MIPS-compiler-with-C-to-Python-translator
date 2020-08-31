#ifndef ast_logic
#define ast_logic

// Defines classes for logical operators - && (line 8) and || (line 138)

//LOGICAL OPERATORS

//LOGIC AND

class LogicAnd : public BinaryOperator
{
  public:
    LogicAnd(ProgramPtr leftop, ProgramPtr rightop): BinaryOperator(leftop, rightop) {}

    std::string Whatami() const
    {
      return "LogicAnd";
    }

    int intvalue(Context& c) const
    {
      std::string tl = reducetype(c.userdefs, left->MyType(c));
      std::string tr = reducetype(c.userdefs, right->MyType(c));
      if((tl == "double")||(tl == "float")||(tr == "double")||(tr == "float"))
      {
        return (left->dblvalue(c))&&(right->dblvalue(c));
      }
      return (left->intvalue(c))&&(right->intvalue(c));
    }

    void PythonPrint(int tabs, std::map<std::string, std::string>& globals, std::ostream& os) const
    {
      left->PythonPrint(tabs, globals, os);
      os << " and ";
      right->PythonPrint(tabs, globals, os);
    }

    // THE NEXT 3 functions account for the different cases in which the operands on either side of the logical
    // operator are integer or floating point.

    void flAndPrint(std::ostream& os, std::string label0, std::string reg) const
    {
      os << "mtc1       $0, $f8" << std::endl;
      os << "nop" << std::endl;
      os << "c.eq.s     " << reg << ", $f8" << std::endl;
      os << "nop" << std::endl;
      os << "bc1t       " << label0 << std::endl;
      os << "nop" << std::endl;
    }

    void dbAndPrint(std::ostream& os, std::string label0, std::string reg) const
    {
      os << "mtc1       $0, $f8" << std::endl;
      os << "nop" << std::endl;
      os << "mtc1       $0, $f9" << std::endl;
      os << "nop" << std::endl;
      os << "c.eq.d     " << reg << ", $f8" << std::endl;
      os << "nop" << std::endl;
      os << "bc1t       " << label0 << std::endl;
      os << "nop" << std::endl;
    }

    void intPrint(std::ostream& os, std::string label0, std::string reg) const
    {
      os << "beq        " << reg << ", $0, " << label0 << std::endl;
      os << "nop" << std::endl;
    }

    void MIPSPrint(Context& c, std::ostream& os, int sp_off, std::string destreg) const
    {
      std::string tl = reducetype(c.userdefs, left->MyType(c));
      std::string tr = reducetype(c.userdefs, right->MyType(c));
      std::string label0 = makeName("L");
      std::string label1 = makeName("L");
      if((tl == "double")&&(tr == "double"))
      {
        setupLR(c, os, sp_off, 2);
        dbAndPrint(os, label0, "$f4");
        dbAndPrint(os, label0, "$f6");
      }
      else if((tl== "float")&&(tr == "float"))
      {
        setupLR(c, os, sp_off, 1);
        flAndPrint(os, label0, "$f4");
        flAndPrint(os, label0, "$f6");
      }
      else if((tl == "float")&&(tr == "double"))
      {
        left->MIPSPrint(c, os, sp_off, "$f4");
        stpushreg("$f4", os);
        right->MIPSPrint(c, os, sp_off, "$f6");
        stpopreg("$f4", os);
        flAndPrint(os, label0, "$f4");
        dbAndPrint(os, label0, "$f6");
      }
      else if((tl == "double")&&(tr == "float"))
      {
        right->MIPSPrint(c, os, sp_off, "$f6");
        stpushreg("$f6", os);
        left->MIPSPrint(c, os, sp_off, "$f4");
        stpopreg("$f4", os);
        dbAndPrint(os, label0, "$f4");
        flAndPrint(os, label0, "$f6");
      }
      else if((tl == "double")||(tl == "float"))
      {
        left->MIPSPrint(c, os, sp_off, "$f4");
        right->MIPSPrint(c, os, sp_off, "$9");
        if(tl == "double") {  dbAndPrint(os, label0, "$f4"); }
        else if(tl == "float") {  flAndPrint(os, label0, "$f4"); }
        intPrint(os, label0, "$9");
      }
      else if((tr == "double")||(tr == "float"))
      {
        left->MIPSPrint(c, os, sp_off, "$8");
        right->MIPSPrint(c, os, sp_off, "$f6");
        if(tr == "double") {  dbAndPrint(os, label0, "$f6"); }
        else if(tr == "float") {  flAndPrint(os, label0, "$f6"); }
        intPrint(os, label0, "$8");
      }
      else
      {
        setupLR(c, os, sp_off, 0);
        intPrint(os, label0, "$8");
        intPrint(os, label0, "$9");
      }
      ///////////////////////////////////////////////////////
      os << "li         " << destreg << ", 1" << std::endl;
      os << "b          " << label1 << std::endl;
      os << "nop" << std::endl;
      os << label0 << ":" << std::endl;
      os << "move       " << destreg << ", $0" << std::endl;
      os << label1 << ":" << std::endl;
    }
};


//LOGIC OR

class LogicOr : public BinaryOperator
{
  public:
    LogicOr(ProgramPtr leftop, ProgramPtr rightop): BinaryOperator(leftop, rightop) {}

    std::string Whatami() const
    {
      return "LogicOr";
    }

    int intvalue(Context& c) const
    {
      std::string tl = reducetype(c.userdefs, left->MyType(c));
      std::string tr = reducetype(c.userdefs, right->MyType(c));
      if((tl == "double")||(tl == "float")||(tr == "double")||(tr == "float"))
      {
        return (left->dblvalue(c))||(right->dblvalue(c));
      }
      return (left->intvalue(c))||(right->intvalue(c));
    }

    void PythonPrint(int tabs, std::map<std::string, std::string>& globals, std::ostream& os) const
    {
      left->PythonPrint(tabs, globals, os);
      os << " or ";
      right->PythonPrint(tabs, globals, os);
    }

    // THE NEXT 3 functions account for the different cases in which the operands on either side of the logical
    // operator are integer or floating point.

    void flAndPrint(std::ostream& os, std::string label0, std::string label1, std::string reg) const
    {
      if(reg == "$f4")
      {
        os << "mtc1       $0, $f8" << std::endl;
        os << "nop" << std::endl;
        os << "c.eq.s     " << reg << ", $f8" << std::endl;
        os << "nop" << std::endl;
        os << "bc1f       " << label0 << std::endl;
        os << "nop" << std::endl;
      }
      else if(reg == "$f6")
      {
        os << "mtc1       $0, $f8" << std::endl;
        os << "nop" << std::endl;
        os << "c.eq.s     " << reg << ", $f8" << std::endl;
        os << "nop" << std::endl;
        os << "bc1t       " << label1 << std::endl;
        os << "nop" << std::endl;
      }
    }

    void dbAndPrint(std::ostream& os, std::string label0, std::string label1, std::string reg) const
    {
      if(reg == "$f4")
      {
        os << "mtc1       $0, $f8" << std::endl;
        os << "nop" << std::endl;
        os << "mtc1       $0, $f9" << std::endl;
        os << "nop" << std::endl;
        os << "c.eq.d     " << reg << ", $f8" << std::endl;
        os << "nop" << std::endl;
        os << "bc1f       " << label0 << std::endl;
        os << "nop" << std::endl;
      }
      else if(reg == "$f6")
      {
        os << "mtc1       $0, $f8" << std::endl;
        os << "nop" << std::endl;
        os << "mtc1       $0, $f9" << std::endl;
        os << "nop" << std::endl;
        os << "c.eq.d     " << reg << ", $f8" << std::endl;
        os << "nop" << std::endl;
        os << "bc1t       " << label1 << std::endl;
        os << "nop" << std::endl;
      }
    }

    void intPrint(std::ostream& os, std::string label0, std::string label1, std::string reg) const
    {
      if(reg == "$8")
      {
        os << "bne        $8, $0, " << label0 << std::endl;
        os << "nop" << std::endl;
      }
      else if(reg == "$9")
      {
        os << "beq        $9, $0, " << label1 << std::endl;
        os << "nop" << std::endl;
      }
    }

    void MIPSPrint(Context& c, std::ostream& os, int sp_off, std::string destreg) const
    {
      std::string label0 = makeName("L");
      std::string label1 = makeName("L");
      std::string labelend = makeName("L");
      std::string tl = reducetype(c.userdefs, left->MyType(c));
      std::string tr = reducetype(c.userdefs, right->MyType(c));
      if((tl == "double")&&(tr == "double"))
      {
        setupLR(c, os, sp_off, 2);
        dbAndPrint(os, label0, label1, "$f4");
        dbAndPrint(os, label0, label1, "$f6");
      }
      else if((tl== "float")&&(tr == "float"))
      {
        setupLR(c, os, sp_off, 1);
        flAndPrint(os, label0, label1, "$f4");
        flAndPrint(os, label0, label1, "$f6");
      }
      else if((tl == "float")&&(tr == "double"))
      {
        left->MIPSPrint(c, os, sp_off, "$f4");
        stpushreg("$f4", os);
        right->MIPSPrint(c, os, sp_off, "$f6");
        stpopreg("$f4", os);
        flAndPrint(os, label0, label1, "$f4");
        dbAndPrint(os, label0, label1, "$f6");
      }
      else if((tl == "double")&&(tr == "float"))
      {
        right->MIPSPrint(c, os, sp_off, "$f6");
        stpushreg("$f6", os);
        left->MIPSPrint(c, os, sp_off, "$f4");
        stpopreg("$f4", os);
        dbAndPrint(os, label0, label1, "$f4");
        flAndPrint(os, label0, label1, "$f6");
      }
      else if((tl == "double")||(tl == "float"))
      {
        left->MIPSPrint(c, os, sp_off, "$f4");
        right->MIPSPrint(c, os, sp_off, "$9");
        if(tl == "double") {  dbAndPrint(os, label0, label1, "$f4"); }
        else if(tl == "float") {  flAndPrint(os, label0, label1, "$f4"); }
        intPrint(os, label0, label1, "$9");
      }
      else if((tr == "double")||(tr == "float"))
      {
        left->MIPSPrint(c, os, sp_off, "$8");
        right->MIPSPrint(c, os, sp_off, "$f6");
        intPrint(os, label0, label1, "$8");
        if(tr == "double") {  dbAndPrint(os, label0, label1, "$f6"); }
        else if(tr == "float") {  flAndPrint(os, label0, label1, "$f6"); }
      }
      else
      {
        setupLR(c, os, sp_off, 0);
        intPrint(os, label0, label1, "$8");
        intPrint(os, label0, label1, "$9");
      }
      ///////////////////////////////////////////////////
      os << label0 << ":" << std::endl;
      os << "li         " << destreg << ", 1" << std::endl;
      os << "b          " << labelend << std::endl;
      os << "nop" << std::endl;
      os << label1 << ":" << std::endl;
      os << "move       " << destreg << ", $0" << std::endl;
      os << labelend << ":" << std::endl;
    }
};


#endif
