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

Var symtab[100];
int symcount = 0;

ASTNode* root = NULL;

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

void add_variable(char* name, int type) {
    for (int i = 0; i < symcount; i++) {
        if (strcmp(symtab[i].name, name) == 0) {
            symtab[i].type = type;
            symtab[i].ivalue = 0;
            symtab[i].dvalue = 0.0;
            symtab[i].bvalue = 0;
            return;
        }
    }

    strcpy(symtab[symcount].name, name);
    symtab[symcount].type = type;
    symtab[symcount].ivalue = 0;
    symtab[symcount].dvalue = 0.0;
    symtab[symcount].bvalue = 0;
    symcount++;
}

Var* get_variable(char* name) {
    for (int i = 0; i < symcount; i++) {
        if (strcmp(symtab[i].name, name) == 0) {
            return &symtab[i];
        }
    }
    return NULL;
}

double eval(ASTNode* n);
void execute(ASTNode* n);

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

    if (strcmp(n->type, "statements") == 0) {
        for (int i = 0; i < n->child_count; i++) {
            execute(n->children[i]);
        }
        return;
    }

    if (strcmp(n->type, "decl") == 0) {
        add_variable(n->name, n->var_type);
        return;
    }

    if (strcmp(n->type, "assign") == 0) {
        Var* v = get_variable(n->name);
        if (!v) {
            printf("Error: variable %s not declared\n", n->name);
            return;
        }

        if (v->type == 3 && strcmp(n->left->type, "string") == 0) {
            printf("Error: cannot assign string to logical variable %s\n", n->name);
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

            if (n->child_count > 0 && n->children[0]) {
                execute(n->children[0]);
            }
            if (n->child_count > 1 && n->children[1]) {
                execute(n->children[1]);
            }
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
        }
        return;
    }
}


#line 364 "parser.tab.c"

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
  YYSYMBOL_BOOL_LITERAL = 19,              /* BOOL_LITERAL  */
  YYSYMBOL_IDENTIFIER = 20,                /* IDENTIFIER  */
  YYSYMBOL_STRING_LITERAL = 21,            /* STRING_LITERAL  */
  YYSYMBOL_INT_LITERAL = 22,               /* INT_LITERAL  */
  YYSYMBOL_FLOAT_LITERAL = 23,             /* FLOAT_LITERAL  */
  YYSYMBOL_ASSIGN = 24,                    /* ASSIGN  */
  YYSYMBOL_SEMICOLON = 25,                 /* SEMICOLON  */
  YYSYMBOL_COMMA = 26,                     /* COMMA  */
  YYSYMBOL_COMP = 27,                      /* COMP  */
  YYSYMBOL_COMP_ASSIGN = 28,               /* COMP_ASSIGN  */
  YYSYMBOL_PLUS = 29,                      /* PLUS  */
  YYSYMBOL_MINUS = 30,                     /* MINUS  */
  YYSYMBOL_MULT = 31,                      /* MULT  */
  YYSYMBOL_DIV = 32,                       /* DIV  */
  YYSYMBOL_LPAREN = 33,                    /* LPAREN  */
  YYSYMBOL_RPAREN = 34,                    /* RPAREN  */
  YYSYMBOL_LBRACE = 35,                    /* LBRACE  */
  YYSYMBOL_RBRACE = 36,                    /* RBRACE  */
  YYSYMBOL_PRINT = 37,                     /* PRINT  */
  YYSYMBOL_YYACCEPT = 38,                  /* $accept  */
  YYSYMBOL_program = 39,                   /* program  */
  YYSYMBOL_statements = 40,                /* statements  */
  YYSYMBOL_statement = 41,                 /* statement  */
  YYSYMBOL_declaration = 42,               /* declaration  */
  YYSYMBOL_type = 43,                      /* type  */
  YYSYMBOL_assignment = 44,                /* assignment  */
  YYSYMBOL_assignment_no_semicolon = 45,   /* assignment_no_semicolon  */
  YYSYMBOL_print_stmt = 46,                /* print_stmt  */
  YYSYMBOL_expression = 47,                /* expression  */
  YYSYMBOL_from_to_loop = 48,              /* from_to_loop  */
  YYSYMBOL_loop_body = 49,                 /* loop_body  */
  YYSYMBOL_when_block = 50,                /* when_block  */
  YYSYMBOL_when_clauses = 51,              /* when_clauses  */
  YYSYMBOL_when_clause = 52,               /* when_clause  */
  YYSYMBOL_bound_block = 53,               /* bound_block  */
  YYSYMBOL_bound_clauses = 54,             /* bound_clauses  */
  YYSYMBOL_bound_clause = 55,              /* bound_clause  */
  YYSYMBOL_until_loop = 56                 /* until_loop  */
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
#define YYFINAL  28
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   322

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  38
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  19
/* YYNRULES -- Number of rules.  */
#define YYNRULES  47
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  130

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   292


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
      35,    36,    37
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   330,   330,   338,   344,   355,   356,   357,   358,   359,
     360,   361,   365,   375,   376,   377,   378,   382,   389,   400,
     407,   418,   427,   433,   439,   445,   451,   457,   461,   468,
     475,   482,   489,   500,   517,   524,   537,   543,   554,   563,
     574,   588,   594,   605,   615,   624,   633,   645
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
  "SAFE", "BOUND", "ABOVE", "BELOW", "EQUALS", "UNTIL", "BOOL_LITERAL",
  "IDENTIFIER", "STRING_LITERAL", "INT_LITERAL", "FLOAT_LITERAL", "ASSIGN",
  "SEMICOLON", "COMMA", "COMP", "COMP_ASSIGN", "PLUS", "MINUS", "MULT",
  "DIV", "LPAREN", "RPAREN", "LBRACE", "RBRACE", "PRINT", "$accept",
  "program", "statements", "statement", "declaration", "type",
  "assignment", "assignment_no_semicolon", "print_stmt", "expression",
  "from_to_loop", "loop_body", "when_block", "when_clauses", "when_clause",
  "bound_block", "bound_clauses", "bound_clause", "until_loop", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-37)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     189,   -37,   -37,   -37,   -37,   -21,   -16,   -13,    29,    -9,
      21,   189,   -37,   -37,     7,   -37,   -37,   -37,   -37,   -37,
     -37,    -5,   208,   208,   208,   208,   208,   208,   -37,   -37,
      26,    22,    23,    28,   -37,   -37,   -37,   -37,   -37,   -37,
     208,   205,   213,   183,   221,   229,   235,   -37,   208,   189,
     -37,   -37,   241,   208,   208,   208,   208,   208,    24,    37,
     208,   -37,   -37,    36,   247,     5,   -37,   290,    12,    12,
     -37,   -37,    54,   189,   191,   -37,    39,   -37,    49,    50,
      59,    60,    13,   -37,    42,    61,   189,   208,   208,   208,
     208,   -37,   -37,   -37,    76,    85,   280,   253,   259,   265,
     -12,    63,   -37,   208,    65,    66,    67,   208,   208,    69,
     271,   189,   189,   189,   286,   286,   189,    71,   109,   129,
     149,   189,    62,   189,   -37,   -37,   -37,   -37,   169,   -37
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,    13,    14,    15,    16,     0,     0,     0,     0,     0,
       0,     2,     4,     5,     0,     6,     7,     8,     9,    10,
      11,     0,     0,     0,     0,     0,     0,     0,     1,     3,
       0,     0,     0,     0,    37,    24,    26,    25,    22,    23,
       0,     0,     0,     0,     0,     0,     0,    12,     0,     0,
      35,    36,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    17,    18,     0,     0,     0,    27,    32,    28,    29,
      30,    31,     0,     0,     0,    21,     0,    39,     0,     0,
       0,     0,     0,    42,     0,     0,     0,     0,     0,     0,
       0,    40,    41,    47,     0,     0,     0,     0,     0,     0,
       0,     0,    38,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    19,    20,     0,     0,     0,     0,
       0,    34,     0,     0,    44,    45,    46,    33,     0,    43
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -37,   -37,   -36,   -11,   -37,   -37,   -37,   -37,   -37,   -22,
     -37,   -37,   -37,   -37,    78,   -37,   -37,    34,   -37
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
       0,    10,    11,    12,    13,    14,    15,   101,    16,    41,
      17,   122,    18,    33,    34,    19,    82,    83,    20
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint8 yytable[] =
{
      29,    42,    43,    44,    45,    46,    31,    32,     1,     2,
       3,     4,   107,    65,    21,     5,   108,    22,    52,     6,
      23,    28,    78,     7,    27,     8,    64,    30,    79,    80,
      81,    67,    68,    69,    70,    71,    24,    84,    74,    31,
      32,    77,     9,    56,    57,     1,     2,     3,     4,    91,
      95,    47,     5,    25,    29,    48,     6,    26,    49,    72,
       7,    75,     8,    78,    50,    96,    97,    98,    99,    79,
      80,    81,    73,    29,    86,   118,   119,   120,    93,     9,
     121,   110,    87,    88,    29,   114,   115,   128,     1,     2,
       3,     4,    89,    90,    94,     5,   100,   109,   127,     6,
     111,   112,   113,     7,   116,     8,   123,    29,    29,    29,
      29,    51,     1,     2,     3,     4,    92,    29,     0,     5,
       0,   102,     9,     6,     0,     0,     0,     7,     0,     8,
       0,     0,     1,     2,     3,     4,     0,     0,     0,     5,
       0,     0,     0,     6,     0,   124,     9,     7,     0,     8,
       0,     0,     1,     2,     3,     4,     0,     0,     0,     5,
       0,     0,     0,     6,     0,   125,     9,     7,     0,     8,
       0,     0,     1,     2,     3,     4,     0,     0,     0,     5,
       0,     0,     0,     6,     0,   126,     9,     7,     0,     8,
       0,    60,     1,     2,     3,     4,     0,     0,     0,     5,
      85,     0,     0,     6,     0,   129,     9,     7,     0,     8,
      53,     0,    54,    55,    56,    57,     0,     0,    53,     0,
      54,    55,    56,    57,     0,     0,     9,    35,    36,    37,
      38,    39,    53,     0,    54,    55,    56,    57,     0,    58,
      53,    40,    54,    55,    56,    57,    61,    59,    53,     0,
      54,    55,    56,    57,    62,     0,    53,     0,    54,    55,
      56,    57,    53,     0,    54,    55,    56,    57,    53,    63,
      54,    55,    56,    57,    53,    66,    54,    55,    56,    57,
      53,    76,    54,    55,    56,    57,    53,   104,    54,    55,
      56,    57,    53,   105,    54,    55,    56,    57,    53,   106,
      54,    55,    56,    57,     0,   117,   103,    53,     0,    54,
      55,    56,    57,    53,     0,    54,    55,    56,    57,    54,
      55,    56,    57
};

static const yytype_int16 yycheck[] =
{
      11,    23,    24,    25,    26,    27,    11,    12,     3,     4,
       5,     6,    24,    49,    35,    10,    28,    33,    40,    14,
      33,     0,     9,    18,    33,    20,    48,    20,    15,    16,
      17,    53,    54,    55,    56,    57,     7,    73,    60,    11,
      12,    36,    37,    31,    32,     3,     4,     5,     6,    36,
      86,    25,    10,    24,    65,    33,    14,    28,    35,    35,
      18,    25,    20,     9,    36,    87,    88,    89,    90,    15,
      16,    17,    35,    84,    35,   111,   112,   113,    36,    37,
     116,   103,    33,    33,    95,   107,   108,   123,     3,     4,
       5,     6,    33,    33,    33,    10,    20,    34,    36,    14,
      35,    35,    35,    18,    35,    20,    35,   118,   119,   120,
     121,    33,     3,     4,     5,     6,    82,   128,    -1,    10,
      -1,    36,    37,    14,    -1,    -1,    -1,    18,    -1,    20,
      -1,    -1,     3,     4,     5,     6,    -1,    -1,    -1,    10,
      -1,    -1,    -1,    14,    -1,    36,    37,    18,    -1,    20,
      -1,    -1,     3,     4,     5,     6,    -1,    -1,    -1,    10,
      -1,    -1,    -1,    14,    -1,    36,    37,    18,    -1,    20,
      -1,    -1,     3,     4,     5,     6,    -1,    -1,    -1,    10,
      -1,    -1,    -1,    14,    -1,    36,    37,    18,    -1,    20,
      -1,     8,     3,     4,     5,     6,    -1,    -1,    -1,    10,
       9,    -1,    -1,    14,    -1,    36,    37,    18,    -1,    20,
      27,    -1,    29,    30,    31,    32,    -1,    -1,    27,    -1,
      29,    30,    31,    32,    -1,    -1,    37,    19,    20,    21,
      22,    23,    27,    -1,    29,    30,    31,    32,    -1,    34,
      27,    33,    29,    30,    31,    32,    25,    34,    27,    -1,
      29,    30,    31,    32,    25,    -1,    27,    -1,    29,    30,
      31,    32,    27,    -1,    29,    30,    31,    32,    27,    34,
      29,    30,    31,    32,    27,    34,    29,    30,    31,    32,
      27,    34,    29,    30,    31,    32,    27,    34,    29,    30,
      31,    32,    27,    34,    29,    30,    31,    32,    27,    34,
      29,    30,    31,    32,    -1,    34,    26,    27,    -1,    29,
      30,    31,    32,    27,    -1,    29,    30,    31,    32,    29,
      30,    31,    32
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     3,     4,     5,     6,    10,    14,    18,    20,    37,
      39,    40,    41,    42,    43,    44,    46,    48,    50,    53,
      56,    35,    33,    33,     7,    24,    28,    33,     0,    41,
      20,    11,    12,    51,    52,    19,    20,    21,    22,    23,
      33,    47,    47,    47,    47,    47,    47,    25,    33,    35,
      36,    52,    47,    27,    29,    30,    31,    32,    34,    34,
       8,    25,    25,    34,    47,    40,    34,    47,    47,    47,
      47,    47,    35,    35,    47,    25,    34,    36,     9,    15,
      16,    17,    54,    55,    40,     9,    35,    33,    33,    33,
      33,    36,    55,    36,    33,    40,    47,    47,    47,    47,
      20,    45,    36,    26,    34,    34,    34,    24,    28,    34,
      47,    35,    35,    35,    47,    47,    35,    34,    40,    40,
      40,    40,    49,    35,    36,    36,    36,    36,    40,    36
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    38,    39,    40,    40,    41,    41,    41,    41,    41,
      41,    41,    42,    43,    43,    43,    43,    44,    44,    45,
      45,    46,    47,    47,    47,    47,    47,    47,    47,    47,
      47,    47,    47,    48,    49,    50,    51,    51,    52,    52,
      53,    54,    54,    55,    55,    55,    55,    56
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     3,     1,     1,     1,     1,     4,     4,     3,
       3,     5,     1,     1,     1,     1,     1,     3,     3,     3,
       3,     3,     3,    12,     1,     4,     2,     1,     7,     4,
       7,     2,     1,     9,     7,     7,     7,     7
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
#line 331 "parser.y"
    {
        root = (yyvsp[0].node);
        (yyval.node) = (yyvsp[0].node);
    }
#line 1522 "parser.tab.c"
    break;

  case 3: /* statements: statements statement  */
#line 339 "parser.y"
      {
          (yyvsp[-1].node)->children = realloc((yyvsp[-1].node)->children, sizeof(ASTNode*) * ((yyvsp[-1].node)->child_count + 1));
          (yyvsp[-1].node)->children[(yyvsp[-1].node)->child_count++] = (yyvsp[0].node);
          (yyval.node) = (yyvsp[-1].node);
      }
#line 1532 "parser.tab.c"
    break;

  case 4: /* statements: statement  */
#line 345 "parser.y"
      {
          ASTNode* n = new_node("statements");
          n->children = (ASTNode**) malloc(sizeof(ASTNode*));
          n->children[0] = (yyvsp[0].node);
          n->child_count = 1;
          (yyval.node) = n;
      }
#line 1544 "parser.tab.c"
    break;

  case 12: /* declaration: type IDENTIFIER SEMICOLON  */
#line 366 "parser.y"
      {
          ASTNode* n = new_node("decl");
          strcpy(n->name, (yyvsp[-1].str));
          n->var_type = (yyvsp[-2].num);
          (yyval.node) = n;
      }
#line 1555 "parser.tab.c"
    break;

  case 13: /* type: INTEGER  */
#line 375 "parser.y"
              { (yyval.num) = 1; }
#line 1561 "parser.tab.c"
    break;

  case 14: /* type: DECIMAL  */
#line 376 "parser.y"
              { (yyval.num) = 2; }
#line 1567 "parser.tab.c"
    break;

  case 15: /* type: LOGICAL  */
#line 377 "parser.y"
              { (yyval.num) = 3; }
#line 1573 "parser.tab.c"
    break;

  case 16: /* type: NOTHING  */
#line 378 "parser.y"
              { (yyval.num) = 4; }
#line 1579 "parser.tab.c"
    break;

  case 17: /* assignment: IDENTIFIER ASSIGN expression SEMICOLON  */
#line 383 "parser.y"
      {
          ASTNode* n = new_node("assign");
          strcpy(n->name, (yyvsp[-3].str));
          n->left = (yyvsp[-1].node);
          (yyval.node) = n;
      }
#line 1590 "parser.tab.c"
    break;

  case 18: /* assignment: IDENTIFIER COMP_ASSIGN expression SEMICOLON  */
#line 390 "parser.y"
      {
          ASTNode* n = new_node("comp_assign");
          strcpy(n->name, (yyvsp[-3].str));
          strcpy(n->op, (yyvsp[-2].str));
          n->left = (yyvsp[-1].node);
          (yyval.node) = n;
      }
#line 1602 "parser.tab.c"
    break;

  case 19: /* assignment_no_semicolon: IDENTIFIER ASSIGN expression  */
#line 401 "parser.y"
      {
          ASTNode* n = new_node("assign");
          strcpy(n->name, (yyvsp[-2].str));
          n->left = (yyvsp[0].node);
          (yyval.node) = n;
      }
#line 1613 "parser.tab.c"
    break;

  case 20: /* assignment_no_semicolon: IDENTIFIER COMP_ASSIGN expression  */
#line 408 "parser.y"
      {
          ASTNode* n = new_node("comp_assign");
          strcpy(n->name, (yyvsp[-2].str));
          strcpy(n->op, (yyvsp[-1].str));
          n->left = (yyvsp[0].node);
          (yyval.node) = n;
      }
#line 1625 "parser.tab.c"
    break;

  case 21: /* print_stmt: PRINT LPAREN expression RPAREN SEMICOLON  */
#line 419 "parser.y"
      {
          ASTNode* n = new_node("print");
          n->left = (yyvsp[-2].node);
          (yyval.node) = n;
      }
#line 1635 "parser.tab.c"
    break;

  case 22: /* expression: INT_LITERAL  */
#line 428 "parser.y"
      {
          ASTNode* n = new_node("int");
          n->value = atoi((yyvsp[0].str));
          (yyval.node) = n;
      }
#line 1645 "parser.tab.c"
    break;

  case 23: /* expression: FLOAT_LITERAL  */
#line 434 "parser.y"
      {
          ASTNode* n = new_node("decimal");
          n->dvalue = atof((yyvsp[0].str));
          (yyval.node) = n;
      }
#line 1655 "parser.tab.c"
    break;

  case 24: /* expression: BOOL_LITERAL  */
#line 440 "parser.y"
      {
          ASTNode* n = new_node("bool");
          n->bvalue = (strcmp((yyvsp[0].str), "true") == 0) ? 1 : 0;
          (yyval.node) = n;
      }
#line 1665 "parser.tab.c"
    break;

  case 25: /* expression: STRING_LITERAL  */
#line 446 "parser.y"
      {
          ASTNode* n = new_node("string");
          strcpy(n->name, (yyvsp[0].str));
          (yyval.node) = n;
      }
#line 1675 "parser.tab.c"
    break;

  case 26: /* expression: IDENTIFIER  */
#line 452 "parser.y"
      {
          ASTNode* n = new_node("var");
          strcpy(n->name, (yyvsp[0].str));
          (yyval.node) = n;
      }
#line 1685 "parser.tab.c"
    break;

  case 27: /* expression: LPAREN expression RPAREN  */
#line 458 "parser.y"
      {
          (yyval.node) = (yyvsp[-1].node);
      }
#line 1693 "parser.tab.c"
    break;

  case 28: /* expression: expression PLUS expression  */
#line 462 "parser.y"
      {
          ASTNode* n = new_node("add");
          n->left = (yyvsp[-2].node);
          n->right = (yyvsp[0].node);
          (yyval.node) = n;
      }
#line 1704 "parser.tab.c"
    break;

  case 29: /* expression: expression MINUS expression  */
#line 469 "parser.y"
      {
          ASTNode* n = new_node("sub");
          n->left = (yyvsp[-2].node);
          n->right = (yyvsp[0].node);
          (yyval.node) = n;
      }
#line 1715 "parser.tab.c"
    break;

  case 30: /* expression: expression MULT expression  */
#line 476 "parser.y"
      {
          ASTNode* n = new_node("mul");
          n->left = (yyvsp[-2].node);
          n->right = (yyvsp[0].node);
          (yyval.node) = n;
      }
#line 1726 "parser.tab.c"
    break;

  case 31: /* expression: expression DIV expression  */
#line 483 "parser.y"
      {
          ASTNode* n = new_node("div");
          n->left = (yyvsp[-2].node);
          n->right = (yyvsp[0].node);
          (yyval.node) = n;
      }
#line 1737 "parser.tab.c"
    break;

  case 32: /* expression: expression COMP expression  */
#line 490 "parser.y"
      {
          ASTNode* n = new_node("comp");
          n->left = (yyvsp[-2].node);
          n->right = (yyvsp[0].node);
          strcpy(n->op, (yyvsp[-1].str));
          (yyval.node) = n;
      }
#line 1749 "parser.tab.c"
    break;

  case 33: /* from_to_loop: IDENTIFIER FROM expression TO expression IN LPAREN assignment_no_semicolon RPAREN LBRACE loop_body RBRACE  */
#line 501 "parser.y"
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
#line 1767 "parser.tab.c"
    break;

  case 34: /* loop_body: statements  */
#line 518 "parser.y"
    {
        (yyval.node) = (yyvsp[0].node);
    }
#line 1775 "parser.tab.c"
    break;

  case 35: /* when_block: WHEN LBRACE when_clauses RBRACE  */
#line 525 "parser.y"
    {
        ASTNode* n = new_node("when");
        n->children = (ASTNode**) malloc(sizeof(ASTNode*) * (yyvsp[-1].node)->child_count);
        for (int i = 0; i < (yyvsp[-1].node)->child_count; i++) {
            n->children[i] = (yyvsp[-1].node)->children[i];
        }
        n->child_count = (yyvsp[-1].node)->child_count;
        (yyval.node) = n;
    }
#line 1789 "parser.tab.c"
    break;

  case 36: /* when_clauses: when_clauses when_clause  */
#line 538 "parser.y"
      {
          (yyvsp[-1].node)->children = realloc((yyvsp[-1].node)->children, sizeof(ASTNode*) * ((yyvsp[-1].node)->child_count + 1));
          (yyvsp[-1].node)->children[(yyvsp[-1].node)->child_count++] = (yyvsp[0].node);
          (yyval.node) = (yyvsp[-1].node);
      }
#line 1799 "parser.tab.c"
    break;

  case 37: /* when_clauses: when_clause  */
#line 544 "parser.y"
      {
          ASTNode* n = new_node("when_clauses");
          n->children = (ASTNode**) malloc(sizeof(ASTNode*));
          n->children[0] = (yyvsp[0].node);
          n->child_count = 1;
          (yyval.node) = n;
      }
#line 1811 "parser.tab.c"
    break;

  case 38: /* when_clause: IS LPAREN expression RPAREN LBRACE statements RBRACE  */
#line 555 "parser.y"
      {
          ASTNode* n = new_node("when_clause");
          n->left = (yyvsp[-4].node);
          n->children = (ASTNode**) malloc(sizeof(ASTNode*));
          n->children[0] = (yyvsp[-1].node);
          n->child_count = 1;
          (yyval.node) = n;
      }
#line 1824 "parser.tab.c"
    break;

  case 39: /* when_clause: OTHERWISE LBRACE statements RBRACE  */
#line 564 "parser.y"
      {
          ASTNode* n = new_node("otherwise");
          n->children = (ASTNode**) malloc(sizeof(ASTNode*));
          n->children[0] = (yyvsp[-1].node);
          n->child_count = 1;
          (yyval.node) = n;
      }
#line 1836 "parser.tab.c"
    break;

  case 40: /* bound_block: BOUND LPAREN expression RPAREN LBRACE bound_clauses RBRACE  */
#line 575 "parser.y"
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
#line 1851 "parser.tab.c"
    break;

  case 41: /* bound_clauses: bound_clauses bound_clause  */
#line 589 "parser.y"
      {
          (yyvsp[-1].node)->children = realloc((yyvsp[-1].node)->children, sizeof(ASTNode*) * ((yyvsp[-1].node)->child_count + 1));
          (yyvsp[-1].node)->children[(yyvsp[-1].node)->child_count++] = (yyvsp[0].node);
          (yyval.node) = (yyvsp[-1].node);
      }
#line 1861 "parser.tab.c"
    break;

  case 42: /* bound_clauses: bound_clause  */
#line 595 "parser.y"
      {
          ASTNode* n = new_node("bound_clauses");
          n->children = (ASTNode**) malloc(sizeof(ASTNode*));
          n->children[0] = (yyvsp[0].node);
          n->child_count = 1;
          (yyval.node) = n;
      }
#line 1873 "parser.tab.c"
    break;

  case 43: /* bound_clause: IN LPAREN expression COMMA expression RPAREN LBRACE statements RBRACE  */
#line 606 "parser.y"
      {
          ASTNode* n = new_node("in_clause");
          n->left = (yyvsp[-6].node);
          n->right = (yyvsp[-4].node);
          n->children = (ASTNode**) malloc(sizeof(ASTNode*));
          n->children[0] = (yyvsp[-1].node);
          n->child_count = 1;
          (yyval.node) = n;
      }
#line 1887 "parser.tab.c"
    break;

  case 44: /* bound_clause: ABOVE LPAREN expression RPAREN LBRACE statements RBRACE  */
#line 616 "parser.y"
      {
          ASTNode* n = new_node("above_clause");
          n->left = (yyvsp[-4].node);
          n->children = (ASTNode**) malloc(sizeof(ASTNode*));
          n->children[0] = (yyvsp[-1].node);
          n->child_count = 1;
          (yyval.node) = n;
      }
#line 1900 "parser.tab.c"
    break;

  case 45: /* bound_clause: BELOW LPAREN expression RPAREN LBRACE statements RBRACE  */
#line 625 "parser.y"
      {
          ASTNode* n = new_node("below_clause");
          n->left = (yyvsp[-4].node);
          n->children = (ASTNode**) malloc(sizeof(ASTNode*));
          n->children[0] = (yyvsp[-1].node);
          n->child_count = 1;
          (yyval.node) = n;
      }
#line 1913 "parser.tab.c"
    break;

  case 46: /* bound_clause: EQUALS LPAREN expression RPAREN LBRACE statements RBRACE  */
#line 634 "parser.y"
      {
          ASTNode* n = new_node("equals_clause");
          n->left = (yyvsp[-4].node);
          n->children = (ASTNode**) malloc(sizeof(ASTNode*));
          n->children[0] = (yyvsp[-1].node);
          n->child_count = 1;
          (yyval.node) = n;
      }
#line 1926 "parser.tab.c"
    break;

  case 47: /* until_loop: UNTIL LPAREN expression RPAREN LBRACE statements RBRACE  */
#line 646 "parser.y"
    {
        ASTNode* n = new_node("until");
        n->left = (yyvsp[-4].node);
        n->children = (ASTNode**) malloc(sizeof(ASTNode*));
        n->children[0] = (yyvsp[-1].node);
        n->child_count = 1;
        (yyval.node) = n;
    }
#line 1939 "parser.tab.c"
    break;


#line 1943 "parser.tab.c"

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

#line 656 "parser.y"


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
        printf("Code is VALID.\n");
        execute(root);
    } else {
        printf("Code is INVALID.\n");
    }

    return 0;
}
