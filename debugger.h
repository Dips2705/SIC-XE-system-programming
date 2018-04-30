#ifndef __DEBEGGER_H__
#define __DEBEGGER_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int find_bp(int addr);

void print_bp_list();

void set_bp(int addr);

void clear_bp_list();

void bp_(int addr);

#endif
