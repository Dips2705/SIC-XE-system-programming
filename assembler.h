#ifndef __ASSEMBLER_H__
#define __ASSEMBLER_H__

#include <stdio.h>
#include <string.h>
#include "proc_command.h"
#include "opcode_table.h"
#include "symbol_table.h"
#define MAX_LEN 1001
#define MAX_TOKEN 1001

int is_empty_line(char* str);

int is_comment(char* str);

int is_operator(char* str);

int is_directive(char* str);

int is_register(char* str);

int is_valid_symbol(char* str);

int is_in_symbol_table(char* str);

int is_valid_constant(char* str);

int str_to_dec(char* str, int boundary);

int upper_char_to_hexa(char c);

int str_to_hexa(char* str, int boundary);

int assem_tokenize(char* str, char token[MAX_TOKEN][MAX_LEN], int* s, int* e);

int comment_tokenize(char* str, char token[MAX_TOKEN][MAX_LEN]);

void str_pop_front(char* str);

int calc_byte_operand(char* str);

int make_intermediate_file(char* asm_file, char* itm_file, char* str, int* line_num);

void byte_operand_to_hexa_code(char* str, char* target);

int make_object_file(char* itm_file, char* asm_file, char* obj_file, char* lst_file, char* str, int* line_num);

void print_assemble_error(int error, char* str, int n);

void assemble_(char* filename);

#endif
