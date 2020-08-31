#ifndef ast_bitwise
#define ast_bitwise

// Defines functions to implement bitwise operators:
//  - &: line 13
//  - |: line 45
//  - ^: line 77
//  - <<: line 109
//  - >>: line 141

//BITWISE OPERATORS

//&

class BitwiseAnd : public BinaryOperator
{
  public:
    BitwiseAnd(ProgramPtr leftop, ProgramPtr rightop): BinaryOperator(leftop, rightop) {}

    std::string Whatami() const
    {
      return "BitwiseAnd";
    }

    int intvalue(Context& c) const
    {
      return (left->intvalue(c))&(right->intvalue(c));
    }

    void PythonPrint(int tabs, std::map<std::string, std::string>& globals, std::ostream& os) const
    {
      left->PythonPrint(tabs, globals, os);
      os << " & ";
      right->PythonPrint(tabs, globals, os);
    }

    void MIPSPrint(Context& c, std::ostream& os, int sp_off, std::string destreg) const
    {
      setupLR(c, os, sp_off, 0);
      os << "and        " << destreg << ", $8, $9" << std::endl;
    }
};


// |

class BitwiseOr : public BinaryOperator
{
  public:
    BitwiseOr(ProgramPtr leftop, ProgramPtr rightop): BinaryOperator(leftop, rightop) {}

    std::string Whatami() const
    {
      return "BitwiseOr";
    }

    int intvalue(Context& c) const
    {
      return (left->intvalue(c))|(right->intvalue(c));
    }

    void PythonPrint(int tabs, std::map<std::string, std::string>& globals, std::ostream& os) const
    {
      left->PythonPrint(tabs, globals, os);
      os << " | ";
      right->PythonPrint(tabs, globals, os);
    }

    void MIPSPrint(Context& c, std::ostream& os, int sp_off, std::string destreg) const
    {
      setupLR(c, os, sp_off, 0);
      os << "or         " << destreg << ", $8, $9" << std::endl;
    }
};


// ^

class BitwiseXor : public BinaryOperator
{
  public:
    BitwiseXor(ProgramPtr leftop, ProgramPtr rightop): BinaryOperator(leftop, rightop) {}

    std::string Whatami() const
    {
      return "BitwiseXor";
    }

    int intvalue(Context& c) const
    {
      return (left->intvalue(c))^(right->intvalue(c));
    }

    void PythonPrint(int tabs, std::map<std::string, std::string>& globals, std::ostream& os) const
    {
      left->PythonPrint(tabs, globals, os);
      os << " ^ ";
      right->PythonPrint(tabs, globals, os);
    }

    void MIPSPrint(Context& c, std::ostream& os, int sp_off, std::string destreg) const
    {
      setupLR(c, os, sp_off, 0);
      os << "xor        " << destreg << ", $8, $9" << std::endl;
    }
};


// <<

class BitwiseLShift : public BinaryOperator
{
  public:
    BitwiseLShift(ProgramPtr leftop, ProgramPtr rightop): BinaryOperator(leftop, rightop) {}

    std::string Whatami() const
    {
      return "BitwiseLShift";
    }

    int intvalue(Context& c) const
    {
      return (left->intvalue(c))<<(right->intvalue(c));
    }

    void PythonPrint(int tabs, std::map<std::string, std::string>& globals, std::ostream& os) const
    {
      left->PythonPrint(tabs, globals, os);
      os << " << ";
      right->PythonPrint(tabs, globals, os);
    }

    void MIPSPrint(Context& c, std::ostream& os, int sp_off, std::string destreg) const
    {
      setupLR(c, os, sp_off, 0);
      os << "sllv       " << destreg << ", $8, $9" << std::endl;
    }
};


// >>

class BitwiseRShift : public BinaryOperator
{
  public:
    BitwiseRShift(ProgramPtr leftop, ProgramPtr rightop): BinaryOperator(leftop, rightop) {}

    std::string Whatami() const
    {
      return "BitwiseRShift";
    }

    int intvalue(Context& c) const
    {
      return (left->intvalue(c))>>(right->intvalue(c));
    }

    void PythonPrint(int tabs, std::map<std::string, std::string>& globals, std::ostream& os) const
    {
      left->PythonPrint(tabs, globals, os);
      os << " >> ";
      right->PythonPrint(tabs, globals, os);
    }

    void MIPSPrint(Context& c, std::ostream& os, int sp_off, std::string destreg) const
    {
      setupLR(c, os, sp_off, 0);
      os << "srlv       " << destreg << ", $8, $9" << std::endl;
    }
};


#endif
