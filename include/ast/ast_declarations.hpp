#ifndef ast_declarations
#define ast_declarations

// Defines classes for declaration statements - with and without initialisation.
//    - Parent class: Line 9
//    - Non initialisation declaration: Line 122
//    - Initialisation declaration: Line 185

// PARENT CLASS
class Declaration : public Program
{
  public:
     Declaration(std::string type, ProgramPtr var): Program(var), type(type) {}

     std::string Whatami() const
     {
       return "Declaration";
     }

     // Here returns the first identifier in the declaration statement i.e. if we have int a, b, c; Name() is 'a'.
     std::string Name() const
     {
       if(components.size() == 1)
       {
         return components[0]->Name();
       }
       else
       {
         return "";
       }
     }

     // This function is used to add all declared variables to maps that record identifier, type and memory address.
     // The maps are locallist and localtype, highest_offset is the highest memory address in the stack frame from
     // where we would begin allocation of memory for variables.
     void addvariables(Context& c, std::map<std::string, int>& locallist, std::map<std::string, std::string>& localtype, int highest_offset)
     {
       int hi = highest_offset;
       int wsize = 4;
       for(unsigned int i = 0; i < components.size(); i++)
       {
         std::string t = reducetype(c.userdefs, type);
         localtype[components[i]->Name()] = t;
         locallist[components[i]->Name()] = hi;
         if(t == "double")
         {    wsize = 8;  }
         else if((isStruct(t))&&(t[t.size()-1] != '*'))
         {
              std::string id = t.substr(7);
              wsize = StructSize(c.userdefs, (c.structs).at(id));
         }
         if((components[i]->Whatami() == "ArrayDefinition")||(components[i]->Whatami() == "ArrayAssignment"))
         {
              hi -= wsize*(components[i]->compsize());
              c.arrays[components[i]->Name()] = components[i]->compsize();
         }
         else
         {   hi -= wsize;   }
       }
     }

     // Returns memory size to be provisioned for declared variables.
     int allocvar(Context& c, int sp_off, int varframe, std::ostream& os) const
     {
       int inc = 0;
       int vf = varframe;
       int wsize = 4;
       std::string t = reducetype(c.userdefs, type);
       if(t == "double")
       {    wsize = 8;  }
       else if((isStruct(t))&&(t[t.size()-1] != '*'))
       {
            std::string id = t.substr(7);
            wsize = StructSize(c.userdefs, (c.structs).at(id));
       }
       for(unsigned int i = 0; i < components.size(); i++)
       {
         if((components[i]->Whatami() == "ArrayDefinition")||(components[i]->Whatami() == "ArrayAssignment"))
         {
           for(int j = 0; j < components[i]->compsize(); j++)
           {
             for(int k = 0; k < wsize; k+=4)
             {
               os << "sw         $0, " << vf+k << "($fp)" << std::endl;
             }
             vf += wsize;
             inc += wsize;
           }
         }
         else
         {
           for(int j = 0; j < wsize; j+=4)
           {
             os << "sw         $0, " << vf+j << "($fp)" << std::endl;
           }
           vf += wsize;
           inc += wsize;
         }
       }
       return inc;
     }

     int compsize() const
     {
       int size = 0;
       for(unsigned int i = 0; i < components.size(); i++)
       {
         std::string iam = components[i]->Whatami();
         if(iam == "ArrayDefinition")
         {    size += components[i]->compsize();   }
         else if(iam == "ArrayAssignment")
         {    size+= components[i]->compsize();     }
         else
         {    size += 1;    }
       }
       return size;
     }
  protected:
    std::string type;
};

//VARIABLE DECLARATION

class VarDeclStmt : public Declaration
{
  public:
    VarDeclStmt(std::string type, ProgramPtr var): Declaration(type, var) {}

    std::string Whatami() const
    {
      return "VarDeclStmt";
    }

    std::string MyType(Context& c) const
    {
      return reducetype(c.userdefs, type);
    }

    void PythonPrint(int tabs, std::map<std::string, std::string>& globals, std::ostream& os) const
    {
      if(type == "int")
      {
        std::string indentation = "";
        for(int j = 0; j < tabs; j++)
        {
          indentation = indentation + "\t";
        }
        for(unsigned int i = 0; i < components.size(); i++)
        {
          if(tabs == 0)
          {
            globals[components[i]->Name()] = "int";
          }
          if(i>0)
          {
            os << indentation;
          }
          os << components[i]->Name() << " = 0" << std::endl;
        }
      }
    }

    void MIPSPrint(Context& c, std::ostream& os, int sp_off, std::string destreg) const
    {
        for(unsigned int i = 0; i < components.size(); i++)
        {
          int wsize = 4;
          if(reducetype(c.userdefs, type) == "double")
          {
            wsize = 8;
          }
          if(components[i]->Whatami() == "ArrayDefinition")
          {
            os <<"            .comm " << components[i]->Name() << "," << wsize*(components[i]->compsize()) << "," << wsize << std::endl;
          }
          else
          {
            os <<"            .comm " << components[i]->Name() << "," << wsize << "," << wsize << std::endl;
          }
        }
    }
};


//VARIABLE DECLARATION WITH ASSIGNMENT

class DeclAssignmentStmt : public Declaration
{
  public:
    DeclAssignmentStmt(std::string type, ProgramPtr var_assignment): Declaration(type, var_assignment) {}

    std::string Whatami() const
    {
      return "DeclAssignmentStmt";
    }
    std::string MyType(Context& c) const
    {
      return reducetype(c.userdefs, type);
    }

    std::string Name() const
    {
      return components[0]->Name();
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
        if(components[i]->Whatami() == "StrAssignment")
        {
          components[i]->findstrings(slabel);
        }
      }
      return 0;
    }

    void PythonPrint(int tabs, std::map<std::string, std::string>& globals, std::ostream& os) const
    {
      if(type == "int")
      {
        std::string indentation = "";
        for(int j = 0; j < tabs; j++)
        {
          indentation = indentation + "\t";
        }
        for(unsigned int i = 0; i < components.size(); i++)
        {
          if(tabs == 0)
          {
            globals[components[i]->Name()] = "int";
          }
          if(i>0)
          {
            os << indentation;
          }
          components[i]->PythonPrint(tabs, globals, os);
          os << std::endl;
        }
      }
    }

    void MIPSPrint(Context& c, std::ostream& os, int sp_off, std::string destreg) const
    {
      for(unsigned int i = 0; i < components.size(); i++)
      {
        components[i]->MIPSPrint(c, os, sp_off, destreg);
      }
    }
};

#endif
