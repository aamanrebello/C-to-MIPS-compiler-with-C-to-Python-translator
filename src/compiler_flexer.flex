%option noyywrap

%{
// Avoid error "error: `fileno' was not declared in this scope"
extern "C" int fileno(FILE *stream);

#include <cstdlib>
#include <string>
#include "ast.hpp"
#include "compiler_parser.tab.hpp"

%}

%%


auto            { yylval.word = new std::string("auto"); return TYPE_AUTO; }
break           { return KEYW_BREAK; }
case            { return KEYW_CASE; }
char            { yylval.word = new std::string("char"); return TYPE_CHAR; }

continue        { return KEYW_CONTINUE; }
default         { return KEYW_DEFAULT; }
do              { return LOOP_DO; }
double          { yylval.word = new std::string("double"); return TYPE_DOUBLE; }
else            { return ELSE; }

enum            { yylval.word = new std::string("enum"); return KEYW_ENUM; }
extern          { return KEYW_EXTERN; }
float           { yylval.word = new std::string("float"); return TYPE_FLOAT; }
for             { return LOOP_FOR; }

if              { return IF; }
int             { yylval.word = new std::string("int"); return TYPE_INT; }
long            { yylval.word = new std::string("long"); return TYPE_LONG; }

return          { return RETURN; }
short           { yylval.word = new std::string("short"); return TYPE_SHORT; }
signed          { yylval.word = new std::string("signed"); return SIGNED; }
sizeof          { return OP_SIZEOF; }
static          { yylval.word = new std::string("static"); return STATIC; }
struct          { yylval.word = new std::string("struct"); return KEYW_STRUCT; }
switch          { return KEYW_SWITCH; }
typedef         { return TYPEDEF; }

unsigned        { yylval.word = new std::string("unsigned"); return UNSIGNED; }
void            { yylval.word = new std::string("void"); return TYPE_VOID; }
volatile        { yylval.word = new std::string("volatile"); return VOLATILE; }
while           { return LOOP_WHILE; }


[a-zA-Z_][a-zA-Z0-9_]*  { std::string var = yytext; yylval.word = new std::string(var); return IDENTIFIER; }

[0][0-9]+       { yylval.integer = strtoul(yytext, NULL, 8); return CONST_INT; }
[0-9]+          { yylval.integer = atoi(yytext); return CONST_INT; }

[0-9]+[u]       { yylval.integer = atoi(yytext); return CONST_INT; }
[0-9]+[U]       { yylval.integer = atoi(yytext); return CONST_INT; }

[0-9]+"ul"      { yylval.integer = atoi(yytext); return CONST_INT; }
[0-9]+"UL"      { yylval.integer = atoi(yytext); return CONST_INT; }

[0-9]+[l]       { yylval.integer = atoi(yytext); return CONST_INT; }
[0-9]+[L]       { yylval.integer = atoi(yytext); return CONST_INT; }
"0x"[A-Fa-f0-9]+ { yylval.integer = strtoul(yytext, NULL, 16); return CONST_INT; }

[0-9]+[.][0-9]+ { yylval.reald = strtod(yytext, 0); return CONST_DOUBLE; }
[0-9]+("."[0-9]*)?"E"[-]?[0-9]+ { yylval.reald = strtod(yytext, 0); return CONST_DOUBLE; }
[0-9]+("."[0-9]*)?"e"[-]?[0-9]+ { yylval.reald = strtod(yytext, 0); return CONST_DOUBLE; }

[0-9]+[.][0-9]+[l] { yylval.reald = strtod(yytext, 0); return CONST_DOUBLE; }
[0-9]+("."[0-9]*)?"E"[-]?[0-9]+[l] { yylval.reald = strtod(yytext, 0); return CONST_DOUBLE; }
[0-9]+("."[0-9]*)?"e"[-]?[0-9]+[l] { yylval.reald = strtod(yytext, 0); return CONST_DOUBLE; }

[0-9]+[.][0-9]+[L] { yylval.reald = strtod(yytext, 0); return CONST_DOUBLE; }
[0-9]+("."[0-9]*)?"E"[-]?[0-9]+[L] { yylval.reald = strtod(yytext, 0); return CONST_DOUBLE; }
[0-9]+("."[0-9]*)?"e"[-]?[0-9]+[L] { yylval.reald = strtod(yytext, 0); return CONST_DOUBLE; }

[0-9]+[.][0-9]+[f] { yylval.realf = strtof(yytext, 0); return CONST_FLOAT; }
[0-9]+("."[0-9]*)?"E"[-]?[0-9]+[f] { yylval.realf = strtof(yytext, 0); return CONST_FLOAT; }
[0-9]+("."[0-9]*)?"e"[-]?[0-9]+[f] { yylval.realf = strtof(yytext, 0); return CONST_FLOAT; }

[0-9]+[.][0-9]+[F] { yylval.realf = strtof(yytext, 0); return CONST_FLOAT; }
[0-9]+("."[0-9]*)?"E"[-]?[0-9]+[F] { yylval.realf = strtof(yytext, 0); return CONST_FLOAT; }
[0-9]+("."[0-9]*)?"e"[-]?[0-9]+[F] { yylval.realf = strtof(yytext, 0); return CONST_FLOAT; }

\"(\\.|[^"\\])*\"        { std::string s = yytext;
                            s[s.size()-1] = '\\';
                            s = s + "000\"";
                             yylval.word = new std::string(s);
                             return STRING;}

\'(\\.|[^"\\])?\'       { char c = *(yytext + 1);
                          if(c == '\'')
                            { c = '\0';}
                          else if(c == '\\')
                          {
                            char d = *(yytext + 2);
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
                          yylval.character = c;
                          return CHAR; }


[/]             { return OP_DIVIDE; }
[*]             { yylval.word = new std::string("*"); return OP_ASTERISK; }
[+]             { return OP_PLUS; }
[-]             { return OP_MINUS; }
[%]             { return OP_MODULUS; }
"++"            { return OP_INCREMENT; }
"--"            { return OP_DECREMENT; }

"=="            { return OP_EQUALS; }
"!="            { return OP_NOTEQUAL; }
[>]             { return OP_GREATER; }
[<]             { return OP_LESS; }
">="            { return OP_GEQUAL; }
"<="            { return OP_LEQUAL; }

"&&"            { return OP_LOG_AND; }
"||"            { return OP_LOG_OR; }
[!]             { return OP_LOG_NOT; }

"<<"            { return BIT_LSHIFT; }
">>"            { return BIT_RSHIFT; }
[~]             { return BIT_NOT; }
[&]             { return AMPERSAND; }
[|]             { return BIT_OR; }
[\^]            { return BIT_XOR; }

[=]             { return OP_ASSIGN; }
"+="            { return OP_ADDASSIGN; }
"-="            { return OP_SUBASSIGN; }
"*="            { return OP_MULTASSIGN; }
"/="            { return OP_DIVASSIGN; }
"%="            { return OP_MODASSIGN; }
"<<="           { return OP_LSASSIGN; }
">>="           { return OP_RSASSIGN; }
"&="            { return OP_BIT_ANDASSIGN; }
"|="            { return OP_BIT_ORASSIGN; }
"^="            { return OP_BIT_XORASSIGN; }

"?"            { return OP_CONDITIONAL_1; }
":"            { return OP_CONDITIONAL_2; }

"->"            { return IND_MEMBER_REF; }
[\.]            { return DIR_MEMBER_REF;}


[(]             { return L_PARANTHESIS; }
[)]             { return R_PARANTHESIS; }
[\[]            { return L_BRACKET; }
[\]]            { return R_BRACKET; }
[{]             { return L_BRACE; }
[}]             { return R_BRACE; }

[,]             { return COMMA; }

[;]             { return STATEMENT_END;}

[ \t\r\n]+		  {;}

.               { fprintf(stderr, "Invalid token\n"); exit(1); }
%%

void yyerror (char const *s)
{
  fprintf (stderr, "Parse error : %s\n", s);
  exit(1);
}
