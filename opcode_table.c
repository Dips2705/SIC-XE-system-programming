#include "opcode_table.h"
#define MAX_LEN 1001
#define TABLE_SIZE 20

hash_node* hash_table[TABLE_SIZE];
hash_node op_arr[0xff];

void make_table()
{
	int i;
	FILE* fp = fopen("./opcode.txt", "r");
	int opcode;
	char mnemonic[MAX_LEN];
	int format, tmp;
	
	for(i = 0; i < TABLE_SIZE; i++) hash_table[i] = NULL;
	while(fscanf(fp, "%X %s %d/%d", &opcode, mnemonic, &format, &tmp) != EOF)
	{
		add_table(opcode, mnemonic, format);
		strcpy(op_arr[opcode].mnemonic, mnemonic);
		op_arr[opcode].opcode = opcode;
		op_arr[opcode].format = format;
	}
	fclose(fp);
}

void clear_table()
{
	int i;
	for(i = 0; i < TABLE_SIZE; i++)
	{
		hash_node* it = hash_table[i];
		while(it != NULL)
		{
			hash_node* tmp = it->next;
			free(it);
			it = tmp;
		}
		hash_table[i] = NULL;
	}
}

void add_table(int opcode, char mnemonic[MAX_LEN], int format)
{
	int idx = get_hash(mnemonic);

	hash_node* n_node = (hash_node*)malloc(sizeof(hash_node));
	n_node->opcode = opcode;
	strcpy(n_node->mnemonic, mnemonic);
	n_node->format = format;
	n_node->next = hash_table[idx];
	hash_table[idx] = n_node;
}

int get_hash(char key[MAX_LEN])
{
	long long number = 0;
	
	while(*key) number += (long long)*key++;
	return (int)(number % TABLE_SIZE);
}

int get_opcode(char key[MAX_LEN])
{
	int idx = get_hash(key);
	
	hash_node* it = hash_table[idx];
	for(; it != NULL; it = it->next)
		if(!strcmp(key, it->mnemonic)) return it->opcode;
	return -1;
}

int get_format(char key[MAX_LEN])
{
	int idx = get_hash(key);
	
	hash_node* it = hash_table[idx];
	for(; it != NULL; it = it->next)
		if(!strcmp(key, it->mnemonic)) return it->format;
	return -1;
}

int get_format_by_opcode(int opcode)
{
	return op_arr[opcode].format;
}

void opcode_(int opcode)
{
	printf("\t");
	printf("%c[1;34m", 27);
	printf("opcode");
	printf("%c[0m", 27);
    printf(" is ");
	printf("%c[1;32m", 27);
	printf("%X\n", opcode);
	printf("%c[0m", 27);
}

void opcode_list_()
{
	int i;
	for(i = 0; i < TABLE_SIZE; i++)
	{
		int flag = 0;
		hash_node* it = hash_table[i];
		
		printf("\t");
		printf("%c[1;33m", 27);
		printf("%2d", i);
		printf("%c[0m", 27);
		printf(" : ");
		for(; it != NULL; it = it->next)
		{
			if(flag) printf(" -> ");
			else flag = 1;
			printf("[");
			printf("%c[1;34m", 27);
			printf("%s", it->mnemonic);
			printf("%c[0m", 27);
			printf(",");
			printf("%c[1;32m", 27);
			printf("%X", it->opcode);
			printf("%c[0m", 27);
			printf("]");
		}
		printf("\n");
	}
}
