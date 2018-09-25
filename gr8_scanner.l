%option c++ prefix="gr8_scanner_" outfile="gr8_scanner.cpp"
%option stack noyywrap yylineno 8bit debug
%{
// make relevant includes before including the parser's tab file
#include <string>
#include <cdk/ast/sequence_node.h>
#include <cdk/ast/expression_node.h>
#include <cdk/ast/lvalue_node.h>
#include "gr8_parser.tab.h"
#include <vector>
#include <climits>
// don't change this
#define yyerror LexerError

std::vector<int> iStack (1,1);

%}

%x X_STRING X_COMMENT X_TILDE
%%

[ \n\t]*"!!".*$                   ; /* ignore line comments */

"..."[ \t\n]*                     ;/* allows continuation and miss identation*/

<INITIAL,X_COMMENT>[ \n\t]*"<<"   yy_push_state(X_COMMENT);
<X_COMMENT>">>"                   yy_pop_state();
<X_COMMENT>.|\n                   ;


"if"                   return tIF;
"then"                 return tTHEN;
"else"                 return tELSE;

"elsif"                return tELIF;

"null"                return tNULL;

"and"                 return tAND;
"or"                  return tOR;
"not"                 return '!';

"plus"                return '+';
"minus"               return '-';
"below"               return '<';
"above"               return '>';
"times"               return '*';
"over"                return '/';
"modulus"             return '%';
"equals"              return '=';
"input"               return tINPUT;

"assign"              return tASSIGN;
"to"                  return tTO;
"stop"                return tSTOP;
"again"               return tAGAIN;
"return"              return tRETURN;
"tweet"               return tTWEET;
"post"                return tPOST;
"sweeping"            return tSWEEP;
"from"                return tFROM;
"do"                  return tDO;
"by"                  return tBY;
"for"                 return tFOR;
"cell"                return tCELL;
"at"                  return tAT;
"objects"             return tOBJECTS;

"initially"           return tINITIALLY;

"public"              yylval.s = new std::string("public"); return tPUBLIC;
"use"                 yylval.s = new std::string("use"); return tUSE;

"small"               return 's';
"huge"                return 'h';
"news"                return 'n';
"fake"                return 'f';

"procedure"           return tPROCEDURE;
"function"            return tFUNCTION;
"uses"                return tUSES;
"on"                  return tON;
"as"                  return tAS;
"define"              return tDEFINE;


[A-Za-z][A-Za-z0-9_]*  yylval.s = new std::string(yytext); return tIDENTIFIER;


\"                     yy_push_state(X_STRING); yylval.s = new std::string("");
<X_STRING>\"           yy_pop_state(); return tSTRING;
<X_STRING>\\\"         *yylval.s += yytext + 1;
<X_STRING>"~"          yy_push_state(X_TILDE);
<X_STRING>.            *yylval.s += yytext;
<X_STRING>\n           yyerror("newline in string");

<X_TILDE>0                      *yylval.s += "\0"; yy_pop_state();
<X_TILDE>\"                     *yylval.s += "\""; yy_pop_state();
<X_TILDE>t                      *yylval.s += "\t"; yy_pop_state();
<X_TILDE>n                      *yylval.s += "\n"; yy_pop_state();
<X_TILDE>r                      *yylval.s += "\r"; yy_pop_state();
<X_TILDE>"~"                    *yylval.s += "~";  yy_pop_state();
<X_TILDE>.                      *yylval.s += yytext;  yy_pop_state();

<X_TILDE>[0-6]{1,3}             {
                                  *yylval.s += strtol(yytext, nullptr, 7);
                                  if(yylval.i >= INT_MAX) yyerror("overflow");
                                  yy_pop_state();
                                }






0[0-6]+                {yylval.i = strtol(yytext, nullptr, 7);  if(yylval.i >= INT_MAX) yyerror("small type overflow"); return tINTEGER;}
[0-9]+                 {yylval.i = strtol(yytext, nullptr, 10); if(yylval.i >= INT_MAX) yyerror("small type overflow"); return tINTEGER;}

[0-9]+\.?[0-9]*([Ee][+-]?[0-9]+)? yylval.d = strtod(yytext, nullptr); if(yylval.d >= LONG_MAX) yyerror("double type overflow"); return tDOUBLE;

[();{}.?,]             return *yytext;



[ \t]          ; /*ignore spaces that dont contribute for identation*/
.              yyerror("Unknown character");

\n[ \t]*$    ; /* ignore empty lines */

 /* treats indentation */
\n[ \t]*     {
  if (yyleng == iStack.back()){
    // Same block indent
  }else{
    if(yyleng > iStack.back()){
      iStack.push_back(yyleng);
      return tBLOCKBEGIN;
    }else{
      iStack.pop_back();
      if(yyleng > iStack.back())
        yyerror("Invalid Indentation");
      if(yyleng == iStack.back()){
        return tBLOCKEND;
      }else{
        yyless(0);
        return tBLOCKEND;
      }
    }
  }
};

 /* closes all identations */
<<EOF>>   {
  if(iStack.back() != 1){
    yyless(0);
    iStack.pop_back();
    return tBLOCKEND;
  }
  else
    return 0;
};


%%
