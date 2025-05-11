%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

ASTNode *root = NULL;
extern int line, column;

int yylex(void);
void yyerror(const char *msg);
%}

%union {
    char *str;
    struct ASTNode *node;
}

%token <str> STRING
%token <str> NUMBER
%token TRUE FALSE NULL_TOK
%token COLON COMMA LBRACE RBRACE LBRACKET RBRACKET

%type <node> value object array pair members elements json
%start json


%%

json:
    value { root = $1; }
    ;

value:
      STRING       { $$ = create_string_node($1); free($1); }
    | NUMBER       { $$ = create_number_node($1); free($1); }
    | TRUE         { $$ = create_bool_node(1); }
    | FALSE        { $$ = create_bool_node(0); }
    | NULL_TOK     { $$ = create_null_node(); }
    | object       { $$ = $1; }
    | array        { $$ = $1; }
    ;

pair:
    STRING COLON value {
        $$ = create_pair_node($1, $3);
        free($1);
    }
    ;

members:
    pair {
        ASTNode **list = malloc(sizeof(ASTNode*));
        list[0] = $1;
        $$ = create_object_node(list, 1);
    }
    | members COMMA pair {
        int new_count = $1->object.count + 1;
        $1->object.pairs = realloc($1->object.pairs, sizeof(ASTNode*) * new_count);
        $1->object.pairs[new_count - 1] = $3;
        $1->object.count = new_count;
        $$ = $1;
    }
    ;

object:
    LBRACE members RBRACE  { $$ = $2; }
    | LBRACE RBRACE        { $$ = create_object_node(NULL, 0); }
    ;

elements:
    value {
        ASTNode **list = malloc(sizeof(ASTNode*));
        list[0] = $1;
        $$ = create_array_node(list, 1);
    }
    | elements COMMA value {
        int new_count = $1->array.count + 1;
        $1->array.elements = realloc($1->array.elements, sizeof(ASTNode*) * new_count);
        $1->array.elements[new_count - 1] = $3;
        $1->array.count = new_count;
        $$ = $1;
    }
    ;

array:
    LBRACKET elements RBRACKET  { $$ = $2; }
    | LBRACKET RBRACKET         { $$ = create_array_node(NULL, 0); }
    ;


%%

void yyerror(const char *msg) {
    extern int yychar;

    const char *token_name;
    switch (yychar) {
        case STRING: token_name = "STRING"; break;
        case NUMBER: token_name = "NUMBER"; break;
        case TRUE: token_name = "true"; break;
        case FALSE: token_name = "false"; break;
        case NULL_TOK: token_name = "null"; break;
        case LBRACE: token_name = "{"; break;
        case RBRACE: token_name = "}"; break;
        case LBRACKET: token_name = "["; break;
        case RBRACKET: token_name = "]"; break;
        case COLON: token_name = ":"; break;
        case COMMA: token_name = ","; break;
        case 0: token_name = "end of file"; break;
        default: token_name = "unknown token"; break;
    }

    fprintf(stderr, "Syntax error at line %d, column %d: unexpected %s\n", line, column, token_name);
    exit(1);
}


extern int yylex(void);
extern int line, column;
