%{
// Avoid error "error: `fileno' was not declared in this scope"
extern "C" int fileno(FILE *stream);

//#include "compiler_parser.tab.hpp"
#include <iostream>
#include <cstdlib>
#include <string>
%}


%%



auto            { std::cout << "TYPE_AUTO" << std::endl; }
break           { std::cout << "KEYW_BREAK" << std::endl; }
case            { std::cout << "KEYW_CASE" << std::endl; }
char            { std::cout << "TYPE_CHAR" << std::endl; }

continue        { std::cout << "KEYW_CONTINUE" << std::endl; }
default         { std::cout << "KEYW_DEFAULT" << std::endl; }
do              { std::cout << "LOOP_DO" << std::endl; }
double          { std::cout << "TYPE_DOUBLE" << std::endl; }
else            {std::cout << " ELSE" << std::endl; }

extern          { std::cout << "KEYW_EXTERN" << std::endl; }
float           {std::cout << " TYPE_FLOAT" << std::endl; }
for             {std::cout << " LOOP_FOR" << std::endl; }

if              {std::cout << " IF" << std::endl; }
int             {std::cout << " TYPE_INT" << std::endl; }
long            {std::cout << " TYPE_LONG" << std::endl; }

return          {std::cout << "RETURN" << std::endl; }
short           {std::cout << " TYPE_SHORT" << std::endl; }
signed          {std::cout << " SIGNED" << std::endl; }
sizeof          {std::cout << " OP_SIZEOF" << std::endl; }
static          {std::cout << " STATIC" << std::endl; }
struct          {std::cout << " KEYW_STRUCT" << std::endl; }
switch          {std::cout << " KEYW_SWITCH" << std::endl; }
typedef         {std::cout << " TYPEDEF" << std::endl; }

unsigned        {std::cout << " UNSIGNED" << std::endl; }
void            {std::cout << " TYPE_VOID" << std::endl; }
volatile        {std::cout << " VOLATILE" << std::endl; }
while           {std::cout << " LOOP_WHILE" << std::endl; }


[a-zA-Z_][a-zA-Z0-9_]*  {std::cout << " IDENTIFIER" << std::endl; }

[0][0-9]+          { int k = strtoul(yytext, NULL, 8); std::cout << " CONST_INT :" << k << std::endl; }
[0-9]+          { int k = atoi(yytext); std::cout << " CONST_INT :" << k << std::endl; }
[0-9]+[u]       { int k = atoi(yytext); std::cout << " CONST_INT: " << k << std::endl; }
[0-9]+[U]       { int k = atoi(yytext); std::cout << " CONST_INT: " << k << std::endl; }
[0-9]+"ul"      { int k = atoi(yytext); std::cout << " CONST_INT: " << k << std::endl; }
[0-9]+"UL"      { int k = atoi(yytext); std::cout << " CONST_INT: " << k << std::endl; }
[0-9]+[l]       { int k = atoi(yytext); std::cout << " CONST_INT: " << k << std::endl; }
[0-9]+[L]       { int k = atoi(yytext); std::cout << " CONST_INT: " << k << std::endl; }
"0x"[A-Fa-f0-9]+ { int k = strtoul(yytext, NULL, 16); std::cout << " CONST_INT: " << k << std::endl; }

[0-9]+[.][0-9]+ { double d = strtod(yytext, 0); std::cout << " CONST_REAL: " << d << std::endl; }
[0-9]+("."[0-9]*)?"E"[-]?[0-9]+ { double d = strtod(yytext, 0); std::cout << " CONST_REAL: " << d << std::endl; }
[0-9]+("."[0-9]*)?"e"[-]?[0-9]+ { double d = strtod(yytext, 0); std::cout << " CONST_REAL: " << d << std::endl; }

[0-9]+[.][0-9]+[f] { float d = strtof(yytext, 0); std::cout << " CONST_FLOAT: " << d << std::endl; }
[0-9]+("."[0-9]*)?"E"[-]?[0-9]+[f] { float d = strtof(yytext, 0); std::cout << " CONST_FLOAT: " << d << std::endl; }
[0-9]+("."[0-9]*)?"e"[-]?[0-9]+[f] { float d = strtof(yytext, 0); std::cout << " CONST_FLOAT: " << d << std::endl; }

[0-9]+[.][0-9]+[F] { float d = strtof(yytext, 0); std::cout << " CONST_FLOAT: " << d << std::endl; }
[0-9]+("."[0-9]*)?"E"[-]?[0-9]+[F] { float d = strtof(yytext, 0); std::cout << " CONST_FLOAT: " << d << std::endl; }
[0-9]+("."[0-9]*)?"e"[-]?[0-9]+[F] { float d = strtof(yytext, 0); std::cout << " CONST_FLOAT: " << d << std::endl; }

\"(\\.|[^"\\])*\"          { std::string s = yytext;
                           /*std::string sout = "";
                           for(int i = 1; (i+1) < s.size(); i++){
                                char c = s[i];
                                if(c == '\"')
                                  { c = '\0';}
                                else if(c == '\\')
                                {
                                  char d = s[i+1];
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
                             }*/
                           std::cout << "STRING " << s << std::endl; }

\'(\\.|[^"\\])?\'      { char c = *(yytext + 1);
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
                        std::cout << " CHAR: " << c << std::endl; }

[/]             {std::cout << " OP_DIVIDE" << std::endl; }
[*]             {std::cout << " OP_ASTERISK" << std::endl; }
[+]             {std::cout << " OP_PLUS" << std::endl; }
[-]             {std::cout << " OP_MINUS" << std::endl; }
[%]             {std::cout << " OP_MODULUS" << std::endl; }
"++"            {std::cout << " OP_INCREMENT" << std::endl; }
"--"            {std::cout << " OP_DECREMENT" << std::endl; }

"=="            {std::cout << " OP_EQUALS" << std::endl; }
"!="            {std::cout << " OP_NOTEQUAL" << std::endl; }
[>]             {std::cout << " OP_GREATER" << std::endl; }
[<]             {std::cout << " OP_LESS" << std::endl; }
">="            {std::cout << " OP_GEQUAL" << std::endl; }
"<="            {std::cout << " OP_LEQUAL" << std::endl; }

"&&"            {std::cout << " OP_LOGIC_AND" << std::endl; }
"||"            {std::cout << " OP_LOGIC_OR" << std::endl; }
[!]             {std::cout << " OP_LOGIC_NOT" << std::endl; }

"<<"            {std::cout << " BIT_LSHIFT" << std::endl; }
">>"            {std::cout << " BIT_RSHIFT" << std::endl; }
[~]             {std::cout << " BIT_NOT" << std::endl; }
[&]             {std::cout << " AMPERSAND" << std::endl; }
[|]             {std::cout << " BIT_OR" << std::endl; }
[\^]            {std::cout << " BIT_XOR" << std::endl; }

[=]             {std::cout << " OP_ASSIGN" << std::endl; }
"+="            {std::cout << " OP_ADDASSIGN" << std::endl; }
"-="            {std::cout << " OP_SUBASSIGN" << std::endl; }
"*="            {std::cout << " OP_MULTASSIGN" << std::endl; }
"/="            {std::cout << " OP_DIVASSIGN" << std::endl; }
"%="            {std::cout << " OP_MODASSIGN" << std::endl; }
"<<="           {std::cout << " OP_LSASSIGN" << std::endl; }
">>="           {std::cout << " OP_RSASSIGN" << std::endl; }
"&="            {std::cout << " OP_BIT_ANDASSIGN" << std::endl; }
"|="            {std::cout << " OP_BIT_ORASSIGN" << std::endl; }
"^="            {std::cout << " OP_BIT_XORASSIGN" << std::endl; }

"?"            {std::cout << " OP_CONDITIONAL_1" << std::endl;}
":"            {std::cout << " OP_CONDITIONAL_2" << std::endl;}


[(]             {std::cout << " L_PARANTHESIS" << std::endl; }
[)]             {std::cout << " R_PARANTHESIS" << std::endl; }
[\[]            {std::cout << " L_BRACKET" << std::endl; }
[\]]            {std::cout << " R_BRACKET" << std::endl; }
[{]             {std::cout << " L_BRACE" << std::endl; }
[}]             {std::cout << " R_BRACE" << std::endl; }


"->"            { std::cout << "IND_MEMBER_REF" << std::endl; }
[\.]            { std::cout << "DIR_MEMBER_REF" << std::endl; }

[,]             { std::cout << "COMMA" << std::endl; }

[;]             { std::cout << "STATEMENT_END" << std::endl;}

[ \t\r\n]+		{;}

.               { fprintf(stderr, "Invalid token\n"); exit(1); }
%%

int yywrap(){}

int main(){
yylex();
return 0;
}

void yyerror (char const *s)
{
  fprintf (stderr, "Parse error : %s\n", s);
  exit(1);
}
