#include "base.h"
#define MAX_LEN 1001

// history list의 head 포인터
node* head = NULL;
// history list의 tail 포인터
node* tail = NULL;
// history list의 size
int history_size = 0;

// 요약: 명령어 리스트를 출력하는 함수
// 기능: 명령어 리스트를 출력한다.
// 반환: 없음.
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
	printf("\tassemble filename\n");
	printf("\ttype filename\n");
	printf("\tsymbol\n");
}

// 요약: 현재 디렉터리의 파일 리스트를 출력하는 함수
// 기능: dirent.h, sys/stat.h 헤더의 함수를 이용하여
// 		 디렉터리의 파일명을 순서대로 읽어들이고,
//		 디렉터리, 실행파일, 일반파일을 구분하여
//		 해당하는 표시를 덧붙여 출력한다.
// 반환: 없음.
void dir_()
{
	int i;
	// 현재 디렉터리 포인터
	DIR* dir = opendir(".");
	// 파일 순차접근에 사용되는 포인터
	struct dirent* p;
	// 파일의 상태를 저장
	struct stat fs;
	
	for(i = 0; p = readdir(dir); i++)
	{
		int len, dir_flag = 0, exe_flag = 0;
		char filename[101], c = ' ';
		
		// 파일명 복사
		strncpy(filename, p->d_name, 100);
		stat(p->d_name, &fs);

		if(!strcmp(filename, ".") || !strcmp(filename, ".."))
		{
			i--;
			continue;
		}
		
		// 디렉터리/실행파일 표시
		if(fs.st_mode & S_IFDIR) c = '/', dir_flag = 1;
		else if(fs.st_mode & S_IXUSR) c = '*', exe_flag = 1;
		
		len = strlen(filename);
		filename[len] = c;
		filename[len+1] = '\0';
		
		// 출력
		if(i != 0 && i % 4 == 0) printf("\n");
		if(dir_flag) printf("%c[1;34m", 27);
		if(exe_flag) printf("%c[1;32m", 27);
		printf("\t%-15s", filename);
		if(dir_flag || exe_flag) printf("%c[0m", 27);
	}
	printf("\n");
	closedir(dir);
}

// 요약: history에 입력받은 명령어를 추가하는 함수
// 기능: 입력받은 명령어 문자열로 새로운 노드를 
// 		 초기화하고, 링크드리스트에 추가한다.
// 반환: 없음.
void add_history(char comm[MAX_LEN])
{
	node* n_node = (node*)malloc(sizeof(node));
	
	strcpy(n_node->comm, comm);
	n_node->next = NULL;
	
	// 리스트가 비어있으면, head = tail = n_node
	if(head == NULL) head = n_node, tail = n_node;
	// 비어있지 않다면, tail에 추가
	else tail->next = n_node, tail = n_node;

	history_size++;
}

// 요약: 동적할당 된 메모리를 해제하는 함수
// 기능: history를 저장하는데 사용한
// 		 링크드리스트 노드들의 동적할당 된
// 		 메모리를 한번에 해제한다.
// 반환: 없음.
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

// 요약: history를 출력하는 함수
// 기능: history를 저장한 링크드리스트를
// 		 순차적으로 접근하면서 history를
// 		 출력한다.
// 반환: 없음.
void history_()
{
	int i=1;
	node* it;

	// history가 비어있는 상태에서
	// history 명령어가 호출되면
	// 아무것도 출력하지 않고 리턴한다.
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

void type_(char filename[MAX_LEN])
{
	FILE* fp = fopen(filename, "r");
	char str[MAX_LEN];
	while(fgets(str, MAX_LEN, fp) != NULL) printf("%s", str);
	fclose(fp);
}
