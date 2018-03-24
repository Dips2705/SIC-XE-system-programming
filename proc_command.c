#include "proc_command.h"
#define MAX_LEN 1001
#define MAX_TOKEN 1001
#define MAX_VAL 256
#define MAX_ADDR 1048576
#define NUM_OF_COMM 17

enum commands
{
	_H,
	_HELP,
	_D,
	_DIR,
	_Q,
	_QUIT,
	_HI,
	_HISTORY,
	_DU,
	_DUMP,
	_E,
	_EDIT,
	_F,
	_FILL,
	_RESET,
	_OPCODE,
	_OPCODE_LIST,
};

const char* command_list[NUM_OF_COMM] = 
{
	"h",
	"help",
	"d",
	"dir",
	"q",
	"quit",
	"hi",
	"history",
	"du",
	"dump",
	"e",
	"edit",
	"f",
	"fill",
	"reset",
	"opcode",
	"opcodelist",
};

int run(int comm, int para1, int para2, int para3, int token_size, char token[MAX_TOKEN][MAX_LEN])
{
	switch(comm)
	{	
		case _H:	
		case _HELP:
			help_();
			break;
		case _D:
		case _DIR:
			dir_();
			break;
		case _Q:
		case _QUIT:
			return 1;	
			break;
		case _HI:
		case _HISTORY:
			history_();
			break;
		case _DU:
		case _DUMP:
			dump_(para1, para2);
			break;
		case _E:
		case _EDIT:
			edit_(para1, para2);
			break;
		case _F:
		case _FILL:
			fill_(para1, para2, para3);
			break;
		case _RESET:
		    reset_();	
			break;
		case _OPCODE:
		    opcode_(para1);	
			break;
		case _OPCODE_LIST:
			opcode_list_();
			break;
		default:
			break;
	}
	return 0;
}

int tokenize(char* str, char token[MAX_TOKEN][MAX_LEN])
{
	int i, j, len = strlen(str);
	int x = 0, y = 0, z = 0;
	int flag = 0;
	int s[5] = {0}, e[5] = {0};
	
	for(i = 0; i < len; i++)
	{
		if(str[i] != ' ' && str[i] != ',' && str[i] != '\t' && str[i] != '\n')
		{
			if(x >= 4) return -2;
			if(!flag) e[z++] = i;
			token[x][y++] = str[i], flag = 1;
		}
		else if(flag)
		{
			s[z] = i;
			token[x++][y] = '\0';
			y = 0;
			flag = 0;
		}
	}

	if(s[z] != len-1)
		for(i = s[z]; i < len; i++)
			if(str[i] != ' ' && str[i] != '\t' && str[i] != '\n') return -2;
	
	for(i = 0; i < z; i++)
	{
		int comma = 0;
		for(j = s[i]; j < e[i]; j++)
			if(str[j] == ',') comma++;
		if(i == 0 && comma != 0) return -1;
		if(i == 1 && comma != 0) return -2;
		if((i != 0 && i != 1) && comma != 1) return -2;
	}
	return x;
}

int char_to_hexa(char c)
{
	int val = -1;
	if('0' <= c && c <= '9') val = (int)(c - '0');
	if('A' <= c && c <= 'F') val = (int)(c - 'A' + 10);
	if('a' <= c && c <= 'f') val = (int)(c - 'a' + 10);
	return val;
}

int str_to_val(char* str, int addr_mode)
{	
	int ret = 0;
	int i, len = strlen(str), val;
	int boundary = MAX_VAL;

	if(addr_mode) boundary = MAX_ADDR;

	for(i = 0; i < len; i++)
	{
		val = char_to_hexa(str[i]);
		if(val == -1) return -1;
	}
	for(i = 0; i < len; i++)
	{
		val = char_to_hexa(str[i]);
		ret *= 16;
		ret += val;
		if(ret >= boundary) return -2;
	}
	return ret;
}

int make_command(char* str, int* comm, int* para1, int* para2, int* para3, int* token_size, char token[MAX_TOKEN][MAX_LEN])
{
	int comm_num, p1 = -1, p2 = -1, p3 = -1, tsz = 0;

	tsz = tokenize(str, token);
	
	for(comm_num = 0; comm_num < NUM_OF_COMM; comm_num++)
		if(!strcmp(token[0], command_list[comm_num])) break;
	*comm = comm_num;

	if(!tsz) return 1565;
	else if(comm_num >= NUM_OF_COMM || !strlen(token[0]) || tsz == -1) return 1;
	else if(tsz == -2) return 2;
	
	else if(comm_num == _DU || comm_num == _DUMP)
	{
		if(tsz > 3) return 2;
		else if(tsz == 2)
		{
			p1 = str_to_val(token[1], 1);
			if(p1 == -1) return 2;
			else if(p1 == -2) return 3;
		}
		else if(tsz == 3)
		{
			p1 = str_to_val(token[1], 1);
			p2 = str_to_val(token[2], 1);

			if(p1 == -1 || p2 == -1) return 2;
			else if(p1 == -2 || p2 == -2) return 3;
			else if(p1 > p2) return 4;
		}
	}
	else if(comm_num == _E || comm_num == _EDIT)
	{
		if(tsz != 3) return 2;

		p1 = str_to_val(token[1], 1);
		p2 = str_to_val(token[2], 0);

		if(p1 == -1 || p2 == -1) return 2;
		else if(p1 == -2 || p2 == -2) return 3;
	}
	else if(comm_num == _F || comm_num == _FILL)
	{
		if(tsz != 4) return 2;

		p1 = str_to_val(token[1], 1);
		p2 = str_to_val(token[2], 1);
		p3 = str_to_val(token[3], 0);

		if(p1 == -1 || p2 == -1 || p3 == -1) return 2;
		else if(p1 == -2 || p2 == -2 || p3 == -2) return 3;
		else if(p1 > p2) return 4;
	}
	else if(comm_num == _OPCODE)
	{
		p1 = get_opcode(token[1]);
		if(tsz != 2) return 2;
		if(p1 == -1) return 5;
	}
	else if(tsz > 1) return 2;
	
	*para1 = p1, *para2 = p2, *para3 = p3, *token_size = tsz;
	return 0;
}

void print_error(int error)
{
	switch(error)
	{
		case 1: printf("\tError: Invalid Command.\n"); break;
		case 2: printf("\tError: Invalid Argument.\n"); break;
		case 3: printf("\tError: Out of Range. [address : 0x00000 ~ 0xFFFFF]\n");
				printf("\t                     [ value  :   0x00  ~  0xFF  ]\n"); break;
		case 4: printf("\tError: Invalid Address Range. [start address <= end address]\n"); break;
		case 5: printf("\tError: Invalid Mnemonic. [See the opcodelist.]\n");
		default: break;	
	}
}
