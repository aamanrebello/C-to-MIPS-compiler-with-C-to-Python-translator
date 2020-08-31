#ifndef ast_unaryop
#define ast_unaryop

// Defines classes for all unary operators
//  - Unary Operator superclass: line 14
//  - Unary Minus class: line 43
//  - Unary plus class: line 102
//  - Pointer dereference class: line 141
//  - Pointer reference class: line 236
//  - Logical not operator class: line 263
//  - Bitwise not operator class: line 302
//  - Type conversion class: line 338

class UnaryOperator : public Program
{
  public:
    UnaryOperator(ProgramPtr op): operand(op) {}
    virtual ~UnaryOperator()
    {
      delete operand;
    }

    std::string Whatami() const
    {
      return "UnaryOperator";
    }

    int maxparams(Context& c) const
    {
      return operand->maxparams(c);
    }

    int findstrings(std::map<std::string, std::string>& slabel) const
    {
      return operand->findstrings(slabel);
    }
  protected:
    ProgramPtr operand;
};

//UMINUS, UPLUS

class UnaryMinus : public UnaryOperator
{
  public:
    UnaryMinus(ProgramPtr op): UnaryOperator(op) {}

    std::string Whatami() const
    {
      return "UnaryMinus";
    }

    std::string MyType(Context& c) const
    {
      return reducetype(c.userdefs, operand->MyType(c));
    }

    int intvalue(Context& c) const
    {
      return -(operand->intvalue(c));
    }

    double dblvalue(Context& c) const
    {
      return -(operand->dblvalue(c));
    }

    void PythonPrint(int tabs, std::map<std::string, std::string>& globals, std::ostream& os) const
    {
      os << "-";
      operand->PythonPrint(tabs, globals, os);
    }

    void MIPSPrint(Context& c, std::ostream& os, int sp_off, std::string destreg) const
    {
      if(destreg != "global")
      {
        std::string type = reducetype(c.userdefs, operand->MyType(c));
        if(type == "float")
        {
          operand->MIPSPrint(c, os, sp_off, "$f4");
          os << "neg.s        " << destreg << ", $f4" << std::endl;
        }
        else if(type == "double")
        {
          operand->MIPSPrint(c, os, sp_off, "$f4");
          os << "neg.d        " << destreg << ", $f4" << std::endl;
        }
        else
        {
          operand->MIPSPrint(c, os, sp_off, "$8");
          os << "subu       " << destreg << ", $0, $8" << std::endl;
        }
      }
      else
      {
        operand->MIPSPrint(c, os, sp_off, "global-");
      }
    }
};

class UnaryPlus : public UnaryOperator
{
  public:
    UnaryPlus(ProgramPtr op): UnaryOperator(op) {}

    std::string Whatami() const
    {
      return "UnaryPlus";
    }

    std::string MyType(Context& c) const
    {
      return reducetype(c.userdefs, operand->MyType(c));
    }

    int intvalue(Context& c) const
    {
      return operand->intvalue(c);
    }

    double dblvalue(Context& c) const
    {
      return operand->dblvalue(c);
    }

    void PythonPrint(int tabs, std::map<std::string, std::string>& globals, std::ostream& os) const
    {
      os << "+";
      operand->PythonPrint(tabs, globals, os);
    }

    void MIPSPrint(Context& c, std::ostream& os, int sp_off, std::string destreg) const
    {
      operand->MIPSPrint(c, os, sp_off, destreg);
    }
};

//POINTER OPERATIONS

class PointerDeref : public UnaryOperator
{
  public:
    PointerDeref(ProgramPtr op): UnaryOperator(op) {}

    std::string Whatami() const
    {
      return "PointerDeref";
    }

    std::string MyType(Context& c) const
    {
      std::string type = reducetype(c.userdefs, operand->MyType(c));
      type.pop_back(); type.pop_back();
      return type;
    }

    // Used to assign value to a pointer dereference.
    void pdassign(Context& c, std::ostream& os, int sp_off)
    {
      int varaddr = c.variablelist[operand->Name()];
      os << "lw         $9, " << (sp_off - varaddr) << "($fp)" << std::endl;
      os << "nop" << std::endl;
      std::string type = reducetype(c.userdefs, operand->MyType(c));
      if((type == "char *")||(type == "unsigned char *")||(type == "signed char *"))
      {
        os << "sb         $8, 3($9)" << std::endl;
      }
      else if((type == "short *")||(type == "unsigned short *")||(type == "signed short *"))
      {
        os << "sh         $8, 2($9)" << std::endl;
      }
      else if(type == "float")
      {
        os << "swc1       $f4, 0($9)" << std::endl;
      }
      else if(type == "double")
      {
        os << "swc1       $f4, 4($9)" << std::endl;
        os << "nop" << std::endl;
        os << "swc1       $f5, 0($fp)" << std::endl;
      }
      else
      {
        os << "sw         $8, 0($9)" << std::endl;
      }
      os << "nop" << std::endl;
    }

    void MIPSPrint(Context& c, std::ostream& os, int sp_off, std::string destreg) const
    {
      int varaddr = c.variablelist[operand->Name()];
      os << "lw         $8, " << (sp_off - varaddr) << "($fp)" << std::endl;
      os << "nop" << std::endl;
      std::string type = reducetype(c.userdefs, operand->MyType(c));
      if((c.assign_str).find(operand->Name()) != (c.assign_str).end())
      {
        os << "lbu        " << destreg << ", 0($8)" << std::endl;
      }
      else if((type == "char *")||(type == "signed char *"))
      {
        os << "lb         " << destreg << ", 0($8)" << std::endl;
      }
      else if(type == "unsigned char *")
      {
        os << "lbu        " << destreg << ", 0($8)" << std::endl;
      }
      else if((type == "short *")||(type == "signed short *"))
      {
        os << "lh         " << destreg << ", 0($8)" << std::endl;
      }
      else if(type == "unsigned short *")
      {
        os << "lhu        " << destreg << ", 0($8)" << std::endl;
      }
      else if(type == "float *")
      {
        os << "lwc1       " << destreg << ", 0($8)" << std::endl;
      }
      else if(type == "double *")
      {
        int n = std::stoi(destreg.substr(2)) + 1;
        std::string reg2 = destreg.substr(0, 2) + std::to_string(n);
        os << "lwc1       " << destreg << ", 4($8)" << std::endl;
        os << "nop" << std::endl;
        os << "lwc1       " << reg2 << ", 0($8)" << std::endl;
      }
      else
      {
        os << "lw         " << destreg << ", 0($8)" << std::endl;
      }
      os << "nop" << std::endl;
    }
};

class PointerAddr : public UnaryOperator
{
  public:
    PointerAddr(ProgramPtr op): UnaryOperator(op) {}

    std::string Whatami() const
    {
      return "PointerAddr";
    }
    std::string MyType(Context& c) const
    {
      return "int";
    }

    void MIPSPrint(Context& c, std::ostream& os, int sp_off, std::string destreg) const
    {
      int varaddr = c.variablelist[operand->Name()];
      if(varaddr <= -1000)
      { os << "lui        " << destreg << ", %hi(" << operand->Name() << ")" << std::endl;
        os << "addiu      " << destreg << ", " << destreg << ", %lo(" << operand->Name() << ")" << std::endl;}
      else
      { os << "addu       " << destreg << ", $fp, " << (sp_off - varaddr) << std::endl; }
    }
};

//NOT OPERATIONS

class LogicalNot : public UnaryOperator
{
  public:
    LogicalNot(ProgramPtr op): UnaryOperator(op) {}

    std::string Whatami() const
    {
      return "LogicalNot";
    }

    std::string MyType(Context& c) const
    {
      return "int";
    }

    int intvalue(Context& c) const
    {
      return !(operand->intvalue(c));
    }

    double dblvalue(Context& c) const
    {
      return !(operand->dblvalue(c));
    }

    void PythonPrint(int tabs, std::map<std::string, std::string>& globals, std::ostream& os) const
    {
      os << "not ";
      operand->PythonPrint(tabs, globals, os);
    }

    void MIPSPrint(Context& c, std::ostream& os, int sp_off, std::string destreg) const
    {
      operand->MIPSPrint(c, os, sp_off, destreg);
      os << "sltiu      " << destreg << ", " << destreg << ", 1" << std::endl;
      os << "andi       " << destreg << ", " << destreg << ", 0x00ff" << std::endl;
    }
};

class BitwiseNot : public UnaryOperator
{
  public:
    BitwiseNot(ProgramPtr op): UnaryOperator(op) {}

    std::string Whatami() const
    {
      return "BitwiseNot";
    }

    std::string MyType(Context& c) const
    {
      return "int";
    }

    int intvalue(Context& c) const
    {
      return ~(operand->intvalue(c));
    }

    void PythonPrint(int tabs, std::map<std::string, std::string>& globals, std::ostream& os) const
    {
      os << "~";
      operand->PythonPrint(tabs, globals, os);
    }

    void MIPSPrint(Context& c, std::ostream& os, int sp_off, std::string destreg) const
    {
      operand->MIPSPrint(c, os, sp_off, destreg);
      os << "nor        " << destreg << ", $0, " << destreg << std::endl;
    }
};


//TYPE CONVERSION - NOT AN OPERATOR BUT IS UNARY

class TypeCast : public UnaryOperator
{
  public:
    TypeCast(ProgramPtr expr, std::string type): UnaryOperator(expr), newtype(type){}

    std::string Whatami() const
    {
      return "TypeCast";
    }
    std::string MyType(Context& c) const
    {
      return newtype;
    }
  private:
    std::string newtype;
};

#endif
