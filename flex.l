%{
#include "parser.tab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
%}

%option noyywrap
%x COMMENT_STATE

DIGIT   [0-9]
LETTER  [a-zA-Z_]
ID      {LETTER}({LETTER}|{DIGIT})*
INT     {DIGIT}+
FLOAT   {DIGIT}+"."{DIGIT}+

%%

"integer"       { return INTEGER; }
"decimal"       { return DECIMAL; }
"symbol"        { return SYMBOL; }
"logical"       { return LOGICAL; }
"nothing"       { return NOTHING; }

"function"      { return FUNCTION; }
"returns"       { return RETURNS; }

"when"          { return WHEN; }
"is"            { return IS; }
"otherwise"     { return OTHERWISE; }

"SAFE"          { return SAFE; }

"from"          { return FROM; }
"to"            { return TO; }
"in"            { return IN; }

"bound"         { return BOUND; }
"xbound"        { return XBOUND; }
"above"         { return ABOVE; }
"below"         { return BELOW; }
"equals"        { return EQUALS; }

"until"         { return UNTIL; }

"true"|"false"  { return BOOL_LITERAL; }

{FLOAT}         { return FLOAT_LITERAL; }
{INT}           { return INT_LITERAL; }
{ID}            { return IDENTIFIER; }

"=="|"!="|"<="|">="|"<"|">"   { return COMP; }

"+="|"-="|"*="|"/="           { return COMP_ASSIGN; }

"="             { return ASSIGN; }

";"             { return SEMICOLON; }
","             { return COMMA; }
"("             { return LPAREN; }
")"             { return RPAREN; }
"{"             { return LBRACE; }
"}"             { return RBRACE; }

"+"             { return PLUS; }
"-"             { return MINUS; }
"*"             { return MULT; }
"/"             { return DIV; }

[ \t\r\n]+      { /* ignore whitespace */ }

"/*"            { BEGIN(COMMENT_STATE); }

<COMMENT_STATE>"*/"     { BEGIN(INITIAL); }
<COMMENT_STATE>[^*\n]+  { /* ignore */ }
<COMMENT_STATE>"*"[^/]  { /* ignore */ }
<COMMENT_STATE>\n       { /* ignore */ }

.               { return yytext[0]; }

%%

