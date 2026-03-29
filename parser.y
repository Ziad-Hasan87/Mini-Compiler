%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

typedef struct Var {
    char name[50];
    int type;   /* 1=int, 2=decimal, 3=bool */
    int ivalue;
    double dvalue;
    int bvalue;
} Var;

typedef struct Function {
    char name[50];
    int return_type;
    ASTNode *params;
    ASTNode *body;
} Function;

Var symtab[500];
int symcount = 0;

Function functab[100];
int funccount = 0;

ASTNode* root = NULL;

int has_return = 0;
double return_value = 0;

extern int yylex();
extern char *yytext;
extern int yylineno;
extern FILE *yyin;

void yyerror(const char *s);

static ASTNode* new_node(const char *type) {
    ASTNode *n = (ASTNode*) malloc(sizeof(ASTNode));
    if (!n) {
        perror("malloc");
        exit(1);
    }
    memset(n, 0, sizeof(ASTNode));
    strcpy(n->type, type);
    return n;
}

void push_variable(char* name, int type) {
    strcpy(symtab[symcount].name, name);
    symtab[symcount].type = type;
    symtab[symcount].ivalue = 0;
    symtab[symcount].dvalue = 0.0;
    symtab[symcount].bvalue = 0;
    symcount++;
}

void add_variable(char* name, int type) {
    push_variable(name, type);
}

Var* get_variable(char* name) {
    for (int i = symcount - 1; i >= 0; i--) {
        if (strcmp(symtab[i].name, name) == 0) {
            return &symtab[i];
        }
    }
    return NULL;
}

void register_function(ASTNode* n) {
    for (int i = 0; i < funccount; i++) {
        if (strcmp(functab[i].name, n->name) == 0) {
            functab[i].return_type = n->var_type;
            functab[i].params = n->children[0];
            functab[i].body = n->children[1];
            return;
        }
    }

    strcpy(functab[funccount].name, n->name);
    functab[funccount].return_type = n->var_type;
    functab[funccount].params = n->children[0];
    functab[funccount].body = n->children[1];
    funccount++;
}

Function* get_function(char* name) {
    for (int i = 0; i < funccount; i++) {
        if (strcmp(functab[i].name, name) == 0) {
            return &functab[i];
        }
    }
    return NULL;
}

void scan_functions(ASTNode* n) {
    if (!n) return;

    if (strcmp(n->type, "function") == 0) {
        register_function(n);
    }

    if (n->left) scan_functions(n->left);
    if (n->right) scan_functions(n->right);

    for (int i = 0; i < n->child_count; i++) {
        scan_functions(n->children[i]);
    }
}

double eval(ASTNode* n);
void execute(ASTNode* n);

double call_function(char* name, ASTNode* args) {
    Function* fn = get_function(name);
    if (!fn) {
        printf("Error: function %s not found\n", name);
        return 0;
    }

    int param_count = (fn->params) ? fn->params->child_count : 0;
    int arg_count = (args) ? args->child_count : 0;

    if (param_count != arg_count) {
        printf("Error: function %s expects %d arguments but got %d\n", name, param_count, arg_count);
        return 0;
    }

    double evaluated_args[100];
    for (int i = 0; i < arg_count; i++) {
        evaluated_args[i] = eval(args->children[i]);
    }

    int old_symcount = symcount;
    int old_has_return = has_return;
    double old_return_value = return_value;

    has_return = 0;
    return_value = 0;

    for (int i = 0; i < param_count; i++) {
        ASTNode *param = fn->params->children[i];
        push_variable(param->name, param->var_type);

        Var *v = get_variable(param->name);
        if (v->type == 1) v->ivalue = (int)evaluated_args[i];
        else if (v->type == 2) v->dvalue = evaluated_args[i];
        else if (v->type == 3) v->bvalue = (evaluated_args[i] != 0);
    }

    execute(fn->body);

    double result = return_value;

    symcount = old_symcount;
    has_return = old_has_return;
    return_value = old_return_value;

    return result;
}

void eval_print(ASTNode* n) {
    if (!n) return;

    if (strcmp(n->type, "int") == 0) {
        printf("%d\n", n->value);
    }
    else if (strcmp(n->type, "decimal") == 0) {
        printf("%g\n", n->dvalue);
    }
    else if (strcmp(n->type, "bool") == 0) {
        printf("%s\n", n->bvalue ? "true" : "false");
    }
    else if (strcmp(n->type, "string") == 0) {
        printf("%s\n", n->name);
    }
    else if (strcmp(n->type, "var") == 0) {
        Var* v = get_variable(n->name);
        if (!v) {
            printf("Error: variable %s not found\n", n->name);
            return;
        }

        if (v->type == 1) printf("%d\n", v->ivalue);
        else if (v->type == 2) printf("%g\n", v->dvalue);
        else if (v->type == 3) printf("%s\n", v->bvalue ? "true" : "false");
    }
    else {
        double val = eval(n);
        printf("%g\n", val);
    }
}

double eval(ASTNode* n) {
    if (!n) return 0;

    if (strcmp(n->type, "int") == 0) return n->value;
    if (strcmp(n->type, "decimal") == 0) return n->dvalue;
    if (strcmp(n->type, "bool") == 0) return n->bvalue;
    if (strcmp(n->type, "string") == 0) return 0;

    if (strcmp(n->type, "var") == 0) {
        Var* v = get_variable(n->name);
        if (!v) {
            printf("Error: variable %s not found\n", n->name);
            return 0;
        }
        if (v->type == 1) return v->ivalue;
        if (v->type == 2) return v->dvalue;
        return v->bvalue;
    }

    if (strcmp(n->type, "func_call") == 0) {
        return call_function(n->name, n->children[0]);
    }

    if (strcmp(n->type, "add") == 0) return eval(n->left) + eval(n->right);
    if (strcmp(n->type, "sub") == 0) return eval(n->left) - eval(n->right);
    if (strcmp(n->type, "mul") == 0) return eval(n->left) * eval(n->right);
    if (strcmp(n->type, "div") == 0) return eval(n->left) / eval(n->right);

    if (strcmp(n->type, "comp") == 0) {
        double l = eval(n->left);
        double r = eval(n->right);

        if (strcmp(n->op, "==") == 0) return l == r;
        if (strcmp(n->op, "!=") == 0) return l != r;
        if (strcmp(n->op, "<")  == 0) return l <  r;
        if (strcmp(n->op, ">")  == 0) return l >  r;
        if (strcmp(n->op, "<=") == 0) return l <= r;
        if (strcmp(n->op, ">=") == 0) return l >= r;
    }

    return 0;
}

void execute(ASTNode* n) {
    if (!n) return;
    if (has_return) return;

    if (strcmp(n->type, "statements") == 0) {
        for (int i = 0; i < n->child_count; i++) {
            execute(n->children[i]);
            if (has_return) return;
        }
        return;
    }

    if (strcmp(n->type, "decl") == 0) {
        add_variable(n->name, n->var_type);
        return;
    }

    if (strcmp(n->type, "function") == 0) {
        register_function(n);
        return;
    }

    if (strcmp(n->type, "return") == 0) {
        return_value = eval(n->left);
        has_return = 1;
        return;
    }

    if (strcmp(n->type, "assign") == 0) {
        Var* v = get_variable(n->name);
        if (!v) {
            printf("Error: variable %s not declared\n", n->name);
            return;
        }

        double val = eval(n->left);
        if (v->type == 1) v->ivalue = (int) val;
        else if (v->type == 2) v->dvalue = val;
        else if (v->type == 3) v->bvalue = (val != 0);
        return;
    }

    if (strcmp(n->type, "comp_assign") == 0) {
        Var* v = get_variable(n->name);
        if (!v) {
            printf("Error: variable %s not declared\n", n->name);
            return;
        }

        double val = eval(n->left);

        if (strcmp(n->op, "+=") == 0) {
            if (v->type == 1) v->ivalue += (int) val;
            else if (v->type == 2) v->dvalue += val;
        }
        else if (strcmp(n->op, "-=") == 0) {
            if (v->type == 1) v->ivalue -= (int) val;
            else if (v->type == 2) v->dvalue -= val;
        }
        else if (strcmp(n->op, "*=") == 0) {
            if (v->type == 1) v->ivalue *= (int) val;
            else if (v->type == 2) v->dvalue *= val;
        }
        else if (strcmp(n->op, "/=") == 0) {
            if (v->type == 1) v->ivalue /= (int) val;
            else if (v->type == 2) v->dvalue /= val;
        }
        return;
    }

    if (strcmp(n->type, "print") == 0) {
        eval_print(n->left);
        return;
    }

    if (strcmp(n->type, "from_to") == 0) {
        int start = (int) eval(n->left);
        int end   = (int) eval(n->right);

        Var* loopVar = get_variable(n->name);
        if (!loopVar) {
            printf("Error: loop variable %s not declared\n", n->name);
            return;
        }

        for (int i = start; i <= end; i++) {
            loopVar->ivalue = i;

            if (n->child_count > 0 && n->children[0]) execute(n->children[0]);
            if (has_return) return;
            if (n->child_count > 1 && n->children[1]) execute(n->children[1]);
            if (has_return) return;
        }
        return;
    }

    if (strcmp(n->type, "when") == 0) {
        for (int i = 0; i < n->child_count; i++) {
            ASTNode *clause = n->children[i];

            if (strcmp(clause->type, "when_clause") == 0) {
                if (eval(clause->left)) {
                    execute(clause->children[0]);
                    return;
                }
            }
            else if (strcmp(clause->type, "otherwise") == 0) {
                execute(clause->children[0]);
                return;
            }
        }
        return;
    }

    if (strcmp(n->type, "bound") == 0) {
        double val = eval(n->left);

        for (int i = 0; i < n->child_count; i++) {
            ASTNode *clause = n->children[i];

            if (strcmp(clause->type, "in_clause") == 0) {
                double s = eval(clause->left);
                double e = eval(clause->right);
                if (val >= s && val <= e) {
                    execute(clause->children[0]);
                    return;
                }
            }
            else if (strcmp(clause->type, "above_clause") == 0) {
                if (val > eval(clause->left)) {
                    execute(clause->children[0]);
                    return;
                }
            }
            else if (strcmp(clause->type, "below_clause") == 0) {
                if (val < eval(clause->left)) {
                    execute(clause->children[0]);
                    return;
                }
            }
            else if (strcmp(clause->type, "equals_clause") == 0) {
                if (val == eval(clause->left)) {
                    execute(clause->children[0]);
                    return;
                }
            }
        }
        return;
    }

    if (strcmp(n->type, "until") == 0) {
        while (!eval(n->left)) {
            execute(n->children[0]);
            if (has_return) return;
        }
        return;
    }
}

%}

%union {
    char* str;
    int num;
    double dnum;
    int bval;
    ASTNode* node;
}

%token INTEGER DECIMAL LOGICAL NOTHING
%token FROM TO IN
%token WHEN IS OTHERWISE SAFE
%token BOUND ABOVE BELOW EQUALS UNTIL
%token FUNCTION RETURNS RETURN
%token <str> BOOL_LITERAL
%token <str> IDENTIFIER
%token <str> STRING_LITERAL
%token <str> INT_LITERAL FLOAT_LITERAL
%token ASSIGN SEMICOLON COMMA
%token <str> COMP
%token <str> COMP_ASSIGN
%token PLUS MINUS MULT DIV
%token LPAREN RPAREN LBRACE RBRACE
%token PRINT

%type <node> program statements statement declaration assignment print_stmt
%type <node> expression from_to_loop when_block when_clauses when_clause
%type <node> bound_block bound_clauses bound_clause until_loop
%type <node> loop_body assignment_no_semicolon
%type <node> function_decl opt_param_list param_list param_decl
%type <node> return_stmt function_call opt_arg_list arg_list
%type <num> type

%left COMP
%left PLUS MINUS
%left MULT DIV

%%

program:
    statements
    {
        root = $1;
        $$ = $1;
    }
;

statements:
      statements statement
      {
          $1->children = realloc($1->children, sizeof(ASTNode*) * ($1->child_count + 1));
          $1->children[$1->child_count++] = $2;
          $$ = $1;
      }
    | statement
      {
          ASTNode* n = new_node("statements");
          n->children = (ASTNode**) malloc(sizeof(ASTNode*));
          n->children[0] = $1;
          n->child_count = 1;
          $$ = n;
      }
;

statement:
      declaration
    | assignment
    | print_stmt
    | from_to_loop
    | when_block
    | bound_block
    | until_loop
    | function_decl
    | return_stmt
;

declaration:
      type IDENTIFIER SEMICOLON
      {
          ASTNode* n = new_node("decl");
          strcpy(n->name, $2);
          n->var_type = $1;
          $$ = n;
      }
;

type:
      INTEGER { $$ = 1; }
    | DECIMAL { $$ = 2; }
    | LOGICAL { $$ = 3; }
    | NOTHING { $$ = 4; }
;

assignment:
      IDENTIFIER ASSIGN expression SEMICOLON
      {
          ASTNode* n = new_node("assign");
          strcpy(n->name, $1);
          n->left = $3;
          $$ = n;
      }
    | IDENTIFIER COMP_ASSIGN expression SEMICOLON
      {
          ASTNode* n = new_node("comp_assign");
          strcpy(n->name, $1);
          strcpy(n->op, $2);
          n->left = $3;
          $$ = n;
      }
;

assignment_no_semicolon:
      IDENTIFIER ASSIGN expression
      {
          ASTNode* n = new_node("assign");
          strcpy(n->name, $1);
          n->left = $3;
          $$ = n;
      }
    | IDENTIFIER COMP_ASSIGN expression
      {
          ASTNode* n = new_node("comp_assign");
          strcpy(n->name, $1);
          strcpy(n->op, $2);
          n->left = $3;
          $$ = n;
      }
;

print_stmt:
      PRINT LPAREN expression RPAREN SEMICOLON
      {
          ASTNode* n = new_node("print");
          n->left = $3;
          $$ = n;
      }
;

return_stmt:
      RETURN expression SEMICOLON
      {
          ASTNode* n = new_node("return");
          n->left = $2;
          $$ = n;
      }
;

expression:
      INT_LITERAL
      {
          ASTNode* n = new_node("int");
          n->value = atoi($1);
          $$ = n;
      }
    | FLOAT_LITERAL
      {
          ASTNode* n = new_node("decimal");
          n->dvalue = atof($1);
          $$ = n;
      }
    | BOOL_LITERAL
      {
          ASTNode* n = new_node("bool");
          n->bvalue = (strcmp($1, "true") == 0) ? 1 : 0;
          $$ = n;
      }
    | STRING_LITERAL
      {
          ASTNode* n = new_node("string");
          strcpy(n->name, $1);
          $$ = n;
      }
    | IDENTIFIER
      {
          ASTNode* n = new_node("var");
          strcpy(n->name, $1);
          $$ = n;
      }
    | function_call
      {
          $$ = $1;
      }
    | LPAREN expression RPAREN
      {
          $$ = $2;
      }
    | expression PLUS expression
      {
          ASTNode* n = new_node("add");
          n->left = $1;
          n->right = $3;
          $$ = n;
      }
    | expression MINUS expression
      {
          ASTNode* n = new_node("sub");
          n->left = $1;
          n->right = $3;
          $$ = n;
      }
    | expression MULT expression
      {
          ASTNode* n = new_node("mul");
          n->left = $1;
          n->right = $3;
          $$ = n;
      }
    | expression DIV expression
      {
          ASTNode* n = new_node("div");
          n->left = $1;
          n->right = $3;
          $$ = n;
      }
    | expression COMP expression
      {
          ASTNode* n = new_node("comp");
          n->left = $1;
          n->right = $3;
          strcpy(n->op, $2);
          $$ = n;
      }
;

function_call:
    IDENTIFIER LPAREN opt_arg_list RPAREN
    {
        ASTNode* n = new_node("func_call");
        strcpy(n->name, $1);
        n->children = (ASTNode**) malloc(sizeof(ASTNode*));
        n->children[0] = $3;
        n->child_count = 1;
        $$ = n;
    }
;

opt_arg_list:
      arg_list
      {
          $$ = $1;
      }
    |
      {
          ASTNode* n = new_node("args");
          n->children = NULL;
          n->child_count = 0;
          $$ = n;
      }
;

arg_list:
      arg_list COMMA expression
      {
          $1->children = realloc($1->children, sizeof(ASTNode*) * ($1->child_count + 1));
          $1->children[$1->child_count++] = $3;
          $$ = $1;
      }
    | expression
      {
          ASTNode* n = new_node("args");
          n->children = (ASTNode**) malloc(sizeof(ASTNode*));
          n->children[0] = $1;
          n->child_count = 1;
          $$ = n;
      }
;

from_to_loop:
    IDENTIFIER FROM expression TO expression IN LPAREN assignment_no_semicolon RPAREN LBRACE loop_body RBRACE
    {
        ASTNode* n = new_node("from_to");
        strcpy(n->name, $1);
        n->left = $3;
        n->right = $5;

        n->children = (ASTNode**) malloc(sizeof(ASTNode*) * 2);
        n->children[0] = $8;
        n->children[1] = $11;
        n->child_count = 2;

        $$ = n;
    }
;

loop_body:
    statements
    {
        $$ = $1;
    }
;

when_block:
    WHEN LBRACE when_clauses RBRACE
    {
        ASTNode* n = new_node("when");
        n->children = (ASTNode**) malloc(sizeof(ASTNode*) * $3->child_count);
        for (int i = 0; i < $3->child_count; i++) {
            n->children[i] = $3->children[i];
        }
        n->child_count = $3->child_count;
        $$ = n;
    }
;

when_clauses:
      when_clauses when_clause
      {
          $1->children = realloc($1->children, sizeof(ASTNode*) * ($1->child_count + 1));
          $1->children[$1->child_count++] = $2;
          $$ = $1;
      }
    | when_clause
      {
          ASTNode* n = new_node("when_clauses");
          n->children = (ASTNode**) malloc(sizeof(ASTNode*));
          n->children[0] = $1;
          n->child_count = 1;
          $$ = n;
      }
;

when_clause:
      IS LPAREN expression RPAREN LBRACE statements RBRACE
      {
          ASTNode* n = new_node("when_clause");
          n->left = $3;
          n->children = (ASTNode**) malloc(sizeof(ASTNode*));
          n->children[0] = $6;
          n->child_count = 1;
          $$ = n;
      }
    | OTHERWISE LBRACE statements RBRACE
      {
          ASTNode* n = new_node("otherwise");
          n->children = (ASTNode**) malloc(sizeof(ASTNode*));
          n->children[0] = $3;
          n->child_count = 1;
          $$ = n;
      }
;

bound_block:
    BOUND LPAREN expression RPAREN LBRACE bound_clauses RBRACE
    {
        ASTNode* n = new_node("bound");
        n->left = $3;
        n->children = (ASTNode**) malloc(sizeof(ASTNode*) * $6->child_count);
        for (int i = 0; i < $6->child_count; i++) {
            n->children[i] = $6->children[i];
        }
        n->child_count = $6->child_count;
        $$ = n;
    }
;

bound_clauses:
      bound_clauses bound_clause
      {
          $1->children = realloc($1->children, sizeof(ASTNode*) * ($1->child_count + 1));
          $1->children[$1->child_count++] = $2;
          $$ = $1;
      }
    | bound_clause
      {
          ASTNode* n = new_node("bound_clauses");
          n->children = (ASTNode**) malloc(sizeof(ASTNode*));
          n->children[0] = $1;
          n->child_count = 1;
          $$ = n;
      }
;

bound_clause:
      IN LPAREN expression COMMA expression RPAREN LBRACE statements RBRACE
      {
          ASTNode* n = new_node("in_clause");
          n->left = $3;
          n->right = $5;
          n->children = (ASTNode**) malloc(sizeof(ASTNode*));
          n->children[0] = $8;
          n->child_count = 1;
          $$ = n;
      }
    | ABOVE LPAREN expression RPAREN LBRACE statements RBRACE
      {
          ASTNode* n = new_node("above_clause");
          n->left = $3;
          n->children = (ASTNode**) malloc(sizeof(ASTNode*));
          n->children[0] = $6;
          n->child_count = 1;
          $$ = n;
      }
    | BELOW LPAREN expression RPAREN LBRACE statements RBRACE
      {
          ASTNode* n = new_node("below_clause");
          n->left = $3;
          n->children = (ASTNode**) malloc(sizeof(ASTNode*));
          n->children[0] = $6;
          n->child_count = 1;
          $$ = n;
      }
    | EQUALS LPAREN expression RPAREN LBRACE statements RBRACE
      {
          ASTNode* n = new_node("equals_clause");
          n->left = $3;
          n->children = (ASTNode**) malloc(sizeof(ASTNode*));
          n->children[0] = $6;
          n->child_count = 1;
          $$ = n;
      }
;

until_loop:
    UNTIL LPAREN expression RPAREN LBRACE statements RBRACE
    {
        ASTNode* n = new_node("until");
        n->left = $3;
        n->children = (ASTNode**) malloc(sizeof(ASTNode*));
        n->children[0] = $6;
        n->child_count = 1;
        $$ = n;
    }
;

function_decl:
    FUNCTION IDENTIFIER LPAREN opt_param_list RPAREN RETURNS type LBRACE statements RBRACE
    {
        ASTNode* n = new_node("function");
        strcpy(n->name, $2);
        n->var_type = $7;

        n->children = (ASTNode**) malloc(sizeof(ASTNode*) * 2);
        n->children[0] = $4;
        n->children[1] = $9;
        n->child_count = 2;

        $$ = n;
    }
;

opt_param_list:
      param_list
      {
          $$ = $1;
      }
    |
      {
          ASTNode* n = new_node("params");
          n->children = NULL;
          n->child_count = 0;
          $$ = n;
      }
;

param_list:
      param_list COMMA param_decl
      {
          $1->children = realloc($1->children, sizeof(ASTNode*) * ($1->child_count + 1));
          $1->children[$1->child_count++] = $3;
          $$ = $1;
      }
    | param_decl
      {
          ASTNode* n = new_node("params");
          n->children = (ASTNode**) malloc(sizeof(ASTNode*));
          n->children[0] = $1;
          n->child_count = 1;
          $$ = n;
      }
;

param_decl:
    type IDENTIFIER
    {
        ASTNode* n = new_node("param");
        strcpy(n->name, $2);
        n->var_type = $1;
        $$ = n;
    }
;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Syntax Error at line %d near '%s': %s\n", yylineno, yytext, s);
}

int main(int argc, char **argv) {
    if (argc > 1) {
        yyin = fopen(argv[1], "r");
        if (!yyin) {
            perror("Cannot open file");
            return 1;
        }
    }

    if (yyparse() == 0) {
        scan_functions(root);
        printf("Code is VALID.\n");
        execute(root);
    } else {
        printf("Code is INVALID.\n");
    }

    return 0;
}