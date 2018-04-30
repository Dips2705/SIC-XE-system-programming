#ifndef __BASE_H__
#define __BASE_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#define MAX_LEN 1001

typedef struct _node
{
	char comm[MAX_LEN];
	struct _node* next;	
} node;

void help_();

void dir_();

void add_history(char comm[MAX_LEN]);

void clear_history();

void history_();

void type_(char filename[MAX_LEN]);

#endif
