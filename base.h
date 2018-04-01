#ifndef __BASE_H__
#define __BASE_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#define MAX_LEN 1001

// history를 저장할 링크드리스트의 node 구조체
// 명령어를 저장할 문자열과
// 다음 노드를 가리키는 포인터로 정의된다.
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
