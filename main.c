#include "main.h"
#define MAX_LEN 1001
#define MAX_TOKEN 1001

// 명령어, 매개변수를 저장
int comm, para1, para2, para3, t_sz;
// 입력받는 문자열 저장
char command_line[MAX_LEN];
// 입력받은 문자열을 tokenize한 token 문자열들 저장
char token[MAX_TOKEN][MAX_LEN];

// 요약: 프로그램이 실행되는 메인 함수
// 기능: 문자열을 입력받아서 다루기 쉬운 형태로 가공하고
// 		 에러가 없으면, 해당 명령어를 실행하고
//		 에러가 있으면, 에러 메시지를 출력한다.
//		 프로그램의 시작에 opcode hash table을 초기화 및 생성하고
//		 프로그램의 끝에 history list와 opcode hash table에 사용한
//		 동적할당 메모리를 해제한다.
// 반환: 0 = 정상종료 / -1 = error 
int main()
{	
	// 프로그램 종료 체크
	int exit = 0;
	
	// opcode hash table 생성	
	make_table();

	while(!exit)
	{
		// 에러 체크, 문자열 길이 저장
		int error, len;
		
		// 변수 초기화
		comm = -1, para1 = -1, para2 = -1, para3 = -1, t_sz = 0;
		memset(command_line, 0, sizeof(command_line));
		memset(token, 0, sizeof(token));

		// 프롬프트 출력 및 문자열 입력
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

		// 문자열을 가공하고, 에러 체크
		error = make_command(command_line, &comm, &para1, &para2, &para3, &t_sz, token);
		if(!error)
		{
			add_history(command_line);
			exit = run(comm, para1, para2, para3, t_sz, token);
		}
		// 에러가 있으면, 에러 메세지를 출력
		else print_error(error);
	}

	// 사용한 동적할당 메모리 해제
	clear_history();
	clear_table();
	clear_sym_table();
	return 0;
}
