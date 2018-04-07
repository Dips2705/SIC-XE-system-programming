#ifndef __ASSEMBLER_H__
#define __ASSEMBLER_H__

#include <stdio.h>
#include <string.h>
#include "proc_command.h"
#include "opcode_table.h"
#include "symbol_table.h"

int is_register(char* str);

void assemble_(char* filename);

#endif
