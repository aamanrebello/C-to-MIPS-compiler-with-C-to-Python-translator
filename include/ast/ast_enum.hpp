#ifndef ast_enum
#define ast_enum

// Defines classes for enum components - list of declarations (line 7), enum declaration (line 47),
// enum typedef (line 84).

//ENUM LIST

class EnumList : public Program
{
  public:
    EnumList(ProgramPtr p): Program(p) {}

    std::string Whatami() const
    {
      return "EnumList";
    }

    // Used to record the enum constants - used when building parse tree.
    void addconst(std::map<std::string, int>& newconst, Context& c) const
    {
      if(components[0]->Whatami() == "Variable")
      {
        newconst[components[0]->Name()] = 0;
      }
      else if(components[0]->Whatami() == "Assignment")
      {
        Assignment* as = (Assignment*)(components[0]);
        newconst[components[0]->Name()] = as->Value(c);
      }
      for(unsigned int i = 1; i < components.size(); i++)
      {
        if(components[i]->Whatami() == "Assignment")
        {
          Assignment* as = (Assignment*)(components[i]);
          newconst[components[i]->Name()] = as->Value(c);
        }
        else
        {
          newconst[components[i]->Name()] = newconst.at(components[i-1]->Name()) + 1;
        }
      }
    }
};


// ENUM DECLARATION

class EnumDecl : public Program
{
  public:
    EnumDecl(std::string name, ProgramPtr list): identifier(name), vlist(list) {}
    ~EnumDecl()
    {
      delete vlist;
    }

    std::string Whatami() const
    {
      return "EnumDecl";
    }
    std::string Name() const
    {
      return identifier;
    }

    // Used to record enum constants - used in building parse tree.
    void addconst(std::map<std::string, int>& newconst, Context& c) const
    {
      EnumList* el = (EnumList*)vlist;
      el->addconst(newconst, c);
    }

    void MIPSPrint(Context& c, std::ostream& os, int sp_off, std::string destreg) const
    {
      os << std::endl;
    }
  private:
    std::string identifier;
    ProgramPtr vlist;
};


//TYPEDEF ENUM

class TypedefEnum : public Program
{
  public:
    TypedefEnum(ProgramPtr decl_enum, std::string id): decl_enum(decl_enum), identifier(id) {}
    ~TypedefEnum()
    {
      delete decl_enum;
    }

    std::string Whatami() const
    {
      return "TypedefEnum";
    }
    std::string Name() const
    {
      return identifier;
    }

    // Used to record enum constants - used in building parse tree.
    void addconst(std::map<std::string, int>& newconst, Context& c) const
    {
      EnumDecl* ed = (EnumDecl*)decl_enum;
      ed->addconst(newconst, c);
    }

    void MIPSPrint(Context& c, std::ostream& os, int sp_off, std::string destreg) const
    {
      c.userdefs[identifier] = "enum " + decl_enum->Name();
    }
  private:
    ProgramPtr decl_enum;
    std::string identifier;
};

#endif
