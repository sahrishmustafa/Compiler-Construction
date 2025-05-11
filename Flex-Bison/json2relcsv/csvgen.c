#include "csvgen.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

static int next_id = 1;

typedef struct Table {
    char name[64];
    FILE *fp;
    int header_written;
} Table;

static Table tables[100];
static int table_count = 0;

static Table* get_table(const char *name) {
    for (int i = 0; i < table_count; i++) {
        if (strcmp(tables[i].name, name) == 0) return &tables[i];
    }

    Table *t = &tables[table_count++];
    strcpy(t->name, name);

    char filename[128];
    snprintf(filename, sizeof(filename), "%s.csv", name);
    t->fp = fopen(filename, "w");
    if (!t->fp) {
        perror("Could not create table");
        exit(1);
    }
    t->header_written = 0;
    return t;
}

// === Write header row ===
static void write_headers(Table *t, ASTNode *object, const char *fk) {
    if (t->header_written) return;
    fprintf(t->fp, "id");
    if (fk) fprintf(t->fp, ",%s_id", fk);

    for (int i = 0; i < object->object.count; i++) {
        const char *key = object->object.pairs[i]->pair.key;
        ASTNode *val = object->object.pairs[i]->pair.value;
        // Only include simple types in CSV
        if (val->type != AST_OBJECT && val->type != AST_ARRAY) {
            fprintf(t->fp, ",%s", key);
        } else if (val->type == AST_OBJECT) {
            fprintf(t->fp, ",%s_id", key);
        }
    }

    fprintf(t->fp, "\n");
    t->header_written = 1;
}

// === Write a single row ===
static void write_row(Table *t, ASTNode *object, int id, int parent_id, const char *fk) {
    fprintf(t->fp, "%d", id);
    if (fk) fprintf(t->fp, ",%d", parent_id);

    for (int i = 0; i < object->object.count; i++) {
        ASTNode *val = object->object.pairs[i]->pair.value;
        if (val->type == AST_STRING) fprintf(t->fp, ",%s", val->string_value);
        else if (val->type == AST_NUMBER) fprintf(t->fp, ",%s", val->number_value);
        else if (val->type == AST_BOOL) fprintf(t->fp, ",%s", val->bool_value ? "true" : "false");
        else if (val->type == AST_NULL) fprintf(t->fp, ",");
        else if (val->type == AST_OBJECT) {
            // nested object already replaced with AST_NUMBER representing child ID
            fprintf(t->fp, ",%s", val->number_value);
        } else {
            fprintf(t->fp, ",");  // skip arrays
        }
    }

    fprintf(t->fp, "\n");
}

static int process_object(ASTNode *object, const char *table, int parent_id, const char *fk);

static void process_array(ASTNode *array, const char *name, int parent_id) {
    for (int i = 0; i < array->array.count; i++) {
        ASTNode *elem = array->array.elements[i];
        if (elem->type == AST_OBJECT) {
            // Pass parent_id and key as FK to child
            process_object(elem, name, parent_id, "main");  // or use parent table name if you track it
        } else {
            // Scalar array → junction table
            Table *t = get_table(name);
            if (!t->header_written) {
                fprintf(t->fp, "parent_id,index,value\n");
                t->header_written = 1;
            }
            fprintf(t->fp, "%d,%d,", parent_id, i);
            if (elem->type == AST_STRING) fprintf(t->fp, "%s", elem->string_value);
            else if (elem->type == AST_NUMBER) fprintf(t->fp, "%s", elem->number_value);
            else if (elem->type == AST_BOOL) fprintf(t->fp, "%s", elem->bool_value ? "true" : "false");
            else fprintf(t->fp, "");
            fprintf(t->fp, "\n");
        }
    }
}


// === Process object recursively ===
static int process_object(ASTNode *object, const char *table, int parent_id, const char *fk) {
    int id = next_id++;
    Table *t = get_table(table);
    
    // First, recursively process nested children and replace them with ID nodes
    for (int i = 0; i < object->object.count; i++) {
        ASTNode *child = object->object.pairs[i]->pair.value;
        const char *key = object->object.pairs[i]->pair.key;

        if (child->type == AST_OBJECT) {
            int child_id = process_object(child, key, id, NULL);
            // Replace nested object with AST_NUMBER (FK)
            char buffer[16];
            snprintf(buffer, sizeof(buffer), "%d", child_id);
            object->object.pairs[i]->pair.value = create_number_node(strdup(buffer));
        } else if (child->type == AST_ARRAY) {
            process_array(child, key, id);
        }
    }

    write_headers(t, object, fk);
    write_row(t, object, id, parent_id, fk);
    return id;
}

void generate_csv_from_ast(ASTNode *root, const char *out_dir) {
    if (root && root->type == AST_OBJECT) {
        process_object(root, "main", 0, NULL);
    }
}
