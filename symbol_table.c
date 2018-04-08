#include "symbol_table.h"
#define SYM_LEN 31
#define REG_NUM 9 
#define TABLE_SIZE 100
#define MAX_SYMBOL_SIZE 100001

// symbol table의 헤더
sym_node* sym_table[TABLE_SIZE];
// symbol table 출력용 배열
sym_node sym_arr[MAX_SYMBOL_SIZE];
// stmbol table on/off
int sym_on = 0;

//요약: symbol table을 생성하는 함수
//기능: symbol table의 헤더를 초기화하고
//		레지스터 이름을 symbol로 추가한다.
//반환: 없음.
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

//요약: symbol table을 초기화하는 함수 
//기능: symbol table의 노드에 차례대로
//		접근하여 symbol table에 사용 된
//		메모리를 해제한다.
//반환: 없음.
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

//요약: symbol을 추가하는 함수 
//기능: symbol 문자열과 그에 해당하는 주소값을
//		입력받아서 symbol을 hashing하여 해당하는
//		헤더에 추가한다.
//반환: 없음.
void add_symbol(int addr, char symbol[SYM_LEN])
{
	int idx = get_sym_hash(symbol);

	sym_node* n_node = (sym_node*)malloc(sizeof(sym_node));
	n_node->addr = addr;
	strcpy(n_node->symbol, symbol);
	n_node->next = sym_table[idx];
	sym_table[idx] = n_node;
}

//요약: symbol table의 hashing 함수 
//기능: key값을 입력받아서 hashing하고
//		그 값을 반환한다.
//반환: hashing value
int get_sym_hash(char key[SYM_LEN])
{
	long long number = 0;
	
	while(*key) number += (long long)*key++;
	return (int)(number % TABLE_SIZE);
}

//요약: symbol에 해당하는 주소값을 반환하는 함수
//기능: key를 입력받아 hashing하고
//		key에 해당하는 주소값을 반한한다.
//		해당하는 symbol이 없다면 -1을 반환한다.
//반환: sybmol에 해당하는 주소값 / error = -1
int get_addr(char key[SYM_LEN])
{
	int idx = get_sym_hash(key);
	
	sym_node* it = sym_table[idx];
	for(; it != NULL; it = it->next)
		if(!strcmp(key, it->symbol)) return it->addr;
	return -1;
}

//요약: symbol들을 모두 출력하는 함수
//기능: symbol을 내림차순으로 정렬하여
//		차례대로 출력한다.
//반환: 없음.
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
			if(strcmp(sym_arr[j].symbol, sym_arr[j+1].symbol) < 0)
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
	for(i = 0; i < idx; i++)
	{
		printf("%c[1;34m", 27);
		printf("\t%s", sym_arr[i].symbol);
		printf("%c[1;32m", 27);
		printf("\t%05X\n", sym_arr[i].addr);
		printf("%c[0m", 27);
	}
}
