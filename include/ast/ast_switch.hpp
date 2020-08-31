#ifndef ast_switch
#define ast_switch

// Defines classes for switch statement components:
//    - Switch: Line 9
//    - Single case: Line 60
//    - Set of cases within switch statement: Line 118

//SWITCH BLOCK

class Switch : public Program
{
  public:
    Switch(ProgramPtr control, ProgramPtr cases): control(control), cases(cases) {}
    ~Switch()
    {
      delete control;
      delete cases;
    }

    std::string Whatami() const
    {
      return "Switch";
    }

    int countdeclarations(Context& c) const
    {
      return cases->countdeclarations(c);
    }

    int maxparams(Context& c) const
    {
      int cont = control->maxparams(c), cas = cases->maxparams(c);
      if(cont >= cas)
      { return cont; }
      else
      { return cas; }
    }

    int findstrings(std::map<std::string, std::string>& slabel) const
    {
      return cases->findstrings(slabel);
    }

    void MIPSPrint(Context& c, std::ostream& os, int sp_off, std::string destreg) const
    {
      std::string endlabel = makeName("L");
      int crlab = c.current_label;
      c.current_label = makeNumber() - 2;
      control->MIPSPrint(c, os, sp_off, "$14");
      cases->MIPSPrint(c, os, sp_off, destreg);
      os << endlabel << ":" << std::endl;
      c.current_label = crlab;
    }
  private:
    ProgramPtr control;
    ProgramPtr cases;
};

//SINGLE CASE

class SingleCase : public Program
{
  public:
    SingleCase(ProgramPtr serial, ProgramPtr statements): casetype(serial), statements(statements) {}
    SingleCase(ProgramPtr statements): casetype(NULL), statements(statements) {}
    ~SingleCase()
    {
      delete casetype;
      delete statements;
    }

    std::string Whatami() const
    {
      return "SingleCase";
    }

    int countdeclarations(Context& c) const
    {
      return statements->countdeclarations(c);
    }

    int maxparams(Context& c) const
    {
      return statements->maxparams(c);
    }

    int findstrings(std::map<std::string, std::string>& slabel) const
    {
      return statements->findstrings(slabel);
    }

    // Sets up initial branching statements before assmebly for actual statements within cases.
    void CaseMIPS(Context& c, std::ostream& os, int sp_off, std::string label) const
    {
      if(casetype != NULL)
      {
        casetype->MIPSPrint(c, os, sp_off, "$9");
        os << "beq        $9, $14, " << label << std::endl;
        os << "nop" << std::endl;
      }
      else
      {
        os << "b          " << label << std::endl;
        os << "nop" << std::endl;
      }
    }
    void MIPSPrint(Context& c, std::ostream& os, int sp_off, std::string destreg) const
    {
      os << destreg << ":" << std::endl;
      statements->MIPSPrint(c, os, sp_off, "");
    }
  private:
    ProgramPtr casetype;
    ProgramPtr statements;
};

//CASES

class Cases : public Program
{
  public:
    Cases(ProgramPtr case1): Program(case1) {}

    std::string Whatami() const
    {
      return "Cases";
    }

    int countdeclarations(Context& c) const
    {
      int size = 0;
      for(unsigned int i = 0; i < components.size(); i++)
      {
        size += (components[i])->countdeclarations(c);
      }
      return size;
    }

    int maxparams(Context& c) const
    {
      int max = 0;
      for(unsigned int i = 0; i < components.size(); i++)
      {
        int mp = components[i]->maxparams(c);
        if(mp > max)
        { max = mp; }
      }
      return max;
    }

    int findstrings(std::map<std::string, std::string>& slabel) const
    {
      for(unsigned int i = 0; i < components.size(); i++)
      {
        components[i]->findstrings(slabel);
      }
      return 0;
    }

    void MIPSPrint(Context& c, std::ostream& os, int sp_off, std::string destreg) const
    {
      std::vector<std::string> labels;
      for(unsigned int i = 0; i < components.size(); i++)
      {
        std::string label = makeName("L");
        labels.push_back(label);
        SingleCase* sc = (SingleCase*)(components[i]);
        sc->CaseMIPS(c, os, sp_off, label);
      }
      for(unsigned int i = 0; i < components.size(); i++)
      {
        components[i]->MIPSPrint(c, os, sp_off, labels[i]);
      }
    }
};

#endif
