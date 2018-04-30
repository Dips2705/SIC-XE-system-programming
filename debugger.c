#include "debugger.h"
#define MAX_BP 1001

int break_point[MAX_BP];
int bp_size = 0;

int find_bp(int addr)
{
	int i;
	for(i = 0; i < bp_size; i++)
		if(break_point[i] == addr) return 1;
	return 0;
}

void print_bp_list()
{
	int i;
	printf("\tbreakpoint\n");
	printf("\t----------\n");
	if(!bp_size) printf("\tempty\n");
	for(i = 0; i < bp_size; i++)
		printf("\t%04X\n", break_point[i]);
}

void set_bp(int addr)
{
	int i, j;

	if(find_bp(addr))
	{
		printf("\tAlready in the breakpoint list.\n");
		return;
	}
	
	break_point[bp_size++] = addr;
	
	for(i = bp_size-1; i >= 0; i--)
		for(j = 0; j < i; j++)
			if(break_point[j] > break_point[j+1])
			{
				int tmp = break_point[j];
				break_point[j] = break_point[j+1];
				break_point[j+1] = tmp;
			}

	printf("\t[ok] create breakpoint %04X\n", addr);
}

void clear_bp_list()
{
	bp_size = 0;
	printf("\t[ok] clear all breakpoints\n");
}

void bp_(int addr)
{
	if(addr == -1) print_bp_list();
	else if(addr == -2) clear_bp_list();
	else set_bp(addr);
}
