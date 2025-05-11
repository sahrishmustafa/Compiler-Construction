%{
#include "parser.tab.h"  // Contains token definitions from Bison
#include <stdlib.h>
#include <string.h>

int line = 1, column = 1;
%}

%option noyywrap

%%

[ \t\r]+            { column += yyleng; }         // Ignore whitespace
\n                  { line++; column = 1; }        // Track newlines

"true"              { column += yyleng; return TRUE; }
"false"             { column += yyleng; return FALSE; }
"null"              { column += yyleng; return NULL_TOK; }

\"([^"\\]|\\[nrtbfn\"\\/]|\\u[0-9a-fA-F]{4})*\" {
    int len = strlen(yytext);
    char *stripped = malloc(len - 1);
    strncpy(stripped, yytext + 1, len - 2);
    stripped[len - 2] = '\0';
    yylval.str = stripped;
    column += yyleng;
    return STRING;
}

\"([^"\\]|\\.)* {
    fprintf(stderr, "Lexical error at line %d, column %d: Unclosed string\n", line, column);
    exit(1);
}


[0-9]+(\.[0-9]+)?([eE][-+]?[0-9]+)? {
                      yylval.str = strdup(yytext);
                      column += yyleng;
                      return NUMBER;
                    }

":"                 { column++; return COLON; }
","                 { column++; return COMMA; }
"{"                 { column++; return LBRACE; }
"}"                 { column++; return RBRACE; }
"["                 { column++; return LBRACKET; }
"]"                 { column++; return RBRACKET; }

.                   {
                      fprintf(stderr, "Lexical error at line %d, column %d: Unexpected character '%c'\n", line, column, yytext[0]);
                      exit(1);
                    }

%%

