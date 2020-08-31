#include "ast.hpp"

// Implementation of member functions defined in the parse tree node classes for
// the sequence class (represents sequence of instructions within scope).

// For explanation of what each member function does, see the corresponding header file
// (Sequence.hpp) under the 'include' diectory.

int Sequence::setvpframe(int val)
{
  return val;
}


std::string Sequence::Whatami() const
{
  return "Sequence";
}

//HELP DETERMINE STACK ALLOCATION BY COUNTING VARIABLE DECLARATIONS

int Sequence::countdeclarations(Context& c) const
{
  int size = 0;
  if(components[0] == NULL)
  { return 0; }
  for(unsigned int i = 0; i < components.size(); i++)
  {
    std::string stmt = components[i]->Whatami();
    if((stmt == "VarDeclStmt")||(stmt == "ArrayDeclStmt")||(stmt == "DeclAssignmentStmt"))
    {
      int wsize = 4;
      std::string type = reducetype(c.userdefs, components[i]->MyType(c));
      if(type == "double")
      {   wsize = 8;  }
      else if((isStruct(type))&&(type[type.size()-1] != '*'))
      {
        std::string id = type.substr(7);
        wsize = StructSize(c.userdefs, (c.structs).at(id));
      }
      int decls = components[i]->compsize();
      size += wsize*decls;
    }
    else
    {
      size += components[i]->countdeclarations(c);
    }
  }
    return size;
}

int Sequence::maxparams(Context& c) const
{
  int max = 0;
  if(components[0] != NULL)
  {
    for(unsigned int i = 0; i < components.size(); i++)
    {
      int mp = components[i]->maxparams(c);
      if(mp > max)
      { max = mp; }
    }
  }
  return max;
}

int Sequence::findstrings(std::map<std::string, std::string>& slabel) const
{
  if(components[0] != NULL)
  {
    for(unsigned int i = 0; i < components.size(); i++)
    {
      std::string iam = components[i]->Whatami();
      if(iam == "Sequence")
      { components[i]->findstrings(slabel); }
      else if((iam == "While")||(iam == "DoWhile")||(iam == "For"))
      { components[i]->findstrings(slabel); }
      else if((iam == "IfElse")||(iam == "If")||(iam == "Switch"))
      { components[i]->findstrings(slabel); }
      else if((iam == "DeclAssignmentStmt")||(iam == "StrAssignment"))
      { components[i]->findstrings(slabel); }
      else if(iam == "Fcall")
      { components[i]->findstrings(slabel); }
    }
  }
  return 0;
}

void Sequence::PythonPrint(int tabs, std::map<std::string, std::string>& globals, std::ostream& os) const
{
  std::string alignment = "";
  for(int i = 0; i < tabs; i++)
  {
    alignment += "\t";
  }
  if(components[0] != NULL)
  {
    for(unsigned int i = 0; i < components.size(); i++)
    {
      os << alignment;
      components[i]->PythonPrint(tabs, globals, os);
      os << std::endl;
    }
  }
  else
  {
    os << alignment << "pass" << std::endl;
  }
}


void Sequence::MIPSPrint(Context& c, std::ostream& os, int sp_off, std::string destreg) const
{
  std::map<std::string, std::string> scopetype;
  std::map<std::string, int> scopelist;
  std::map<std::string, int> oldlist;
  std::map<std::string, std::string> oldtype;
  std::map<std::string, int> newconst;
  std::map<std::string, int> oldconst;
  std::vector<std::string> scopestructs;
  int vf = c.vpframe;
  int crlab = c.current_label;
  if(components[0] != NULL)
  {
    for(unsigned int i = 0; i < components.size(); i++)
    {
      std::string s = components[i]->Whatami();
      if((s == "VarDeclStmt")||(s == "DeclAssignmentStmt"))
      {
        std::string type = reducetype(c.userdefs, components[i]->MyType(c));
        if((isStruct(type))&&(type[type.size()-1] != '*'))
        {
          std::string id = type.substr(7);
          scopestructs.push_back(id);
        }
        Declaration* decl = (Declaration*)components[i];
        int inc = decl->allocvar(c, sp_off, vf, os);
        decl->addvariables(c, scopelist, scopetype, (sp_off - vf));
        addmap<int>(scopelist, c.variablelist, oldlist);
        addmap<std::string>(scopetype, c.vartype, oldtype);
        if(s == "DeclAssignmentStmt")
        {
          components[i]->MIPSPrint(c, os, sp_off, "");
        }
        vf = vf + inc;
        c.vpframe = vf;
      }
      else if(s == "EnumDecl")
      {
        EnumDecl* ed = (EnumDecl*)components[i];
        ed->addconst(newconst, c);
        addmap<int>(newconst, c.constants, oldconst);
      }
      else if(s == "TypedefEnum")
      {
        TypedefEnum* te = (TypedefEnum*)(components[i]);
        te->addconst(newconst, c);
        addmap<int>(newconst, c.constants, oldconst);
      }
      else
      {
        components[i]->MIPSPrint(c, os, sp_off, "");
        if((components[i]->Whatami() == "While")||(components[i]->Whatami() == "DoWhile")||(components[i]->Whatami() == "For"))
        { c.current_label = crlab; }
      }
    }
    removemap<int>(scopelist, c.variablelist);
    removemap<std::string>(scopetype, c.vartype);
    removemap<int>(newconst, c.constants);
    if(!oldlist.empty())
    {
      resetmap<int>(oldlist, c.variablelist);
      resetmap<std::string>(oldtype, c.vartype);
      resetmap<int>(oldconst, c.constants);
    }
    for(unsigned int k = 0; k < scopestructs.size(); k++)
    {
      c.variablelist.erase(scopestructs[k]);
      c.vartype.erase(scopestructs[k]);
    }
  }
}
