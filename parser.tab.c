/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "parser.y"

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


#line 471 "parser.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "parser.tab.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_INTEGER = 3,                    /* INTEGER  */
  YYSYMBOL_DECIMAL = 4,                    /* DECIMAL  */
  YYSYMBOL_LOGICAL = 5,                    /* LOGICAL  */
  YYSYMBOL_NOTHING = 6,                    /* NOTHING  */
  YYSYMBOL_FROM = 7,                       /* FROM  */
  YYSYMBOL_TO = 8,                         /* TO  */
  YYSYMBOL_IN = 9,                         /* IN  */
  YYSYMBOL_WHEN = 10,                      /* WHEN  */
  YYSYMBOL_IS = 11,                        /* IS  */
  YYSYMBOL_OTHERWISE = 12,                 /* OTHERWISE  */
  YYSYMBOL_SAFE = 13,                      /* SAFE  */
  YYSYMBOL_BOUND = 14,                     /* BOUND  */
  YYSYMBOL_ABOVE = 15,                     /* ABOVE  */
  YYSYMBOL_BELOW = 16,                     /* BELOW  */
  YYSYMBOL_EQUALS = 17,                    /* EQUALS  */
  YYSYMBOL_UNTIL = 18,                     /* UNTIL  */
  YYSYMBOL_FUNCTION = 19,                  /* FUNCTION  */
  YYSYMBOL_RETURNS = 20,                   /* RETURNS  */
  YYSYMBOL_RETURN = 21,                    /* RETURN  */
  YYSYMBOL_BOOL_LITERAL = 22,              /* BOOL_LITERAL  */
  YYSYMBOL_IDENTIFIER = 23,                /* IDENTIFIER  */
  YYSYMBOL_STRING_LITERAL = 24,            /* STRING_LITERAL  */
  YYSYMBOL_INT_LITERAL = 25,               /* INT_LITERAL  */
  YYSYMBOL_FLOAT_LITERAL = 26,             /* FLOAT_LITERAL  */
  YYSYMBOL_ASSIGN = 27,                    /* ASSIGN  */
  YYSYMBOL_SEMICOLON = 28,                 /* SEMICOLON  */
  YYSYMBOL_COMMA = 29,                     /* COMMA  */
  YYSYMBOL_COMP = 30,                      /* COMP  */
  YYSYMBOL_COMP_ASSIGN = 31,               /* COMP_ASSIGN  */
  YYSYMBOL_PLUS = 32,                      /* PLUS  */
  YYSYMBOL_MINUS = 33,                     /* MINUS  */
  YYSYMBOL_MULT = 34,                      /* MULT  */
  YYSYMBOL_DIV = 35,                       /* DIV  */
  YYSYMBOL_LPAREN = 36,                    /* LPAREN  */
  YYSYMBOL_RPAREN = 37,                    /* RPAREN  */
  YYSYMBOL_LBRACE = 38,                    /* LBRACE  */
  YYSYMBOL_RBRACE = 39,                    /* RBRACE  */
  YYSYMBOL_PRINT = 40,                     /* PRINT  */
  YYSYMBOL_YYACCEPT = 41,                  /* $accept  */
  YYSYMBOL_program = 42,                   /* program  */
  YYSYMBOL_statements = 43,                /* statements  */
  YYSYMBOL_statement = 44,                 /* statement  */
  YYSYMBOL_declaration = 45,               /* declaration  */
  YYSYMBOL_type = 46,                      /* type  */
  YYSYMBOL_assignment = 47,                /* assignment  */
  YYSYMBOL_assignment_no_semicolon = 48,   /* assignment_no_semicolon  */
  YYSYMBOL_print_stmt = 49,                /* print_stmt  */
  YYSYMBOL_return_stmt = 50,               /* return_stmt  */
  YYSYMBOL_expression = 51,                /* expression  */
  YYSYMBOL_function_call = 52,             /* function_call  */
  YYSYMBOL_opt_arg_list = 53,              /* opt_arg_list  */
  YYSYMBOL_arg_list = 54,                  /* arg_list  */
  YYSYMBOL_from_to_loop = 55,              /* from_to_loop  */
  YYSYMBOL_loop_body = 56,                 /* loop_body  */
  YYSYMBOL_when_block = 57,                /* when_block  */
  YYSYMBOL_when_clauses = 58,              /* when_clauses  */
  YYSYMBOL_when_clause = 59,               /* when_clause  */
  YYSYMBOL_bound_block = 60,               /* bound_block  */
  YYSYMBOL_bound_clauses = 61,             /* bound_clauses  */
  YYSYMBOL_bound_clause = 62,              /* bound_clause  */
  YYSYMBOL_until_loop = 63,                /* until_loop  */
  YYSYMBOL_function_decl = 64,             /* function_decl  */
  YYSYMBOL_opt_param_list = 65,            /* opt_param_list  */
  YYSYMBOL_param_list = 66,                /* param_list  */
  YYSYMBOL_param_decl = 67                 /* param_decl  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_uint8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  41
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   395

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  41
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  27
/* YYNRULES -- Number of rules.  */
#define YYNRULES  62
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  159

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   295


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   440,   440,   448,   454,   465,   466,   467,   468,   469,
     470,   471,   472,   473,   477,   487,   488,   489,   490,   494,
     501,   512,   519,   530,   539,   548,   554,   560,   566,   572,
     578,   582,   586,   593,   600,   607,   614,   625,   637,   642,
     651,   657,   668,   685,   692,   705,   711,   722,   731,   742,
     756,   762,   773,   783,   792,   801,   813,   825,   841,   846,
     855,   861,   872
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "INTEGER", "DECIMAL",
  "LOGICAL", "NOTHING", "FROM", "TO", "IN", "WHEN", "IS", "OTHERWISE",
  "SAFE", "BOUND", "ABOVE", "BELOW", "EQUALS", "UNTIL", "FUNCTION",
  "RETURNS", "RETURN", "BOOL_LITERAL", "IDENTIFIER", "STRING_LITERAL",
  "INT_LITERAL", "FLOAT_LITERAL", "ASSIGN", "SEMICOLON", "COMMA", "COMP",
  "COMP_ASSIGN", "PLUS", "MINUS", "MULT", "DIV", "LPAREN", "RPAREN",
  "LBRACE", "RBRACE", "PRINT", "$accept", "program", "statements",
  "statement", "declaration", "type", "assignment",
  "assignment_no_semicolon", "print_stmt", "return_stmt", "expression",
  "function_call", "opt_arg_list", "arg_list", "from_to_loop", "loop_body",
  "when_block", "when_clauses", "when_clause", "bound_block",
  "bound_clauses", "bound_clause", "until_loop", "function_decl",
  "opt_param_list", "param_list", "param_decl", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-48)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     274,   -48,   -48,   -48,   -48,   -33,   -28,   -11,   -12,   280,
      -3,    -9,    51,   274,   -48,   -48,    31,   -48,   -48,   -48,
     -48,   -48,   -48,   -48,   -48,    10,   280,   280,    17,   -48,
      19,   -48,   -48,   -48,   280,    69,   -48,   280,   280,   280,
     280,   -48,   -48,    40,    43,    29,    -5,   -48,   278,   287,
      32,   280,   293,   -48,   280,   280,   280,   280,   280,    39,
     301,   309,   315,   -48,   280,   274,   -48,   -48,    42,    47,
      63,    54,    77,   -48,   360,    55,    80,   -48,    30,    15,
      15,   -48,   -48,   280,   -48,   -48,    70,   321,    38,    67,
     274,   -48,    96,    32,   -48,   280,   266,   -48,    79,   -48,
      82,    84,    86,    88,     1,   -48,   109,    32,   -48,   360,
      89,   274,   280,   280,   280,   280,   -48,   -48,   -48,    91,
     103,   136,   354,   327,   333,   339,   274,    -8,    94,   -48,
     280,    95,   100,   105,   159,   280,   280,   107,   345,   274,
     274,   274,   -48,   360,   360,   274,   113,   182,   205,   228,
     274,   108,   274,   -48,   -48,   -48,   -48,   251,   -48
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,    15,    16,    17,    18,     0,     0,     0,     0,     0,
       0,     0,     0,     2,     4,     5,     0,     6,     7,    13,
       8,     9,    10,    11,    12,     0,     0,     0,     0,    27,
      29,    28,    25,    26,     0,     0,    30,     0,     0,     0,
       0,     1,     3,     0,     0,     0,     0,    46,     0,     0,
      59,    39,     0,    24,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    14,     0,     0,    44,    45,     0,     0,
       0,     0,    58,    61,    41,     0,    38,    31,    36,    32,
      33,    34,    35,     0,    19,    20,     0,     0,     0,     0,
       0,    62,     0,     0,    37,     0,     0,    23,     0,    48,
       0,     0,     0,     0,     0,    51,     0,     0,    60,    40,
       0,     0,     0,     0,     0,     0,    49,    50,    56,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    47,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    57,    21,    22,     0,     0,     0,     0,     0,
      43,     0,     0,    53,    54,    55,    42,     0,    52
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -48,   -48,   -45,   -13,   -48,   -47,   -48,   -48,   -48,   -48,
     -25,   -48,   -48,   -48,   -48,   -48,   -48,   -48,   106,   -48,
     -48,    49,   -48,   -48,   -48,   -48,    65
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_uint8 yydefgoto[] =
{
       0,    12,    13,    14,    15,    16,    17,   128,    18,    19,
      35,    36,    75,    76,    20,   151,    21,    46,    47,    22,
     104,   105,    23,    24,    71,    72,    73
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint8 yytable[] =
{
      42,    48,    49,    70,    37,    25,    44,    45,    26,    52,
     100,    28,    59,    60,    61,    62,   101,   102,   103,   135,
      88,    44,    45,   136,    38,    27,    74,    40,    39,    78,
      79,    80,    81,    82,    66,     1,     2,     3,     4,    87,
     116,     1,     2,     3,     4,   106,    70,    83,     5,    57,
      58,    41,     6,    50,    43,    51,     7,     8,    96,     9,
     119,    10,    55,    56,    57,    58,   121,    65,    63,    54,
     109,    55,    56,    57,    58,    42,   100,    99,    11,    64,
      89,   134,   101,   102,   103,    90,    91,   122,   123,   124,
     125,    92,    94,    42,   147,   148,   149,    53,    97,    54,
     150,    55,    56,    57,    58,   138,    93,   157,    42,    95,
     143,   144,     1,     2,     3,     4,   107,   111,   112,     5,
     113,    42,   114,     6,   115,   120,   127,     7,     8,   126,
       9,   137,    10,   139,    42,    42,    42,    42,   140,     1,
       2,     3,     4,   141,    42,   145,     5,   156,   118,    11,
       6,   152,    67,   117,     7,     8,     0,     9,   108,    10,
       0,     0,     1,     2,     3,     4,     0,     0,     0,     5,
       0,     0,     0,     6,     0,   129,    11,     7,     8,     0,
       9,     0,    10,     0,     0,     1,     2,     3,     4,     0,
       0,     0,     5,     0,     0,     0,     6,     0,   142,    11,
       7,     8,     0,     9,     0,    10,     0,     0,     1,     2,
       3,     4,     0,     0,     0,     5,     0,     0,     0,     6,
       0,   153,    11,     7,     8,     0,     9,     0,    10,     0,
       0,     1,     2,     3,     4,     0,     0,     0,     5,     0,
       0,     0,     6,     0,   154,    11,     7,     8,     0,     9,
       0,    10,     0,     0,     1,     2,     3,     4,     0,     0,
       0,     5,     0,     0,     0,     6,     0,   155,    11,     7,
       8,     0,     9,     0,    10,   110,     0,     1,     2,     3,
       4,     0,     0,     0,     5,     0,     0,     0,     6,     0,
     158,    11,     7,     8,     0,     9,    54,    10,    55,    56,
      57,    58,    29,    30,    31,    32,    33,     0,    54,     0,
      55,    56,    57,    58,    11,    68,    34,    54,     0,    55,
      56,    57,    58,    54,    69,    55,    56,    57,    58,    84,
      77,    54,     0,    55,    56,    57,    58,    85,     0,    54,
       0,    55,    56,    57,    58,    54,     0,    55,    56,    57,
      58,    54,    86,    55,    56,    57,    58,    54,    98,    55,
      56,    57,    58,    54,   131,    55,    56,    57,    58,    54,
     132,    55,    56,    57,    58,    54,   133,    55,    56,    57,
      58,     0,   146,   130,    54,     0,    55,    56,    57,    58,
      54,     0,    55,    56,    57,    58
};

static const yytype_int16 yycheck[] =
{
      13,    26,    27,    50,     7,    38,    11,    12,    36,    34,
       9,    23,    37,    38,    39,    40,    15,    16,    17,    27,
      65,    11,    12,    31,    27,    36,    51,    36,    31,    54,
      55,    56,    57,    58,    39,     3,     4,     5,     6,    64,
      39,     3,     4,     5,     6,    90,    93,     8,    10,    34,
      35,     0,    14,    36,    23,    36,    18,    19,    83,    21,
     107,    23,    32,    33,    34,    35,   111,    38,    28,    30,
      95,    32,    33,    34,    35,    88,     9,    39,    40,    36,
      38,   126,    15,    16,    17,    38,    23,   112,   113,   114,
     115,    37,    37,   106,   139,   140,   141,    28,    28,    30,
     145,    32,    33,    34,    35,   130,    29,   152,   121,    29,
     135,   136,     3,     4,     5,     6,    20,    38,    36,    10,
      36,   134,    36,    14,    36,    36,    23,    18,    19,    38,
      21,    37,    23,    38,   147,   148,   149,   150,    38,     3,
       4,     5,     6,    38,   157,    38,    10,    39,    39,    40,
      14,    38,    46,   104,    18,    19,    -1,    21,    93,    23,
      -1,    -1,     3,     4,     5,     6,    -1,    -1,    -1,    10,
      -1,    -1,    -1,    14,    -1,    39,    40,    18,    19,    -1,
      21,    -1,    23,    -1,    -1,     3,     4,     5,     6,    -1,
      -1,    -1,    10,    -1,    -1,    -1,    14,    -1,    39,    40,
      18,    19,    -1,    21,    -1,    23,    -1,    -1,     3,     4,
       5,     6,    -1,    -1,    -1,    10,    -1,    -1,    -1,    14,
      -1,    39,    40,    18,    19,    -1,    21,    -1,    23,    -1,
      -1,     3,     4,     5,     6,    -1,    -1,    -1,    10,    -1,
      -1,    -1,    14,    -1,    39,    40,    18,    19,    -1,    21,
      -1,    23,    -1,    -1,     3,     4,     5,     6,    -1,    -1,
      -1,    10,    -1,    -1,    -1,    14,    -1,    39,    40,    18,
      19,    -1,    21,    -1,    23,     9,    -1,     3,     4,     5,
       6,    -1,    -1,    -1,    10,    -1,    -1,    -1,    14,    -1,
      39,    40,    18,    19,    -1,    21,    30,    23,    32,    33,
      34,    35,    22,    23,    24,    25,    26,    -1,    30,    -1,
      32,    33,    34,    35,    40,    37,    36,    30,    -1,    32,
      33,    34,    35,    30,    37,    32,    33,    34,    35,    28,
      37,    30,    -1,    32,    33,    34,    35,    28,    -1,    30,
      -1,    32,    33,    34,    35,    30,    -1,    32,    33,    34,
      35,    30,    37,    32,    33,    34,    35,    30,    37,    32,
      33,    34,    35,    30,    37,    32,    33,    34,    35,    30,
      37,    32,    33,    34,    35,    30,    37,    32,    33,    34,
      35,    -1,    37,    29,    30,    -1,    32,    33,    34,    35,
      30,    -1,    32,    33,    34,    35
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     3,     4,     5,     6,    10,    14,    18,    19,    21,
      23,    40,    42,    43,    44,    45,    46,    47,    49,    50,
      55,    57,    60,    63,    64,    38,    36,    36,    23,    22,
      23,    24,    25,    26,    36,    51,    52,     7,    27,    31,
      36,     0,    44,    23,    11,    12,    58,    59,    51,    51,
      36,    36,    51,    28,    30,    32,    33,    34,    35,    51,
      51,    51,    51,    28,    36,    38,    39,    59,    37,    37,
      46,    65,    66,    67,    51,    53,    54,    37,    51,    51,
      51,    51,    51,     8,    28,    28,    37,    51,    43,    38,
      38,    23,    37,    29,    37,    29,    51,    28,    37,    39,
       9,    15,    16,    17,    61,    62,    43,    20,    67,    51,
       9,    38,    36,    36,    36,    36,    39,    62,    39,    46,
      36,    43,    51,    51,    51,    51,    38,    23,    48,    39,
      29,    37,    37,    37,    43,    27,    31,    37,    51,    38,
      38,    38,    39,    51,    51,    38,    37,    43,    43,    43,
      43,    56,    38,    39,    39,    39,    39,    43,    39
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    41,    42,    43,    43,    44,    44,    44,    44,    44,
      44,    44,    44,    44,    45,    46,    46,    46,    46,    47,
      47,    48,    48,    49,    50,    51,    51,    51,    51,    51,
      51,    51,    51,    51,    51,    51,    51,    52,    53,    53,
      54,    54,    55,    56,    57,    58,    58,    59,    59,    60,
      61,    61,    62,    62,    62,    62,    63,    64,    65,    65,
      66,    66,    67
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     3,     1,     1,     1,     1,     4,
       4,     3,     3,     5,     3,     1,     1,     1,     1,     1,
       1,     3,     3,     3,     3,     3,     3,     4,     1,     0,
       3,     1,    12,     1,     4,     2,     1,     7,     4,     7,
       2,     1,     9,     7,     7,     7,     7,    10,     1,     0,
       3,     1,     2
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* program: statements  */
#line 441 "parser.y"
    {
        root = (yyvsp[0].node);
        (yyval.node) = (yyvsp[0].node);
    }
#line 1674 "parser.tab.c"
    break;

  case 3: /* statements: statements statement  */
#line 449 "parser.y"
      {
          (yyvsp[-1].node)->children = realloc((yyvsp[-1].node)->children, sizeof(ASTNode*) * ((yyvsp[-1].node)->child_count + 1));
          (yyvsp[-1].node)->children[(yyvsp[-1].node)->child_count++] = (yyvsp[0].node);
          (yyval.node) = (yyvsp[-1].node);
      }
#line 1684 "parser.tab.c"
    break;

  case 4: /* statements: statement  */
#line 455 "parser.y"
      {
          ASTNode* n = new_node("statements");
          n->children = (ASTNode**) malloc(sizeof(ASTNode*));
          n->children[0] = (yyvsp[0].node);
          n->child_count = 1;
          (yyval.node) = n;
      }
#line 1696 "parser.tab.c"
    break;

  case 14: /* declaration: type IDENTIFIER SEMICOLON  */
#line 478 "parser.y"
      {
          ASTNode* n = new_node("decl");
          strcpy(n->name, (yyvsp[-1].str));
          n->var_type = (yyvsp[-2].num);
          (yyval.node) = n;
      }
#line 1707 "parser.tab.c"
    break;

  case 15: /* type: INTEGER  */
#line 487 "parser.y"
              { (yyval.num) = 1; }
#line 1713 "parser.tab.c"
    break;

  case 16: /* type: DECIMAL  */
#line 488 "parser.y"
              { (yyval.num) = 2; }
#line 1719 "parser.tab.c"
    break;

  case 17: /* type: LOGICAL  */
#line 489 "parser.y"
              { (yyval.num) = 3; }
#line 1725 "parser.tab.c"
    break;

  case 18: /* type: NOTHING  */
#line 490 "parser.y"
              { (yyval.num) = 4; }
#line 1731 "parser.tab.c"
    break;

  case 19: /* assignment: IDENTIFIER ASSIGN expression SEMICOLON  */
#line 495 "parser.y"
      {
          ASTNode* n = new_node("assign");
          strcpy(n->name, (yyvsp[-3].str));
          n->left = (yyvsp[-1].node);
          (yyval.node) = n;
      }
#line 1742 "parser.tab.c"
    break;

  case 20: /* assignment: IDENTIFIER COMP_ASSIGN expression SEMICOLON  */
#line 502 "parser.y"
      {
          ASTNode* n = new_node("comp_assign");
          strcpy(n->name, (yyvsp[-3].str));
          strcpy(n->op, (yyvsp[-2].str));
          n->left = (yyvsp[-1].node);
          (yyval.node) = n;
      }
#line 1754 "parser.tab.c"
    break;

  case 21: /* assignment_no_semicolon: IDENTIFIER ASSIGN expression  */
#line 513 "parser.y"
      {
          ASTNode* n = new_node("assign");
          strcpy(n->name, (yyvsp[-2].str));
          n->left = (yyvsp[0].node);
          (yyval.node) = n;
      }
#line 1765 "parser.tab.c"
    break;

  case 22: /* assignment_no_semicolon: IDENTIFIER COMP_ASSIGN expression  */
#line 520 "parser.y"
      {
          ASTNode* n = new_node("comp_assign");
          strcpy(n->name, (yyvsp[-2].str));
          strcpy(n->op, (yyvsp[-1].str));
          n->left = (yyvsp[0].node);
          (yyval.node) = n;
      }
#line 1777 "parser.tab.c"
    break;

  case 23: /* print_stmt: PRINT LPAREN expression RPAREN SEMICOLON  */
#line 531 "parser.y"
      {
          ASTNode* n = new_node("print");
          n->left = (yyvsp[-2].node);
          (yyval.node) = n;
      }
#line 1787 "parser.tab.c"
    break;

  case 24: /* return_stmt: RETURN expression SEMICOLON  */
#line 540 "parser.y"
      {
          ASTNode* n = new_node("return");
          n->left = (yyvsp[-1].node);
          (yyval.node) = n;
      }
#line 1797 "parser.tab.c"
    break;

  case 25: /* expression: INT_LITERAL  */
#line 549 "parser.y"
      {
          ASTNode* n = new_node("int");
          n->value = atoi((yyvsp[0].str));
          (yyval.node) = n;
      }
#line 1807 "parser.tab.c"
    break;

  case 26: /* expression: FLOAT_LITERAL  */
#line 555 "parser.y"
      {
          ASTNode* n = new_node("decimal");
          n->dvalue = atof((yyvsp[0].str));
          (yyval.node) = n;
      }
#line 1817 "parser.tab.c"
    break;

  case 27: /* expression: BOOL_LITERAL  */
#line 561 "parser.y"
      {
          ASTNode* n = new_node("bool");
          n->bvalue = (strcmp((yyvsp[0].str), "true") == 0) ? 1 : 0;
          (yyval.node) = n;
      }
#line 1827 "parser.tab.c"
    break;

  case 28: /* expression: STRING_LITERAL  */
#line 567 "parser.y"
      {
          ASTNode* n = new_node("string");
          strcpy(n->name, (yyvsp[0].str));
          (yyval.node) = n;
      }
#line 1837 "parser.tab.c"
    break;

  case 29: /* expression: IDENTIFIER  */
#line 573 "parser.y"
      {
          ASTNode* n = new_node("var");
          strcpy(n->name, (yyvsp[0].str));
          (yyval.node) = n;
      }
#line 1847 "parser.tab.c"
    break;

  case 30: /* expression: function_call  */
#line 579 "parser.y"
      {
          (yyval.node) = (yyvsp[0].node);
      }
#line 1855 "parser.tab.c"
    break;

  case 31: /* expression: LPAREN expression RPAREN  */
#line 583 "parser.y"
      {
          (yyval.node) = (yyvsp[-1].node);
      }
#line 1863 "parser.tab.c"
    break;

  case 32: /* expression: expression PLUS expression  */
#line 587 "parser.y"
      {
          ASTNode* n = new_node("add");
          n->left = (yyvsp[-2].node);
          n->right = (yyvsp[0].node);
          (yyval.node) = n;
      }
#line 1874 "parser.tab.c"
    break;

  case 33: /* expression: expression MINUS expression  */
#line 594 "parser.y"
      {
          ASTNode* n = new_node("sub");
          n->left = (yyvsp[-2].node);
          n->right = (yyvsp[0].node);
          (yyval.node) = n;
      }
#line 1885 "parser.tab.c"
    break;

  case 34: /* expression: expression MULT expression  */
#line 601 "parser.y"
      {
          ASTNode* n = new_node("mul");
          n->left = (yyvsp[-2].node);
          n->right = (yyvsp[0].node);
          (yyval.node) = n;
      }
#line 1896 "parser.tab.c"
    break;

  case 35: /* expression: expression DIV expression  */
#line 608 "parser.y"
      {
          ASTNode* n = new_node("div");
          n->left = (yyvsp[-2].node);
          n->right = (yyvsp[0].node);
          (yyval.node) = n;
      }
#line 1907 "parser.tab.c"
    break;

  case 36: /* expression: expression COMP expression  */
#line 615 "parser.y"
      {
          ASTNode* n = new_node("comp");
          n->left = (yyvsp[-2].node);
          n->right = (yyvsp[0].node);
          strcpy(n->op, (yyvsp[-1].str));
          (yyval.node) = n;
      }
#line 1919 "parser.tab.c"
    break;

  case 37: /* function_call: IDENTIFIER LPAREN opt_arg_list RPAREN  */
#line 626 "parser.y"
    {
        ASTNode* n = new_node("func_call");
        strcpy(n->name, (yyvsp[-3].str));
        n->children = (ASTNode**) malloc(sizeof(ASTNode*));
        n->children[0] = (yyvsp[-1].node);
        n->child_count = 1;
        (yyval.node) = n;
    }
#line 1932 "parser.tab.c"
    break;

  case 38: /* opt_arg_list: arg_list  */
#line 638 "parser.y"
      {
          (yyval.node) = (yyvsp[0].node);
      }
#line 1940 "parser.tab.c"
    break;

  case 39: /* opt_arg_list: %empty  */
#line 642 "parser.y"
      {
          ASTNode* n = new_node("args");
          n->children = NULL;
          n->child_count = 0;
          (yyval.node) = n;
      }
#line 1951 "parser.tab.c"
    break;

  case 40: /* arg_list: arg_list COMMA expression  */
#line 652 "parser.y"
      {
          (yyvsp[-2].node)->children = realloc((yyvsp[-2].node)->children, sizeof(ASTNode*) * ((yyvsp[-2].node)->child_count + 1));
          (yyvsp[-2].node)->children[(yyvsp[-2].node)->child_count++] = (yyvsp[0].node);
          (yyval.node) = (yyvsp[-2].node);
      }
#line 1961 "parser.tab.c"
    break;

  case 41: /* arg_list: expression  */
#line 658 "parser.y"
      {
          ASTNode* n = new_node("args");
          n->children = (ASTNode**) malloc(sizeof(ASTNode*));
          n->children[0] = (yyvsp[0].node);
          n->child_count = 1;
          (yyval.node) = n;
      }
#line 1973 "parser.tab.c"
    break;

  case 42: /* from_to_loop: IDENTIFIER FROM expression TO expression IN LPAREN assignment_no_semicolon RPAREN LBRACE loop_body RBRACE  */
#line 669 "parser.y"
    {
        ASTNode* n = new_node("from_to");
        strcpy(n->name, (yyvsp[-11].str));
        n->left = (yyvsp[-9].node);
        n->right = (yyvsp[-7].node);

        n->children = (ASTNode**) malloc(sizeof(ASTNode*) * 2);
        n->children[0] = (yyvsp[-4].node);
        n->children[1] = (yyvsp[-1].node);
        n->child_count = 2;

        (yyval.node) = n;
    }
#line 1991 "parser.tab.c"
    break;

  case 43: /* loop_body: statements  */
#line 686 "parser.y"
    {
        (yyval.node) = (yyvsp[0].node);
    }
#line 1999 "parser.tab.c"
    break;

  case 44: /* when_block: WHEN LBRACE when_clauses RBRACE  */
#line 693 "parser.y"
    {
        ASTNode* n = new_node("when");
        n->children = (ASTNode**) malloc(sizeof(ASTNode*) * (yyvsp[-1].node)->child_count);
        for (int i = 0; i < (yyvsp[-1].node)->child_count; i++) {
            n->children[i] = (yyvsp[-1].node)->children[i];
        }
        n->child_count = (yyvsp[-1].node)->child_count;
        (yyval.node) = n;
    }
#line 2013 "parser.tab.c"
    break;

  case 45: /* when_clauses: when_clauses when_clause  */
#line 706 "parser.y"
      {
          (yyvsp[-1].node)->children = realloc((yyvsp[-1].node)->children, sizeof(ASTNode*) * ((yyvsp[-1].node)->child_count + 1));
          (yyvsp[-1].node)->children[(yyvsp[-1].node)->child_count++] = (yyvsp[0].node);
          (yyval.node) = (yyvsp[-1].node);
      }
#line 2023 "parser.tab.c"
    break;

  case 46: /* when_clauses: when_clause  */
#line 712 "parser.y"
      {
          ASTNode* n = new_node("when_clauses");
          n->children = (ASTNode**) malloc(sizeof(ASTNode*));
          n->children[0] = (yyvsp[0].node);
          n->child_count = 1;
          (yyval.node) = n;
      }
#line 2035 "parser.tab.c"
    break;

  case 47: /* when_clause: IS LPAREN expression RPAREN LBRACE statements RBRACE  */
#line 723 "parser.y"
      {
          ASTNode* n = new_node("when_clause");
          n->left = (yyvsp[-4].node);
          n->children = (ASTNode**) malloc(sizeof(ASTNode*));
          n->children[0] = (yyvsp[-1].node);
          n->child_count = 1;
          (yyval.node) = n;
      }
#line 2048 "parser.tab.c"
    break;

  case 48: /* when_clause: OTHERWISE LBRACE statements RBRACE  */
#line 732 "parser.y"
      {
          ASTNode* n = new_node("otherwise");
          n->children = (ASTNode**) malloc(sizeof(ASTNode*));
          n->children[0] = (yyvsp[-1].node);
          n->child_count = 1;
          (yyval.node) = n;
      }
#line 2060 "parser.tab.c"
    break;

  case 49: /* bound_block: BOUND LPAREN expression RPAREN LBRACE bound_clauses RBRACE  */
#line 743 "parser.y"
    {
        ASTNode* n = new_node("bound");
        n->left = (yyvsp[-4].node);
        n->children = (ASTNode**) malloc(sizeof(ASTNode*) * (yyvsp[-1].node)->child_count);
        for (int i = 0; i < (yyvsp[-1].node)->child_count; i++) {
            n->children[i] = (yyvsp[-1].node)->children[i];
        }
        n->child_count = (yyvsp[-1].node)->child_count;
        (yyval.node) = n;
    }
#line 2075 "parser.tab.c"
    break;

  case 50: /* bound_clauses: bound_clauses bound_clause  */
#line 757 "parser.y"
      {
          (yyvsp[-1].node)->children = realloc((yyvsp[-1].node)->children, sizeof(ASTNode*) * ((yyvsp[-1].node)->child_count + 1));
          (yyvsp[-1].node)->children[(yyvsp[-1].node)->child_count++] = (yyvsp[0].node);
          (yyval.node) = (yyvsp[-1].node);
      }
#line 2085 "parser.tab.c"
    break;

  case 51: /* bound_clauses: bound_clause  */
#line 763 "parser.y"
      {
          ASTNode* n = new_node("bound_clauses");
          n->children = (ASTNode**) malloc(sizeof(ASTNode*));
          n->children[0] = (yyvsp[0].node);
          n->child_count = 1;
          (yyval.node) = n;
      }
#line 2097 "parser.tab.c"
    break;

  case 52: /* bound_clause: IN LPAREN expression COMMA expression RPAREN LBRACE statements RBRACE  */
#line 774 "parser.y"
      {
          ASTNode* n = new_node("in_clause");
          n->left = (yyvsp[-6].node);
          n->right = (yyvsp[-4].node);
          n->children = (ASTNode**) malloc(sizeof(ASTNode*));
          n->children[0] = (yyvsp[-1].node);
          n->child_count = 1;
          (yyval.node) = n;
      }
#line 2111 "parser.tab.c"
    break;

  case 53: /* bound_clause: ABOVE LPAREN expression RPAREN LBRACE statements RBRACE  */
#line 784 "parser.y"
      {
          ASTNode* n = new_node("above_clause");
          n->left = (yyvsp[-4].node);
          n->children = (ASTNode**) malloc(sizeof(ASTNode*));
          n->children[0] = (yyvsp[-1].node);
          n->child_count = 1;
          (yyval.node) = n;
      }
#line 2124 "parser.tab.c"
    break;

  case 54: /* bound_clause: BELOW LPAREN expression RPAREN LBRACE statements RBRACE  */
#line 793 "parser.y"
      {
          ASTNode* n = new_node("below_clause");
          n->left = (yyvsp[-4].node);
          n->children = (ASTNode**) malloc(sizeof(ASTNode*));
          n->children[0] = (yyvsp[-1].node);
          n->child_count = 1;
          (yyval.node) = n;
      }
#line 2137 "parser.tab.c"
    break;

  case 55: /* bound_clause: EQUALS LPAREN expression RPAREN LBRACE statements RBRACE  */
#line 802 "parser.y"
      {
          ASTNode* n = new_node("equals_clause");
          n->left = (yyvsp[-4].node);
          n->children = (ASTNode**) malloc(sizeof(ASTNode*));
          n->children[0] = (yyvsp[-1].node);
          n->child_count = 1;
          (yyval.node) = n;
      }
#line 2150 "parser.tab.c"
    break;

  case 56: /* until_loop: UNTIL LPAREN expression RPAREN LBRACE statements RBRACE  */
#line 814 "parser.y"
    {
        ASTNode* n = new_node("until");
        n->left = (yyvsp[-4].node);
        n->children = (ASTNode**) malloc(sizeof(ASTNode*));
        n->children[0] = (yyvsp[-1].node);
        n->child_count = 1;
        (yyval.node) = n;
    }
#line 2163 "parser.tab.c"
    break;

  case 57: /* function_decl: FUNCTION IDENTIFIER LPAREN opt_param_list RPAREN RETURNS type LBRACE statements RBRACE  */
#line 826 "parser.y"
    {
        ASTNode* n = new_node("function");
        strcpy(n->name, (yyvsp[-8].str));
        n->var_type = (yyvsp[-3].num);

        n->children = (ASTNode**) malloc(sizeof(ASTNode*) * 2);
        n->children[0] = (yyvsp[-6].node);
        n->children[1] = (yyvsp[-1].node);
        n->child_count = 2;

        (yyval.node) = n;
    }
#line 2180 "parser.tab.c"
    break;

  case 58: /* opt_param_list: param_list  */
#line 842 "parser.y"
      {
          (yyval.node) = (yyvsp[0].node);
      }
#line 2188 "parser.tab.c"
    break;

  case 59: /* opt_param_list: %empty  */
#line 846 "parser.y"
      {
          ASTNode* n = new_node("params");
          n->children = NULL;
          n->child_count = 0;
          (yyval.node) = n;
      }
#line 2199 "parser.tab.c"
    break;

  case 60: /* param_list: param_list COMMA param_decl  */
#line 856 "parser.y"
      {
          (yyvsp[-2].node)->children = realloc((yyvsp[-2].node)->children, sizeof(ASTNode*) * ((yyvsp[-2].node)->child_count + 1));
          (yyvsp[-2].node)->children[(yyvsp[-2].node)->child_count++] = (yyvsp[0].node);
          (yyval.node) = (yyvsp[-2].node);
      }
#line 2209 "parser.tab.c"
    break;

  case 61: /* param_list: param_decl  */
#line 862 "parser.y"
      {
          ASTNode* n = new_node("params");
          n->children = (ASTNode**) malloc(sizeof(ASTNode*));
          n->children[0] = (yyvsp[0].node);
          n->child_count = 1;
          (yyval.node) = n;
      }
#line 2221 "parser.tab.c"
    break;

  case 62: /* param_decl: type IDENTIFIER  */
#line 873 "parser.y"
    {
        ASTNode* n = new_node("param");
        strcpy(n->name, (yyvsp[0].str));
        n->var_type = (yyvsp[-1].num);
        (yyval.node) = n;
    }
#line 2232 "parser.tab.c"
    break;


#line 2236 "parser.tab.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 881 "parser.y"


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
