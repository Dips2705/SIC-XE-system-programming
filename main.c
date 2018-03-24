#include "main.h"
#define MAX_LEN 1001
#define MAX_TOKEN 1001

int comm, para1, para2, para3, t_sz;
char command_line[MAX_LEN];
char token[MAX_TOKEN][MAX_LEN];

int main()
{	
	int exit = 0;
	
	make_table();

	while(!exit)
	{
		int error, len;
		
		comm = -1, para1 = -1, para2 = -1, para3 = -1, t_sz = 0;
		memset(command_line, 0, sizeof(command_line));
		memset(token, 0, sizeof(token));

		printf("sicsim> ");
		fgets(command_line, sizeof(command_line), stdin);
		
		// flush buffer
		len = strlen(command_line);
		if(command_line[len-1] != '\n')
		{
			char tmp = 0;
			command_line[len-1] = '\n';
			while(tmp != '\n') tmp = getchar();
		}

		error = make_command(command_line, &comm, &para1, &para2, &para3, &t_sz, token);
		if(!error)
		{
			add_history(command_line);
			exit = run(comm, para1, para2, para3, t_sz, token);
		}
		else print_error(error);
	}

	clear_history();
	clear_table();
	return 0;
}
