#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/*
	wait(&status) : 현재 프로세스의 자식 프로세스가 종료되는 것을 기다림
     	            ⇨ status는 자식프로세스의 상태를 나타내며 정상적으로 종료가 된다면
                    0을 반환합니다.
                     ⇨ wait함수는 자식프로세스가 정상적으로 종료되었다면 자식프로세스 id를
		             반환합니다. 비정상적으로 종료가 된다면 에러값을 반환합니다.
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
	
