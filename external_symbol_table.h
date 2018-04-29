#ifndef __EXTERNAL_SYMBOL_TABLE_H__
#define __EXTERNAL_SYMBOL_TABLE_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define SYM_LEN 7

typedef struct _ext_node
{
	int addr;
	int len;
	char section[SYM_LEN];
	char symbol[SYM_LEN];
	struct _ext_node* next;
} ext_node;

void make_ext_table();

void clear_ext_table();

void add_ext_symbol(int addr, int len, char section[SYM_LEN], char symbol[SYM_LEN]);

int get_ext_hash(char key[SYM_LEN]);

int get_ext_addr(char key[SYM_LEN]);

void print_ext_table();

#endif
