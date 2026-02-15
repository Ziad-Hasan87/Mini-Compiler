%{
#include <stdio.h>
#include <stdlib.h>

extern int yylex();
void yyerror(const char *s);
%}

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
%token STRING_LITERAL
%token PRINT

%%

program:
    declarations functions
;

declarations:
    declarations declaration
    | /* empty */
;

declaration:
    type IDENTIFIER SEMICOLON
;

type:
    INTEGER | DECIMAL | SYMBOL | LOGICAL | NOTHING
;

functions:
    functions function
    | /* empty */
;

function:
    FUNCTION IDENTIFIER LPAREN parameters RPAREN RETURNS type LBRACE statements RBRACE
;


function_call:
      IDENTIFIER LPAREN arguments RPAREN
    | PRINT LPAREN arguments RPAREN
;

arguments:
      argument_list
    | /* empty */
;

argument_list:
      expression
    | argument_list COMMA expression
;


parameters:
    parameter_list
    | /* empty */
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
    | function_call SEMICOLON
    | safe_block
    | from_to_loop
    | when_block
    | bound_block
    | until_loop
;


assignment:
    IDENTIFIER ASSIGN expression SEMICOLON
    | IDENTIFIER COMP_ASSIGN expression
;

expression:
      INT_LITERAL
    | FLOAT_LITERAL
    | STRING_LITERAL
    | IDENTIFIER
    | function_call
    | expression PLUS expression
    | expression MINUS expression
    | expression MULT expression
    | expression DIV expression
    | expression COMP expression
;


safe_block:
    SAFE IDENTIFIER LBRACE statements RBRACE
;

from_to_loop:
    IDENTIFIER FROM expression TO expression IN LPAREN assignment RPAREN LBRACE statements RBRACE
;

when_block:
    WHEN LBRACE when_clauses RBRACE
;

when_clauses:
      when_clauses when_clause
    | /* empty */
;

when_clause:
      IS LPAREN expression RPAREN LBRACE statements RBRACE
    | OTHERWISE LBRACE statements RBRACE
;

bound_block:
    BOUND LPAREN expression RPAREN LBRACE bound_clauses RBRACE
;

bound_clauses:
      bound_clauses bound_clause
    | /* empty */
;

bound_clause:
      IN LPAREN expression COMMA expression RPAREN LBRACE statements RBRACE
    | ABOVE LPAREN expression RPAREN LBRACE statements RBRACE
    | BELOW LPAREN expression RPAREN LBRACE statements RBRACE
    | EQUALS LPAREN expression RPAREN LBRACE statements RBRACE
;

until_loop:
    UNTIL LPAREN expression RPAREN LBRACE statements RBRACE
;

%%

void yyerror(const char *s) {
    extern char *yytext;
    extern int yylineno;
    fprintf(stderr, "Syntax Error at line %d near '%s': %s\n", yylineno, yytext, s);
}

int main(int argc, char **argv) {
    if (argc > 1) {
        extern FILE *yyin;
        yyin = fopen(argv[1], "r");
        if (!yyin) {
            perror("Cannot open file");
            return 1;
        }
    }

    if (yyparse() == 0)
        printf("Code is VALID.\n");
    else
        printf("Code is INVALID.\n");

    return 0;
}
