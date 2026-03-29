/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_PARSER_TAB_H_INCLUDED
# define YY_YY_PARSER_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    INTEGER = 258,                 /* INTEGER  */
    DECIMAL = 259,                 /* DECIMAL  */
    LOGICAL = 260,                 /* LOGICAL  */
    NOTHING = 261,                 /* NOTHING  */
    FROM = 262,                    /* FROM  */
    TO = 263,                      /* TO  */
    IN = 264,                      /* IN  */
    WHEN = 265,                    /* WHEN  */
    IS = 266,                      /* IS  */
    OTHERWISE = 267,               /* OTHERWISE  */
    SAFE = 268,                    /* SAFE  */
    BOUND = 269,                   /* BOUND  */
    ABOVE = 270,                   /* ABOVE  */
    BELOW = 271,                   /* BELOW  */
    EQUALS = 272,                  /* EQUALS  */
    UNTIL = 273,                   /* UNTIL  */
    FUNCTION = 274,                /* FUNCTION  */
    RETURNS = 275,                 /* RETURNS  */
    RETURN = 276,                  /* RETURN  */
    BOOL_LITERAL = 277,            /* BOOL_LITERAL  */
    IDENTIFIER = 278,              /* IDENTIFIER  */
    STRING_LITERAL = 279,          /* STRING_LITERAL  */
    INT_LITERAL = 280,             /* INT_LITERAL  */
    FLOAT_LITERAL = 281,           /* FLOAT_LITERAL  */
    ASSIGN = 282,                  /* ASSIGN  */
    SEMICOLON = 283,               /* SEMICOLON  */
    COMMA = 284,                   /* COMMA  */
    COMP = 285,                    /* COMP  */
    COMP_ASSIGN = 286,             /* COMP_ASSIGN  */
    PLUS = 287,                    /* PLUS  */
    MINUS = 288,                   /* MINUS  */
    MULT = 289,                    /* MULT  */
    DIV = 290,                     /* DIV  */
    LPAREN = 291,                  /* LPAREN  */
    RPAREN = 292,                  /* RPAREN  */
    LBRACE = 293,                  /* LBRACE  */
    RBRACE = 294,                  /* RBRACE  */
    PRINT = 295,                   /* PRINT  */
    SCAN = 296                     /* SCAN  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 466 "parser.y"

    char* str;
    int num;
    double dnum;
    int bval;
    ASTNode* node;

#line 113 "parser.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_PARSER_TAB_H_INCLUDED  */
