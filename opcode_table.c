#include "opcode_table.h"
#define MAX_LEN 1001
#define TABLE_SIZE 20

// opcode hash table의 각 head 포인터
hash_node* hash_table[TABLE_SIZE];
hash_node op_arr[0xff];

// 요약: opcode hash table을 초기화하고 생성하는 함수
// 기능: opcode.txt를 파일입력으로 읽어들여서
// 	 	 hash table에 추가한다.
// 반환: 없음.
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

// 요약: hash table의 동적할당 된 메모리를 해제하는 함수
// 기능: 각 head의 노드들을 순차적으로 접근하여
// 		 동적할당 된 메모리를 해제한다.
// 반환: 없음.
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

// 요약: 새로운 opcode를 hash table에 추가하는 함수
// 기능: opcode, mnemonic, format을 입력받아
//    	 새로운 동적할당 된 노드를 초기화하고
//    	 mnemonic을 hashing하여 얻은 index에 해당하는
//    	 hash table의 basket에 노드를 추가한다.
//    	 충돌이 있을 경우 해당 노드의 다음 노드에 새 노드를 추가한다.
// 반환: 없음.
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

// 요약: key에 해당하는 hashing값을 반환하는 함수
// 기능: key 배열의 처음부터 끝까지 접근하며
// 		 값을 모두 더하고, table size로
// 		 나눈 나머지 값을 반환한다.
// 반환: hashing 값
int get_hash(char key[MAX_LEN])
{
	long long number = 0;
	
	while(*key) number += (long long)*key++;
	return (int)(number % TABLE_SIZE);
}

// 요약: key에 해당하는 opcode를 반환하는 함수
// 기능: key를 hashing하여 얻은 값을 기반으로
// 		 hash table을 탐색하여 opcode 값을 얻어 반환한다.
// 		 만약, key와 일치하는 노드가 존재하지 않을 경우
// 		 오류로 체크한다.
// 반환: -1 = 오류 / else = key에 해당하는 opcode
int get_opcode(char key[MAX_LEN])
{
	int idx = get_hash(key);
	
	hash_node* it = hash_table[idx];
	for(; it != NULL; it = it->next)
		if(!strcmp(key, it->mnemonic)) return it->opcode;
	return -1;
}

//요약: key에 해당하는 format을 반환하는 함수 
//기능: key를 hashing하여 얻은 값을 기반으로
//		hash table을 탐색하여 format 값을 얻어 반환한다.
//		만약, key와 일치하는 노드가 존재하지 않을 경우
//		오류로 체크한다.
//반환: -1 = 오류 / else  = key에 해당하는 format
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

// 요약: opcode를 출력하는 함수
// 기능: opcode를 출력한다.
// 반환: 없음.
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

// 요약: opcode hash table의 전체 상태를 출력하는 함수
// 기능: opcode hash table을 순서대로 접근하면서
// 		 format에 맞게 출력한다.
// 반환: 없음.
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
