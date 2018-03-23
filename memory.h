#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <stdio.h>

int max(int x, int y);

void dump_(int start, int end);

void edit_(int addr, int val);

void fill_(int start, int end, int val);

void reset_();

#endif
