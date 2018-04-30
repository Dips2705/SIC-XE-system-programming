#include "processor.h"
#define MAX_ADDR 0x100000
#define IMM 1
#define IND 2
#define SIM 3
#define LT 1
#define EQ 2
#define GT 3

enum registers
{
	_A,
	_X,
	_L,
	_B,
	_S,
	_T,
	_F,
	_TMP,
	_PC,
	_SW
};

int reg[10] = {0, 0, MAX_ADDR, 0};
int debug_flag = 0, cur_debug = -1;

void print_register(int addr)
{
	printf("\t\tA : %06X X : %06X\n", reg[_A], reg[_X]);
	printf("\t\tL : %06X PC: %06X\n", reg[_L], reg[_PC]);
	printf("\t\tB : %06X S : %06X\n", reg[_B], reg[_S]);
	printf("\t\tT : %06X\n", reg[_T]);
	if(addr == -1) printf("\tEnd Program\n");
	else printf("\tStop at checkpoint[%04X]\n", addr);
}

void print_runtime_error(int error)
{	
	switch(error)
	{
		case 1: printf("\tRuntime Error: Invalid Memory Access.\n"); break;
		case 2: printf("\tRuntime Error: Invalid Execution.\n"); break;
		default: break;
	}
}

void execute(int opcode, int r1, int r2, int ni, int ta)
{
	int val = 0, tmp;
	if(ni == IMM) val = ta;
	else if(ni == IND)
	{
		int tta;
		tta = get_value(ta); tta *= 256;
		tta += get_value(ta+1); tta *= 256;
		tta += get_value(ta+2); ta = tta;
		val += get_value(ta); val *= 256;
		val += get_value(ta+1); val *= 256;
    	val += get_value(ta+2);
	}
	else
	{
		val += get_value(ta); val *= 256;
		val += get_value(ta+1); val *= 256;
    	val += get_value(ta+2);
	}
	// two's complement
	if(val >= (int)(1 << 23)) val |= 0xff000000;

	switch(opcode)
	{
		case 0x18://ADD
			reg[_A] += val;
			break;
		case 0x90://ADDR
			reg[r2] += reg[r1];
			break;
		case 0x40://AND
			reg[_A] &= val;
			break;
		case 0xB4://CLEAR
			reg[r1] = 0;
			break;
		case 0x28://COMP
			if(reg[_A] < val) reg[_SW] = LT;
			else if(reg[_A] == val) reg[_SW] = EQ;
			else reg[_SW] = GT;
			break;
		case 0xA0://COMPR
			if(reg[r1] < reg[r2]) reg[_SW] = LT;
			else if(reg[r1] == reg[r2]) reg[_SW] = EQ;
			else reg[_SW] = GT;
			break;
		case 0x24://DIV
			if(val) reg[_A] /= val;
			break;
		case 0x9C://DIVR
			if(reg[r1]) reg[r2] /= reg[r1];
			break;
		case 0x3C://J
			reg[_PC] = ta;
			break;
		case 0x30://JEQ
			if(reg[_SW] == EQ) reg[_PC] = ta;
			break;
		case 0x34://JGT
			if(reg[_SW] == GT) reg[_PC] = ta;
			break;
		case 0x38://JLT
			if(reg[_SW] == LT) reg[_PC] = ta;
			break;
		case 0x48://JSUB
			reg[_L] = reg[_PC];
			reg[_PC] = ta;
			break;
		case 0x00://LDA
			reg[_A] = val;
			break;
		case 0x68://LDB
			reg[_B] = val;
			break;
		case 0x50://LDCH
			reg[_A] = get_value(ta);
			break;
		case 0x08://LDL
			reg[_L] = val;
			break;
		case 0x6C://LDS
			reg[_S] = val;
			break;
		case 0x74://LDT
			reg[_T] = val;
			break;
		case 0x04://LDX
			reg[_B] = val;
			break;
		case 0x20://MUL
			reg[_A] *= val;
			break;
		case 0x98://MULR
			reg[r2] *= reg[r1];
			break;
		case 0x44://OR
			reg[_A] |= val;
			break;
		case 0xAC://RMO
			reg[r2] = reg[r1];
			break;
		case 0x4C://RSUB
			reg[_PC] = reg[_L];
			break;
		case 0x0C://STA
			tmp = reg[_A];
			edit_(ta + 2, tmp % 256); tmp /= 256;
			edit_(ta + 1, tmp % 256); tmp /= 256;
			edit_(ta, tmp % 256);
			break;
		case 0x78://STB
			tmp = reg[_B];
			edit_(ta + 2, tmp % 256); tmp /= 256;
			edit_(ta + 1, tmp % 256); tmp /= 256;
			edit_(ta, tmp % 256);
			break;
		case 0x54://STCH
			tmp = reg[_A];
			edit_(ta, tmp % 256);
			break;
		case 0x14://STL
			tmp = reg[_L];
			edit_(ta + 2, tmp % 256); tmp /= 256;
			edit_(ta + 1, tmp % 256); tmp /= 256;
			edit_(ta, tmp % 256);
			break;
		case 0x7C://STS
			tmp = reg[_S];
			edit_(ta + 2, tmp % 256); tmp /= 256;
			edit_(ta + 1, tmp % 256); tmp /= 256;
			edit_(ta, tmp % 256);
			break;
		case 0xE8://STSW
			tmp = reg[_SW];
			edit_(ta + 2, tmp % 256); tmp /= 256;
			edit_(ta + 1, tmp % 256); tmp /= 256;
			edit_(ta, tmp % 256);
			break;
		case 0x84://STT
			tmp = reg[_T];
			edit_(ta + 2, tmp % 256); tmp /= 256;
			edit_(ta + 1, tmp % 256); tmp /= 256;
			edit_(ta, tmp % 256);
			break;
		case 0x10://STX
			tmp = reg[_X];
			edit_(ta + 2, tmp % 256); tmp /= 256;
			edit_(ta + 1, tmp % 256); tmp /= 256;
			edit_(ta, tmp % 256);
			break;
		case 0x1C://SUB
			reg[_A] -= val;
			break;
		case 0x94://SUBR
			reg[r2] -= reg[r1];
			break;
		case 0xE0://TD
			reg[_SW] = LT;
			break;
		case 0x2C://TIX
			reg[_X]++;
			if(reg[_X] < val) reg[_SW] = LT;
			else if(reg[_X] == val) reg[_SW] = EQ;
			else reg[_SW] = GT;
			break;
		case 0xB8://TIXR
			reg[_X]++;
			if(reg[_X] < reg[r1]) reg[_SW] = LT;
			else if(reg[_X] == reg[r1]) reg[_SW] = EQ;
			else reg[_SW] = GT;
			break;
		default: break;
	}
}

int run_process()
{
	int i;

	if(!debug_flag) reg[_PC] = get_start_addr();

	while(reg[_PC] < MAX_ADDR)
	{
		// fetch & decode
		int n = 0, i = 0, x = 0, b = 0, p = 0, e = 0;
		int opcode = get_value(reg[_PC]++), r1 = -1, r2 = -1, ta = -1;
		int ni = opcode % 4;
		int format;
		
		debug_flag = 1;

		opcode -= ni;	
		format = get_format_by_opcode(opcode);
		if(format == 1)
		{
			if(ni != 0) return 2;
		}
		else if(format == 2)
		{
			int regs = get_value(reg[_PC]++); 
			
			if(opcode % 4 != 0) return 2;
			r2 = regs % 16;
			r1 = regs / 16;
			if(r1 > 9 || r2 > 9) return 2;
		}
		else if(format == 3)
		{
			int tmp = get_value(reg[_PC]++);
			int remains = tmp % 16;
			int flags = tmp / 16;

			if(flags % 2) e = 1;
			else e = 0;
			flags /= 2;
			if(flags % 2) p = 1;
			else p = 0;
			flags /= 2;
			if(flags % 2) b = 1;
			else b = 0;
			flags /= 2;
			if(flags % 2) x = 1;
			else x = 0;

			ta = get_value(reg[_PC]++);
			ta += (remains << 8);

			if(e)
			{
				int extend_val = get_value(reg[_PC]++);
				
				ta = ta << 8;
				ta += extend_val;
				if(b || p) return 2;
			}
			
			if(x) ta += reg[_X];
			if(b == 0 && p == 1) 
			{
				if(ta > 2047) ta |= 0xfffff000;
				ta += reg[_PC];
			}
			else if(b == 1 && p == 0) ta += reg[_B];
			
			if(ta < 0 || ta >= MAX_ADDR) return 1;
			if(ni == IND)
			{
				int tta = get_value(ta);
				if(tta < 0 || tta >= MAX_ADDR) return 1;
			}
		}

		int cur_pc = reg[_PC] - 1;

		for(i = format + e - 1; i > 0; i--)
			if(find_bp(cur_pc - i) && ((cur_pc - i) > cur_debug || (cur_pc - i) < cur_debug - format - e))
			{
				cur_debug = cur_pc - i;
				reg[_PC] -= (format + e);
				return -1 * cur_debug;
			}

		// execute
		execute(opcode, r1, r2, ni, ta);
		
		if(reg[_PC] >= MAX_ADDR)
		{
			reg[_PC] = 0;
			break;
		}

		if(find_bp(cur_pc) && cur_pc != cur_debug)
		{
			cur_debug = cur_pc;
			return -1 * cur_debug;
		}
	}
	return 0;
}

void run_()
{
	int i;
	int error;

	error = run_process();
	if(error < 0) // breakpoint
	{
		print_register(-1 * error);
		return;
	}
	else if(error == 0) print_register(-1); // end program
	else print_runtime_error(error); // error

	for(i = 0; i < 10; i++) reg[i] = 0;
	reg[_L] = MAX_ADDR;
	debug_flag = 0;
	cur_debug = -1;
}
