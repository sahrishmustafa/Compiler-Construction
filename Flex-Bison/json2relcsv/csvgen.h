#ifndef CSVGEN_H
#define CSVGEN_H

#include "ast.h"

void generate_csv_from_ast(ASTNode *root, const char *out_dir);

#endif
