#ifndef ast_functions
#define ast_functions

// Defines classes for function declaration statements, function calls, returns, function bodies.
// Implementation in Function.cpp and FuncDeclCall.cpp in src/impl.

//FUNCTION DECLARATION STATEMENT

// All the functions (except for storepars) are virtual functions that are derived from Program class in ast_BASIC.hpp
class FunctionDecl : public Program
{
  public:
    FunctionDecl(std::string type, std::string id, ProgramPtr par): identifier(id), returntype(type),  parameters(par) {}
    FunctionDecl(std::string type, std::string id): identifier(id), returntype(type),  parameters(NULL) {}
    ~FunctionDecl();

    std::string Name() const;
    std::string Whatami() const;
    int compsize() const;

    void addparameter(std::map<std::string, int>& parlist, std::map<std::string, std::string>& partype, Context& c, int sp_off);
    // This function is used to allocate memory to store parameters
    void storepars(Context& c, std::ostream& os, int framesize) const;

    void PythonPrint(int tabs, std::map<std::string, std::string>& globals, std::ostream& os) const;
    void MIPSPrint(Context& c, std::ostream& os, int sp_off, std::string destreg) const;

  private:
    std::string identifier;
    std::string returntype;
    ProgramPtr parameters;
};

//ACTUAL FUNCTION BLOCK

// All the functions are virtual functions that are derived from Program class in ast_BASIC.hpp
class Function : public Program
{
  public:
    Function(ProgramPtr header, ProgramPtr body): function_signature(header), statements(body) {}
    ~Function();

    std::string Name() const;
    std::string Whatami() const;
    int findstrings(std::map<std::string, std::string>& slabel) const;

    void PythonPrint(int tabs, std::map<std::string, std::string>& globals, std::ostream& os) const;
    void MIPSPrint(Context& c, std::ostream& os, int sp_off, std::string destreg) const;
  private:
    ProgramPtr function_signature;
    ProgramPtr statements;
};

//FUNCTION CALL

// All the functions are virtual functions that are derived from Program class in ast_BASIC.hpp
class Fcall : public Program
{
  public:
    Fcall(std::string name, ProgramPtr parameterentries): identifier(name), parametervalues(parameterentries) {}
    Fcall(std::string name): identifier(name), parametervalues(NULL) {}
    ~Fcall();

    std::string Whatami() const;
    std::string MyType(Context& c) const;
    int maxparams(Context& c) const;
    int findstrings(std::map<std::string, std::string>& slabel) const;

    void PythonPrint(int tabs, std::map<std::string, std::string>& globals, std::ostream& os) const;
    void MIPSPrint(Context& c, std::ostream& os, int sp_off, std::string destreg) const;
  private:
    std::string identifier;
    ProgramPtr parametervalues;
};


//RETURN STATEMENT

// All the functions are virtual functions that are derived from Program class in ast_BASIC.hpp
class Return : public Program
{
  public:
    Return(ProgramPtr expr): expression(expr) {}
    ~Return();

    std::string Whatami() const;
    int maxparams(Context& c) const;
    int findstrings(std::map<std::string, std::string>& slabel) const;

    void PythonPrint(int tabs, std::map<std::string, std::string>& globals, std::ostream& os) const;
    void MIPSPrint(Context& c, std::ostream& os, int sp_off, std::string destreg) const;
  private:
    ProgramPtr expression;
    std::string returntype;
};

#endif
