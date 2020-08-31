#ifndef ast_operands
#define ast_operands

// Used to define classes for various kinds of operands - literals, variables and bracketed expressions.
//    - literal superclass: line 14
//    - integer literal class: line 29
//    - double literal class: line 60
//    - float literal class: line 106
//    - char literal class: line 144
//    - string literal class: line 177
//    - variable class: line 222
//    - bracketed term class: line 345

class Literal : public Program
{
  public:
    Literal(std::string rettype): returntype(rettype) {}

    std::string Whatami() const
    {
      return "Literal";
    }
  private:
    std::string returntype;
};

//LITERALS

class IntLiteral : public Literal
{
  public:
    IntLiteral(int val): Literal("int"), value(val) {}

    std::string Whatami() const
    {
      return "IntLiteral";
    }
    std::string MyType(Context& c) const
    {
      return "int";
    }

    int intvalue(Context& c) const
    {
      return value;
    }

    void PythonPrint(int tabs, std::map<std::string, std::string>& globals, std::ostream& os) const
    {
      os << value;
    }
    void MIPSPrint(Context& c, std::ostream& os, int sp_off, std::string destreg) const
    {
       os << "li         "<< destreg << ", " << value << std::endl;
    }
  private:
    int value;
};

class DoubleLiteral : public Literal
{
  public:
    DoubleLiteral(double val): Literal("double"), value(val) {}

    std::string Whatami() const
    {
      return "DoubleLiteral";
    }
    std::string MyType(Context& c) const
    {
      return "double";
    }

    double dblvalue(Context& c) const
    {
      return value;
    }

    void PythonPrint(int tabs, std::map<std::string, std::string>& globals, std::ostream& os) const
    {
      os << value;
    }

    void MIPSPrint(Context& c, std::ostream& os, int sp_off, std::string destreg) const
    {
      dbConv myValue;
      myValue.dValue = value;
      std::string label = makeName("$LC");
      unsigned int a = myValue.INTValue&(0xFFFFFFFF);
      myValue.INTValue >>= 32;
      unsigned int b = myValue.INTValue&(0xFFFFFFFF);
      c.dbl_labels[label].upper = b;
      c.dbl_labels[label].lower = a;
      int n = std::stoi(destreg.substr(2)) + 1;
      std::string reg2 = destreg.substr(0, 2) + std::to_string(n);
      os << "lui        $24, %hi(" << label << ")" << std::endl;
      os << "lwc1       " << destreg << ", %lo(" << label << "+4)($24)" << std::endl;
      os << "nop" << std::endl;
      os << "lwc1       " << reg2 << ", %lo(" << label << ")($24)" << std::endl;
      os << "nop" << std::endl;
    }
  private:
    double value;
};

class FloatLiteral : public Literal
{
  public:
    FloatLiteral(double val): Literal("float"), value(val) {}

    std::string Whatami() const
    {
      return "FloatLiteral";
    }
    std::string MyType(Context& c) const
    {
      return "float";
    }

    double dblvalue(Context& c) const
    {
      return value;
    }

    void PythonPrint(int tabs, std::map<std::string, std::string>& globals, std::ostream& os) const
    {
      os << value;
    }

    void MIPSPrint(Context& c, std::ostream& os, int sp_off, std::string destreg) const
    {
      fpconv myValue;
      myValue.fValue = value;
      std::string label = makeName("$LC");
      c.float_labels[label] = myValue.iValue;
      os << "lui        $24,  %hi(" << label << ")" << std::endl;
      os << "lwc1       " << destreg << ", %lo(" << label << ")($24)" << std::endl;
      os << "nop" << std::endl;
    }
  private:
    float value;
};

class CharLiteral : public Literal
{
  public:
    CharLiteral(char val): Literal("char"), value(val) {}

    std::string Whatami() const
    {
      return "CharLiteral";
    }
    std::string MyType(Context& c) const
    {
      return "char";
    }

    int intvalue(Context& c) const
    {
      return (int)value;
    }

    void PythonPrint(int tabs, std::map<std::string, std::string>& globals, std::ostream& os) const
    {
      os << "\'" <<value << "\'";
    }

    void MIPSPrint(Context& c, std::ostream& os, int sp_off, std::string destreg) const
    {
      int v = (int)value;
      os << "li         "<< destreg << ", " << v << std::endl;
    }
  private:
    char value;
};

class StringLiteral : public Literal
{
  public:
    StringLiteral(std::string val): Literal("string"), value(val) {}

    std::string Whatami() const
    {
      return "StringLiteral";
    }
    std::string MyType(Context& c) const
    {
      return "char *";
    }

    int compsize() const
    {
      std::string s = stringconvert(value);
      return s.size();
    }

    int findstrings(std::map<std::string, std::string>& slabel) const
    {
      std::string label = makeName("$LC");
      slabel[value] = label;
      return 0;
    }

    void PythonPrint(int tabs, std::map<std::string, std::string>& globals, std::ostream& os) const
    {
      os << "\"" << value << "\"";
    }

    void MIPSPrint(Context& c, std::ostream& os, int sp_off, std::string destreg) const
    {
      std::string clabel = c.strlabels.at(value);
      os << "lui        " << destreg << ", %hi(" << clabel << ")" << std::endl;
      os << "addiu      " << destreg << ", " << destreg << ", %lo(" << clabel << ")" << std::endl;
    }
  private:
    std::string value;
};


//VARIABLE

class Variable : public Program
{
  public:
    Variable(std::string id): identifier(id) {}

    std::string Whatami() const
    {
      return "Variable";
    }
    std::string Name() const
    {
      return identifier;
    }
    std::string MyType(Context& c) const
    {
      return reducetype(c.userdefs, c.vartype[identifier]);
    }

    void PythonPrint(int tabs, std::map<std::string, std::string>& globals, std::ostream& os) const
    {
      os << identifier;
    }
    void MIPSPrint(Context& c, std::ostream& os, int sp_off, std::string destreg) const
    {
      int addr = c.variablelist[identifier];
      std::string type = reducetype(c.userdefs, c.vartype[identifier]);
      if((c.constants).find(identifier) != (c.constants).end())
      {
        os << "li         " << destreg << ", " << (c.constants).at(identifier) << std::endl;
      }
      else if(addr <= -1000)
      {
        if((type != "double")&&(type != "float"))
        {
          os << "lui        " << destreg << ", %hi(" << identifier << ")" << std::endl;
          if((type == "char")||(type == "signed char"))
          {
            os << "addiu      " << destreg << ", " << destreg << ", %lo(" << identifier << ")" << std::endl;
            os << "lb         " << destreg << ", " << "3(" << destreg << ")" << std::endl;
          }
          else if(type == "unsigned char")
          {
            os << "addiu      " << destreg << ", " << destreg << ", %lo(" << identifier << ")" << std::endl;
            os << "lb         " << destreg << ", " << "3(" << destreg << ")" << std::endl;
          }
          else if((type == "short")||(type == "signed short"))
          {
            os << "addiu      " << destreg << ", " << destreg << ", %lo(" << identifier << ")" << std::endl;
            os << "lh         " << destreg << ", " << "2(" << destreg << ")" << std::endl;
          }
          else if(type == "unsigned short")
          {
            os << "addiu      " << destreg << ", " << destreg << ", %lo(" << identifier << ")" << std::endl;
            os << "lhu         " << destreg << ", " << "2(" << destreg << ")" << std::endl;
          }
          else
          {
            os << "lw         " << destreg << ", %lo(" << identifier << ")(" << destreg << ")" << std::endl;
          }
          os << "nop" << std::endl;
        }
        else
        {
          os << "lui        $24, %hi(" << identifier << ")" << std::endl;
          if(type == "float")
          {
            os << "lwc1       " << destreg << "%lo(" << identifier << ")($24)" << std::endl;
            os << "nop" << std::endl;
          }
          else if(type == "double")
          {
            int n = std::stoi(destreg.substr(2)) + 1;
            std::string reg2 = destreg.substr(0, 2) + std::to_string(n);
            os << "lwc1       " << destreg << ", %lo(" << identifier << "+4)($24)" << std::endl;
            os << "nop" << std::endl;
            os << "lwc1       " << reg2 << ", %lo(" << identifier << ")($24)" << std::endl;
          }
          os << "nop" << std::endl;
        }
      }
      else
      {
        if((type == "char")||(type == "signed char"))
        {
          os << "lb         " << destreg << ", " << (sp_off - addr + 3) << "($fp)" << std::endl;
        }
        else if(type == "unsigned char")
        {
          os << "lbu        " << destreg << ", " << (sp_off - addr + 3) << "($fp)" << std::endl;
        }
        else if((type == "short")||(type == "signed short"))
        {
          os << "lh         " << destreg << ", " << (sp_off - addr + 2) << "($fp)" << std::endl;
        }
        else if(type == "unsigned short")
        {
          os << "lhu        " << destreg << ", " << (sp_off - addr + 2) << "($fp)" << std::endl;
        }
        else if(type == "float")
        {
          os << "lwc1       " << destreg << ", " << (sp_off - addr) << "($fp)" << std::endl;
        }
        else if(type == "double")
        {
          int n = std::stoi(destreg.substr(2)) + 1;
          std::string reg2 = destreg.substr(0, 2) + std::to_string(n);
          os << "lwc1       " << destreg << ", " << (sp_off - addr + 4) << "($fp)" << std::endl;
          os << "nop" << std::endl;
          os << "lwc1       " << reg2 << ", " << (sp_off - addr) << "($fp)" << std::endl;
        }
        else
        {
          os << "lw         " << destreg << ", " << (sp_off - addr) << "($fp)" << std::endl;
        }
        os << "nop" << std::endl;
      }
    }
  private:
    std::string identifier;
};

//BRACKETED TERM

class BracketedTerm : public Program
{
  public:
    BracketedTerm(ProgramPtr expression): expression(expression) {}
    ~BracketedTerm()
    {
      delete expression;
    }

    std::string Whatami() const
    {
      return "BracketedTerm";
    }

    std::string MyType(Context& c) const
    {
      return expression->MyType(c);
    }

    int maxparams(Context& c) const
    {
      return expression->maxparams(c);
    }

    int intvalue(Context& c) const
    {
      return expression->intvalue(c);
    }

    double dblvalue(Context& c) const
    {
      return expression->dblvalue(c);
    }

    void PythonPrint(int tabs, std::map<std::string, std::string>& globals, std::ostream& os) const
    {
      os << "( ";
      expression->PythonPrint(tabs, globals, os);
      os << " )";
    }

    void MIPSPrint(Context& c, std::ostream& os, int sp_off, std::string destreg) const
    {
      expression->MIPSPrint(c, os, sp_off, destreg);
    }
  private:
    ProgramPtr expression;
};


#endif
