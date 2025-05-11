# Tools
CC = gcc
FLEX = flex
BISON = bison

# Files
LEX = scanner.l
YACC = parser.y
YACC_C = parser.tab.c
YACC_H = parser.tab.h
LEX_C = lex.yy.c
EXEC = json2relcsv

SRCS = $(LEX_C) $(YACC_C) ast.c csvgen.c main.c

.PHONY: all clean run

all: $(EXEC)

$(YACC_C) $(YACC_H): $(YACC)
	$(BISON) -d $(YACC)

$(LEX_C): $(LEX)
	$(FLEX) $(LEX)

$(EXEC): $(SRCS)
	$(CC) $(SRCS) -o $(EXEC)

run: all
	./$(EXEC) < input.json --print-ast

clean:
	rm -f $(EXEC) $(LEX_C) $(YACC_C) $(YACC_H) *.o *.csv
