#ifndef __PROC_COMMAND_H__
#define __PROC_COMMAND_H__

#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include "base.h"
#include "memory.h"
#include "opcode_table.h"
#include "symbol_table.h"
#include "assembler.h"
#define MAX_LEN 1001
#define MAX_TOKEN 1001

int run(int comm, int para1, int para2, int para3, int token_size, char token[MAX_LEN][MAX_LEN]);

int tokenize(char* str, char token[MAX_TOKEN][MAX_LEN]);

int char_to_hexa(char c);

int str_to_val(char* str, int addr_mode);

int is_in_dir(char filename[MAX_LEN]);

int make_command(char* str, int* comm, int* para1, int* para2, int* para3, int* token_size, char token[MAX_TOKEN][MAX_LEN]);

void print_error(int error);

#endif
