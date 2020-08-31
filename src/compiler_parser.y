%{
    #include "ast.hpp"

    #include <cstdlib>
    #include <cmath>
    #include <iostream>
    #include <string>

    #include <cassert>

    extern const Program *g_root;
    extern FILE *yyin;

    int yylex(void);
    void yyerror(const char*);

    int count = 0;
%}


%union{
  int integer;
  double reald;
  float realf;
  std::string *word;
  char character;
  Program *tree;
}

%token IDENTIFIER

%token KEYW_BREAK KEYW_CASE KEYW_CONTINUE KEYW_DEFAULT KEYW_ENUM KEYW_EXTERN KEYW_STRUCT KEYW_SWITCH
%token IF ELSE LOOP_DO LOOP_FOR LOOP_WHILE TYPEDEF OP_SIZEOF RETURN

%token TYPE_AUTO TYPE_CHAR TYPE_DOUBLE TYPE_FLOAT TYPE_INT TYPE_LONG TYPE_SHORT TYPE_VOID
%token STATIC SIGNED UNSIGNED VOLATILE
%token CONST_INT CONST_DOUBLE CONST_FLOAT STRING CHAR

%token OP_DIVIDE OP_ASTERISK OP_PLUS OP_MINUS OP_MODULUS
%token OP_INCREMENT OP_DECREMENT
%token OP_EQUALS OP_NOTEQUAL OP_GREATER OP_LESS OP_GEQUAL OP_LEQUAL
%token OP_LOG_AND OP_LOG_OR OP_LOG_NOT
%token BIT_LSHIFT BIT_RSHIFT BIT_NOT BIT_OR BIT_XOR
%token OP_ASSIGN OP_ADDASSIGN OP_SUBASSIGN OP_MULTASSIGN OP_DIVASSIGN OP_MODASSIGN OP_LSASSIGN OP_RSASSIGN
%token OP_BIT_ANDASSIGN OP_BIT_ORASSIGN OP_BIT_XORASSIGN
%token OP_CONDITIONAL_1 OP_CONDITIONAL_2

%token L_PARANTHESIS R_PARANTHESIS L_BRACKET R_BRACKET L_BRACE R_BRACE

%token AMPERSAND IND_MEMBER_REF DIR_MEMBER_REF

%token COMMA
%token STATEMENT_END


%type <integer> CONST_INT

%type <reald> CONST_DOUBLE

%type <realf> CONST_FLOAT

%type <word> DATATYPE MODIFIER TYPE_SPECIFY
%type <word> STRING IDENTIFIER
%type <word> TYPE_AUTO TYPE_CHAR TYPE_DOUBLE TYPE_FLOAT TYPE_INT TYPE_LONG TYPE_SHORT TYPE_VOID UNSIGNED SIGNED OP_ASTERISK KEYW_STRUCT KEYW_ENUM
%type <word> STATIC VOLATILE

%type <character> CHAR

%type <tree> LITERAL OPERAND
%type <tree> VARDECL USER_DEF_DECL
%type <tree> STRUCT STRUCT_LIST ENUM_DECL ENUM_LIST
%type <tree> INCDECREMENT
%type <tree> FUNCTION_CALL PARAMETER_ENTRIES PARAMETER_LIST FUNCTION_SIG FUNCTION
%type <tree> LHSASSIGNVALUE ASSIGNMENT DECL_ASSIGNMENT
%type <tree> FACTORL1 FACTORL2 TERM EXPRESSIONL1 EXPRESSIONL2 EXPRESSIONL3 LOGICALEXPL1 LOGICALEXPL2 TEREXP
%type <tree> WHILESTAT DOWHILESTAT FORSTAT IFELSESTAT SWITCHSTAT CASES
%type <tree> COMPOUND_STATEMENT SEQUENCE STATEMENT_GROUP ALLOWED_GLOBAL_STATEMENT LOCAL_STATEMENT
%type <tree> ARRAY_INIT_BLOCK
%type <tree> BLOCK PROGRAM ROOT

%nonassoc XIF
%nonassoc ELSE
%left COMMA
%right OP_ASSIGN OP_ADDASSIGN OP_SUBASSIGN OP_MULTASSIGN OP_DIVASSIGN OP_MODASSIGN OP_LSASSIGN OP_RSASSIGN OP_BIT_ANDASSIGN OP_BIT_ORASSIGN OP_BIT_XORASSIGN
%right OP_CONDITIONAL_1 OP_CONDITIONAL_2
%left OP_LOG_AND OP_LOG_OR
%left BIT_OR
%left BIT_XOR
%left AMPERSAND
%left OP_EQUALS OP_NOTEQUAL
%left OP_GREATER OP_GEQUAL OP_LESS OP_LEQUAL
%left BIT_LSHIFT BIT_RSHIFT
%left OP_PLUS OP_MINUS
%left OP_ASTERISK OP_DIVIDE OP_MODULUS
%nonassoc ID
%right PRE_INCREMENT PRE_DECREMENT UMINUS UPLUS OP_LOG_NOT BIT_NOT TYPECAST POINTER ADDRESS OP_SIZEOF
%left IND_MEMBER_REF DIR_MEMBER_REF OP_INCREMENT OP_DECREMENT

%start ROOT

%%

ROOT: PROGRAM {g_root = $1;}

PROGRAM: BLOCK { $$ = new Program($1);}
       | PROGRAM BLOCK { ($1)->addcomponent($2); $$ = $1;}

BLOCK: FUNCTION {$$ = $1;}
     | ALLOWED_GLOBAL_STATEMENT {$$ = $1;}

FUNCTION: FUNCTION_SIG SEQUENCE { if($2 != NULL)
                                  { $$ = new Function($1, $2); }
                                  else
                                  { $$ = new Function($1, NULL);}
                                }

FUNCTION_SIG: TYPE_SPECIFY IDENTIFIER L_PARANTHESIS PARAMETER_LIST R_PARANTHESIS { std::string store1 = *($1); std::string store2 = *($2); delete $1; delete $2; $$ = new FunctionDecl(store1, store2, $4);}
            | TYPE_SPECIFY IDENTIFIER L_PARANTHESIS R_PARANTHESIS { std::string store1 = *($1); std::string store2 = *($2); delete $1; delete $2; $$ = new FunctionDecl(store1, store2);}
            | IDENTIFIER IDENTIFIER L_PARANTHESIS PARAMETER_LIST R_PARANTHESIS { std::string store1 = *($1); std::string store2 = *($2); delete $1; delete $2; $$ = new FunctionDecl(store1, store2, $4);}
            | IDENTIFIER IDENTIFIER L_PARANTHESIS R_PARANTHESIS { std::string store1 = *($1); std::string store2 = *($2); delete $1; delete $2; $$ = new FunctionDecl(store1, store2);}

PARAMETER_LIST: DATATYPE IDENTIFIER {ProgramPtr var = new Variable(*($2)); ProgramPtr vardecl = new VarDeclStmt(*($1), var); $$ = new ParameterList(vardecl);}
              | IDENTIFIER IDENTIFIER {ProgramPtr var = new Variable(*($2)); ProgramPtr vardecl = new VarDeclStmt(*($1), var); $$ = new ParameterList(vardecl);}
              | PARAMETER_LIST COMMA DATATYPE IDENTIFIER {ProgramPtr var = new Variable(*($4)); ProgramPtr vardecl = new VarDeclStmt(*($3), var); ($1)->addcomponent(vardecl); $$ = $1;}
              | PARAMETER_LIST COMMA IDENTIFIER IDENTIFIER {ProgramPtr var = new Variable(*($4)); ProgramPtr vardecl = new VarDeclStmt(*($3), var); ($1)->addcomponent(vardecl); $$ = $1;}

WHILESTAT: LOOP_WHILE L_PARANTHESIS TEREXP R_PARANTHESIS COMPOUND_STATEMENT { if($5 != NULL)
                                                                              { $$ = new While($3, $5); }
                                                                              else
                                                                              { $$ = new While($3, NULL);}
                                                                            }

DOWHILESTAT: LOOP_DO COMPOUND_STATEMENT LOOP_WHILE L_PARANTHESIS TEREXP R_PARANTHESIS { if($2 != NULL)
                                                                                        { $$ = new DoWhile($5, $2); }
                                                                                        else
                                                                                        { $$ = new DoWhile($5, NULL);}
                                                                                      }

FORSTAT: LOOP_FOR L_PARANTHESIS ALLOWED_GLOBAL_STATEMENT TEREXP STATEMENT_END INCDECREMENT R_PARANTHESIS COMPOUND_STATEMENT { if($8 != NULL)
                                                                                                                              { $$ = new For($3, $4, $6, $8); }
                                                                                                                              else
                                                                                                                              { $$ = new For($3, $4, $6, NULL);}
                                                                                                                            }
       | LOOP_FOR L_PARANTHESIS ALLOWED_GLOBAL_STATEMENT TEREXP STATEMENT_END ASSIGNMENT R_PARANTHESIS COMPOUND_STATEMENT { if($8 != NULL)
                                                                                                                            { $$ = new For($3, $4, $6, $8); }
                                                                                                                            else
                                                                                                                            { $$ = new For($3, $4, $6, NULL);}
                                                                                                                          }
       | LOOP_FOR L_PARANTHESIS ALLOWED_GLOBAL_STATEMENT TEREXP STATEMENT_END R_PARANTHESIS COMPOUND_STATEMENT {  if($7 != NULL)
                                                                                                                  { $$ = new For($3, $4, $7); }
                                                                                                                  else
                                                                                                                  { $$ = new For($3, $4, NULL); }
                                                                                                               }
       | LOOP_FOR L_PARANTHESIS LOCAL_STATEMENT TEREXP STATEMENT_END INCDECREMENT R_PARANTHESIS COMPOUND_STATEMENT { if($8 != NULL)
                                                                                                                    { $$ = new For($3, $4, $6, $8); }
                                                                                                                    else
                                                                                                                    { $$ = new For($3, $4, $6, NULL);}
                                                                                                                    }
       | LOOP_FOR L_PARANTHESIS LOCAL_STATEMENT TEREXP STATEMENT_END ASSIGNMENT R_PARANTHESIS COMPOUND_STATEMENT {  if($8 != NULL)
                                                                                                                    { $$ = new For($3, $4, $6, $8); }
                                                                                                                    else
                                                                                                                    { $$ = new For($3, $4, $6, NULL);}
                                                                                                                  }
       | LOOP_FOR L_PARANTHESIS LOCAL_STATEMENT TEREXP STATEMENT_END R_PARANTHESIS COMPOUND_STATEMENT {   if($7 != NULL)
                                                                                                          { $$ = new For($3, $4, $7); }
                                                                                                          else
                                                                                                          { $$ = new For($3, $4, NULL); }
                                                                                                      }

IFELSESTAT: IF L_PARANTHESIS TEREXP R_PARANTHESIS COMPOUND_STATEMENT %prec XIF {  if($5 != NULL)
                                                                                  { $$ = new If($3, $5);  }
                                                                                  else
                                                                                  { $$ = new If($3, NULL); }
                                                                               }
          | IF L_PARANTHESIS TEREXP R_PARANTHESIS COMPOUND_STATEMENT ELSE COMPOUND_STATEMENT {  ProgramPtr elsest = NULL, ifst = NULL;
                                                                                                if($7 != NULL)
                                                                                                { elsest = new Else($7); }
                                                                                                else
                                                                                                { elsest = new Else(NULL); }
                                                                                                if($5 != NULL)
                                                                                                { ifst = new If($3, $5); }
                                                                                                else
                                                                                                { ifst = new If($3, NULL); }
                                                                                                 $$ = new IfElse(ifst, elsest);
                                                                                              }

SWITCHSTAT: KEYW_SWITCH L_PARANTHESIS TEREXP R_PARANTHESIS L_BRACE CASES R_BRACE {$$ = new Switch($3, $6);}

CASES: KEYW_CASE LITERAL OP_CONDITIONAL_2 STATEMENT_GROUP { ProgramPtr newcase = new SingleCase($2, $4); $$ = new Cases(newcase);}
     | KEYW_DEFAULT OP_CONDITIONAL_2 STATEMENT_GROUP {ProgramPtr newcase = new SingleCase($3); $$ = new Cases(newcase);}
     | CASES KEYW_CASE LITERAL OP_CONDITIONAL_2 STATEMENT_GROUP { ProgramPtr newcase = new SingleCase($3, $5); ($1)->addcomponent(newcase); $$ = $1;}
     | CASES KEYW_DEFAULT OP_CONDITIONAL_2 STATEMENT_GROUP { ProgramPtr newcase = new SingleCase($4); ($1)->addcomponent(newcase); $$ = $1;}

COMPOUND_STATEMENT: ALLOWED_GLOBAL_STATEMENT {$$ = $1;}
                  | LOCAL_STATEMENT {$$ = $1;}

SEQUENCE: L_BRACE STATEMENT_GROUP R_BRACE { $$ = $2;}
        | L_BRACE R_BRACE { $$ = new Sequence(NULL); }

STATEMENT_GROUP: LOCAL_STATEMENT {$$ = new Sequence($1);}
               | ALLOWED_GLOBAL_STATEMENT {$$ = new Sequence($1);}
               | STATEMENT_GROUP LOCAL_STATEMENT {($1)->addcomponent($2); $$ = $1;}
               | STATEMENT_GROUP ALLOWED_GLOBAL_STATEMENT {($1)->addcomponent($2); $$ = $1;}

ALLOWED_GLOBAL_STATEMENT: STATEMENT_END { $$ = new NullStatement();}
                        | DECL_ASSIGNMENT STATEMENT_END { $$ = $1;}
                        | VARDECL STATEMENT_END { $$ = $1;}
                        | USER_DEF_DECL STATEMENT_END { $$ = $1;}
                        | FUNCTION_SIG STATEMENT_END { $$ = $1;}
                        | TYPEDEF DATATYPE IDENTIFIER STATEMENT_END { std::string store2 = *($2); std::string store3 = *($3); delete $3; delete $2; $$ = new Typedef(store2, store3);}
                        | TYPEDEF IDENTIFIER IDENTIFIER STATEMENT_END { std::string store2 = *($2); std::string store3 = *($3); delete $3; delete $2; $$ = new Typedef(store2, store3);}
                        | TYPEDEF KEYW_STRUCT L_BRACE STRUCT_LIST R_BRACE IDENTIFIER STATEMENT_END { std::string store = *($6); delete $6;
                                                                                                     std::string name = "typedef" + std::to_string(count++);
                                                                                                     ProgramPtr p = new Struct(name, $4);
                                                                                                     $$ = new TypedefStruct(p, store);}
                        | TYPEDEF STRUCT IDENTIFIER STATEMENT_END { std::string store = *($3); delete $3; $$ = new TypedefStruct($2, store);}
                        | TYPEDEF KEYW_ENUM L_BRACE ENUM_LIST R_BRACE IDENTIFIER STATEMENT_END {  std::string store = *($6); delete $6;
                                                                                                  std::string name = "typedef" + std::to_string(count++);
                                                                                                  ProgramPtr p = new EnumDecl(name, $4);
                                                                                                  $$ = new TypedefEnum(p, store); }
                        | STRUCT STATEMENT_END { $$ = $1;}
                        | ENUM_DECL STATEMENT_END { $$ = $1; }

LOCAL_STATEMENT: ASSIGNMENT STATEMENT_END { $$ = $1;}
                | INCDECREMENT STATEMENT_END { $$ = $1;}
                | RETURN TEREXP STATEMENT_END { $$ = new Return($2);}
                | FUNCTION_CALL STATEMENT_END { $$ = $1;}
                | KEYW_BREAK STATEMENT_END { $$ = new Break();}
                | KEYW_CONTINUE STATEMENT_END { $$ = new Continue();}
                | IFELSESTAT {$$ = $1;}
                | WHILESTAT {$$ = $1;}
                | DOWHILESTAT STATEMENT_END {$$ = $1;}
                | FORSTAT {$$ = $1;}
                | SWITCHSTAT {$$ = $1;}
                | SEQUENCE {$$ = $1;}

DECL_ASSIGNMENT: TYPE_SPECIFY IDENTIFIER OP_ASSIGN TEREXP { std::string store1 = *($1); std::string store2 = *($2); delete $1; delete $2; ProgramPtr var = new Variable(store2); ProgramPtr assignme = new Assignment(var, $4); $$ = new DeclAssignmentStmt(store1, assignme);}
               | IDENTIFIER IDENTIFIER OP_ASSIGN TEREXP { std::string store1 = *($1); std::string store2 = *($2); delete $1; delete $2; ProgramPtr var = new Variable(store2); ProgramPtr assignme = new Assignment(var, $4); $$ = new DeclAssignmentStmt(store1, assignme);}
               | TYPE_SPECIFY IDENTIFIER L_BRACKET CONST_INT R_BRACKET OP_ASSIGN ARRAY_INIT_BLOCK { std::string store1 = *($1); std::string store2 = *($2); delete $1; delete $2; ProgramPtr arrdef = new ArrayDefinition(store2, $4); ProgramPtr assignme = new ArrayAssignment(arrdef, $7); $$ = new DeclAssignmentStmt(store1, assignme);}
               | IDENTIFIER IDENTIFIER L_BRACKET CONST_INT R_BRACKET OP_ASSIGN ARRAY_INIT_BLOCK { std::string store1 = *($1); std::string store2 = *($2); delete $1; delete $2; ProgramPtr arrdef = new ArrayDefinition(store2, $4); ProgramPtr assignme = new ArrayAssignment(arrdef, $7); $$ = new DeclAssignmentStmt(store1, assignme);}
               | DECL_ASSIGNMENT COMMA IDENTIFIER OP_ASSIGN TEREXP { std::string store = *($3); delete $3; ProgramPtr var = new Variable(store); ProgramPtr assignme = new Assignment(var, $5); ($1)->addcomponent(assignme); $$ = $1;}
               | DECL_ASSIGNMENT COMMA IDENTIFIER L_BRACKET CONST_INT R_BRACKET OP_ASSIGN ARRAY_INIT_BLOCK { std::string store = *($3); delete $3; ProgramPtr arrdef = new ArrayDefinition(store, $5); ProgramPtr assignme = new ArrayAssignment(arrdef, $8); ($1)->addcomponent(assignme); $$ = $1;}

ASSIGNMENT: LHSASSIGNVALUE OP_ASSIGN TEREXP { $$ = new Assignment($1, $3);}
          | LHSASSIGNVALUE OP_ADDASSIGN TEREXP { $$ = new AddAssignment($1, $3);}
          | LHSASSIGNVALUE OP_SUBASSIGN TEREXP { $$ = new SubAssignment($1, $3);}
          | LHSASSIGNVALUE OP_MULTASSIGN TEREXP { $$ = new MultAssignment($1, $3);}
          | LHSASSIGNVALUE OP_DIVASSIGN TEREXP { $$ = new DivAssignment($1, $3); }
          | LHSASSIGNVALUE OP_MODASSIGN TEREXP { $$ = new ModAssignment($1, $3); }
          | LHSASSIGNVALUE OP_LSASSIGN TEREXP { $$ = new LShiftAssignment($1, $3);}
          | LHSASSIGNVALUE OP_RSASSIGN TEREXP { $$ = new RShiftAssignment($1, $3);}
          | LHSASSIGNVALUE OP_BIT_ANDASSIGN TEREXP { $$ = new BitAndAssignment($1, $3);}
          | LHSASSIGNVALUE OP_BIT_ORASSIGN TEREXP { $$ = new BitOrAssignment($1, $3);}
          | LHSASSIGNVALUE OP_BIT_XORASSIGN TEREXP { $$ = new BitXorAssignment($1, $3);}

LHSASSIGNVALUE: IDENTIFIER { std::string store = *($1); delete $1; $$ = new Variable(store);}
              | IDENTIFIER L_BRACKET TEREXP R_BRACKET { std::string store = *($1); delete $1; $$ = new ArraySubscript(store, $3);}
              | AMPERSAND LHSASSIGNVALUE %prec ADDRESS { $$ = new PointerAddr($2);}
              | OP_ASTERISK LHSASSIGNVALUE { $$ = new PointerDeref($2);}
              | LHSASSIGNVALUE IND_MEMBER_REF IDENTIFIER { ProgramPtr member = new Variable(*($3)); delete $3; $$ = new IndMemberRef($1, member);}
              | LHSASSIGNVALUE DIR_MEMBER_REF IDENTIFIER { ProgramPtr member = new Variable(*($3)); delete $3; $$ = new DirMemberRef($1, member);}

ARRAY_INIT_BLOCK: L_BRACE PARAMETER_ENTRIES R_BRACE { $$ = $2;}

FUNCTION_CALL: IDENTIFIER L_PARANTHESIS PARAMETER_ENTRIES R_PARANTHESIS { std::string store = *($1); delete $1; $$ = new Fcall(store, $3);}
             | IDENTIFIER L_PARANTHESIS R_PARANTHESIS { std::string store = *($1); delete $1; $$ = new Fcall(store);}

PARAMETER_ENTRIES: TEREXP {$$ = new ParameterEntries($1);}
                 | PARAMETER_ENTRIES COMMA TEREXP {($1)->addcomponent($3); $$ = $1;}

TEREXP: LOGICALEXPL2 { $$ = $1;}
      | TEREXP OP_CONDITIONAL_1 TEREXP OP_CONDITIONAL_2 TEREXP { $$ = new ternaryif($1, $3, $5);}

LOGICALEXPL2: LOGICALEXPL1 { $$ = $1;}
            | LOGICALEXPL2 OP_LOG_AND LOGICALEXPL2 { $$ = new LogicAnd($1, $3);}
            | LOGICALEXPL2 OP_LOG_OR LOGICALEXPL2 { $$ = new LogicOr($1, $3);}

LOGICALEXPL1: EXPRESSIONL3 { $$ = $1;}
            | LOGICALEXPL1 OP_GREATER LOGICALEXPL1 { $$ = new Greater($1, $3);}
            | LOGICALEXPL1 OP_GEQUAL LOGICALEXPL1 { $$ = new GreaterEqual($1, $3);}
            | LOGICALEXPL1 OP_LESS LOGICALEXPL1 { $$ = new Less($1, $3);}
            | LOGICALEXPL1 OP_LEQUAL LOGICALEXPL1 { $$ = new LessEqual($1, $3);}
            | LOGICALEXPL1 OP_EQUALS LOGICALEXPL1 { $$ = new Equal($1, $3);}
            | LOGICALEXPL1 OP_NOTEQUAL LOGICALEXPL1 { $$ = new NotEqual($1, $3);}

EXPRESSIONL3: EXPRESSIONL2 { $$ = $1;}
            | EXPRESSIONL3 AMPERSAND EXPRESSIONL3 { $$ = new BitwiseAnd($1, $3);}
            | EXPRESSIONL3 BIT_XOR EXPRESSIONL3 { $$ = new BitwiseXor($1, $3);}
            | EXPRESSIONL3 BIT_OR EXPRESSIONL3 { $$ = new BitwiseOr($1, $3);}

EXPRESSIONL2: EXPRESSIONL1 { $$ = $1;}
            | EXPRESSIONL2 BIT_LSHIFT EXPRESSIONL2 {  $$ = new BitwiseLShift($1, $3);}
            | EXPRESSIONL2 BIT_RSHIFT EXPRESSIONL2 {  $$ = new BitwiseRShift($1, $3);}

EXPRESSIONL1: TERM { $$ = $1;}
            | EXPRESSIONL1 OP_PLUS EXPRESSIONL1 { $$ = new Plus($1, $3);}
            | EXPRESSIONL1 OP_MINUS EXPRESSIONL1 { $$ = new Minus($1, $3);}

TERM: FACTORL2 { $$ = $1;}
    | TERM OP_ASTERISK TERM { $$ = new Multiply($1, $3);}
    | TERM OP_DIVIDE TERM { $$ = new Divide($1, $3);}
    | TERM OP_MODULUS TERM { $$ = new Modulus($1, $3);}

FACTORL2: FACTORL1 { $$ = $1;}
        | OP_MINUS FACTORL1 %prec UMINUS { $$ = new UnaryMinus($2); }
        | OP_PLUS FACTORL1 %prec UPLUS { $$ = new UnaryPlus($2);}
        | OP_ASTERISK FACTORL2 %prec POINTER { $$ = new PointerDeref($2); }
        | AMPERSAND FACTORL2 %prec ADDRESS { $$ = new PointerAddr($2);}
        | OP_LOG_NOT FACTORL2 { $$ = new LogicalNot($2);}
        | BIT_NOT FACTORL2 { $$ = new BitwiseNot($2);}
        | OP_SIZEOF L_PARANTHESIS DATATYPE R_PARANTHESIS { std::string store = *($3); delete $3; $$ = new Sizeof(store);}
        | OP_SIZEOF L_PARANTHESIS TEREXP R_PARANTHESIS { $$ = new Sizeof($3); }
        | L_PARANTHESIS DATATYPE R_PARANTHESIS FACTORL2 %prec TYPECAST { std::string store = *($2); delete $2; $$ = new TypeCast($4, store);}

FACTORL1: OPERAND { $$ = $1;}
        | FACTORL1 IND_MEMBER_REF IDENTIFIER { std::string store = *($3); ProgramPtr member = new Variable(store); delete $3; $$ = new IndMemberRef($1, member);}
        | FACTORL1 DIR_MEMBER_REF IDENTIFIER { std::string store = *($3); ProgramPtr member = new Variable(store); delete $3; $$ = new DirMemberRef($1, member);}
        | INCDECREMENT { $$ = $1;}

INCDECREMENT: OPERAND OP_INCREMENT { $$ = new Increment($1, true);}
            | OPERAND OP_DECREMENT { $$ = new Decrement($1, true);}
            | OP_INCREMENT FACTORL1 %prec PRE_INCREMENT { $$ = new Increment($2, false);}
            | OP_DECREMENT FACTORL1 %prec PRE_DECREMENT { $$ = new Decrement($2, false);}

OPERAND: IDENTIFIER %prec ID { std::string store = *($1); $$ = new Variable(store);}
       | IDENTIFIER L_BRACKET TEREXP R_BRACKET %prec ID { std::string store = *($1); delete $1; $$ = new ArraySubscript(store, $3);}
       | LITERAL { $$ = $1;}
       | FUNCTION_CALL { $$ = $1;}
       | L_PARANTHESIS TEREXP R_PARANTHESIS { $$ = new BracketedTerm($2);}

LITERAL: CONST_INT { $$ = new IntLiteral($1);}
       | CONST_DOUBLE { $$ = new DoubleLiteral($1);}
       | CONST_FLOAT {$$ = new FloatLiteral($1);}
       | STRING { std::string store = *($1); delete $1; $$ = new StringLiteral(store);}
       | CHAR { $$ = new CharLiteral($1);}

STRUCT: KEYW_STRUCT IDENTIFIER L_BRACE STRUCT_LIST R_BRACE { std::string store = *($2); delete $2; $$ = new Struct(store, $4);}

ENUM_DECL: KEYW_ENUM IDENTIFIER L_BRACE ENUM_LIST R_BRACE { std::string store = *($2); delete $2; $$ = new EnumDecl(store, $4); }

STRUCT_LIST: VARDECL STATEMENT_END { $$ = new StructList($1);}
           | USER_DEF_DECL STATEMENT_END { $$ = new StructList($1);}
           | STRUCT_LIST VARDECL STATEMENT_END {($1)->addcomponent($2); $$ = $1;}
           | STRUCT_LIST USER_DEF_DECL STATEMENT_END {($1)->addcomponent($2); $$ = $1;}

ENUM_LIST: IDENTIFIER { std::string store = *($1); delete $1; ProgramPtr p = new Variable(store); $$ = new EnumList(p); }
         | ASSIGNMENT { $$ = new EnumList($1); }
         | ENUM_LIST COMMA IDENTIFIER { std::string store = *($3); delete $3; ProgramPtr p = new Variable(store); ($1)->addcomponent(p); $$ = $1; }
         | ENUM_LIST COMMA ASSIGNMENT { ($1)->addcomponent($3); $$ = $1; }

USER_DEF_DECL: IDENTIFIER IDENTIFIER { std::string store = *($1); ProgramPtr var = new Variable(*($2)); delete $1; delete $2;  $$ = new VarDeclStmt(store, var);}
             | IDENTIFIER IDENTIFIER L_BRACKET CONST_INT R_BRACKET { std::string store = *($1);  ProgramPtr arrdef = new ArrayDefinition(*($2), $4); delete $1; delete $2; $$ = new VarDeclStmt(store, arrdef);}
             | USER_DEF_DECL COMMA IDENTIFIER { ProgramPtr var = new Variable(*($3)); delete $3; ($1)->addcomponent(var); $$ = $1;}
             | USER_DEF_DECL COMMA IDENTIFIER L_BRACKET CONST_INT R_BRACKET { ProgramPtr arrdef = new ArrayDefinition(*($3), $5); delete $3; ($1)->addcomponent(arrdef); $$ = $1;}

VARDECL: TYPE_SPECIFY IDENTIFIER { std::string store = *($1); ProgramPtr var = new Variable(*($2)); delete $1; delete $2; $$ = new VarDeclStmt(store, var);}
         | TYPE_SPECIFY IDENTIFIER L_BRACKET CONST_INT R_BRACKET { std::string store = *($1); ProgramPtr arrdef = new ArrayDefinition(*($2), $4); delete $1; delete $2; $$ = new VarDeclStmt(store, arrdef);}
         | VARDECL COMMA IDENTIFIER {  ProgramPtr var = new Variable(*($3)); delete $3; ($1)->addcomponent(var); $$ = $1;}
         | VARDECL COMMA IDENTIFIER  L_BRACKET CONST_INT R_BRACKET { ProgramPtr arrdef = new ArrayDefinition(*($3), $5); delete $3; ($1)->addcomponent(arrdef); $$ = $1;}

TYPE_SPECIFY: DATATYPE {std::string store = *($1); delete $1; $$ = new std::string(store);}
            | DATATYPE MODIFIER {std::string store1 = *($1); delete $1; $$ = new std::string(store1);}
            | MODIFIER DATATYPE {std::string store2 = *($2); delete $2; $$ = new std::string(store2);}

MODIFIER: STATIC {;}
        | VOLATILE {;}

DATATYPE: TYPE_AUTO { std::string store = *($1); delete $1; $$ = new std::string(store);}
        | TYPE_CHAR { std::string store = *($1); delete $1; $$ = new std::string(store);}
        | UNSIGNED TYPE_CHAR { std::string store1 = *($1); std::string store2 = *($2); delete $1; delete $2; $$ = new std::string(store1 + " " + store2);}
        | SIGNED TYPE_CHAR { std::string store1 = *($1); std::string store2 = *($2); delete $1; delete $2; $$ = new std::string(store1 + " " + store2);}
        | TYPE_DOUBLE { std::string store = *($1); delete $1; $$ = new std::string(store);}
        | TYPE_FLOAT { std::string store = *($1); delete $1; $$ = new std::string(store);}
        | TYPE_INT { std::string store = *($1); delete $1; $$ = new std::string(store);}
        | UNSIGNED { std::string store = *($1); delete $1; $$ = new std::string(store + " int");}
        | SIGNED { std::string store = *($1); delete $1; $$ = new std::string(store + " int");}
        | UNSIGNED TYPE_INT { std::string store1 = *($1); std::string store2 = *($2); delete $1; delete $2; $$ = new std::string(store1 + " " + store2);}
        | SIGNED TYPE_INT { std::string store1 = *($1); std::string store2 = *($2); delete $1; delete $2; $$ = new std::string(store1 + " " + store2);}
        | TYPE_LONG { std::string store = *($1); delete $1; $$ = new std::string(store);}
        | UNSIGNED TYPE_LONG { std::string store1 = *($1); std::string store2 = *($2); delete $1; delete $2; $$ = new std::string(store1 + " " + store2);}
        | SIGNED TYPE_LONG { std::string store1 = *($1); std::string store2 = *($2); delete $1; delete $2; $$ = new std::string(store1 + " " + store2);}
        | TYPE_SHORT { std::string store = *($1); delete $1; $$ = new std::string(store);}
        | UNSIGNED TYPE_SHORT { std::string store1 = *($1); std::string store2 = *($2); delete $1; delete $2; $$ = new std::string(store1 + " " + store2);}
        | SIGNED TYPE_SHORT { std::string store1 = *($1); std::string store2 = *($2); delete $1; delete $2; $$ = new std::string(store1 + " " + store2);}
        | TYPE_VOID { std::string store = *($1); delete $1; $$ = new std::string(store);}
        | KEYW_STRUCT IDENTIFIER { std::string store1 = *($1); std::string store2 = *($2); delete $1; delete $2; $$ = new std::string(store1 + " " + store2);}
        | KEYW_ENUM IDENTIFIER { std::string store1 = *($1); std::string store2 = *($2); delete $1; delete $2; $$ = new std::string(store1 + " " + store2);}
        | IDENTIFIER OP_ASTERISK %prec POINTER { std::string store1 = *($1); std::string store2 = *($2); delete $1; delete $2; $$ = new std::string(store1 + " " + store2);}
        | DATATYPE OP_ASTERISK %prec POINTER {  std::string store1 = *($1); std::string store2 = *($2); delete $1; delete $2; $$ = new std::string(store1 + " " + store2);}

%%

const Program *g_root;

const Program* parseAST()
{
 g_root = 0;
 yyparse();
 return g_root;
}

void yyerror(char *s)
{
 fprintf(stdout, "\n%s", s);
}
