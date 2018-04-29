#include "memory.h"
#define MAX_ADDR 0x100000

// 1Mbyte 가상메모리에 대응되는 배열
int mem[MAX_ADDR] = {0};
// dump 명령어의 마지막 주소값
int last_addr = -1;

int min(int x, int y)
{
	if(x < y) return x;
	else return y;
}

// 요약: 가상메모리 [start,end] 주소의 값을 출력하는 함수
// 기능: [start,end]의 범위에 해당하는 메모리를
// 		 순차적으로 접근하면서 16진수와 문자값을 출력한다.
//		 start = end = -1일 경우, 마지막으로 호출 된 주소값을
//		 기준으로 10 line을 출력한다.
//		 end = -1 일 경우, start를 기준으로 10 line을 출력한다.
//		 메모리의 값이 아스키 코드의 범위를 벗어난다면 '.'을 출력한다.
// 반환: 없음.
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
	// 마지막 주소값을 갱신한다.
	last_addr = end;
}

// 요약: 입력받은 주소값의 메모리에 값을 저장하는 함수
// 기능: 입력받은 주소값의 메모리에 값을 할당한다.
// 반환: 없음.
void edit_(int addr, int val)
{
	mem[addr] = val;	
}

int get_value(int addr)
{
	return mem[addr];
}

// 요약: 입력받은 주소값의 범위에 값을 저장하는 함수
// 기능: 입력받은 주소값의 범위에 값을 할당한다.
// 반환: 없음.
void fill_(int start, int end, int val)
{
	int i;
	for(i = start; i <= end; i++) mem[i] = val;
}

// 요약: 가상메모리를 reset한다.
// 기능: 가상메모리 배열의 전체 범위를 접근하면서 0을 할당한다.
// 반환: 없음.
void reset_()
{
	int i;
	for(i = 0; i < MAX_ADDR; i++) mem[i] = 0;
}
