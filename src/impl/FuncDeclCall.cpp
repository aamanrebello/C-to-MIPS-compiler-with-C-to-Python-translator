#include "ast.hpp"

// Implementation of member functions defined in the parse tree node classes for
// function declarations (line 10) and function call statements (line 84) .

// For explanation of what each member function does, see the corresponding header file
// (FuncDeclCall.hpp) under the 'include' diectory.


//FUNCTION DECL (Function declaration)

FunctionDecl::~FunctionDecl()
{
  delete parameters;
}

std::string FunctionDecl::Name() const
{
  // return name of function
  return identifier;
}

std::string FunctionDecl::Whatami() const
{
  // return what kind of class this is
  return "FunctionDecl";
}

int FunctionDecl::compsize() const
{
  // return how many parameters there are
  if(parameters != NULL)
  { return parameters->compsize(); }
  else
  { return 0; }
}

void FunctionDecl::storepars(Context& c, std::ostream& os, int framesize) const
{
  if(parameters != NULL)
  {
    ParameterList* pl = (ParameterList*)parameters;
    return pl->storepars(c, os, framesize);
  }
}

void FunctionDecl::addparameter(std::map<std::string, int>& parlist, std::map<std::string, std::string>& partype, Context& c, int sp_off)
{
  if(parameters != NULL)
  {
    ParameterList* pl = (ParameterList*)parameters;
    pl->addparameter(parlist, partype, c, sp_off);
  }
}


void FunctionDecl::PythonPrint(int tabs, std::map<std::string, std::string>& globals, std::ostream& os) const
{
  os << "def " << identifier << "(";
  if(parameters != NULL)
  {
    parameters->PythonPrint(tabs, globals, os);
  }
  os << "):";
}

void FunctionDecl::MIPSPrint(Context& c, std::ostream& os, int sp_off, std::string destreg) const
{
  // pseudo code used by GCC compiler for function declaration
  if(destreg == "def")
  {
    os << "           .text" << std::endl;
    os << "           .align 2" << std::endl;
    os << "           .globl " << identifier << std::endl;
    os << "           .type " << identifier << ", @function" << std::endl;
    os << identifier << ":" << std::endl;
    os << "           .set noreorder" << std::endl;
    os << "           .set nomacro" << std::endl;
  }
  c.functionlist[identifier] = returntype;
}


//FUNCTION CALL


Fcall::~Fcall()
{
  delete parametervalues;
}

std::string Fcall::Whatami() const
{
  return "Fcall";
}

std::string Fcall::MyType(Context& c) const
{
  // return return type of function
  if(c.functionlist[identifier] != "void")
  { return reducetype(c.userdefs, c.functionlist[identifier]); }
  else
  { return "void"; }
}

int Fcall::maxparams(Context& c) const
{
  if(parametervalues != NULL)
  { ParameterEntries* pe = (ParameterEntries*)parametervalues;
    return pe->allocsize(c); }
  else
  { return 0; }
}

int Fcall::findstrings(std::map<std::string, std::string>& slabel) const
{
  if(parametervalues != NULL)
  { return parametervalues->findstrings(slabel); }
  return 0;
}

void Fcall::PythonPrint(int tabs, std::map<std::string, std::string>& globals, std::ostream& os) const
{
  os << identifier << "(";
  if(parametervalues != NULL)
  {
    parametervalues->PythonPrint(tabs, globals, os);
  }
  os << ")";
}

void Fcall::MIPSPrint(Context& c, std::ostream& os, int sp_off, std::string destreg) const
{
  if(parametervalues != NULL)
  { parametervalues->MIPSPrint(c, os, sp_off, "$4"); }
  os << "jal        " << identifier << std::endl;
  os << "nop" << std::endl;
  if(MyType(c) != "void")
  {
    if(MyType(c) == "double")
    { os << "mov.d        " << destreg << ", $f0" << std::endl; }
    else if(MyType(c) == "float")
    { os << "mov.s        " << destreg << ", $f0" << std::endl; }
    else
    { os << "move       " << destreg << ", $2" << std::endl; }
  }
}
