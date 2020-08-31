#ifndef ast_struct
#define ast_struct

// Implememts struct component classes.

//STRUCT DECLARATION LIST

class StructList : public Program
{
  public:
    StructList(ProgramPtr p): Program(p) {}

    std::string Whatami() const
    {
      return "StructList";
    }

    // Iterating through variables in struct declaration list and adds them to Sdecl struct instance.
    void addstruct(Context& c, std::string id) const
    {
      std::map<std::string, std::string> decls;
      for(unsigned int i = 0; i < components.size(); i++)
      {
        c.structs[id].strvars.push_back(components[i]->Name());
        c.structs[id].types.push_back(components[i]->MyType(c));
      }
    }
};

//STRUCT BLOCK

class Struct : public Program
{
  public:
    Struct(std::string name, ProgramPtr declarations): identifier(name), decl_list(declarations) {}
    ~Struct()
    {
      delete decl_list;
    }

    std::string Whatami() const
    {
      return "Struct";
    }
    std::string Name() const
    {
      return identifier;
    }

    int compsize() const
    {
      return decl_list->compsize();
    }

    virtual void MIPSPrint(Context& c, std::ostream& os, int sp_off, std::string destreg) const
    {
      StructList* sl = (StructList*)decl_list;
      sl->addstruct(c, identifier);
    }
  private:
    std::string identifier;
    ProgramPtr decl_list;
};



#endif
