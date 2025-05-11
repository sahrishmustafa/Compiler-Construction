#include <stdio.h>
#include "ast.h"
#include "csvgen.h"
#include <string.h>

extern int yyparse(void);
extern ASTNode *root;

int main(int argc, char *argv[]) {
    if (yyparse() == 0) {
        printf("Parsing completed.\n");

        int print_Ast = 0;
        const char *out_dir = ".";

        for (int i = 1; i < argc; i++) {
            if (strcmp(argv[i], "--print-ast") == 0) print_Ast = 1;
            if (strcmp(argv[i], "--out-dir") == 0 && i + 1 < argc) out_dir = argv[++i];
        }

        if (print_Ast) {
            printf("\n=== AST ===\n");
            print_ast(root, 0);
        }

        generate_csv_from_ast(root, out_dir);
        free_ast(root);
    } else {
        printf("Parsing failed.\n");
    }

    return 0;
}
