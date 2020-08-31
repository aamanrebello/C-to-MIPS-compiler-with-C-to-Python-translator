#include "ast.hpp"

// Implementation of member functions defined in the parse tree node classes for
// function body (line 10) and return statement (line 110).

// For explanation of what each member function does, see the corresponding header file
// (Function.hpp) under the 'include' diectory.


//FUNCTION

Function::~Function()
{
    delete function_signature;
    delete statements;
}

std::string Function::Name() const
{
    return function_signature->Name();
}

std::string Function::Whatami() const
{
    return "Function";
}

int Function::findstrings(std::map<std::string, std::string>& slabel) const
{
  statements->findstrings(slabel);
  return 0;
}

void Function::PythonPrint(int tabs, std::map<std::string, std::string>& globals, std::ostream& os) const
{
    std::string alignment = "";
    for(int i = 0; i <= tabs; i++)
    {
        alignment += "\t";
    }
    function_signature->PythonPrint(tabs, globals, os);
    os << std::endl;
    std::map<std::string, std::string>::iterator it;
    for(it = globals.begin(); it != globals.end(); it++)
    {
        os << alignment << "global " << it->first << std::endl;
    }
    statements->PythonPrint((tabs + 1), globals, os);
}


void Function::MIPSPrint(Context& c, std::ostream& os, int sp_off, std::string destreg) const
{
    std::map<std::string, std::string> partype;
    std::map<std::string, int> parlist;
    std::map<std::string, int> oldlist;
    std::map<std::string, std::string> oldtype;
    // Size of stack frame we will allocate for function initially 32. If there are variable declarations
    // in the function scope we will increase the frame size accordingly.
    int framesize = 32;
    function_signature->MIPSPrint(c, os, sp_off, "def");
    FunctionDecl* fd = (FunctionDecl*)function_signature;
    fd->addparameter(parlist, partype, c, sp_off);
    if(!parlist.empty())
    {
      addmap<int>(parlist, c.variablelist, oldlist);
      addmap<std::string>(partype, c.vartype, oldtype);
    }
    if(statements != NULL)
    {
      framesize = framesize + statements->countdeclarations(c);
    }
    int n_extra_pars = statements->maxparams(c) - 4;
    c.vpframe = 24;
    if(n_extra_pars > 0)
    {
      c.vpframe += n_extra_pars*4;
      framesize += n_extra_pars*4;
    }
    // frame needs to be double word aligned
    if(framesize%8 != 0)
    {
      framesize = framesize + (8 - framesize%8);
    }
    c.framesize = framesize;
    std::string flabel = makeName("FUN");
    os << "addiu      $sp, $sp, -" << framesize << std::endl;
    os << "sw         $31, " << (framesize - 4) << "($sp)" << std::endl;
    os << "sw         $fp, " << (framesize - 8) << "($sp)" << std::endl;
    os << "move       $fp, $sp" << std::endl;
    fd->storepars(c, os, framesize);
    os << "sw         $16, " << c.vpframe - 4 << "($fp)" << std::endl;
    if(statements != NULL)
    { statements->MIPSPrint(c, os, (sp_off + framesize), ""); }
    os << flabel << ":" << std::endl;
    os << "lw         $16, 20($fp)" << std::endl;
    os << "move       $sp, $fp" << std::endl;
    os << "lw         $31, " << (framesize - 4) << "($sp)" << std::endl;
    os << "lw         $fp, "  << (framesize - 8) << "($sp)" << std::endl;
    os << "addiu      $sp, $sp, " << framesize << std::endl;
    os << "j          $31" << std::endl;
    os << "nop" << std::endl;
    if(!parlist.empty())
    {
      removemap<int>(parlist, c.variablelist);
      removemap<std::string>(partype, c.vartype);
      resetmap<int>(oldlist, c.variablelist);
      resetmap<std::string>(oldtype, c.vartype);
    }
    c.framesize = 0;
}

//RETURN

Return::~Return()
{
  delete expression;
}

std::string Return::Whatami() const
{
  return "Return";
}

int Return::maxparams(Context& c) const
{
  return expression->maxparams(c);
}

int Return::findstrings(std::map<std::string, std::string>& slabel) const
{
  return expression->findstrings(slabel);
}

void Return::PythonPrint(int tabs, std::map<std::string, std::string>& globals, std::ostream& os) const
{
  os << "return ";
  expression->PythonPrint(tabs, globals, os);
  os << std::endl;
}

void Return::MIPSPrint(Context& c, std::ostream& os, int sp_off, std::string destreg) const
{
  std::string type = reducetype(c.userdefs, expression->MyType(c));
  std::string iam = expression->Whatami();
  if((type == "float")||((type == "float *")&&(iam == "ArraySubscript"))||((type == "float *")&&(iam == "PointerDeref")))
  {
    expression->MIPSPrint(c, os, sp_off, "$f4");
    os << "mov.s      $f0, $f4" << std::endl;
  }
  else if((type == "double")||((type == "double *")&&(iam == "ArraySubscript"))||((type == "double *")&&(iam == "PointerDeref")))
  {
    expression->MIPSPrint(c, os, sp_off, "$f4");
    os << "mov.d      $f0, $f4" << std::endl;
  }
  else
  {
    expression->MIPSPrint(c, os, sp_off, "$8");
    os << "move       $2, $8" << std::endl;
  }
  os << "b          FUN" << makeNumber() - 1 << std::endl;
  os << "nop" << std::endl;
}
