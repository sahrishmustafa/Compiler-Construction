#include "ast.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ASTNode* create_string_node(char *value) {
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = AST_STRING;
    node->string_value = strdup(value);
    return node;
}

ASTNode* create_number_node(char *value) {
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = AST_NUMBER;
    node->number_value = strdup(value);
    return node;
}

ASTNode* create_bool_node(int value) {
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = AST_BOOL;
    node->bool_value = value;
    return node;
}

ASTNode* create_null_node() {
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = AST_NULL;
    return node;
}

ASTNode* create_pair_node(char *key, ASTNode *value) {
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = AST_PAIR;
    node->pair.key = strdup(key);
    node->pair.value = value;
    return node;
}

ASTNode* create_object_node(ASTNode **pairs, int count) {
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = AST_OBJECT;
    node->object.pairs = pairs;
    node->object.count = count;
    return node;
}

ASTNode* create_array_node(ASTNode **elements, int count) {
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = AST_ARRAY;
    node->array.elements = elements;
    node->array.count = count;
    return node;
}

void print_indent(int n) {
    for (int i = 0; i < n; i++) printf("  ");
}

void print_ast(ASTNode *node, int indent) {
    if (!node) return;

    switch (node->type) {
        case AST_STRING:
            print_indent(indent); printf("STRING: \"%s\"\n", node->string_value); break;
        case AST_NUMBER:
            print_indent(indent); printf("NUMBER: %s\n", node->number_value); break;
        case AST_BOOL:
            print_indent(indent); printf("BOOLEAN: %s\n", node->bool_value ? "true" : "false"); break;
        case AST_NULL:
            print_indent(indent); printf("NULL\n"); break;
        case AST_PAIR:
            print_indent(indent); printf("PAIR: \"%s\"\n", node->pair.key);
            print_ast(node->pair.value, indent + 1); break;
        case AST_OBJECT:
            print_indent(indent); printf("OBJECT (%d pairs):\n", node->object.count);
            for (int i = 0; i < node->object.count; i++) {
                print_ast(node->object.pairs[i], indent + 1);
            }
            break;
        case AST_ARRAY:
            print_indent(indent); printf("ARRAY (%d elements):\n", node->array.count);
            for (int i = 0; i < node->array.count; i++) {
                print_ast(node->array.elements[i], indent + 1);
            }
            break;
    }
}

void free_ast(ASTNode *node) {
    if (!node) return;

    switch (node->type) {
        case AST_STRING: free(node->string_value); break;
        case AST_NUMBER: free(node->number_value); break;
        case AST_PAIR:
            free(node->pair.key);
            free_ast(node->pair.value);
            break;
        case AST_OBJECT:
            for (int i = 0; i < node->object.count; i++) {
                free_ast(node->object.pairs[i]);
            }
            free(node->object.pairs);
            break;
        case AST_ARRAY:
            for (int i = 0; i < node->array.count; i++) {
                free_ast(node->array.elements[i]);
            }
            free(node->array.elements);
            break;
        default: break;
    }

    free(node);
}
