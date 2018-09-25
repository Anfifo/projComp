  %{
//-- don't change *any* of these: if you do, you'll break the compiler.
#include <cdk/compiler.h>
#include "ast/all.h"
#define LINE               compiler->scanner()->lineno()
#define yylex()            compiler->scanner()->scan()
#define yyerror(s)         compiler->scanner()->error(s)
#define YYPARSE_PARAM_TYPE std::shared_ptr<cdk::compiler>
#define YYPARSE_PARAM      compiler
//-- don't change *any* of these --- END!
%}

%union {
  int                   i;	/* integer value */
  std::string          *s;	/* symbol name or string literal */
  double                 d;
  cdk::basic_node      *node;	/* node pointer */
  cdk::sequence_node   *sequence;
  cdk::expression_node *expression; /* expression nodes */
  cdk::lvalue_node     *lvalue;
  basic_type           *t;
  gr8::block_node      *b;
  gr8::sweeping_node   *sw;
  gr8::null_node       *null;
};

%token <i> tINTEGER
%token <d> tDOUBLE

%token <null> tNULL
%token <s> tIDENTIFIER tSTRING tTYPE tPUBLIC tUSE
%token <t> tSMALL tHUGE tNEWS
%token <expression> tINPUT

%token tIF tAND tOR tINITIALLY tASSIGN tTO tSTOP tAGAIN tRETURN tTWEET tPOST tCELL tAT tOBJECTS
%token tTHEN tFROM tBY tSWEEP tDO tDEF tFOR
%token tFUNCTION tUSES tON tAS tDEFINE tPROCEDURE
%token tBLOCKEND tBLOCKBEGIN

%nonassoc tIFX
%nonassoc tELIF
%nonassoc tELSE

%left tAND tOR
%nonassoc '!'
%left '='
%left '<' '>'
%left '+' '-'
%left '*' '/' '%'
%nonassoc tUNARY


%type <node> var instr func decl cond
%type <b> block elfs
%type <sw> sweep
%type <s> qualif strs
%type <sequence> vars decls instrs args
%type <expression> expr call
%type <lvalue> lval

%type <t> type small huge fake


%{
//-- The rules below will be included in yyparse, the main parsing function.
%}
%%


file : decls {compiler->ast($1); }
     ;

decl : func                 { $$ = $1; }
     | var                  { $$ = $1; }
     ;


decls : decls decl         { $$ = new cdk::sequence_node(LINE, $2, $1); }
      | decl               { $$ = new cdk::sequence_node(LINE, $1); }
      ;

instrs : instrs instr              { $$ = new cdk::sequence_node(LINE, $2, $1); }
       | instr                     { $$ = new cdk::sequence_node(LINE, $1); }
       ;

block : tBLOCKBEGIN instrs tBLOCKEND           { $$ = new gr8::block_node(LINE,nullptr,$2); }
      | tBLOCKBEGIN decls tBLOCKEND            { $$ = new gr8::block_node(LINE,$2,nullptr); }
      | tBLOCKBEGIN decls instrs tBLOCKEND     { $$ = new gr8::block_node(LINE,$2,$3); }
      ;


func : type tFUNCTION tIDENTIFIER                        {$$ = new gr8::function_prototype_node(LINE, nullptr, $1, $3, nullptr);}
     | type tFUNCTION tIDENTIFIER tUSES vars             {$$ = new gr8::function_prototype_node(LINE, nullptr, $1, $3, $5);}
     | qualif type tFUNCTION tIDENTIFIER                 {$$ = new gr8::function_prototype_node(LINE, $1, $2, $4, nullptr);}
     | qualif type tFUNCTION tIDENTIFIER tUSES vars      {$$ = new gr8::function_prototype_node(LINE, $1, $2, $4, $6);}

     | tPROCEDURE tIDENTIFIER                            {$$ = new gr8::function_prototype_node(LINE, nullptr, new basic_type(), $2, nullptr);}
     | tPROCEDURE tIDENTIFIER tUSES vars                 {$$ = new gr8::function_prototype_node(LINE, nullptr, new basic_type(), $2, $4);}
     | qualif tPROCEDURE tIDENTIFIER                     {$$ = new gr8::function_prototype_node(LINE, $1, new basic_type(), $3, nullptr);}
     | qualif tPROCEDURE tIDENTIFIER tUSES vars          {$$ = new gr8::function_prototype_node(LINE, $1, new basic_type(), $3, $5);}

     | tDEFINE type tFUNCTION tIDENTIFIER tAS block                      {$$ = new gr8::function_node(LINE, nullptr, $2, $4, nullptr, $6);}
     | tDEFINE type tFUNCTION tIDENTIFIER tON vars tAS block             {$$ = new gr8::function_node(LINE, nullptr, $2, $4, $6, $8);}
     | tDEFINE qualif type tFUNCTION tIDENTIFIER tAS block               {$$ = new gr8::function_node(LINE, $2, $3, $5, nullptr, $7);}
     | tDEFINE qualif type tFUNCTION tIDENTIFIER tON vars tAS block      {$$ = new gr8::function_node(LINE, $2, $3, $5, $7, $9);}

     | tDEFINE tPROCEDURE tIDENTIFIER tAS block                          {$$ = new gr8::function_node(LINE, nullptr, new basic_type(), $3, nullptr, $5);}
     | tDEFINE tPROCEDURE tIDENTIFIER tON vars tAS block                 {$$ = new gr8::function_node(LINE, nullptr, new basic_type(), $3, $5, $7);}
     | tDEFINE qualif tPROCEDURE tIDENTIFIER tAS block                   {$$ = new gr8::function_node(LINE, $2, new basic_type(), $4, nullptr, $6);}
     | tDEFINE qualif tPROCEDURE tIDENTIFIER tON vars tAS block          {$$ = new gr8::function_node(LINE, $2, new basic_type(), $4, $6, $8);}
     ;

qualif : tPUBLIC                    { $$ = $1; }
       | tUSE                       { $$ = $1; }
       ;

call : tDO tIDENTIFIER              { $$ = new gr8::invocation_node(LINE, $2, nullptr); }
     | tUSE args tFOR tIDENTIFIER   { $$ = new gr8::invocation_node(LINE, $4, $2); }
     ;

args : expr	                        { $$ = new cdk::sequence_node(LINE, $1); }
 	   | args ',' expr                { $$ = new cdk::sequence_node(LINE, $3, $1); }
 	   ;

var : type tIDENTIFIER                                                  { $$ = new gr8::variable_node(LINE, nullptr, $1, $2, nullptr);}
    | qualif type tIDENTIFIER                                           { $$ = new gr8::variable_node(LINE, $1, $2, $3, nullptr);}
    | type tIDENTIFIER '(' tINITIALLY expr ')'     %prec tUNARY         { $$ = new gr8::variable_node(LINE, nullptr, $1, $2, $5);}
    | qualif type tIDENTIFIER '(' tINITIALLY expr ')'   %prec tUNARY    { $$ = new gr8::variable_node(LINE, $1, $2, $3, $6);}
    ;

small : 's'                           {  $$ = new basic_type(4, basic_type::TYPE_POINTER); }
      | 's' small                     {  $$ = new basic_type(4, basic_type::TYPE_POINTER); $$->_subtype = $2; }
      ;

huge  : 'h'                           { $$ = new basic_type(4, basic_type::TYPE_POINTER); }
      | 'h' huge                      { $$ = new basic_type(4, basic_type::TYPE_POINTER); $$->_subtype = $2; }
      ;

fake  : 'f'                           { $$ = new basic_type(4, basic_type::TYPE_POINTER); }
      | 'f' fake                      { $$ = new basic_type(4, basic_type::TYPE_POINTER); $$->_subtype = $2; }
      ;

type : 's'                        { $$ = new basic_type(4, basic_type::TYPE_INT); }
     | small 'f'                  { basic_type *t = $1;
                                    for(; t->subtype() != NULL; t = t->subtype());
                                    t->_subtype = new basic_type(4, basic_type::TYPE_INT);
                                    $$ = $1; }

     | 'h'                        { $$ = new basic_type(8, basic_type::TYPE_DOUBLE); }
     | huge 'f'                   { basic_type *t = $1;
                                    for(; t->subtype() != NULL; t = t->subtype());
                                    t->_subtype = new basic_type(8, basic_type::TYPE_DOUBLE);
                                    $$ = $1; }

     | 'n'                        { $$ = new basic_type(4, basic_type::TYPE_STRING); }
     | fake 'n'                   { basic_type *t = $1;
                                    for(; t->subtype() != NULL; t = t->subtype());
                                    t->_subtype = new basic_type(4, basic_type::TYPE_STRING);
                                    $$ = $1; }
     ;



instr : expr                                  { $$ = $1; }
      | tASSIGN expr tTO lval                 { $$ = new cdk::assignment_node(LINE, $4, $2); }
      | tSTOP                                 { $$ = new gr8::stop_node(LINE, 1); }
      | tAGAIN                                { $$ = new gr8::again_node(LINE, 1); }
      | tSTOP tINTEGER                        { $$ = new gr8::stop_node(LINE, $2); }
      | tAGAIN tINTEGER                       { $$ = new gr8::again_node(LINE, $2); }
      | tRETURN  expr                         { $$ = new gr8::return_node(LINE, $2); }
      | tTWEET expr                           { $$ = new gr8::print_node(LINE, $2, false); }
      | tPOST expr                            { $$ = new gr8::print_node(LINE, $2, true); }
      | cond                                  { $$ = $1; }
      | sweep                                 { $$ = $1; }
      ;

cond  : tIF expr tTHEN block  %prec tIFX      { $$ = new gr8::if_node(LINE, $2, $4); }
      | tIF expr tTHEN block elfs             { $$ = new gr8::if_else_node(LINE, $2, $4, $5); }
      ;

elfs   : tELSE block                            { $$ = $2 ;}
       | tELIF expr tTHEN block %prec tIFX      { gr8::if_node *i = new gr8::if_node(LINE, $2, $4);
                                                  cdk::sequence_node *s = new cdk::sequence_node(LINE, i);
                                                  $$ = new gr8::block_node(LINE, nullptr, s);}
       | tELIF expr tTHEN block elfs            { gr8::if_else_node *i = new gr8::if_else_node(LINE, $2, $4, $5);
                                                  cdk::sequence_node *s = new cdk::sequence_node(LINE, i);
                                                  $$ = new gr8::block_node(LINE, nullptr, s);}
       ;

sweep : tSWEEP lval tFROM expr tTO expr tDO block               { $$ = new gr8::sweeping_node(LINE, $2, $4, $6, new cdk::integer_node(LINE, 1), $8); }
      | tSWEEP lval tFROM expr tTO expr tBY expr tDO block      { $$ = new gr8::sweeping_node(LINE, $2, $4, $6, $8, $10); }
      ;


vars  : var	                       { $$ = new cdk::sequence_node(LINE, $1); }
	    | vars ',' var               { $$ = new cdk::sequence_node(LINE, $3, $1); }
	    ;

strs  : tSTRING                    { $$ = $1;}
      | strs tSTRING               { $$ = new std::string(*$1 + *$2);}
      ;

 expr : tINTEGER                         { $$ = new cdk::integer_node(LINE, $1); }
 	    | strs                             { $$ = new cdk::string_node(LINE, $1); }
      | tDOUBLE                          { $$ = new cdk::double_node(LINE, $1); }
      | tNULL                            { $$ = new gr8::null_node(LINE); }
      | '+' expr %prec tUNARY            { $$ = new gr8::identity_node(LINE, $2); }
      | '-' expr %prec tUNARY            { $$ = new cdk::neg_node(LINE, $2); }
      | expr '+' expr                    { $$ = new cdk::add_node(LINE, $1, $3); }
      | expr '-' expr                    { $$ = new cdk::sub_node(LINE, $1, $3); }
      | expr '*' expr	                   { $$ = new cdk::mul_node(LINE, $1, $3); }
      | expr '/' expr	                   { $$ = new cdk::div_node(LINE, $1, $3); }
      | expr '%' expr	                   { $$ = new cdk::mod_node(LINE, $1, $3); }
      | expr '<' expr	                   { $$ = new cdk::lt_node(LINE, $1, $3); }
      | expr '>' expr	                   { $$ = new cdk::gt_node(LINE, $1, $3); }
      | expr '=' expr                    { $$ = new cdk::eq_node(LINE, $1, $3); }
      | '!' expr                         { $$ = new cdk::not_node(LINE, $2); }
      | expr tAND expr                   { $$ = new cdk::and_node(LINE, $1, $3); }
      | expr tOR expr                    { $$ = new cdk::or_node(LINE, $1, $3); }
      | '(' expr ')'                     { $$ = $2; }
      | lval                             { $$ = new cdk::rvalue_node(LINE, $1); }  //FIXME
      | lval'?'                          { $$ = new gr8::ind_pos_node(LINE, $1); }
      | tINPUT                           { $$ = new gr8::read_node(LINE); }
      | expr tOBJECTS                    { $$ = new gr8::mem_alloc_node(LINE, $1); }
      | call                             { $$ = $1; }
      ;




lval : tIDENTIFIER                     { $$ = new cdk::identifier_node(LINE, $1); }
     | tCELL expr tAT lval             { cdk::rvalue_node *exp = new cdk::rvalue_node(LINE, $4); $$ = new gr8::indexation_node(LINE, exp, $2); }
     | tCELL expr tAT '(' expr ')'     { $$ = new gr8::indexation_node(LINE, $5, $2); }
     ;

%%
