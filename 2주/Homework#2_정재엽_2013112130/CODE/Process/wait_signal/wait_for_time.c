#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>


/*
sleep() : 지정한 시간동안 현재 프로세스 대기, 시그널 받을 경우 대기 해제
⇨ sleep(1) 설정할 경우 1초동안 현재 실행중인 프로세스를 대기시킵니다.
반환값 : 남은 시간, 인수는 초단위로 받습니다.
*/
int main()
{
	int pid;
	int status;
	
	pid = fork();
	
	if(pid<0){
		printf("자식 생성 실패\n");
		exit(1);
	}
	if(pid == 0)
	{
		int i;
		for(i=0 ; i < 5 ; i++)
		{
			printf("child : %d\n", i);
			sleep(2);
			// 프로세스는 약간의 시간을 기다리도록 함 
		}
		exit(3);
	}
	else{
		printf("자식프로세스 종료 기다리는 중(%d)\n", pid); 
		//자식프로세스 id = pid
		wait(&status);
		printf("child process is finished (%d) \n ", status);
		//프로세스 종료 status
	}
	return 0;
}
	
