%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE *yyin;
FILE *yyout;
%}

%option noyywrap
%x COMMENT_STATE

DIGIT   [0-9]
LETTER  [a-zA-Z_]
ID      {LETTER}({LETTER}|{DIGIT})*
INT     {DIGIT}+
FLOAT   {DIGIT}+"."{DIGIT}+

%%

"integer"       { fprintf(yyout,"INTEGER\n"); }
"decimal"       { fprintf(yyout,"DECIMAL\n"); }
"symbol"        { fprintf(yyout,"SYMBOL\n"); }
"logical"       { fprintf(yyout,"LOGICAL\n"); }
"nothing"       { fprintf(yyout,"NOTHING\n"); }

"function"      { fprintf(yyout,"FUNCTION\n"); }
"returns"       { fprintf(yyout,"RETURNS\n"); }

"when"          { fprintf(yyout,"WHEN\n"); }
"is"            { fprintf(yyout,"IS\n"); }
"otherwise"     { fprintf(yyout,"OTHERWISE\n"); }

"SAFE"          { fprintf(yyout,"SAFE\n"); }

"from"          { fprintf(yyout,"FROM\n"); }
"to"            { fprintf(yyout,"TO\n"); }
"in"            { fprintf(yyout,"IN\n"); }

"bound"         { fprintf(yyout,"BOUND\n"); }
"xbound"        { fprintf(yyout,"XBOUND\n"); }
"above"         { fprintf(yyout,"ABOVE\n"); }
"below"         { fprintf(yyout,"BELOW\n"); }
"equals"        { fprintf(yyout,"EQUALS\n"); }

"until"         { fprintf(yyout,"UNTIL\n"); }

"true"|"false"  { fprintf(yyout,"BOOL_LITERAL (%s)\n", yytext); }

{FLOAT}         { fprintf(yyout,"FLOAT_LITERAL (%s)\n", yytext); }
{INT}           { fprintf(yyout,"INT_LITERAL (%s)\n", yytext); }
{ID}            { fprintf(yyout,"IDENTIFIER (%s)\n", yytext); }

"=="|"!="|"<="|">="|"<"|">" { fprintf(yyout,"COMP (%s)\n", yytext); }

"+="|"-="|"*="|"/=" { fprintf(yyout,"COMP_ASSIGN (%s)\n", yytext); }

"="                 { fprintf(yyout,"ASSIGN\n"); }

[ \t\r\n]+          { /* ignore */ }

"/*"                { BEGIN(COMMENT_STATE); }
";"                 { fprintf(yyout,"SEMICOLON\n"); }
","                 { fprintf(yyout,"COMMA\n"); }
"("                 { fprintf(yyout,"LPAREN\n"); }
")"                 { fprintf(yyout,"RPAREN\n"); }
"{"                 { fprintf(yyout,"LBRACE\n"); }
"}"                 { fprintf(yyout,"RBRACE\n"); }
"+"                 { fprintf(yyout,"PLUS\n"); }
"-"                 { fprintf(yyout,"MINUS\n"); }
"*"                 { fprintf(yyout,"MULT\n"); }
"/"                 { fprintf(yyout,"DIV\n"); }
"\""                { fprintf(yyout,"QUOTE\n"); }


<COMMENT_STATE>"*/"     { BEGIN(INITIAL); }
<COMMENT_STATE>[^*\n]+  { /* ignore*/ }
<COMMENT_STATE>"*"[^/]  { /* ignore */ }
<COMMENT_STATE>\n       { /* ignore */ }


.               { fprintf(yyout,"UNKNOWN (%s)\n", yytext); }

%%

int main(void) {
    yyin = fopen("input.txt", "r");
    yyout = fopen("output.txt", "w");

    if (!yyin || !yyout) {
        perror("File error");
        return 1;
    }

    yylex();
    fclose(yyin);
    fclose(yyout);
    return 0;
}
