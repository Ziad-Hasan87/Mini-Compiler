// ast.h
#ifndef AST_H
#define AST_H

typedef struct ASTNode {
    char type[20];           // "decl", "assign", "print", etc.
    int value;               // for int
    double dvalue;           // for decimal
    int bvalue;              // for bool
    char name[50];           // variable name
    char op[3];              // for +=, -= etc.
    struct ASTNode *left;    // for expressions
    struct ASTNode *right;
    struct ASTNode **children;
    int child_count;
    int var_type;            // 1=int, 2=decimal, 3=bool
} ASTNode;

#endif