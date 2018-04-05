#ifndef __SYMBOL_TABLE_H__
#define __SYMBOL_TABLE_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define SYM_LEN 31

typedef struct _sym_node
{
	int addr;
	char symbol[SYM_LEN];
	struct _sym_node* next;
} sym_node;

void make_sym_table();

void clear_sym_table();

void add_symbol(int addr, char symbol[SYM_LEN]);

int get_sym_hash(char key[SYM_LEN]);

int get_addr(char key[SYM_LEN]);

#endif
