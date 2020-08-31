#include "ast.hpp"

// Implementation of member functions defined in the parse tree node classes for
// the program class (represents whole program).

// For explanation of what each member function does, see the corresponding header file
// (Program.hpp) under the 'include' diectory.

Program::Program(ProgramPtr p)
{
  components.push_back(p);
}

Program::~Program()
{
  for(unsigned int i = 0; i < components.size(); i++)
  {
    delete components[i];
  }
}

std::string Program::Whatami() const
{
  return "Program";
}

std::string Program::Name() const
{
  // return nothing as this function is only useful to find identifiers of variables/functions.
  return "";
}

std::string Program::MyType(Context& c) const
{
  // return nothing as this function is only useful to find types of variables/functions.
  return "";
}

void Program::PythonPrint(int tabs, std::map<std::string, std::string>& globals, std::ostream& os) const
{
  for(unsigned int i = 0; i < components.size(); i++)
  {
    if(components[i]->Whatami() != "FunctionDecl")
    {
      components[i]->PythonPrint(tabs, globals, os);
    }
    os << std::endl;
  }
  for(unsigned int i = 0; i < components.size(); i++)
  {
    std::string s = components[i]->Whatami();
    if(s == "Function")
    {
      if(components[i]->Name() == "main")
      {
        // This python code is necessary to replicate a 'main' function like in C++.
        os << std::endl << std::endl;
        os << "if __name__ == \"__main__\":" << std::endl;
        os << "\timport sys" << std::endl;
        os << "\tret=main()" << std::endl;
        os << "\tsys.exit(ret)" << std::endl;
      }
    }
  }
}


void Program::MIPSPrint(Context& c, std::ostream& os, int sp_off, std::string destreg) const
{
  bool dataset = false;
  c.current_label = -1;
  c.framesize = 0;
  // First set up GCC pseudo-assembly that precedes actual MIPS so that we can declare
  // global variables and functions.
  os << "           .text" << std::endl;
  ////////////////////////////////////////////////////////
  for(unsigned int i = 0; i < components.size(); i++)
  {
    std::string iam = components[i]->Whatami();
    if((iam == "VarDeclStmt")||(iam == "DeclAssignmentStmt"))
    {
      if(!dataset)
      { os << "           .data" << std::endl; dataset = true;}
      std::map<std::string, std::string> gtype;
      std::map<std::string, int> glist;
      std::map<std::string, int> st1;
      std::map<std::string, std::string> st2;
      Declaration* dl = (Declaration*)(components[i]);
      dl->addvariables(c, glist, gtype, -1000);
      addmap<int>(glist, c.variablelist, st1);
      addmap<std::string>(gtype, c.vartype, st2);
    }
    else if(iam == "Function")
    {
      dataset = false;
      components[i]->findstrings(c.strlabels);
      std::map<std::string, std::string>::iterator it;
      if(!(c.strlabels).empty())
      { os <<"            .rdata" << std::endl; }
      for(it = (c.strlabels).begin(); it != (c.strlabels).end(); it++)
      {
        os <<"            .align 2" << std::endl;
        os << it->second << ":" << std::endl;
        os <<"            .ascii  " << it->first << std::endl;
      }
    }
    else if(iam == "EnumDecl")
    {
      std::map<std::string, int> econst;
      std::map<std::string, int> st1;
      EnumDecl* ed = (EnumDecl*)(components[i]);
      ed->addconst(econst, c);
      addmap<int>(econst, c.constants, st1);
    }
    else if(iam == "TypedefEnum")
    {
      std::map<std::string, int> econst;
      std::map<std::string, int> st1;
      TypedefEnum* te = (TypedefEnum*)(components[i]);
      te->addconst(econst, c);
      addmap<int>(econst, c.constants, st1);
    }
    components[i]->MIPSPrint(c, os, sp_off, "global");
    os << std::endl;
  }
  ////////////////////////////////////////////////////////////
  if(( !c.float_labels.empty() )||( !c.dbl_labels.empty() ))
  {
    os << "           .rdata" << std::endl;
  }
  std::map<std::string, dblrepresent>::iterator it;
  for(it = c.dbl_labels.begin(); it != c.dbl_labels.end(); it++)
  {
    os << "           .align 3" << std::endl;
    os << it->first << ":" << std::endl;
    os << "           .word " << (it->second).upper << std::endl;
    os << "           .word " << (it->second).lower << std::endl;
  }
  std::map<std::string, unsigned int>::iterator iu;
  for(iu = c.float_labels.begin(); iu != c.float_labels.end(); iu++)
  {
    os << "           .align 2" << std::endl;
    os << iu->first << ":" << std::endl;
    os << "           .word " << iu->second << std::endl;
  }
}

int Program::findstrings(std::map<std::string, std::string>& slabel) const
{
  return 0;
}

int Program::maxparams(Context& c) const
{
  return -1;
}

int Program::countdeclarations(Context& c) const
{
  return 0;
}

int Program::compsize() const
{
  return components.size();
}

void Program::addcomponent(ProgramPtr p)
{
  components.push_back(p);
}

int Program::intvalue(Context& c) const
{
  return 0;
}

double Program::dblvalue(Context& c) const
{
  return 0.0;
}
