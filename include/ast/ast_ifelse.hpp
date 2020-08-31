#ifndef ast_ifelse
#define ast_ifelse

// Implements classes for if (line 6), else (line 84), if-else statements (line 138). 

//IF BLOCK

class If : public Program
{
  public:
    If(ProgramPtr condition, ProgramPtr statements): expression(condition), block(statements) {}
    ~If()
    {
      delete expression;
      delete block;
    }

    std::string Whatami() const
    {
      return "If";
    }

    int countdeclarations(Context& c) const
    {
        return block->countdeclarations(c);
    }

    int maxparams(Context& c) const
    {
      int ex = expression->maxparams(c), bl = block->maxparams(c);
      if(ex >= bl)
      { return ex; }
      else
      { return bl; }
    }

    int findstrings(std::map<std::string, std::string>& slabel) const
    {
      return block->findstrings(slabel);
    }

    void PythonPrint(int tabs, std::map<std::string, std::string>& globals, std::ostream& os) const
    {
      os << "if (";
      expression->PythonPrint(tabs, globals, os);
      os << "):" << std::endl;
      if(block->Whatami() != "Sequence")
      {
        for(int i = 0; i <= tabs; i++)
        {
          os << "\t";
        }
      }
      block->PythonPrint((tabs + 1), globals, os);
    }

    void MIPSPrint(Context& c, std::ostream& os, int sp_off, std::string destreg) const
    {
        std::string endlabel;
        int labelno = makeNumber();
        if(destreg != "IfElse")
        { endlabel = makeName("L"); }
        else
        { endlabel = "L" + std::to_string(labelno - 2); }
        expression->MIPSPrint(c, os, sp_off, "$8");
        os << "beq        $8, $0, " << endlabel << std::endl;
        os << "nop" << std::endl;
        int crlab = c.current_label;
        block->MIPSPrint(c, os, sp_off, "");
        if((block->Whatami() == "While")||(block->Whatami() == "DoWhile")||(block->Whatami() == "For"))
        { c.current_label = crlab; }
        if(destreg == "IfElse")
        {
          os << "b          L" << (labelno - 1) << std::endl;
          os << "nop" << std::endl;
        }
        os << endlabel << ":" << std::endl;
    }
  private:
    ProgramPtr expression;
    ProgramPtr block;
};

//ELSE BLOCK

class Else : public Program
{
  public:
    Else(ProgramPtr statements): block(statements) {}
    ~Else()
    {
      delete block;
    }

    std::string Whatami() const
    {
      return "Else";
    }

    int countdeclarations(Context& c) const
    {
        return block->countdeclarations(c);
    }

    int maxparams(Context& c) const
    {
      return block->maxparams(c);
    }

    int findstrings(std::map<std::string, std::string>& slabel) const
    {
        return block->findstrings(slabel);
    }

    void PythonPrint(int tabs, std::map<std::string, std::string>& globals, std::ostream& os) const
    {
      os << "else:" << std::endl;
      if(block->Whatami() != "Sequence")
      {
        for(int i = 0; i <= tabs; i++)
        {
          os << "\t";
        }
      }
      block->PythonPrint((tabs + 1), globals, os);
    }
    void MIPSPrint(Context& c, std::ostream& os, int sp_off, std::string destreg) const
    {
      int crlab = c.current_label;
      block->MIPSPrint(c, os, sp_off, "");
      if((block->Whatami() == "While")||(block->Whatami() == "DoWhile")||(block->Whatami() == "For"))
      { c.current_label = crlab; }
    }
  private:
    ProgramPtr block;
};

//IFELSE STATEMENTS

class IfElse : public Program
{
  public:
    IfElse(ProgramPtr ifst, ProgramPtr elsest): ifstat(ifst), elsestat(elsest) {}
    ~IfElse()
    {
      delete ifstat;
      delete elsestat;
    }

    std::string Whatami() const
    {
      return "IfElse";
    }

    int countdeclarations(Context& c) const
    {
      return ifstat->countdeclarations(c) + elsestat->countdeclarations(c);
    }

    int maxparams(Context& c) const
    {
      int ifst = ifstat->maxparams(c), elst = elsestat->maxparams(c);
      if(ifst >= elst)
      {  return ifst; }
      else
      {  return elst; }
    }

    int findstrings(std::map<std::string, std::string>& slabel) const
    {
      return ifstat->findstrings(slabel) + elsestat->findstrings(slabel);
    }

    void PythonPrint(int tabs, std::map<std::string, std::string>& globals, std::ostream& os) const
    {
      ifstat->PythonPrint(tabs, globals, os);
      os << std::endl;
      for(int i = 0; i < tabs; i++)
      {
        os << "\t";
      }
      elsestat->PythonPrint(tabs, globals, os);
    }

    void MIPSPrint(Context& c, std::ostream& os, int sp_off, std::string destreg) const
    {
      std::string ifendlabel = makeName("L");
      std::string elsendlabel = makeName("L");
      int crlab = c.current_label;
      ifstat->MIPSPrint(c, os, sp_off, "IfElse");
      if((ifstat->Whatami() == "While")||(ifstat->Whatami() == "DoWhile")||(ifstat->Whatami() == "For"))
      { c.current_label = crlab; }
      crlab = c.current_label;
      elsestat->MIPSPrint(c, os, sp_off, "IfElse");
      if((elsestat->Whatami() == "While")||(elsestat->Whatami() == "DoWhile")||(elsestat->Whatami() == "For"))
      { c.current_label = crlab; }
      os << elsendlabel << ":" << std::endl;
    }
  private:
    ProgramPtr ifstat;
    ProgramPtr elsestat;
};


#endif
