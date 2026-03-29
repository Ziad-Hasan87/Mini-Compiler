%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "parser.tab.h"
%}

%option noyywrap
%option yylineno
%x COMMENT_STATE

DIGIT   [0-9]
LETTER  [a-zA-Z_]
ID      {LETTER}({LETTER}|{DIGIT})*
INT     {DIGIT}+
FLOAT   {DIGIT}+"."{DIGIT}+
STRING  \"([^\\\"]|\\.)*\"

%%

"integer"       { return INTEGER; }
"decimal"       { return DECIMAL; }
"logical"       { return LOGICAL; }
"nothing"       { return NOTHING; }

"function"      { return FUNCTION; }
"returns"       { return RETURNS; }
"return"        { return RETURN; }

"when"          { return WHEN; }
"is"            { return IS; }
"otherwise"     { return OTHERWISE; }
"safe"          { return SAFE; }

"from"          { return FROM; }
"to"            { return TO; }
"in"            { return IN; }

"bound"         { return BOUND; }
"above"         { return ABOVE; }
"below"         { return BELOW; }
"equals"        { return EQUALS; }

"until"         { return UNTIL; }

"scan"          { return SCAN;}
"print"         { return PRINT; }

"true"|"false"  {
    yylval.str = strdup(yytext);
    return BOOL_LITERAL;
}

{FLOAT}         {
    yylval.str = strdup(yytext);
    return FLOAT_LITERAL;
}

{INT}           {
    yylval.str = strdup(yytext);
    return INT_LITERAL;
}

{ID}            {
    yylval.str = strdup(yytext);
    return IDENTIFIER;
}

{STRING}        {
    yylval.str = strdup(yytext);

    size_t len = strlen(yylval.str);
    if (len >= 2 && yylval.str[0] == '"' && yylval.str[len - 1] == '"') {
        yylval.str[len - 1] = '\0';
        memmove(yylval.str, yylval.str + 1, len - 1);
    }

    return STRING_LITERAL;
}

"=="|"!="|"<="|">="|"<"|">" {
    yylval.str = strdup(yytext);
    return COMP;
}

"+="|"-="|"*="|"/=" {
    yylval.str = strdup(yytext);
    return COMP_ASSIGN;
}

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

[ \t\r\n]+      { }

"/*"            { BEGIN(COMMENT_STATE); }
<COMMENT_STATE>"*/"      { BEGIN(INITIAL); }
<COMMENT_STATE>[^*\n]+   { }
<COMMENT_STATE>"*"[^/]   { }
<COMMENT_STATE>\n        { }

.               { return yytext[0]; }

%%