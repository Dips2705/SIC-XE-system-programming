#include "external_symbol_table.h"
#define SYM_LEN 7
#define TABLE_SIZE 100
#define MAX_SYMBOL_SIZE 100001

ext_node* ext_table[TABLE_SIZE];
ext_node ext_arr[MAX_SYMBOL_SIZE];

void make_ext_table()
{
	int i;
	for(i = 0; i < TABLE_SIZE; i++) ext_table[i] = NULL;
}

void clear_ext_table()
{
	int i;
	for(i = 0; i < TABLE_SIZE; i++)
	{
		ext_node* it = ext_table[i];
		while(it != NULL)
		{
			ext_node* tmp = it->next;
			free(it);
			it = tmp;
		}
		ext_table[i] = NULL;
	}
}

void add_ext_symbol(int addr, int len, char section[SYM_LEN], char symbol[SYM_LEN])
{
	int idx = get_ext_hash(symbol);

	ext_node* n_node = (ext_node*)malloc(sizeof(ext_node));
	n_node->addr = addr;
	n_node->len = len;
	strcpy(n_node->section, section);
	strcpy(n_node->symbol, symbol);
	n_node->next = ext_table[idx];
	ext_table[idx] = n_node;
}

int get_ext_hash(char key[SYM_LEN])
{
	long long number = 0;
	
	while(*key) number += (long long)*key++;
	return (int)(number % TABLE_SIZE);
}

int get_ext_addr(char key[SYM_LEN])
{
	int idx = get_ext_hash(key);
	
	ext_node* it;
	for(it = ext_table[idx]; it != NULL; it = it->next)
		if(!strcmp(key, it->symbol)) return it->addr;
	return -1;
}

void print_ext_table()
{
	int i, j, idx = 0, total_len = 0;
	for(i = 0; i < TABLE_SIZE; i++)
	{
		ext_node* it;
		for(it = ext_table[i]; it != NULL; it = it->next)
		{
			ext_arr[idx].addr = it->addr;
			ext_arr[idx].len = it->len;
			strcpy(ext_arr[idx].section, it->section);
			strcpy(ext_arr[idx].symbol, it->symbol);
			idx++;
		}
	}
	// sorting
	for(i = idx - 1; i > 0; i--)
		for(j = 0; j < i; j++)
			if(ext_arr[j].addr > ext_arr[j+1].addr)
			{
				char tmp_sec[SYM_LEN], tmp_sym[SYM_LEN];
				int tmp_a, tmp_l;
				
				tmp_a = ext_arr[j].addr;
				tmp_l = ext_arr[j].len;
				strcpy(tmp_sec, ext_arr[j].section);
				strcpu(tmp_sym, ext_arr[j].symbol);
				
				ext_arr[j].addr = ext_arr[j+1].addr;
				ext_arr[j].len = ext_arr[j+1].len;
				strcpy(ext_arr[j].section, ext_arr[j+1].section);
				strcpy(ext_arr[j].symbol, ext_arr[j+1].symbol);

				ext_arr[j+1].addr = tmp_a;
				ext_arr[j+1].len = tmp_l;
				strcpy(ext_arr[j+1].section, tmp_sec);
				strcpy(ext_arr[j+1].symbol, tmp_sym);
			}
	// print
	printf("control\tsymbol\taddress\tlength\n");
	printf("section\tname\n");
	printf("--------------------------------------\n");
	for(i = 0; i < idx; i++)
	{
		if(!strcmp(ext_arr[i].section, ext_arr[i].symbol))
		{
			total_len += ext_arr[i].len;
			printf("%s\t%s\t%04X\t%04X\n",ext_arr[i].section, "      ", ext_arr[i].addr, ext_arr[i].len);
		}
		else printf("%s\t%s\t%04X\n", "      ", ext_arr[i].symbol, ext_arr[i].addr);
	}
	printf("--------------------------------------\n");
	printf("\t\t\t\t\ttotal length\t%04X\n", total_len);
}
