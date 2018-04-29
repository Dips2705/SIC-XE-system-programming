#include "linking_loader.h"
#define MAX_LEN 1001
#define MAX_TOKEN 1001

int program_start_addr = 0;

void print_loader_error(int n)
{

}

void progaddr_(int addr)
{
	program_start_addr = addr;
}

void loader_(int tsz, char filename[MAX_TOKEN][MAX_LEN])
{

}
