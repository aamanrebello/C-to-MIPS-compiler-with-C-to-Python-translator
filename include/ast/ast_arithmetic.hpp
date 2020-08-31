#ifndef ast_arithmetic
#define ast_arithmetic

// Defines Arithmetic operator classes:
//    - Arithmetic operator parent class: line 14
//    - + operator: line 52
//    - - operator: line 130
//    - * operator: line 206
//    - / operator: line 274
//    - % operator: line 345

//ARITHMETIC OPERATORS

//PARENT CLASS

class Arithmetic : public BinaryOperator
{
  public:
    Arithmetic(ProgramPtr leftop, ProgramPtr rightop): BinaryOperator(leftop, rightop) {}

    // returns type that comes as a result of arithmetic operation
    std::string MyType(Context& c) const
    {
      std::string l = left->Whatami(), r = right->Whatami();
      std::string tl = reducetype(c.userdefs, left->MyType(c)), tr = reducetype(c.userdefs, right->MyType(c));
      bool ptr_arr_sub_l = ((left->Whatami() == "ArraySubscript")&&((tl == "double *")||(tl == "float *")));
      bool ptr_arr_sub_r = ((right->Whatami() == "ArraySubscript")&&((tr == "double *")||(tr == "float *")));
      if((tl[tl.size() - 1] == '*')&&(!ptr_arr_sub_l))
      {
        return tl;
      }
      else if((tr[tr.size() - 1] == '*')&&(!ptr_arr_sub_r))
      {
        return tr;
      }
      else if(isLiteral(l))
      {
        return tl;
      }
      else if(isLiteral(r))
      {
        return tr;
      }
      else
      {
        return tl;
      }
    }
};


// +

class Plus : public Arithmetic
{
  public:
    Plus(ProgramPtr leftop, ProgramPtr rightop): Arithmetic(leftop, rightop) {}

    std::string Whatami() const
    {
      return "Plus";
    }

    int intvalue(Context& c) const
    {
      return (left->intvalue(c)) + (right->intvalue(c));
    }

    double dblvalue(Context& c) const
    {
      return (left->dblvalue(c)) + (right->dblvalue(c));
    }

    void PythonPrint(int tabs, std::map<std::string, std::string>& globals, std::ostream& os) const
    {
      if(left->Whatami() == "LogicalNot")
      {os << "("; left->PythonPrint(tabs, globals, os); os << ")";}
      else
      { left->PythonPrint(tabs, globals, os); }
      os << " + ";
      if(right->Whatami() == "LogicalNot")
      { os << "("; right->PythonPrint(tabs, globals, os); os << ")";}
      else
      { right->PythonPrint(tabs, globals, os); }
    }

    // Used for floating point operand(s)
    void fpMIPSPr(Context& c, std::ostream& os, int sp_off, std::string destreg) const
    {
      if(MyType(c) == "float")
      {
        setupLR(c, os, sp_off, 1);
        os << "add.s       " << destreg << ", $f4, $f6" << std::endl;
      }
      else if(MyType(c) == "double")
      {
        setupLR(c, os, sp_off, 2);
        os << "add.d       " << destreg << ", $f4, $f6" << std::endl;
      }
    }

    void MIPSPrint(Context& c, std::ostream& os, int sp_off, std::string destreg) const
    {
      if((MyType(c) == "double")||(MyType(c) == "float"))
      {
        fpMIPSPr(c, os, sp_off, destreg);
      }
      else
      {
        setupLR(c, os, sp_off, 0);
        std::string tl = left->MyType(c), tr = right->MyType(c);
        if(tl[tl.size() - 1] == '*')
        {
          int shift = findShift(c, sp_off, true);
          if(shift > 0)
          { os << "sll        $9, $9, " << shift << std::endl; }
        }
        else if(tr[tr.size() - 1] == '*')
        {
          int shift = findShift(c, sp_off, false);
          if(shift > 0)
          { os << "sll        $8, $8, " << shift << std::endl;}
        }
        os << "addu       " << destreg << ", $8, $9" << std::endl;
      }
    }
};


// -

class Minus : public Arithmetic
{
  public:
    Minus(ProgramPtr leftop, ProgramPtr rightop): Arithmetic(leftop, rightop) {}

    std::string Whatami() const
    {
      return "Minus";
    }

    int intvalue(Context& c) const
    {
      return (left->intvalue(c)) - (right->intvalue(c));
    }

    double dblvalue(Context& c) const
    {
      return (left->dblvalue(c)) - (right->dblvalue(c));
    }

    void PythonPrint(int tabs, std::map<std::string, std::string>& globals, std::ostream& os) const
    {
      if(left->Whatami() == "LogicalNot")
      {os << "("; left->PythonPrint(tabs, globals, os); os << ")";}
      else
      { left->PythonPrint(tabs, globals, os); }
      os << " - ";
      if(right->Whatami() == "LogicalNot")
      { os << "("; right->PythonPrint(tabs, globals, os); os << ")";}
      else
      { right->PythonPrint(tabs, globals, os); }
    }

    // Used for floating point operand(s)
    void fpMIPSPr(Context& c, std::ostream& os, int sp_off, std::string destreg) const
    {
      if(MyType(c) == "float")
      {
        setupLR(c, os, sp_off, 1);
        os << "sub.s       " << destreg << ", $f4, $f6" << std::endl;
      }
      else if(MyType(c) == "double")
      {
        setupLR(c, os, sp_off, 2);
        os << "sub.d       " << destreg << ", $f4, $f6" << std::endl;
      }
    }

    void MIPSPrint(Context& c, std::ostream& os, int sp_off, std::string destreg) const
    {
      if((MyType(c) == "double")||(MyType(c) == "float"))
      {
        fpMIPSPr(c, os, sp_off, destreg);
      }
      else
      {
        setupLR(c, os, sp_off, 0);
        std::string tl = left->MyType(c), tr = right->MyType(c);
        if(tl[tl.size() - 1] == '*')
        {
          int shift = findShift(c, sp_off, true);
          if(shift > 0)
          { os << "sll        $9, $9, " << shift << std::endl; }
        }
        else if(tr[tr.size() - 1] == '*')
        {
          int shift = findShift(c, sp_off, false);
          if(shift > 0)
          { os << "sll        $8, $8, " << shift << std::endl;}
        }
        os << "subu       " << destreg << ", $8, $9" << std::endl;
      }
    }
};


// *

class Multiply : public Arithmetic
{
  public:
    Multiply(ProgramPtr leftop, ProgramPtr rightop): Arithmetic(leftop, rightop) {}

    std::string Whatami() const
    {
      return "Multiply";
    }

    int intvalue(Context& c) const
    {
      return (left->intvalue(c))*(right->intvalue(c));
    }

    double dblvalue(Context& c) const
    {
      return (left->dblvalue(c))*(right->dblvalue(c));
    }

    void PythonPrint(int tabs, std::map<std::string, std::string>& globals, std::ostream& os) const
    {
      if(left->Whatami() == "LogicalNot")
      {os << "("; left->PythonPrint(tabs, globals, os); os << ")";}
      else
      { left->PythonPrint(tabs, globals, os); }
      os << " * ";
      if(right->Whatami() == "LogicalNot")
      { os << "("; right->PythonPrint(tabs, globals, os); os << ")";}
      else
      { right->PythonPrint(tabs, globals, os); }
    }

    // Used for floating point operand(s)
    void fpMIPSPr(Context& c, std::ostream& os, int sp_off, std::string destreg) const
    {
      if(MyType(c) == "float")
      {
        setupLR(c, os, sp_off, 1);
        os << "mul.s       " << destreg << ", $f4, $f6" << std::endl;
      }
      else if(MyType(c) == "double")
      {
        setupLR(c, os, sp_off, 2);
        os << "mul.d       " << destreg << ", $f4, $f6" << std::endl;
      }
    }

    void MIPSPrint(Context& c, std::ostream& os, int sp_off, std::string destreg) const
    {
      if((MyType(c) == "double")||(MyType(c) == "float"))
      {
        fpMIPSPr(c, os, sp_off, destreg);
      }
      else
      {
        setupLR(c, os, sp_off, 0);
        os << "mult       $8, $9" << std::endl;
        os << "mflo       " << destreg << std::endl;
      }
    }
};


// DIVIDE

class Divide : public Arithmetic
{
  public:
    Divide(ProgramPtr leftop, ProgramPtr rightop): Arithmetic(leftop, rightop) {}

    std::string Whatami() const
    {
      return "Divide";
    }

    int intvalue(Context& c) const
    {
      return (left->intvalue(c))/(right->intvalue(c));
    }

    double dblvalue(Context& c) const
    {
      return (left->dblvalue(c))/(right->dblvalue(c));
    }

    void PythonPrint(int tabs, std::map<std::string, std::string>& globals, std::ostream& os) const
    {
      if(left->Whatami() == "LogicalNot")
      {os << "("; left->PythonPrint(tabs, globals, os); os << ")";}
      else
      { left->PythonPrint(tabs, globals, os); }
      os << " / ";
      if(right->Whatami() == "LogicalNot")
      { os << "("; right->PythonPrint(tabs, globals, os); os << ")";}
      else
      { right->PythonPrint(tabs, globals, os); }
    }

    // Used for floating point operand(s)
    void fpMIPSPr(Context& c, std::ostream& os, int sp_off, std::string destreg) const
    {
      if(MyType(c) == "float")
      {
        setupLR(c, os, sp_off, 1);
        os << "div.s       " << destreg << ", $f4, $f6" << std::endl;
      }
      else if(MyType(c) == "double")
      {
        setupLR(c, os, sp_off, 2);
        os << "div.d       " << destreg << ", $f4, $f6" << std::endl;
      }
    }

    void MIPSPrint(Context& c, std::ostream& os, int sp_off, std::string destreg) const
    {
      if((MyType(c) == "double")||(MyType(c) == "float"))
      {
        fpMIPSPr(c, os, sp_off, destreg);
      }
      else
      {
        setupLR(c, os, sp_off, 0);
        std::string labelend = makeName("L");
        os << "bne        $9, $0, " << labelend << std::endl;
        os << "div        $0, $8, $9" << std::endl;
        os << "break        7" << std::endl;
        os << labelend << ":" << std::endl;
        os << "mfhi       " << destreg << std::endl;
        os << "mflo       " << destreg << std::endl;
      }
    }
};


// %

class Modulus : public Arithmetic
{
  public:
    Modulus(ProgramPtr leftop, ProgramPtr rightop): Arithmetic(leftop, rightop) {}

    std::string Whatami() const
    {
      return "Modulus";
    }

    int intvalue(Context& c) const
    {
      return (left->intvalue(c))%(right->intvalue(c));
    }

    void PythonPrint(int tabs, std::map<std::string, std::string>& globals, std::ostream& os) const
    {
      if(left->Whatami() == "LogicalNot")
      {os << "("; left->PythonPrint(tabs, globals, os); os << ")";}
      else
      { left->PythonPrint(tabs, globals, os); }
      os << " % ";
      if(right->Whatami() == "LogicalNot")
      { os << "("; right->PythonPrint(tabs, globals, os); os << ")";}
      else
      { right->PythonPrint(tabs, globals, os); }
    }

    void MIPSPrint(Context& c, std::ostream& os, int sp_off, std::string destreg) const
    {
      setupLR(c, os, sp_off, 0);
      std::string labelend = makeName("L");
      os << "bne        $9, $0, " << labelend << std::endl;
      os << "div        $0, $8, $9" << std::endl;
      os << "break        7" << std::endl;
      os << labelend << ":" << std::endl;
      os << "mfhi       " << destreg << std::endl;
    }
};


#endif
