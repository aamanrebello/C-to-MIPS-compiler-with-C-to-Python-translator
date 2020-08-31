#ifndef ast_parameters
#define ast_parameters

// Defines classes for function parameter lists in function definition and call.

//FUNCTION PARAMETERS AT BOTH ENDS

//PARAMETER LIST IN FUNCTION DECLARATION

class ParameterList : public Program
{
  public:
    ParameterList(ProgramPtr decl): Program(decl) {}

    std::string Whatami() const
    {
      return "ParameterList";
    }

    // Stores parameter identifiers, datatypes and addresses into maps passed by reference - parlist and partype.
    void addparameter(std::map<std::string, int>& parlist, std::map<std::string, std::string>& partype, Context& c, int sp_off)
    {
      int j = 0;
      for(unsigned int i = 0; i < components.size(); i++)
      {
        std::string nexttype;
        std::string type = reducetype(c.userdefs, components[i]->MyType(c));
        if(i < components.size() - 1)
        {  nexttype = reducetype(c.userdefs, components[i+1]->MyType(c)); }
        else
        { nexttype = ""; }
        parlist[components[i]->Name()] = sp_off - 4*j;
        partype[components[i]->Name()] = type;
        if(type == "double")
        { j++; }
        j++;
        if(nexttype == "double")
        {
            if(j%2 != 0)
            {j++;}
        }
      }
    }

    // Prints assembly that stores parameters in allocated memory in stack frame.
    void storepars(Context& c, std::ostream& os, int framesize) const
    {
      bool allowfloat = true;
      int fps = 0;
      int j = 0;
      for(unsigned int i = 0; (i < components.size())&&(j < 4); i++)
      {
        std::string type = reducetype(c.userdefs, components[i]->MyType(c));
        if((type == "float")&&(allowfloat))
        {
          fps++;
          if(fps == 2) { allowfloat = false; }
          os << "swc1       $f" << 12 + 2*i << ", " << framesize + 4*j << "($fp)" << std::endl;
        }
        else if((type == "double")&&(allowfloat)&&(j != 3))
        {
          fps++;
          if(fps == 2) { allowfloat = false; }
          os << "swc1       $f" << 12 + 2*i << ", " << framesize + 4*j + 4 << "($fp)" << std::endl;
          os << "swc1       $f" << 12 + 2*i + 1 << ", " << framesize + 4*j << "($fp)" << std::endl;
          j++;
        }
        else
        {
          if((type == "double"))
          {
            if(j != 3)
            {
              os << "sw         $" << 4+j << ", " << framesize + 4*(j+1) << "($fp)" << std::endl;
              j++;
              os << "sw         $" << 4+j << ", " << framesize + 4*(j-1) << "($fp)" << std::endl;
            }
          }
          else
          {
            allowfloat = false;
            os << "sw         $" << 4+j << ", " << framesize + 4*j << "($fp)" << std::endl;
          }
        }
        j++;
      }
    }

    void PythonPrint(int tabs, std::map<std::string, std::string>& globals, std::ostream& os) const
    {
      for(unsigned int i = 0; (i + 1) < components.size(); i++)
      {
        os << components[i]->Name() << ", ";
      }
      if(components.size() > 0)
      {
        os << components[components.size() - 1]->Name() << " ";
      }
    }

    void MIPSPrint(Context& c, std::ostream& os, int sp_off, std::string destreg) const
    {
      for(unsigned int i = 0; (i + 1) < components.size(); i++)
      {
        os << components[i]->MyType(c) << ", ";
      }
      if(components.size() > 0)
      {
        os << components[components.size() - 1]->MyType(c);
      }
    }
};

//PARAMETER ENTRIES IN FUNCTION CALL


class ParameterEntries : public Program
{
  public:
    ParameterEntries(ProgramPtr parameter): Program(parameter) {}

    std::string Whatami() const
    {
      return "ParameterEntries";
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

    // Used to allocate memory space in stack frame for parameters (kind of a lookahead function)
    int allocsize(Context& c) const
    {
      int size = 0;
      for(unsigned int i = 0; i < components.size(); i++)
      {
        std::string type = reducetype(c.userdefs, components[i]->MyType(c));
        if(type == "double")
        {
          if(i%2 != 0)
          { size += 1; }
          size += 1;
        }
        size += 1;
      }
      return size;
    }

    int findstrings(std::map<std::string, std::string>& slabel) const
    {
      for(unsigned int i = 0; i < components.size(); i++)
      {
        if(components[i]->Whatami() == "StringLiteral")
        { components[i]->findstrings(slabel); }
      }
      return 0;
    }

    void PythonPrint(int tabs, std::map<std::string, std::string>& globals, std::ostream& os) const
    {
      for(unsigned int i = 0; (i + 1) < components.size(); i++)
      {
        components[i]->PythonPrint(tabs, globals, os);
        os << ", ";
      }
      if(components.size() > 0)
      {
        components[components.size() - 1]->PythonPrint(tabs, globals, os);
        os << " ";
      }
    }

    void MIPSPrint(Context& c, std::ostream& os, int sp_off, std::string destreg) const
    {
      if(destreg == "$4")
      {
        int j = 0;
        bool allowfloat = true;
        int fps = 0;
        for(unsigned int i = 0; i < components.size(); i++)
        {
          std::string type = reducetype(c.userdefs, components[i]->MyType(c));
          if(j < 4)
          {
            if((type == "float")||(type == "double"))
            {
              if(allowfloat)
              {
                std::string reg = "$f" + std::to_string(12+2*i);
                components[i]->MIPSPrint(c, os, sp_off, reg);
                fps++;
                if(fps == 2)
                { allowfloat = false; }
              }
              else
              {
                if(type == "float")
                {
                  components[i]->MIPSPrint(c, os, sp_off, "$f10");
                  std::string reg = "$" + std::to_string(4+j);
                  os << "mfc1       " << reg << ", $f10" << std::endl;
                }
                else if(type == "double")
                {
                  if(j == 3) {i--; j++; continue;}
                  else if(j == 1) {j++;}
                  components[i]->MIPSPrint(c, os, sp_off, "$f10");
                  std::string reg1 = "$" + std::to_string(4+j);
                  std::string reg2 = "$" + std::to_string(5+j);
                  os << "mfc1       " << reg2 << ", $f10" << std::endl;
                  os << "mfc1       " << reg1 << ", $f11" << std::endl;
                }
              }
              if(type == "double")
              { j++; }
            }
            else
            {
              allowfloat = false;
              std::string reg = "$" + std::to_string(4+j);
              components[i]->MIPSPrint(c, os, sp_off, reg);
            }
          }
          else
          {
            if(type == "float")
            {
              components[i]->MIPSPrint(c, os, sp_off, "$f4");
              os << "swc1       $f4, " << 16 + (j - 4)*4 << "($fp)" << std::endl;
              os << "nop" << std::endl;
            }
            else if(type == "double")
            {
              components[i]->MIPSPrint(c, os, sp_off, "$f4");
              os << "swc1       $f4, " << 16 + (j - 4)*4 + 4 << "($fp)" << std::endl;
              os << "nop" << std::endl;
              os << "swc1       $f5, " << 16 + (j - 4)*4 << "($fp)" << std::endl;
              os << "nop" << std::endl;
              j++;
            }
            else
            {
              components[i]->MIPSPrint(c, os, sp_off, "$8");
              os << "sw         $8, " << 16 + (j - 4)*4 << "($fp)" << std::endl;
              os << "nop" << std::endl;
            }
          }
          j++;
        }
      }
      /////////////////////////////////////////////////////////////
      else if(destreg == "ArrayAssignment")
      {
        int base = c.vpframe;
        int wsize = 4;
        std::string type = components[0]->MyType(c);
        type = reducetype(c.userdefs, type);
        if(type == "double")
        { wsize = 8; }
        for(unsigned int i = 0; i < components.size(); i++)
        {
          if(type == "float")
          {
            components[i]->MIPSPrint(c, os, sp_off, "$f4");
            os << "swc1       $f4, " << base << "($fp)" << std::endl;
            os << "nop" << std::endl;
          }
          else if(type == "double")
          {
            components[i]->MIPSPrint(c, os, sp_off, "$f4");
            os << "swc1       $f4, " << base+4 << "($fp)" << std::endl;
            os << "nop" << std::endl;
            os << "swc1       $f5, " << base << "($fp)" << std::endl;
            os << "nop" << std::endl;
          }
          else
          {
            components[i]->MIPSPrint(c, os, sp_off, "$8");
            os << "sw         $8, " << base << "($fp)" << std::endl;
          }
          base += wsize;
        }
      }
      else if(destreg == "global")
      {
        std::string type = components[0]->MyType(c);
        type = reducetype(c.userdefs, type);
        for(unsigned int i = 0; i < components.size(); i++)
        {
          if(type == "double")
          {
            dbConv myValue;
            myValue.dValue = components[i]->dblvalue(c);
            unsigned int low = myValue.INTValue&(0xFFFFFFFF);
            myValue.INTValue >>= 32;
            unsigned int upp = myValue.INTValue&(0xFFFFFFFF);
            os << "           .word " << upp << std::endl;
            os << "           .word " << low << std::endl;
          }
          else if(type == "float")
          {
            fpconv myValue;
            myValue.fValue = (float)components[i]->dblvalue(c);
            os << "           .word " << myValue.iValue << std::endl;
          }
          else
          {
            os << "           .word " << components[i]->intvalue(c) << std::endl;
          }
        }
    }
  }
};


#endif
