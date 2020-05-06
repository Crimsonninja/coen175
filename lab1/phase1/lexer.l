%{
/*
 * File:	lexer.l
 *
 * Description:	This file contains the flex description for the lexical
 *		analyzer for Simple C.
 *
 */

# include <cerrno>
# include <cstdlib>
# include <iostream>
# include "string.h"
//# include "lexer.h"

using namespace std;

void printToken(string);

%}

%option nounput noyywrap yylineno
%%
"auto"    {printToken("keyword");}
"break"    {printToken("keyword");}
"case"    {printToken("keyword");}
"char"    {printToken("keyword");}
"const"    {printToken("keyword");}
"continue"    {printToken("keyword");}
"default"   {printToken("keyword");}
"double"    {printToken("keyword");}
"do"    {printToken("keyword");}
"else"    {printToken("keyword");}
"enum"    {printToken("keyword");}
"extern"    {printToken("keyword");}
"float"    {printToken("keyword");}
"for"    {printToken("keyword");}
"goto"    {printToken("keyword");}
"if"    {printToken("keyword");}
"int"    {printToken("keyword");}
"long"    {printToken("keyword");}
"register"    {printToken("keyword");}
"return"    {printToken("keyword");}
"short"    {printToken("keyword");}
"signed"    {printToken("keyword");}
"sizeof"    {printToken("keyword");}
"static"    {printToken("keyword");}
"struct"    {printToken("keyword");}
"switch"    {printToken("keyword");}
"typedef"    {printToken("keyword");}
"union"    {printToken("keyword");}
"unsigned"    {printToken("keyword");}
"void"    {printToken("keyword");}
"volatile"    {printToken("keyword");}
"while"    {printToken("keyword");}

"="    {printToken("operator");}
"||"    {printToken("operator");}
"&&"    {printToken("operator");}
"|"    {printToken("operator");}
"=="    {printToken("operator");}
"!="    {printToken("operator");}
"<"    {printToken("operator");}
">"    {printToken("operator");}
"<="    {printToken("operator");}
">="    {printToken("operator");}
"+"    {printToken("operator");}
"-"    {printToken("operator");}
"*"    {printToken("operator");}
"/"    {printToken("operator");}
"%"    {printToken("operator");}
"&"    {printToken("operator");}
"!"    {printToken("operator");}
"++"    {printToken("operator");}
"--"    {printToken("operator");}
"->"    {printToken("operator");}
"("    {printToken("operator");}
")"    {printToken("operator");}
"["    {printToken("operator");}
"]"    {printToken("operator");}
"{"    {printToken("operator");}
"}"    {printToken("operator");}
";"    {printToken("operator");}
":"    {printToken("operator");}
"."    {printToken("operator");}
","    {printToken("operator");}
"..."    {printToken("operator");}

[ \t\n]+    {/*do nothing */}
[a-zA-Z_][a-zA-Z0-9_]*      {printToken("identifier");}
[0-9]+    {printToken("integer");}
[0-9]+\.[0-9]+([eE][+-]?[0-9]+)?  {printToken("real");}

\"([^"\\\n]|\\.)*\"     {printToken("string");}
\'([^'\\\n]|\\.)+\'     {printToken("character");}

"/*"    { char old_char = 0;
          char new_char = yyinput();
          while(new_char!=0) {
              if (old_char=='*' && new_char=='/') {
                  break;
              }
              old_char = new_char;
              new_char = yyinput();
          }
        }

. {/* ignores everything*/}
%%

int main()
{
    yylex();
    return 0;
}

void printToken(string x) {
    cout << x << " " << yytext << "\n";
}
