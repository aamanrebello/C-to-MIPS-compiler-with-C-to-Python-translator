#ifndef ast_hpp
#define ast_hpp

// Top level header file that includes all necessary class definitions (from other header files)
// and global variable and function definitions for the compilation process.

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <fstream>

// This union is used to convert a floating point value to its binary representation expressed as
// an integer. This is needed to declare float constants.
union fpconv{
    float fValue;
    unsigned int iValue;
};

// Same as fpconv above, but for double type.
union dbConv {
    double dValue;
    uint64_t INTValue;
};

// Stores the inforamtion of the struct variable declarations and their order in vectors.
struct Sdecl
{
  std::vector<std::string> strvars;
  std::vector<std::string> types;
};

// integer values representing least and most significant 32 bits of double binary representation
struct dblrepresent
{
  unsigned int lower;
  unsigned int upper;
};

// Contextual information that needs to be remembered as we iterate over the parse tree.
struct Context
{
  // Maps variable to address i.e offset from initial position of stack pointer.
  std::map<std::string, int> variablelist;
  // Maps variables to types
  std::map<std::string, std::string> vartype;
  // Maps all declared functions to return types
  std::map<std::string, std::string> functionlist;
  // Maps all typedefs - alias to type
  std::map<std::string, std::string> userdefs;
  // Pseudocode labels to access heap storage of globally declared strings.
  std::map<std::string, std::string> strlabels;
  // Labels mapped to string size
  std::map<std::string, int> assign_str;
  // Pseudocode labels to access heap storage of globally declared constants mapped to their size
  std::map<std::string, int> constants;
  // Keeping track of all struct declarations
  std::map<std::string, Sdecl> structs;
  // Pseudocode labels to access heap storage of float type constants.
  std::map<std::string, unsigned int> float_labels;
  // Pseudocode labels to access heap storage of double type constants.
  std::map<std::string, dblrepresent> dbl_labels;
  // mapping all declared arrys to their size
  std::map<std::string, int> arrays;
  // Number indicating how much of the current stack frame has already been allocated to variables.
  int vpframe;
  // We allocate labels as L1, L2 etc for loops etc so we need to keep track of the integer value after L.
  int current_label;
  // Size of the current stack frame.
  int framesize;
};

// This variable is used to add numbers ahead of function labels (labelled as FUN0, FUN1 etc)
static int makeNameUnq=0;

// Used to generate the labels e.g. FUN0, FUN1
static std::string makeName(std::string base)
{
    return base + std::to_string(makeNameUnq++);
}

// returns current value of makeNameUnq
static int makeNumber()
{
  return makeNameUnq;
}

// General function used to append a given map to a current map. Useful to update the maps defined above in Context
// struct when we enter a new scope.
template<typename T>
void addmap(std::map<std::string, T>& addme, std::map<std::string, T>& addto, std::map<std::string, T>& oldv)
{
  typename std::map<std::string, T>::iterator it;
  for(it = addme.begin(); it != addme.end(); it++)
  {
    if(addto.find(it->first) != addto.end())
    {oldv[it->first] = addto.at(it->first);}
    addto[it->first] = it->second;
  }
}

// General function used to remove map from a larger map - useful when we leave scope.
template<typename Typ>
void removemap(std::map<std::string, Typ>& removeme, std::map<std::string, Typ>& removefrom)
{
  typename std::map<std::string, Typ>::iterator it;
  for(it = removeme.begin(); it != removeme.end(); it++)
  {
    if(removefrom.find(it->first) != removefrom.end())
    {
      removefrom.erase(it->first);
    }
  }
}

// Used to restore values in variable map to their old values
template<typename Type>
void resetmap(std::map<std::string, Type>& oldv, std::map<std::string, Type>& vtable)
{
  typename std::map<std::string, Type>::iterator it;
  for(it = oldv.begin(); it != oldv.end(); it++)
  {
    vtable[it->first] = it->second;
  }
}

// Helper functions tha are defined and explained in src/helperfunctions.cpp
void assignment(int addr, int sp_off, std::string type, std::ostream& os);
void gvassign(std::string vname, std::string type, std::ostream& os);
void stpushreg(std::string reg, std::ostream& os);
void stpopreg(std::string reg, std::ostream& os);
bool IsOperand(std::string p);
bool isStdType(std::string type);
bool isEnum(std::string type);
bool isStruct(std::string type);
bool isLiteral(std::string iam);
std::string stringconvert(std::string inp);
std::string reducetype(std::map<std::string, std::string>& table, std::string type);
int StructSize(std::map<std::string, std::string>& userdefs, Sdecl& s);

// Header file inclusions:

#include "ast/ast_BASIC.hpp"
#include "ast/ast_array.hpp"
#include "ast/ast_operands.hpp"
#include "ast/ast_member_ref.hpp"
#include "ast/ast_unaryop.hpp"
#include "ast/ast_incdecrement.hpp"
#include "ast/ast_struct.hpp"

//BINARY OPERATORS
#include "ast/ast_binaryop.hpp"
#include "ast/ast_logic.hpp"
#include "ast/ast_comparison.hpp"
#include "ast/ast_bitwise.hpp"
#include "ast/ast_arithmetic.hpp"

#include "ast/ast_enum.hpp"
#include "ast/ast_declarations.hpp"
#include "ast/ast_functions.hpp"
#include "ast/ast_parameters.hpp"

//CONTROL FLOW
#include "ast/ast_ifelse.hpp"
#include "ast/ast_loop.hpp"
#include "ast/ast_switch.hpp"


extern const Program* parseAST();

#endif
