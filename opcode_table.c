#include "opcode_table.h"
#define MAX_LEN 1001
#define TABLE_SIZE 20

hash_node* hash_table[TABLE_SIZE];

void make_table()
{
	int i;
	FILE* fp = fopen("./opcode.txt", "r");
	int opcode;
	char mnemonic[MAX_LEN];
	char format[MAX_LEN];
	
	for(i = 0; i < TABLE_SIZE; i++) hash_table[i] = NULL;
	while(fscanf(fp, "%X %s %s", &opcode, mnemonic, format) != EOF)
		add_table(opcode, mnemonic, format);
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
	}
}

void add_table(int opcode, char mnemonic[MAX_LEN], char format[MAX_LEN])
{
	int idx = get_hash(mnemonic);
	hash_node* it = hash_table[idx];
	hash_node* prev;

	hash_node* n_node = (hash_node*)malloc(sizeof(hash_node));
	n_node->opcode = opcode;
	strcpy(n_node->mnemonic, mnemonic);
	strcpy(n_node->format, format);	
	n_node->next = NULL;
	
	if(it == NULL)
	{
		hash_table[idx] = n_node;
	   	return;
	}
	while(it != NULL)
	{
		prev = it;
		it = it->next;
	}
	prev->next = n_node;
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
