#include "assembler.h"
#define MAX_LEN 1001
#define MAX_TOKEN 1001
#define MAX_MODI 10001
#define MAX_ADDR 0x100000
#define MAX_WORD 0x1000000
#define SYM_LEN 7
#define DIR_NUM 8
#define REG_NUM 9
#define DIRECTIVE_ 1
#define OPERATOR_ 2
#define COMMENT_ 3

char program_name[SYM_LEN];
int first_addr, last_addr, start_addr;
char assem_token[MAX_TOKEN][MAX_LEN];
// assemble 시작 여부를 체크
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

//요약: 빈 라인인지 확인하는 함수
//기능: string을 입력받아 빈라인인지 여부를 반환한다.
//반환: 빈 라인 = 1 / 아님 = 0
int is_empty_line(char* str)
{
	int i, len = strlen(str);
	for(i = 0; i < len; i++)
		if(str[i] != ' ' && str[i] != '\t' && str[i] != '\n') return 0;
	return 1;
}

//요약: comment인지 확인하는 함수
//기능: string을 입력받아 comment인지 확인한다.
//반환: comment = 1 / 아님 = 0
int is_comment(char* str)
{
	if(!strcmp(str, ".")) return 1;
	else return 0;
}

//요약: operator인지 확인하는 함수
//기능: string을 입력받아 operator인지 확인한다.
//반환: operator = 1 / 아님 = 0
int is_operator(char* str)
{
	if(get_opcode(str) != -1) return 1;
	else return 0;
}

//요약: directive인지 확인하는 함수
//기능: string을 입력받아 directive인지 확인한다.
//반환: directive = 1 / 아님 = 0
int is_directive(char* str)
{
	int i;
	for(i = 0; i < DIR_NUM; i++)
		if(!strcmp(str, directive_list[i])) return 1;
	return 0;
}

//요약: register인지 확인하는 함수
//기능: string을 입력받아 register인지 확인한다.
//반환: register = 1 / 아님 = 0
int is_register(char* str)
{
	int i;
	for(i = 0; i < REG_NUM; i++)
		if(!strcmp(str, register_list[i])) return 1;
	return 0;
}

//요약: 올바른 symbol format인지 확인하는 함수
//기능: string을 입력받아 올바른 symbol format인지 확인한다.
//반환: valid = 1 / invalid = 0
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

//요약: symbol table에 존재하는지 확인하는 함수
//기능: string을 입력받아, 해당 symbol이 symbol table에
//		존재하는지 확인한다.
//반환: 존재 = 1 / 아님 = 0
int is_in_symbol_table(char* str)
{
	if(get_addr(str) != -1) return 1;
	else return 0;
}

//요약: 올바른 constant format인지 확인하는 함수 
//기능: string을 입력받아 올바른 constant format인지 확인한다.
//반환: valid = 1 / invalid = 0
int is_valid_constant(char* str)
{
	int i, len = strlen(str);
	for(i = 0; i < len; i++)
		if(!('0' <= str[i] && str[i] <= '9')) return 0;
	return 1;
}

//요약: string을 10진수로 변환하는 함수
//기능: string을 입력받아서 10진수로 변환한다.
//		올바르지 않는 문자나 해당 값이 boundary보다
//		크거나 같으면 에러를 반환한다.
//반환: 10진수 값 / error = -1
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

//요약: charater을 입력받아 16진수값으로 변환하는 함수
//기능: 해당하는 16진수 값으로 변환하여 반환한다.
//		올바르지 않는 문자(소문자 포함)일 경우 에러를 출력한다.
//반환: 16진수 값 / error = -1
int upper_char_to_hexa(char c)
{
	int val = -1;
	if('0' <= c && c <= '9') val = (int)(c - '0');
	if('A' <= c && c <= 'F') val = (int)(c - 'A' + 10);
	return val;
}

//요약: string을 입력받아 16진수값으로 변환하는 함수
//기능: string을 16진수 값으로 변환하여 반환한다.
//		올바르지 않는 문자이거나 해당 값이 범위를 초과할
//		경우에 에러를 출력한다.
//반환: 16진수 값 / error = -1
int str_to_hexa(char* str, int boundary)
{	
	int ret = 0;
	int i, len = strlen(str), val;

	for(i = 0; i < len; i++)
	{
		val = upper_char_to_hexa(str[i]);
		if(val == -1) return -1;
	}
	for(i = 0; i < len; i++)
	{
		val = upper_char_to_hexa(str[i]);
		ret *= 16;
		ret += val;
		if(ret >= boundary) return -2;
	}
	return ret;
}

//요약: assembly code를 tokenize하는 함수
//기능: assemblt code를 tokenize한다.
//		s와 e에는 token들 사이의 시작과 끝 index가 저장된다.
//반환: token의 개수 / error = -1
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

//요약: comment를 tokenize하는 함수
//기능: string을 입력받아 comment를 tokenize한다.
//반환: token의 개수
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

//요약: string의 제일 앞 문자를 pop하는 함수
//기능: string을 입력받아 제일 앞 문자를 pop한다.
//반환: 없음.
void str_pop_front(char* str)
{
	int i, len = strlen(str);
	for(i = 0; i < len-1; i++) str[i] = str[i+1];
	str[len-1] = '\0';
}

//요약: byte directive의 operand를 처리하는 함수
//기능: string을 입력받아서 operand에 해당하는
//		byte의 크기를 계산하여 반환한다.
//반환: size of byte / error = -1
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
			if(upper_char_to_hexa(str[i]) == -1) return -1;
		else ret = (len - 2) / 2;
	}
	else if(str[0] == 'C') ret = len - 3;
	else return -1;

	return ret;
}

//요약: assembly file의 symbol들을 기반으로 symbol table을 만들고
//		intermediate file을 생성하는 함수
//기능: assembly file의 code를 분석하여 symbol을 저장하고
//		object file, list file으로 변환하기 전 intermediate file을 생성한다.
//		범위체크, 올바르지 않는 문법등 주요 예외사항을 체크한다.
//반환: error num / no error = 0
// pass 1
// ----------------------intermediate format----------------------
// type n i x e  Loc  Symbol[.]  Operator  Operand1[.]  Operand2[.] 
// ---------------------------------------------------------------
// type 1 : directive
// type 2 : operator
// type 3 : comment
int make_intermediate_file(char* asm_file, char* itm_file, char* str, int* line_num)
{
	int error = 0;
	FILE* rp = fopen(asm_file, "r");	
	FILE* wp = fopen(itm_file, "w");
	int loc_cnt = 0;
	
	while(fgets(str, MAX_LEN, rp) != NULL)
	{
		int idx;
		int blank_s[5] = {0};
		int blank_e[5] = {0};
		int tsz = 0, loc_inc = 0;
		int is_di = 0, is_op = 0, is_co = 0;
		int type = 0, n = 0, i = 0, x = 0, e = 0;
		char symbol[SYM_LEN], op[SYM_LEN], p1[SYM_LEN], p2[SYM_LEN];

		tsz = assem_tokenize(str, assem_token, blank_s, blank_e);

		if(!tsz) continue; // empty line
		else *line_num += 5;

		// check if it is comment
		if(is_comment(assem_token[0])) is_co = 1;
		
		if(is_co) // it is comment
		{
			type = COMMENT_;
			tsz = comment_tokenize(str, assem_token);

			fprintf(wp, "%d %d %d %d %d\t%05X\t", type, n, i, x, e, 0);		
			fprintf(wp, "%s\t", assem_token[0]);
			for(idx = 1; idx < tsz; idx++)
				fprintf(wp, "%s ", assem_token[idx]);
			fprintf(wp, "\n");
		}
		else
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
				int comma, jdx;
				
				// comma error check
				for(jdx = 0; jdx < tsz-1; jdx++)
				{
					comma = 0;
					for(idx = blank_s[jdx]; idx < blank_e[jdx]; idx++)
						if(str[idx] == ',') comma++;
					if(comma != 0) return error = 1;
				}
				comma = 0;
				for(idx = blank_s[tsz-1]; idx < blank_e[tsz-1]; idx++)
					if(str[idx] == ',') comma++;
				// error : invalid # of comma
				if(tsz == 4 && comma != 1) return error = 1;
				else if(tsz != 4 && comma != 0) return error = 1;
				
				// error : invalid symbol format
				if(!is_valid_symbol(assem_token[0])) return error = 1; 
				else strcpy(symbol, assem_token[0]);
				
				// error : duplicate symbol
				if(is_in_symbol_table(symbol)) return error = 2;
				else if(!strcmp(assem_token[1], "START"))
				{
					// error : invalid program name
					if(strlen(symbol) > 6) return error = 7;
					strcpy(program_name, symbol);
				}
				else add_symbol(loc_cnt, symbol);

				// pull the tokens forward
				tsz--;
				for(idx = 0; idx < tsz; idx++)
					strcpy(assem_token[idx], assem_token[idx+1]);

			}
			else // it has not symbol
			{
				int comma, jdx;
				
				// comma error check
				for(jdx = 0; jdx < tsz-1; jdx++)
				{
					comma = 0;
					for(idx = blank_s[jdx]; idx < blank_e[jdx]; idx++)
						if(str[idx] == ',') comma++;
					if(comma != 0) return error = 1;
				}
				comma = 0;
				for(idx = blank_s[tsz-1]; idx < blank_e[tsz-1]; idx++)
					if(str[idx] == ',') comma++;
				// error : invalid # of comma
				if(tsz == 3 && comma != 1) return error = 1;
				else if(tsz != 3 && comma != 0) return error = 1;
				
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
				else if(!strcmp(assem_token[0], "END"))
				{
					if(tsz == 1) strcpy(assem_token[1], ".");
					else if(tsz == 2)
					{
						// error : invalid operand (not symbol or out of range)
						if(str_to_hexa(assem_token[1], MAX_ADDR) < 0 && !is_valid_symbol(assem_token[1]))
							return error = 1;
					}
					else return error = 1;
					
					strcpy(op, "END");
					strcpy(p1, assem_token[1]);
					last_addr = loc_cnt;
					loc_inc = loc_cnt;
					loc_cnt = 0;
				}
				else
				{	
					// error : invalid # of operand
					if(tsz != 2) return error = 1;
					
					if(!strcmp(assem_token[0], "START"))
					{
						int addr = str_to_hexa(assem_token[1], MAX_ADDR);
						// error : invalid position of start
						if(assemble_start_flag) return error = 1;
						// error : invalid operand
						if(addr < 0) return error = 1;
							
					   	strcpy(op, "START");
						strcpy(p1, assem_token[1]);
						first_addr = addr;
						loc_cnt = addr;
						loc_inc = 0;
					}
					else if(!strcmp(assem_token[0], "BASE"))
					{
						// error : invalid operand (not symbol or out of range)
						if(str_to_hexa(assem_token[1], MAX_ADDR) < 0 && !is_valid_symbol(assem_token[1]))
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
					// error : invalid operand
					if(tsz != 1 || e == 1) return error = 1;
					strcpy(p1, ".");
					strcpy(p2, ".");
					n = 0, i = 0, x = 0;
					loc_inc = 3;
				}
				else if(format == 1)
				{
					// error : invalid operand
					if(tsz != 1 || e == 1) return error = 1;
					strcpy(p1, ".");
					strcpy(p2, ".");
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
			
			// error : program exceeds memory size
			if(loc_cnt + loc_inc >= MAX_ADDR) return error = 4;
			
			fprintf(wp, "%d %d %d %d %d\t%05X\t%s\t%s\t%s\t%s\n", type, n, i, x, e, loc_cnt, symbol, op, p1, p2);
			
			assemble_start_flag = 1;
			loc_cnt += loc_inc;
		}
	}

	fclose(rp);
	fclose(wp);
	return error = 0;
}

//요약: byte operand를 hexa code로 변환하는 함수
//기능: byte operand를 해석해서 hexa code로 변환하여
//		target string에 저장한다.
//반환: 없음.
void byte_operand_to_hexa_code(char* str, char* target)
{
	int i, len = strlen(str);
	if(str[0] == 'C')
	{
		int j = 0;
		for(i = 2; i < len-1; i++)
			j += sprintf(target+j, "%02X", (int)(str[i]));
	}
	else
	{
		int j = 2;
		if((len - 3) % 2 == 1) target[0] = '0', j = 1;
		for(i = 2; i < len-1; i++) target[i-j] = str[i];
		target[len-1] = '\0';
	}
}

//요약: object file, list file을 생성하는 함수
//기능: intermediate file을 읽어서 symbol table을 기준으로
//		object file과 list file을 생성한다.
//		정의되지 않은 symbol을 사용하거나, extended를 명시하지 않은 경우
//		에러를 출력한다.
//반환: error num / no error = 0
int make_object_file(char* itm_file, char* asm_file, char* obj_file, char* lst_file, char* str, int* line_num)
{
	int error = 0;
	FILE* rp_itm = fopen(itm_file, "r");
	FILE* rp_asm = fopen(asm_file, "r");
	FILE* wp_obj = fopen(obj_file, "w");
	FILE* wp_lst = fopen(lst_file, "w");

	int modi[MAX_MODI], midx = 0, idx;
	char obj_line[MAX_LEN] = {0};
	int obj_idx = 0;
	int type, n, i, x, e, loc;
	char symbol[SYM_LEN], op[SYM_LEN], p1[SYM_LEN], p2[SYM_LEN];
	int base = -1;
	
	obj_idx += sprintf(obj_line+obj_idx, "H%-6s", program_name);
	obj_idx += sprintf(obj_line+obj_idx, "%06X", first_addr);
	obj_idx += sprintf(obj_line+obj_idx, "%06X", last_addr - first_addr);
	
	fprintf(wp_obj, "%s\n", obj_line);
	memset(obj_line, 0, sizeof(obj_line));
	obj_idx = 0;

	while(fscanf(rp_itm, "%d %d %d %d %d %X", &type, &n, &i, &x, &e, &loc) != EOF)
	{
		char obj_code[MAX_LEN] = {0};
		int obj_idx2 = 0;
		int no_loc = 0;

		*line_num += 5;
		// save error string
		fgets(str, MAX_LEN, rp_asm);
		while(is_empty_line(str)) fgets(str, MAX_LEN, rp_asm);

		if(type == COMMENT_)
		{
			fscanf(rp_itm, "%[^\n]", obj_code);
			fprintf(wp_lst, "%4d\t%s\n", *line_num, obj_code);
			continue;
		}
		
		fscanf(rp_itm, "%s %s %s %s", symbol, op, p1, p2);	
		if(type == DIRECTIVE_)
		{
			if(!strcmp(op, "END"))
			{
				if(!strcmp(p1, ".")) start_addr = 0;
				else if(is_valid_symbol(p1))
				{
					// error : undefined symbol
					if(!is_in_symbol_table(p1)) return error = 5;
					else start_addr = get_addr(p1);
				}
				else start_addr = str_to_hexa(p1, MAX_ADDR);
				no_loc = 1;
			}
			else if(!strcmp(op, "BASE"))
			{
				if(is_valid_symbol(p1))
				{
					// error : undefined symbol
					if(!is_in_symbol_table(p1)) return error = 5;
					else base = get_addr(p1);
				}
				else base = str_to_hexa(p1, MAX_ADDR);
				no_loc = 1;
			}
			else if(!strcmp(op, "NOBASE")) base = -1, no_loc = 1;
			else if(!strcmp(op, "BYTE")) byte_operand_to_hexa_code(p1, obj_code);
			else if(!strcmp(op, "WORD")) sprintf(obj_code, "%06X", str_to_dec(p1, MAX_WORD));
			else if(!strcmp(op, "RESB") || !strcmp(op, "RESW"))
			{
				int len = strlen(obj_line);
				if(len)
				{
					char tmp[5];
					sprintf(tmp, "%02X", (len-9)/2);
					obj_line[7] = tmp[0], obj_line[8] = tmp[1];
					fprintf(wp_obj, "%s\n", obj_line);
					memset(obj_line, 0, sizeof(obj_line));
					obj_idx = 0;
				}
			}
		}
		else if(type == OPERATOR_)
		{
			int opcode = get_opcode(op);
			int format = get_format(op);

			if(!strcmp(op, "RSUB")) sprintf(obj_code, "4F0000");
			else if(format == 1) sprintf(obj_code, "%02X", opcode);
			else if(format == 2)
			{
				obj_idx2 += sprintf(obj_code+obj_idx2, "%02X", opcode);
				obj_idx2 += sprintf(obj_code+obj_idx2, "%1X", get_addr(p1));
				obj_idx2 += sprintf(obj_code+obj_idx2, "%1X", get_addr(p2));
				if(strcmp(op, "CLEAR") && strcmp(op, "TIXR"))
				{
					strcat(p1, ",");
					strcat(p1, p2);
				}
			}
			else // format 3
			{
				int ta = 0;
				int xbpe = 0;
				int pc = loc + 3;
				int is_sym = 0;

				if(is_valid_symbol(p1)) ta = get_addr(p1), is_sym = 1;
				else ta = str_to_dec(p1, 0x100000);
				
				// error : undefined symbol
				if(ta == -1) return error = 5;

				if(n) opcode += 2;
				if(i) opcode += 1;
				if(x) xbpe += 8;
				if(e) xbpe += 1;
				
				obj_idx2 += sprintf(obj_code+obj_idx2, "%02X", opcode);
			
				if(!e) // format 3
				{
					if(is_sym)
					{
						if(pc-2048 <= ta && ta <= pc+2047)
						{
							ta -= pc;
							if(ta < 0) ta &= 0x00000FFF;
							xbpe += 2;
						}
						else if(base != -1 && base <= ta && ta <= base+4095)
						{
							ta -= base;
							xbpe += 4;
						}
						else return error = 6; // error : not use format 4
					}
					sprintf(obj_code+obj_idx2, "%1X%03X", xbpe, ta);
				}
				else // format 4
				{
					if(is_sym) modi[midx++] = loc + 1;
					sprintf(obj_code+obj_idx2, "%1X%05X", xbpe, ta);
				}

				if(n == 1 && i == 0)
				{
					char* tmp = strdup(p1);
					strcpy(p1, "@");
					strcat(p1, tmp);
				}
				if(n == 0 && i == 1)
				{
					char* tmp = strdup(p1);
					strcpy(p1, "#");
					strcat(p1, tmp);
				}
				if(e)
				{
					char* tmp = strdup(op);
					strcpy(op, "+");
					strcat(op, tmp);
				}
				if(x) strcat(p1, ",X");
			}
		} 
		
		// write object file
		if(strlen(obj_code))
		{
			int len = strlen(obj_line);
			int record_len = len + strlen(obj_code);
			if(record_len > 69)
			{
				char tmp[5];
				sprintf(tmp, "%02X", (len-9)/2);
				obj_line[7] = tmp[0], obj_line[8] = tmp[1];
				fprintf(wp_obj, "%s\n", obj_line);
				memset(obj_line, 0, sizeof(obj_line));
				obj_idx = 0;
			}
			if(obj_idx == 0) obj_idx += sprintf(obj_line+obj_idx, "T%06X00", loc);
			obj_idx += sprintf(obj_line+obj_idx, "%s", obj_code);
		}
		// write list file
		if(!strcmp(symbol, ".")) strcpy(symbol, " ");
		if(!strcmp(p1, ".")) strcpy(p1, " ");
		if(no_loc) fprintf(wp_lst, "%4d\t     \t%s\t%s\t%-30s\t\t%s\n", *line_num, symbol, op, p1, obj_code);
		else fprintf(wp_lst, "%4d\t%04X\t%s\t%s\t%-30s\t\t%s\n", *line_num, loc, symbol, op, p1, obj_code);
	}
	// write last line of object file
	int len = strlen(obj_line);
	if(len)
	{	
		char tmp[5];
		sprintf(tmp, "%02X", (len-9)/2);
		obj_line[7] = tmp[0], obj_line[8] = tmp[1];
		fprintf(wp_obj, "%s\n", obj_line);
		memset(obj_line, 0, sizeof(obj_line));
		obj_idx = 0;
	}
	for(idx = 0; idx < midx; idx++)
		fprintf(wp_obj, "M%06X05\n", modi[idx]);
	fprintf(wp_obj, "E%06X\n", start_addr);

	fclose(rp_itm);
	fclose(rp_asm);
	fclose(wp_obj);
	fclose(wp_lst);
	return error = 0;
}

//요약: assemble error를 출력하는 함수 
//기능: error의 번호를 받아서 해당하는
//		error 사항을 출력한다.
//반환: 없음.
void print_assemble_error(int error, char* str, int n)
{
	switch(error)
	{
		case 1: printf("\tAssembler Error: Invalid Syntax.\n");
				printf("\t==> [line:%d] %s", n, str); break;
		case 2: printf("\tAssembler Error: Duplicate Symbol.\n");
				printf("\t==> [line:%d] %s", n, str); break;
		case 3: printf("\tAssembler Error: Invalid Operation Code.\n");
				printf("\t==> [line:%d] %s", n, str); break;
		case 4: printf("\tAssembler Error: This program exceeds SIC/XE memory size.\n");
				printf("\t==> [line:%d] %s", n, str); break;
		case 5: printf("\tAssembler Error: Undefined Symbol.\n");
				printf("\t==> [line:%d] %s", n, str); break;
		case 6: printf("\tAssembler Error: Need using Format 4. (+operator)\n");
				printf("\t==> [line:%d] %s", n, str); break;
		case 7: printf("\tAssembler Error: Invalid Program Name.\n");
				printf("\t==> [line:%d] %s", n, str); break;
		default: break;
	}
}

//요약: assemble 명령어 함수
//기능: filename을 입력받아서 assemble을 수행한다.
//		에러가 발생할 경우, 생성했던 file들을 삭제하고
//		에러 메세지를 출력한다.
//반환: 없음.
void assemble_(char* asm_file)
{
	char str[MAX_LEN];
	char lst_file[MAX_LEN], obj_file[MAX_LEN];
	char itm_file[] = "itm_file";
	int len = strlen(asm_file);
	int error, line_num;

	strcpy(lst_file, asm_file);
	lst_file[len-3] = 'l', lst_file[len-2] = 's', lst_file[len-1] = 't';
	strcpy(obj_file, asm_file);
	obj_file[len-3] = 'o', obj_file[len-2] = 'b', obj_file[len-1] = 'j';

	// init
	strcpy(program_name, "MAIN");
	first_addr = 0, last_addr = 0, start_addr = 0;
	assemble_start_flag = 0;
	clear_sym_table();	
	make_sym_table();
	line_num = 0;

	// pass 1
	error = make_intermediate_file(asm_file, itm_file, str, &line_num);
	if(error)
	{
		clear_sym_table();
		if(is_in_dir(itm_file)) remove(itm_file);
		print_assemble_error(error, str, line_num);
		return;
	}

	// pass 2
	line_num = 0;
	error = make_object_file(itm_file, asm_file, obj_file, lst_file, str, &line_num);
	if(error)
	{
		clear_sym_table();
		if(is_in_dir(itm_file)) remove(itm_file);
		if(is_in_dir(obj_file)) remove(obj_file);
		if(is_in_dir(lst_file)) remove(lst_file);
		print_assemble_error(error, str, line_num);
		return;
	}
	else
	{
		if(is_in_dir(itm_file)) remove(itm_file);
		printf("\toutput file : [%s], [%s]\n", lst_file, obj_file);
	}
}
