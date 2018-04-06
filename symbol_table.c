#include "symbol_table.h"
#define SYM_LEN 31
#define TABLE_SIZE 100

sym_node* sym_table[TABLE_SIZE];
int sym_on = 0;

void make_sym_table()
{
	int i;
	for(i = 0; i < TABLE_SIZE; i++) sym_table[i] = NULL;
	
	add_symbol(0x0, "A");
	add_symbol(0x1, "X");
	add_symbol(0x2, "L");
	add_symbol(0x8, "PC");
	add_symbol(0x9, "SW");
	add_symbol(0x3, "B");
	add_symbol(0x4, "S");
	add_symbol(0x5, "T");
	add_symbol(0x6, "F");
	sym_on = 1;
}

void clear_sym_table()
{
	int i;
	for(i = 0; i < TABLE_SIZE; i++)
	{
		sym_node* it = sym_table[i];
		while(it != NULL)
		{
			sym_node* tmp = it->next;
			free(it);
			it = tmp;
		}
	}
	sym_on = 0;
}

void add_symbol(int addr, char symbol[SYM_LEN])
{
	int idx = get_sym_hash(symbol);

	sym_node* n_node = (sym_node*)malloc(sizeof(sym_node));
	n_node->addr = addr;
	strcpy(n_node->symbol, symbol);
	n_node->next = sym_table[idx];
	sym_table[idx] = n_node;
}

int get_sym_hash(char key[SYM_LEN])
{
	long long number = 0;
	
	while(*key) number += (long long)*key++;
	return (int)(number % TABLE_SIZE);
}

int get_addr(char key[SYM_LEN])
{
	int idx = get_sym_hash(key);
	
	sym_node* it = sym_table[idx];
	for(; it != NULL; it = it->next)
		if(!strcmp(key, it->symbol)) return it->addr;
	return -1;
}
