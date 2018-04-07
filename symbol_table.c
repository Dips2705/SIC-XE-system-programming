#include "symbol_table.h"
#define SYM_LEN 31
#define REG_NUM 9 
#define TABLE_SIZE 100
#define MAX_SYMBOL_SIZE 100001

sym_node* sym_table[TABLE_SIZE];
sym_node sym_arr[MAX_SYMBOL_SIZE];
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
		sym_table[i] = NULL;
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

void symbol_()
{
	int i, j, idx = 0;
	if(!sym_on)
	{
		printf("\tSymbol Table is empty.\n");
		return;
	}
	for(i = 0; i < TABLE_SIZE; i++)
	{
		sym_node* it;
		for(it = sym_table[i]; it != NULL; it = it->next)
			if(!is_register(it->symbol))
			{
				strcpy(sym_arr[idx].symbol, it->symbol);
				sym_arr[idx++].addr = it->addr;
			}
	}
	for(i = idx - 1; i > 0; i--)
		for(j = 0; j < i; j++)
			if(strcmp(sym_arr[j].symbol, sym_arr[j+1].symbol) > 0)
			{
				char tmp_s[SYM_LEN];
				int tmp_a;

				strcpy(tmp_s, sym_arr[j].symbol);
				tmp_a = sym_arr[j].addr;	
				strcpy(sym_arr[j].symbol, sym_arr[j+1].symbol);
				sym_arr[j].addr = sym_arr[j+1].addr;
				strcpy(sym_arr[j+1].symbol, tmp_s);
				sym_arr[j+1].addr = tmp_a;
			}
	for(i = 0; i < idx; i++) printf("\t%s\t%05X\n", sym_arr[i].symbol, sym_arr[i].addr);
}
