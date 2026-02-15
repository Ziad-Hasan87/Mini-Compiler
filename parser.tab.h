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
    SYMBOL = 260,                  /* SYMBOL  */
    LOGICAL = 261,                 /* LOGICAL  */
    NOTHING = 262,                 /* NOTHING  */
    FUNCTION = 263,                /* FUNCTION  */
    RETURNS = 264,                 /* RETURNS  */
    WHEN = 265,                    /* WHEN  */
    IS = 266,                      /* IS  */
    OTHERWISE = 267,               /* OTHERWISE  */
    SAFE = 268,                    /* SAFE  */
    FROM = 269,                    /* FROM  */
    TO = 270,                      /* TO  */
    IN = 271,                      /* IN  */
    BOUND = 272,                   /* BOUND  */
    XBOUND = 273,                  /* XBOUND  */
    ABOVE = 274,                   /* ABOVE  */
    BELOW = 275,                   /* BELOW  */
    EQUALS = 276,                  /* EQUALS  */
    UNTIL = 277,                   /* UNTIL  */
    BOOL_LITERAL = 278,            /* BOOL_LITERAL  */
    FLOAT_LITERAL = 279,           /* FLOAT_LITERAL  */
    INT_LITERAL = 280,             /* INT_LITERAL  */
    IDENTIFIER = 281,              /* IDENTIFIER  */
    COMP = 282,                    /* COMP  */
    COMP_ASSIGN = 283,             /* COMP_ASSIGN  */
    ASSIGN = 284,                  /* ASSIGN  */
    SEMICOLON = 285,               /* SEMICOLON  */
    COMMA = 286,                   /* COMMA  */
    LPAREN = 287,                  /* LPAREN  */
    RPAREN = 288,                  /* RPAREN  */
    LBRACE = 289,                  /* LBRACE  */
    RBRACE = 290,                  /* RBRACE  */
    PLUS = 291,                    /* PLUS  */
    MINUS = 292,                   /* MINUS  */
    MULT = 293,                    /* MULT  */
    DIV = 294,                     /* DIV  */
    STRING_LITERAL = 295,          /* STRING_LITERAL  */
    PRINT = 296                    /* PRINT  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_PARSER_TAB_H_INCLUDED  */
