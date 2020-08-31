#ifndef ast_loop
#define ast_loop

// Defines classes for for, while, do-while, break keyword, continue keyword.
//    - Loop parent class: Line 12
//    - While class: Line 51
//    - do-while class: Line 99
//    - for class: Line 130
//    - break keyword class: Line 229
//    - continue keyword class: Line 252

class Loop : public Program
{
  public:
    Loop(ProgramPtr condition, ProgramPtr statements): expression(condition), block(statements) {}

    virtual ~Loop()
    {
      delete expression;
      delete block;
    }

    std::string Whatami() const
    {
      return "Loop";
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
  protected:
    ProgramPtr expression;
    ProgramPtr block;
};

//WHILE LOOP

class While : public Loop
{
  public:
    While(ProgramPtr condition, ProgramPtr statements): Loop(condition, statements) {}

    std::string Whatami() const
    {
      return "While";
    }

    void PythonPrint(int tabs, std::map<std::string, std::string>& globals, std::ostream& os) const
    {
      os << "while (";
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
      c.current_label = makeNumber();
      std::string startlabel = makeName("L");
      std::string endlabel = makeName("L");
      std::string midlabel = makeName("L");
      os << "b          " << midlabel << std::endl;
      os << "nop" << std::endl;
      os << startlabel << ":" << std::endl;
      int crlab = c.current_label;
      block->MIPSPrint(c, os, sp_off, destreg);
      if((block->Whatami() == "While")||(block->Whatami() == "DoWhile")||(block->Whatami() == "For"))
      { c.current_label = crlab; }
      os << midlabel << ":" << std::endl;
      expression->MIPSPrint(c, os, sp_off, "$8");
      os << "bne        $8, $0, " << startlabel << std::endl;
      os << "nop" << std::endl;
      os << endlabel << ":" << std::endl;
    }
};

//DO WHILE LOOP

class DoWhile : public Loop
{
  public:
    DoWhile(ProgramPtr condition, ProgramPtr statements): Loop(condition, statements) {}

    std::string Whatami() const
    {
      return "DoWhile";
    }

    void MIPSPrint(Context& c, std::ostream& os, int sp_off, std::string destreg) const
    {
      c.current_label = makeNumber();
      std::string startlabel = makeName("L");
      std::string endlabel = makeName("L");
      std::string midlabel = makeName("L");
      os << startlabel << ":" << std::endl;
      int crlab = c.current_label;
      block->MIPSPrint(c, os, sp_off, destreg);
      if((block->Whatami() == "While")||(block->Whatami() == "DoWhile")||(block->Whatami() == "For"))
      { c.current_label = crlab; }
      os << midlabel << ":" << std::endl;
      expression->MIPSPrint(c, os, sp_off, "$8");
      os << "bne        $8, $0, " << startlabel << std::endl;
      os << "nop" << std::endl;
      os << endlabel << ":" << std::endl;
    }
};

//FOR LOOP

class For : public Loop
{
  public:
    For(ProgramPtr start, ProgramPtr condition, ProgramPtr changevariable, ProgramPtr statements): Loop(condition, statements), initial(start), control(changevariable) {}
    For(ProgramPtr start, ProgramPtr condition, ProgramPtr statements): Loop(condition, statements), initial(start), control(NULL) {}
    ~For()
    {
      delete initial;
      delete control;
    }

    std::string Whatami() const
    {
      return "For";
    }
    int countdeclarations(Context& c) const
    {
      int size = 0;
      size += block->countdeclarations(c);

      if(initial->Whatami() == "DeclAssignmentStmt")
      {
        int addme = 8;
        if(initial->MyType(c) != "double")
        { addme = 4; }
        size += addme;
      }
      return size;
    }

    int maxparams(Context& c) const
    {
      int bl = block->maxparams(c), ini = initial->maxparams(c), cont = control->maxparams(c);
      if(bl >= ini)
      {
        if(bl >= cont)
        {  return bl; }
      }
      else
      {
        if(ini >= cont)
        {  return ini; }
      }
      return cont;
    }

    void MIPSPrint(Context& c, std::ostream& os, int sp_off, std::string destreg) const
    {
      c.current_label = makeNumber();
      std::string startlabel = makeName("L");
      std::string endlabel = makeName("L");
      std::string midlabel1 = makeName("L");
      std::string midlabel2 = makeName("L");
      int oldaddr = -1;
      std::string oldtype = "";
      if(initial->Whatami() == "DeclAssignmentStmt")
      {
        os << "sw         $0, " << c.vpframe << "($fp)" << std::endl;
        if(c.variablelist.find(initial->Name()) != c.variablelist.end())
        {
          oldaddr = (c.variablelist).at(initial->Name());
          oldtype = (c.vartype).at(initial->Name());
        }
        c.variablelist[initial->Name()] = sp_off - c.vpframe;
        c.vartype[initial->Name()] = initial->MyType(c);
        c.vpframe += 4;
      }
      initial->MIPSPrint(c, os, sp_off, destreg);
      os << "b          " << midlabel2 << std::endl;
      os << "nop" << std::endl;
      os << startlabel << ":" << std::endl;
      int crlab = c.current_label;
      block->MIPSPrint(c, os, sp_off, destreg);
      if((block->Whatami() == "While")||(block->Whatami() == "DoWhile")||(block->Whatami() == "For"))
      { c.current_label = crlab; }
      os << midlabel1 << ":" << std::endl;
      if(control != NULL)
      { control->MIPSPrint(c, os, sp_off, destreg); }
      os << midlabel2 << ":" << std::endl;
      expression->MIPSPrint(c, os, sp_off, "$8");
      os << "bne        $8, $0, " << startlabel << std::endl;
      os << "nop" << std::endl;
      os << endlabel << ":" << std::endl;
      if(initial->Whatami() == "DeclAssignmentStmt")
      {
        if((oldaddr != -1)&&(oldtype != ""))
        {
          c.variablelist[initial->Name()] = oldaddr;
          c.vartype[initial->Name()] = oldtype;
        }
      }
    }
  private:
    ProgramPtr initial;
    ProgramPtr control;
};

//COMPONENTS - BREAK, CONTINUE

class Break : public Program
{
  public:
    Break() {}

    std::string Whatami() const
    {
      return "Break";
    }

    void PythonPrint(int tabs, std::map<std::string, std::string>& globals, std::ostream& os) const
    {
      os << "break";
    }
    void MIPSPrint(Context& c, std::ostream& os, int sp_off, std::string destreg) const
    {
      os << "b          L" << (c.current_label + 1) << std::endl;
      os << "nop" << std::endl;
    }
};

class Continue : public Program
{
  public:
    Continue() {}

    std::string Whatami() const
    {
      return "Continue";
    }

    void PythonPrint(int tabs, std::map<std::string, std::string>& globals, std::ostream& os) const
    {
      os << "continue";
    }
    void MIPSPrint(Context& c, std::ostream& os, int sp_off, std::string destreg) const
    {
      os << "b          L" << c.current_label + 2 << std::endl;
      os << "nop" << std::endl;
    }
};

#endif
