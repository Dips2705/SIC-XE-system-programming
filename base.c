#include "base.h"
#define MAX_LEN 1001

node* head = NULL;
node* tail = NULL;
int history_size = 0;

void help_()
{
	printf("\th[elp]\n");
	printf("\td[ir]\n");
	printf("\tq[uit]\n");
	printf("\thi[story]\n");
	printf("\tdu[mp] [start, end]\n");
	printf("\te[dit] address, value\n");
	printf("\tf[ill] start, end, value\n");
	printf("\treset\n");
	printf("\topcode mnemonic\n");
	printf("\topcodelist\n");
}

void dir_()
{
	int i;
	DIR* dir = opendir(".");
	struct dirent* p;
	struct stat fs;
	
	for(i = 0; p = readdir(dir); i++)
	{
		int len, dir_flag = 0, exe_flag = 0;
		char filename[101], c = ' ';
		
		strncpy(filename, p->d_name, 100);
		stat(p->d_name, &fs);

		if(!strcmp(filename, ".") || !strcmp(filename, ".."))
		{
			i--;
			continue;
		}
		
		if(fs.st_mode & S_IFDIR) c = '/', dir_flag = 1;
		else if(fs.st_mode & S_IXUSR) c = '*', exe_flag = 1;
		
		len = strlen(filename);
		filename[len] = c;
		filename[len+1] = '\0';
		
		if(i != 0 && i % 4 == 0) printf("\n");
		if(dir_flag) printf("%c[1;34m", 27);
		if(exe_flag) printf("%c[1;32m", 27);
		printf("\t%-15s", filename);
		if(dir_flag || exe_flag) printf("%c[0m", 27);
	}
	printf("\n");
	closedir(dir);
}

void add_history(char comm[MAX_LEN])
{
	node* n_node = (node*)malloc(sizeof(node));
	
	strcpy(n_node->comm, comm);
	n_node->next = NULL;
	
	if(head == NULL) head = n_node, tail = n_node;
	else tail->next = n_node, tail = n_node;

	history_size++;
}

void clear_history()
{
	node* it = head;
	while(it != NULL)
	{
		node* tmp = it->next;
		free(it);
		it = tmp;
	}
}

void history_()
{
	int i=1;
	node* it;

	if(history_size == 1) return;

	for(it = head; it != NULL; it = it->next)
	{
		printf("\t");
		printf("%c[1;33m", 27);
		printf("%-3d", i++);
		printf("%c[0m", 27);
		printf(" %s", it->comm);
	}
}
