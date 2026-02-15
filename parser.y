%{
#include <stdio.h>
#include <stdlib.h>

extern int yylex();
void yyerror(const char *s);
%}

/* Token declarations (must match flex returns) */

%token INTEGER DECIMAL SYMBOL LOGICAL NOTHING
%token FUNCTION RETURNS
%token WHEN IS OTHERWISE SAFE
%token FROM TO IN
%token BOUND XBOUND ABOVE BELOW EQUALS
%token UNTIL
%token BOOL_LITERAL
%token FLOAT_LITERAL INT_LITERAL IDENTIFIER
%token COMP COMP_ASSIGN ASSIGN
%token SEMICOLON COMMA
%token LPAREN RPAREN LBRACE RBRACE
%token PLUS MINUS MULT DIV

%%

program:
    declarations functions
    ;

declarations:
    declarations declaration
    | /* */
    ;

declaration:
    type IDENTIFIER SEMICOLON
    ;

type:
    INTEGER
    | DECIMAL
    | SYMBOL
    | LOGICAL
    | NOTHING
    ;

functions:
    functions function
    | /* */
    ;

function:
    FUNCTION IDENTIFIER LPAREN parameters RPAREN RETURNS type LBRACE statements RBRACE
    ;

parameters:
    parameter_list
    | /* */
    ;

parameter_list:
    parameter
    | parameter_list COMMA parameter
    ;

parameter:
    type IDENTIFIER
    ;

statements:
    statements statement
    | /* empty */
    ;

statement:
      declaration
    | assignment
    | expression SEMICOLON
    ;

assignment:
    IDENTIFIER ASSIGN expression SEMICOLON
    ;

expression:
      INT_LITERAL
    | FLOAT_LITERAL
    | IDENTIFIER
    | expression PLUS expression
    | expression MINUS expression
    | expression MULT expression
    | expression DIV expression
    ;

%%

void yyerror(const char *s) {
    printf("Parse error: %s\n", s);
}

int main() {
    return yyparse();
}
