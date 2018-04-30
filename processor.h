#ifndef __PROCESSOR_H__
#define __PROCESSOR_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "linking_loader.h"
#include "memory.h"
#include "opcode_table.h"
#include "debugger.h"

void print_register(int addr);

void print_runtime_error(int error);

void execute(int opcode, int r1, int r2, int ni, int ta);

int run_process();

void init_process();

void run_();

#endif
