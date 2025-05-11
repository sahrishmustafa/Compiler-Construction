#ifndef AST_H
#define AST_H

typedef enum {
    AST_OBJECT,
    AST_ARRAY,
    AST_STRING,
    AST_NUMBER,
    AST_BOOL,
    AST_NULL,
    AST_PAIR
} ASTNodeType;

typedef struct ASTNode {
    ASTNodeType type;

    union {
        struct {
            char *key;
            struct ASTNode *value;
        } pair;

        struct {
            struct ASTNode **pairs;
            int count;
        } object;

        struct {
            struct ASTNode **elements;
            int count;
        } array;

        char *string_value;
        char *number_value;
        int bool_value;
    };
} ASTNode;

// Constructors
ASTNode* create_string_node(char *value);
ASTNode* create_number_node(char *value);
ASTNode* create_bool_node(int value);
ASTNode* create_null_node();
ASTNode* create_pair_node(char *key, ASTNode *value);
ASTNode* create_object_node(ASTNode **pairs, int count);
ASTNode* create_array_node(ASTNode **elements, int count);

// Helpers
void print_ast(ASTNode *node, int indent);
void free_ast(ASTNode *node);

#endif
