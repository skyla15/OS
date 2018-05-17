#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

/*
int execvp(cont char* file, char*const argv[])
명령라인을 배열로 받습니다.
명령 라인으로 들어오는 인수를 실행하고 자신의 프로세스는 종료합니다.
반환값 : 실패일 경우 -1
*/

int spawn (char* program, char** arg_list){
	pid_t child_pid;
	
	child_pid = fork();
	if(child_pid != 0){ //프로세스 성공이 되었을 경우 
		return child_pid;
	}
	else{
		execvp(program, arg_list); 
		//경로로 들어온 프로그램 실행 후 현재 프로레스 종료
		//루트 디렉토리의 파일목록들을 보여줌
		fprintf(stderr, "an error has occured\n");
		abort();
		//오류가 발생하였을 경우 abort를 이용한 강제종료 
	}
}

int main()
{
	char* arg_list[] = { "ls", "-l", "/", NULL };
	spawn("ls", arg_list);
	printf("done with main Program\n");

}
