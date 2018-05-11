#include "linking_loader.h"
#define MAX_LEN 1001
#define MAX_TOKEN 1001
#define MAX_ADDR 0x100000
#define SYM_LEN 7
#define MAX_REF 100

int program_start_addr = 0;

void print_loader_error(int error)
{
	switch(error)
	{
		case 1: printf("\tLoader Error: Invalid object file.\n"); break;
		case 2: printf("\tLoader Error: Duplicate external symbol.\n"); break;
		case 3: printf("\tLoader Error: Undefined external symbol.\n"); break;
		default: break;
	}
}

int update_ext_table(int tsz, char filename[MAX_TOKEN][MAX_LEN])
{
	int f, i;
	char str[MAX_LEN];
	int csaddr = program_start_addr;
	
	for(f = 1; f < tsz; f++)
	{
		char prog_name[SYM_LEN], tag[SYM_LEN];
		int start_addr, prog_len, check;
		FILE* fp = fopen(filename[f], "r");

		fgets(str, MAX_LEN, fp);
		check = sscanf(str, "%1s%6s%6X%6X", tag, prog_name, &start_addr, &prog_len);
		if(start_addr != 0) return 1;
		if(check == EOF) return 1;
		if(get_ext_addr(prog_name) != -1) return 2;
		add_ext_symbol(start_addr + csaddr, prog_len, prog_name, prog_name);

		while(fgets(str, MAX_LEN, fp) != NULL)
		{
			if(str[0] == 'D')
			{
				char symbol[6][SYM_LEN] = {0};
				int addr[6] = {-1, -1, -1, -1, -1, -1};
				check = sscanf(str, "%1s%6s%6X%6s%6X%6s%6X%6s%6X%6s%6X%6s%6X", tag,
						symbol[0], &addr[0],
						symbol[1], &addr[1],
						symbol[2], &addr[2],
						symbol[3], &addr[3],
						symbol[4], &addr[4],
						symbol[5], &addr[5]);
				if(tag[0] == '.') continue;
				if(check == EOF) return 1;
				for(i = 0; i < 6; i++)
					if(symbol[i][0] != 0 && addr[i] != -1)
					{
						if(get_ext_addr(symbol[i]) != -1) return 2;
						add_ext_symbol(addr[i] + csaddr, 0, prog_name, symbol[i]);			
					}
			}
			else if(str[0] == 'H') return 1;
			else if(str[0] == 'E') break;
		}
		csaddr += prog_len;
		fclose(fp);
	}
	return 0;
}

int linking_load(int tsz, char filename[MAX_TOKEN][MAX_LEN])
{
	int f, i;
	char str[MAX_LEN];

	for(f = 1; f < tsz; f++)
	{
		char prog_name[SYM_LEN], tag[SYM_LEN];
		int start_addr, prog_len, check;
		FILE* fp = fopen(filename[f], "r");
		int ref[MAX_REF], ref_size = 0;

		fgets(str, MAX_LEN, fp);
		check = sscanf(str, "%1s%6s%6X%6X", tag, prog_name, &start_addr, &prog_len);
		if(check == EOF) return 1;

		ref[1] = get_ext_addr(prog_name);
		if(ref[1] == -1) return 3;
		else ref_size = 1;
		
		while(fgets(str, MAX_LEN, fp) != NULL)
		{
			if(str[0] == 'R')
			{
				int ref_num[9] = {-1, -1, -1, -1, -1, -1, -1, -1, -1};
				char ref_sym[9][SYM_LEN] = {0};

				check = sscanf(str, "%1s%2d%6s%2d%6s%2d%6s%2d%6s%2d%6s%2d%6s%2d%6s%2d%6s%2d%6s", tag,
						&ref_num[0], ref_sym[0],
						&ref_num[1], ref_sym[1],
						&ref_num[2], ref_sym[2],
						&ref_num[3], ref_sym[3],
						&ref_num[4], ref_sym[4],
						&ref_num[5], ref_sym[5],
						&ref_num[6], ref_sym[6],
						&ref_num[7], ref_sym[7],
						&ref_num[8], ref_sym[8]);
				if(check == EOF) return 1;
				for(i = 0; i < 9; i++)
					if(ref_sym[i][0] != 0 && ref_num[i] != -1)
					{
						ref[ref_num[i]] = get_ext_addr(ref_sym[i]);
						if(ref[ref_num[i]] == -1) return 3;
						else ref_size++;
					}
			}
			else if(str[0] == 'T')
			{
				int line_addr, line_len;
				char line[71];

				check = sscanf(str, "%1s%6X%2X%s", tag, &line_addr, &line_len, line);
				if(check == EOF) return 1;
				
				line_addr += ref[1];
				line_len *= 2;
				if((int)strlen(line) != line_len) return 1;

				for(i = 0; i < line_len; i += 2)
				{
					int val;
					val = char_to_hexa(line[i]);
					val = val << 4;
					val += char_to_hexa(line[i+1]);
					
					if(line_addr >= MAX_ADDR) return 1;
					edit_(line_addr, val & 0x000000ff);
					line_addr++;
				}
			}
			else if(str[0] == 'M')
			{
				int modi_addr, modi_len, ref_num;
				char op[SYM_LEN];
				int val = 0;

				check = sscanf(str, "%1s%6X%2X%1s%2d", tag, &modi_addr, &modi_len, op, &ref_num);
				if(check == EOF) return 1;
				if(ref_num > ref_size) return 3;

				modi_addr += ref[1];
				modi_len = (modi_len + 1) / 2;
				for(i = 0; i < modi_len; i++)
				{
					val = val << 8;
					val += get_value(modi_addr + i);
				}

				if(op[0] == '+') val += ref[ref_num];
				else val -= ref[ref_num];

				for(i = modi_len-1; i >= 0; i--)
				{
					if(modi_addr + i >= MAX_ADDR) return 1;		
					edit_(modi_addr + i, val & 0x000000ff);
					val = val >> 8;
				}
			}
			else if(str[0] == 'E') break;	
		}
		fclose(fp);
	}
	return 0;
}

void loader_(int tsz, char filename[MAX_TOKEN][MAX_LEN])
{
	int error;

	init_process();
	make_ext_table();
	error = update_ext_table(tsz, filename);
	if(error)
	{
		clear_ext_table();
		print_loader_error(error);
		return;
	}
	error = linking_load(tsz, filename);
	if(error)
	{
		clear_ext_table();
		print_loader_error(error);
		return;
	}
	print_ext_table();
	clear_ext_table();
}

void progaddr_(int addr)
{
	program_start_addr = addr;
}

int get_start_addr()
{
	return program_start_addr;
}
