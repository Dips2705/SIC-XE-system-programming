#include "memory.h"
#define MAX_ADDR 0x100000

int mem[MAX_ADDR] = {0};
int last_addr = -1;

int min(int x, int y)
{
	if(x < y) return x;
	else return y;
}

void dump_(int start, int end)
{
	int i, j;

	if(start == -1)
	{
		start = last_addr + 1;
		if(start == MAX_ADDR) start = 0;
	}
	if(end == -1) end = min(MAX_ADDR-1, start + 160 - 1);
	
	for(i = start/16; i <= end/16; i++)
	{
		printf("%c[1;34m", 27);
		printf("\t%05X ", i*16);
		printf("%c[0m", 27);
		for(j = i*16; j < (i+1)*16; j++)
		{
			if(start <= j && j <= end)
			{
				if(mem[j]) printf("%c[1;32m", 27);
				printf("%02X ", mem[j]);
				if(mem[j]) printf("%c[0m", 27);
			}
			else printf("   ");
		}
		printf("; ");
		for(j = i*16; j < (i+1)*16; j++)
		{
			if(start <= j && j <= end && 0x20 <= mem[j] && mem[j] <= 0x7E)
			{
				printf("%c[1;35m", 27);	
				printf("%c", (char)mem[j]);
				printf("%c[0m", 27);
			}
			else printf(".");
		}
		printf("\n");
	}
	last_addr = end;
}

void edit_(int addr, int val)
{
	mem[addr] = val;	
}

int get_value(int addr)
{
	return mem[addr];
}

void fill_(int start, int end, int val)
{
	int i;
	for(i = start; i <= end; i++) mem[i] = val;
}

void reset_()
{
	int i;
	for(i = 0; i < MAX_ADDR; i++) mem[i] = 0;
}
