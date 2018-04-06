#include "assembler.h"
#define MAX_LEN 1001
#define MAX_TOKEN 1001
#define MAX_CODE_LINE 10001
#define MAX_ADDR 0x100000
#define MAX_WORD 0x1000000
#define SYM_LEN 31
#define DIR_NUM 8
#define REG_NUM 9
#define DIRECTIVE_ 1
#define OPERATOR_ 2
#define COMMENT_ 3

char tmp_code[MAX_CODE_LINE][MAX_LEN] = {0};
char assem_token[MAX_TOKEN][MAX_LEN];
int assemble_start_flag;

char* directive_list[DIR_NUM] =
{	
	"START",
	"END",
	"BYTE",
	"WORD",
	"RESB",
	"RESW",
	"BASE",
	"NOBASE"
};

char* register_list[REG_NUM] =
{
	"A",
	"X",
	"L",
	"PC",
	"SW",
	"B",
	"S",
	"T",
	"F"
};

int is_comment(char* str)
{
	if(!strcmp(str, ".")) return 1;
	else return 0;
}

int is_operator(char* str)
{
	if(get_opcode(str) != -1) return 1;
	else return 0;
}

int is_directive(char* str)
{
	int i;
	for(i = 0; i < DIR_NUM; i++)
		if(!strcmp(str, directive_list[i])) return 1;
	return 0;
}

int is_register(char* str)
{
	int i;
	for(i = 0; i < REG_NUM; i++)
		if(!strcmp(str, register_list[i])) return 1;
	return 0;
}

int is_valid_symbol(char* str)
{
	int i, len = strlen(str);
	for(i = 0; i < len; i++)
	{
		int flag = 0;
		if('0' <= str[i] && str[i] <= '9') flag = 1;
		if('A' <= str[i] && str[i] <= 'Z') flag = 1;
		if('a' <= str[i] && str[i] <= 'z') flag = 1;
		if(!flag) return 0;
	}

	if('0' <= str[0] && str[0] <= '9') return 0;
	else return 1; 
}

int is_in_symbol_table(char* str)
{
	if(get_addr(str) != -1) return 1;
	else return 0;
}

int is_valid_constant(char* str)
{
	int i, len = strlen(str);
	for(i = 0; i < len; i++)
		if(!('0' <= str[i] && str[i] <= '9')) return 0;
	
	return 1;
}

int str_to_dec(char* str, int boundary)
{
	int i, len = strlen(str);
	int ret = 0;

	if(!is_valid_constant(str)) return -1;
	for(i = 0; i < len; i++)
	{
		ret *= 10; 
		ret += (str[i] - '0');
		if(ret >= boundary) return -1;
	}
	return ret;
}

int assem_tokenize(char* str, char token[MAX_TOKEN][MAX_LEN], int* s, int* e)
{
	int i, j, len = strlen(str);
	int x = 0, y = 0, z = 0, flag = 0;
	
	for(i = 0; i < len; i++)
	{
		if(str[i] != ' ' && str[i] != ',' && str[i] != '\t' && str[i] != '\n')
		{
			// 토큰 5개 이상 오류
			if(x >= 4) return -1;
			if(!flag) e[z++] = i;
			token[x][y++] = str[i], flag = 1;
		}
		else if(flag)
		{
			s[z] = i;
			token[x++][y] = '\0';
			y = 0, flag = 0;
		}
	}

	if(s[z] != len-1)
		for(i = s[z]; i < len; i++)
			if(str[i] != ' ' && str[i] != '\t' && str[i] != '\n') return -1;
	return x;
}

int comment_tokenize(char* str, char token[MAX_TOKEN][MAX_LEN])
{
	int i, j, len = strlen(str);
	int x = 0, y = 0, flag = 0;
	
	for(i = 0; i < len; i++)
	{
		if(str[i] != ' ' && str[i] != '\t' && str[i] != '\n')
		{
			token[x][y++] = str[i];
			flag = 1;
		}
		else if(flag)
		{
			token[x++][y] = '\0';
			y = 0, flag = 0;
		}
	}
	return x;
}

void str_pop_front(char* str)
{
	int i, len = strlen(str);
	for(i = 0; i < len-1; i++) str[i] = str[i+1];
	str[len-1] = '\0';
}

// return size of byte
// error : -1
int calc_byte_operand(char* str)
{
	int i, len = strlen(str);
	int ret;

	if(len < 4) return -1;
	if(str[1] != 39 || str[len-1] != 39) return -1;
	if(str[0] == 'X')
	{
		for(i = 2; i < len-1; i++)
			if(char_to_hexa(str[i]) == -1) return -1;
		if((len - 3) % 2 == 1) return -1;
		else ret = (len - 3) / 2;
	}
	else if(str[0] == 'C') ret = len - 3;
	else return -1;

	return ret;
}

// pass 1
// ----------------------intermediate format----------------------
// type n i x e  Loc  Symbol[.]  Operator  Operand1[.]  Operand2[.] 
// ---------------------------------------------------------------
// type 1 : directive
// type 2 : operator
// type 3 : comment
int make_intermediate_file(char* filename, int* line_num)
{
	char str[MAX_LEN];
	FILE* rp = fopen(filename, "r");	
	FILE* wp = fopen("itm.dat", "w");
	int loc_cnt = 0;

	while(fgets(str, MAX_LEN, rp) != NULL)
	{
		int idx;
		int error = 0;
		int blank_s[5] = {0};
	    int	blank_e[5] = {0};
		int tsz = 0, loc_inc = 0;
		int is_di = 0, is_op = 0, is_co = 0;

		int type = 0, n = 0, i = 0, x = 0, e = 0;
		char symbol[SYM_LEN], op[SYM_LEN], p1[SYM_LEN], p2[SYM_LEN];

		tsz = assem_tokenize(str, assem_token, blank_s, blank_e);

		if(!tsz) continue; // blank line
		else *line_num += 5;

		// save code
		strcpy(tmp_code[(*line_num)/5], str);

		// error : program exceeds memory size
		if(loc_cnt >= MAX_ADDR) return error = 4;
		
		// check if it is comment
		if(is_comment(assem_token[0])) is_co = 1;

		if(!is_co)
		{
			// error: token > 4 or symbol size > 30
			if(tsz == -1) return error = 1;
			for(idx = 0; idx < tsz; idx++)
				if(strlen(assem_token[idx]) > 30) return error = 1;

			// check first word type
			if(is_directive(assem_token[0])) is_di = 1;
			else if(is_operator(assem_token[0])) is_op = 1;
			else if(assem_token[0][0] == '+' && is_operator(assem_token[0] + 1)) is_op = 1;

			if(!is_di && !is_op) // it has symbol
			{
				// comma error check
				if(tsz == 4)
				{
					int comma = 0;
					for(idx = blank_s[3]; idx < blank_e[3]; idx++)
						if(str[idx] == ',') comma++;
					// error : invalid # of comma
					if(comma != 1) return error = 1;
				}	
				
				// error : invalid symbol format
				if(!is_valid_symbol(assem_token[0])) return error = 1; 
				else strcpy(symbol, assem_token[0]);
				
				// error : duplicate symbol
				if(is_in_symbol_table(symbol)) return error = 2;
				else add_symbol(loc_cnt, symbol);

				// pull the tokens forward
				tsz--;
				for(idx = 0; idx < tsz; idx++)
					strcpy(assem_token[idx], assem_token[idx+1]);

			}
			else // it has not symbol
			{
				// comma error check
				if(tsz == 3)
				{
					int comma = 0;
					for(idx = blank_s[2]; idx < blank_e[2]; idx++)
						if(str[idx] == ',') comma++;
					// error : invalid # of comma
					if(comma != 1) return error = 1;
				}
				strcpy(symbol, ".");
			}

			// check first word type
			if(is_directive(assem_token[0])) is_di = 1;
			if(is_operator(assem_token[0])) is_op = 1;
			if(assem_token[0][0] == '+' && is_operator(assem_token[0] + 1)) is_op = 1;

			if(is_di)
			{
				if(!strcmp(assem_token[0], "NOBASE"))
				{
					// error : invalid # of operand
					if(tsz != 1) return error = 1;
					
					strcpy(op, "NOBASE");
					strcpy(p1, ".");
					strcpy(p2, ".");
				}
				else
				{	
					// error : invalid # of operand
					if(tsz != 2) return error = 1;
					
					if(!strcmp(assem_token[0], "START"))
					{
						int addr = str_to_val(assem_token[1], MAX_ADDR);
						// error : invalid position of start
						if(assemble_start_flag) return error = 1;
						// error : invalid operand
						if(addr < 0) return error = 1;
							
					   	strcpy(op, "START");
						strcpy(p1, assem_token[1]);
						loc_cnt = addr;
						loc_inc = 0;
					}
					else if(!strcmp(assem_token[0], "END"))
					{
						// error : invalid operand (not symbol or out of range)
						if(str_to_val(assem_token[1], MAX_ADDR) < 0 && !is_valid_symbol(assem_token[1]))
							return error = 1;

						strcpy(op, "END");
						strcpy(p1, assem_token[1]);
						loc_inc = loc_cnt;
						loc_cnt = 0;
					}
					else if(!strcmp(assem_token[0], "BASE"))
					{
						// error : invalid operand (not symbol or out of range)
						if(str_to_val(assem_token[1], MAX_ADDR) < 0 && !is_valid_symbol(assem_token[1]))
							return error = 1;

						strcpy(op, "BASE");
						strcpy(p1, assem_token[1]);
						loc_inc = loc_cnt;
						loc_cnt = 0;
					}
					else if(!strcmp(assem_token[0], "BYTE"))
					{
						int val = calc_byte_operand(assem_token[1]);
						// error : invalid operand
						if(val == -1) return error = 1;
						
						strcpy(op, "BYTE");
						strcpy(p1, assem_token[1]);
						loc_inc = val;
					}
					else if(!strcmp(assem_token[0], "WORD"))
					{
						int val = str_to_dec(assem_token[1], MAX_WORD);
						// error : invalid operand (out of 24bits)
						if(val == -1) return error = 1;
						
						strcpy(op, "WORD");
						strcpy(p1, assem_token[1]);
						loc_inc = 3;
					}
					else if(!strcmp(assem_token[0], "RESB"))
					{
						int val = str_to_dec(assem_token[1], MAX_WORD);
						// error : invalid operand (out of range)
						if(val == -1) return error = 1;
						strcpy(op, "RESB");
						strcpy(p1, assem_token[1]);
						loc_inc = val;
					}
					else if(!strcmp(assem_token[0], "RESW"))
					{	
						int val = str_to_dec(assem_token[1], MAX_WORD);
						// error : invalid operand (out of range)
						if(val == -1) return error = 1;
						strcpy(op, "RESW");
						strcpy(p1, assem_token[1]);
						loc_inc = val * 3;
					}
					strcpy(p2, ".");
				}
				type = DIRECTIVE_;
			}
			else if(is_op)
			{
				int format;

				// set flag 'e'
				if(assem_token[0][0] == '+') e = 1, str_pop_front(assem_token[0]);
				
				format = get_format(assem_token[0]);
				strcpy(op, assem_token[0]);

				// rsub (format 3)
				if(!strcmp(assem_token[0], "RSUB"))
				{
					if(tsz != 1 || e == 1) return error = 1;
					n = 0, i = 0, x = 0;
					loc_inc = 3;
				}
				else if(format == 1)
				{
					// error : invalid operand
					if(tsz != 1 || e == 1) return error = 1;
					n = 0, i = 0, x = 0;
					loc_inc = 1;
				}
				else if(format == 2)
				{
					// error : invalid operand
					if(!(tsz == 2 || tsz == 3) || e == 1) return error = 1;
					if(tsz == 2)
					{
						// error : invalid operand
						if(!is_register(assem_token[1])) return error = 1;
						strcpy(p1, assem_token[1]);
						strcpy(p2, "A");
					}
					else // tsz == 3
					{
						// error : invalid operand
						if(!is_register(assem_token[1]) || !is_register(assem_token[2])) return error = 1;
						strcpy(p1, assem_token[1]);
						strcpy(p2, assem_token[2]);
					}
					n = 0, i = 0, x = 0;
					loc_inc = 2;
				}
				else // format 3
				{
					// error : invalid operand
					if(tsz != 2 && tsz != 3) return error = 1;
					if(tsz == 3)
					{
						if(!strcmp(assem_token[2], "X")) x = 1, strcpy(p2, ".");
						else return error = 1; // error : not x register
					}
					else x = 0, strcpy(p2, ".");

					if(assem_token[1][0] == '@')
					{
						// error : indirect + indexed
						if(x) return error = 1;
						n = 1, i = 0, str_pop_front(assem_token[1]);
					}
					else if(assem_token[1][0] == '#')
					{
						// error : immediate + indexed
						if(x) return error = 1;
						n = 0, i = 1, str_pop_front(assem_token[1]);
					}
					else n = 1, i = 1;

					if(!e) // format 3
					{
						// error : invalid operand (out of 12bits)
						if(str_to_dec(assem_token[1], 0x1000) < 0 && !is_valid_symbol(assem_token[1]))
							return error = 1;
						loc_inc = 3;
					}
					else // format 4
					{
						// error : invalid operand (out of 20bits)
						if(str_to_dec(assem_token[1], 0x100000) < 0 && !is_valid_symbol(assem_token[1]))
							return error = 1;
						loc_inc = 4;
					}
					
					strcpy(p1, assem_token[1]);	
				}
				type = OPERATOR_;
			}
			else return error = 3; // error : invalid operation code

			fprintf(wp, "%d %d %d %d %d\t%05X\t%s\t%s\t%s\t%s\n", type, n, i, x, e, loc_cnt, symbol, op, p1, p2);
			
			assemble_start_flag = 1;
			loc_cnt += loc_inc;
		}
		else // it is comment
		{
			type = COMMENT_;
			tsz = comment_tokenize(str, assem_token);

			fprintf(wp, "%d %d %d %d %d\t%05X\t", type, n, i, x, e, 0x00000);		
			fprintf(wp, "%s\t", assem_token[0]);
			for(idx = 1; idx < tsz; idx++)
				fprintf(wp, "%s ", assem_token[idx]);
			fprintf(wp, "\n");
		}
	}

	fclose(rp);
	fclose(wp);
	return 0;
}

void print_assemble_error(int error, int n)
{
	switch(error)
	{
		case 1: printf("\tAssembler Error: Invalid Syntax.\n");
				printf("\t==> [line:%d] %s", n, tmp_code[n/5]); break;
		case 2: printf("\tAssembler Error: Duplicate Symbol.\n");
				printf("\t==> [line:%d] %s", n, tmp_code[n/5]); break;
		case 3: printf("\tAssembler Error: Invalid Operation Code.\n");
				printf("\t==> [line:%d] %s", n, tmp_code[n/5]); break;
		case 4: printf("\tAssembler Error: This program exceeds SIC/XE memory size.\n");
				printf("\t==> [line:%d] %s", n, tmp_code[n/5]); break;
		default: break;
	}
}

void assemble_(char* filename)
{
	char lst_filename[MAX_LEN], obj_filename[MAX_LEN];
	int len = strlen(filename);
	int error, line_num = 0;

	strcpy(lst_filename, filename);
	lst_filename[len-3] = 'l', lst_filename[len-2] = 's', lst_filename[len-1] = 't';
	strcpy(obj_filename, filename);
	obj_filename[len-3] = 'o', obj_filename[len-2] = 'b', obj_filename[len-1] = 'j';

	// init
	assemble_start_flag = 0;
	clear_sym_table();	
	make_sym_table();

	// pass 1
	error = make_intermediate_file(filename, &line_num);
	if(error)
	{
		clear_sym_table();
		if(is_in_dir("itm.dat")) remove("itm.dat");
		print_assemble_error(error, line_num);
		return;
	}
	// pass 2
}
