#include "ast.hpp"

// Functions that are useful to all the tree node classes and hence need to be defined globally:
//      - void assignment(int, int, string, ostream&): line 19
//      - void gvassign(string, string, ostream&): line 59
//      - void stpushreg(string, ostream): line 104
//      - void stpopreg(string, ostream): line 119
//      - string stringconvert(string): line 133
//      - bool IsOperand(string): line 184
//      - bool isStdType(string): line 207
//      - string reducetype(string): line 246
//      - int StructSize(map, Sdecl&): line 289
//      - isEnum(string): line 348
//      - isStruct(string): line 362
//      - isLiteral(string): line 375

//-------------------------------------------------------------------------------------------------

// Used to perform assignment to a variable (memory slot on stack) based on the type
//  - int addr: required address of variable
//  - int sp_off: offset of stack pointer as compared to intial position before compilation
//  - std::string type: type of variable
//  - std::ostream& os: used to write output to file/terminal window.
void assignment(int addr, int sp_off, std::string type, std::ostream& os)
{
    if((type != "double")&&(type != "float"))
    {
      if((type == "char")||(type == "unsigned char")||(type == "signed char"))
      {
        os << "sb         $8, " << (sp_off - addr + 3) << "($fp)" << std::endl;
      }
      else if((type == "short")||(type == "unsigned short")||(type == "signed short"))
      {
        os << "sh         $8, " << (sp_off - addr + 2) << "($fp)" << std::endl;
      }
      else
      {
        os << "sw         $8, " << (sp_off - addr) << "($fp)" << std::endl;
      }
    }
    else
    {
      if(type == "float")
      {
        os << "swc1       $f4, " << (sp_off - addr) << "($fp)" << std::endl;
      }
      else if(type == "double")
      {
        os << "swc1       $f4, " << (sp_off - addr + 4) << "($fp)" << std::endl;
        os << "nop" << std::endl;
        os << "swc1       $f5, " << (sp_off - addr) << "($fp)" << std::endl;
      }
    }
    os << "nop" << std::endl;
}

//------------------------------------------------------------------------------------

// Used to perform assignment to global variable (needs pseudo MIPS assembly)
//  - std::string vname: variable name (identifier)
//  - std::string type: variable type
//  - std::ostream& os: output stream
void gvassign(std::string vname, std::string type, std::ostream& os)
{
  if((type != "double")&&(type != "float"))
  {
    os << "lui        $9, %hi(" << vname << ")" << std::endl;
    if((type == "char")||(type == "unsigned char")||(type == "signed char"))
    {
      os << "addiu      $9, $9, %lo(" << vname << ")" << std::endl;
      os << "sb         $8, 3($9)" << std::endl;
    }
    else if((type == "short")||(type == "unsigned short")||(type == "signed short"))
    {
      os << "addiu      $9, $9, %lo(" << vname << ")" << std::endl;
      os << "sh         $8, 2($9)" << std::endl;
    }
    else
    {
      os << "sw         $8, %lo(" << vname << ")($9)" << std::endl;
    }
    os << "nop" << std::endl;
  }
  else
  {
    if(type == "float")
    {
      os << "lui        $24, %hi(" << vname << ")" << std::endl;
      os << "swc1       $f4, %lo(" << vname << ")($24)" << std::endl;
      os << "nop" << std::endl;
    }
    else if(type == "double")
    {
      os << "lui        $24, %hi(" << vname << ")" << std::endl;
      os << "swc1       $f4, %lo(" << vname << "+4)($24)" << std::endl;
      os << "swc1       $f5, %lo(" << vname << ")($24)" << std::endl;
      os << "nop" << std::endl;
    }
  }
}

//-----------------------------------------------------------------------------------

// Used to temporarily store data on the stack to account for register overflow.
//  - std::string reg: register name
//  - std::ostream& os: output stream
void stpushreg(std::string reg, std::ostream& os)
{
  os << "addiu      $sp, $sp, -4" << std::endl;
  if(reg[1] == 'f')
  { os << "swc1       " << reg << ", 0($sp)" << std::endl; }
  else
  { os << "sw         " << reg << ", 0($sp)" << std::endl; }
  os << "nop" << std::endl;
}

//----------------------------------------------------------------------------------

// Used to reobtain temporarily stored values from the stack (antagonist to above function).
//  - std::string reg: register name
//  - std::ostream& os: output stream
void stpopreg(std::string reg, std::ostream& os)
{
  if(reg[1] == 'f')
  { os << "lwc1       " << reg << ", 0($sp)" << std::endl; }
  else
  { os << "lw         " << reg << ", 0($sp)" << std::endl; }
  os << "addiu      $sp, $sp, 4" << std::endl;
}

//-----------------------------------------------------------------------------------

// Used to convert a C-string to C++ std::string format so that our program can work
// with output tokens of Flex lexer (which works with C-strings).

std::string stringconvert(std::string inp)
{
  std::string sout = "";
  inp.pop_back();
  inp.pop_back();
  inp.pop_back();
  inp.pop_back();
  for(unsigned int i = 0; (i+1) < inp.size(); i++)
  {
         char c = inp[i];
         if(c == '\"')
           { c = '\0';}
         else if(c == '\\')
         {
           char d = inp[i+1];
           i++;
           if(d == 'a')
            { c = '\a'; }
           else if(d == 'b')
            { c = '\b'; }
           else if(d == 'f')
            { c = '\f'; }
           else if(d == 'n')
            { c = '\n'; }
           else if(d == 'r')
            { c = '\r'; }
           else if(d == 't')
            { c = '\t'; }
           else if(d == 'v')
            { c = '\v'; }
           else if(d == '\\')
            { c = '\\'; }
           else if(d == '\'')
            { c = '\''; }
           else if(d == '\"')
            { c = '\"'; }
           else if(d == '\?')
            { c = '\?'; }
           else if(d == '0')
            { c = '\0'; }
         }
         sout = sout + c;
    }
    return sout;
}

//-----------------------------------------------------------------------------------

// Based on the type of the class (provided as a string), determines if it can
// be used as an operand to standard C++ operators (e.g. +, -, * etc)

bool IsOperand(std::string p)
{
  if(p == "Variable")
  {return true;}
  else if(p == "IntLiteral")
  {return true;}
  else if(p == "CharLiteral")
  {return true;}
  else if(p == "RealLiteral")
  {return true;}
  else if(p == "StringLiteral")
  {return true;}
  else if(p == "ArraySubscript")
  {return true;}
  else
  {return false;}
}

//-----------------------------------------------------------------------------------

// Determines if a given type is a primitive C++ type (e.g. int) or not.

bool isStdType(std::string type)
{
  if((type == "int")||(type == "int *"))
  { return true; }
  if((type == "unsigned int")||(type == "unsigned int *"))
  { return true; }
  if((type == "signed int")||(type == "signed int *"))
  { return true; }
  if((type == "char")||(type == "char *"))
  { return true; }
  if((type == "unsigned char")||(type == "unsigned char *"))
  { return true; }
  if((type == "signed char")||(type == "signed char *"))
  { return true; }
  if((type == "short")||(type == "short *"))
  { return true; }
  if((type == "unsigned short")||(type == "unsigned short *"))
  { return true; }
  if((type == "signed short")||(type == "signed short *"))
  { return true; }
  if((type == "long")||(type == "long *"))
  { return true; }
  if((type == "unsigned long")||(type == "unsigned long *"))
  { return true; }
  if((type == "signed long")||(type == "signed long *"))
  { return true; }
  if((type == "float")||(type == "float *"))
  { return true; }
  if((type == "double")||(type == "double *"))
  { return true; }
  if(isStruct(type))
  { return true; }
  return false;
}

//----------------------------------------------------------------------------------

// Given a derived type (i.e. a typedef alias for a type name) deduces the primitive/
// non primitive type that it corresponds to.

// table is a map that maps typedef aliases to the type names that they mask.

std::string reducetype(std::map<std::string, std::string>& table, std::string type)
{
  int stars = 0;
  std::string t = type;
  std::string tred = type;
  if(tred.size() >= 2)
  { tred.erase(t.size() - 2, 2); }
  while(!isStdType(t))
  {
    if((isEnum(t))||(isEnum(tred)))
    {
      return "int";
    }
    if(table.find(t) != table.end())
    {
      t = table.at(t);
      tred = t;
      if(tred.size() >= 2)
      { tred.erase(t.size() - 2, 2); }
    }
    else if(table.find(tred) != table.end())
    {
      stars++;
      t = table.at(tred);
      tred = t;
      if(tred.size() >= 2)
      { tred.erase(t.size() - 2, 2); }
    }
    else
    { return ""; }
  }
  for(int l = 1; l <= stars; l++)
  { t += " *"; }
  return t;
}

//-----------------------------------------------------------------------------------

// Used to determine how much storage space is needed for a struct.

//  userdefs is used to reduce any user defined types to the basic types they represent

int StructSize(std::map<std::string, std::string>& userdefs, Sdecl& s)
{
  int size = 0;
  std::string type, nexttype;
  for(unsigned int i = 0; i < s.strvars.size(); i++)
  {
    type = reducetype(userdefs, s.types[i]);
    if(i != s.strvars.size() - 1)
    {
      nexttype = reducetype(userdefs, s.types[i+1]);
    }
    else
    {
      nexttype = "";
    }
    if((type == "char")||(type == "unsigned char")||(type == "signed char"))
    {
      if((nexttype == "char")||(nexttype == "unsigned char")||(nexttype == "signed char")||(nexttype == ""))
      { size += 1; }
      else if((nexttype == "short")||(nexttype == "unsigned short")||(nexttype == "signed short"))
      { size += 2 - (size%2); }
      else if(nexttype == "double")
      { size += 8 - (size%8); }
      else
      { size += 4 - (size%4); }
    }
    else if((type == "short")||(type == "unsigned short")||(type == "signed short"))
    {
      if((nexttype == "short")||(nexttype == "unsigned short")||(nexttype == "signed short")||(nexttype == ""))
      { size += 2; }
      else if((nexttype == "char")||(nexttype == "unsigned char")||(nexttype == "signed char"))
      { size += 2; }
      else if(nexttype == "double")
      { size += 8 - (size%8); }
      else
      { size += 4 - (size%4); }
    }
    else if(type == "double")
    {
      size += 8;
    }
    else
    {
      size += 4;
    }
  }
  if(size%4 != 0)
  {
    return size + (4 - size%4);
  }
  return size;
}

//---------------------------------------------------------------------------------

// Since this compiler specifies types as "enum int" or "enum char", we do string
// processing of the type to determine if it is enum or not.

bool isEnum(std::string type)
{
  if(type.substr(0, 4) == "enum")
  {
    return true;
  }
  return false;
}

//---------------------------------------------------------------------------------

// Same as above, but for struct

bool isStruct(std::string type)
{
  if(type.substr(0, 6) == "struct")
  {
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------

// Same as above, but for literals

bool isLiteral(std::string iam)
{
  if(iam.size() <= 7)
  {
    return false;
  }
  else
  {
    if(iam.substr(iam.size() - 7) == "Literal")
    {
      return true;
    }
  }
  return false;
}

//-----------------------------------------------END-------------------------------------------------------
