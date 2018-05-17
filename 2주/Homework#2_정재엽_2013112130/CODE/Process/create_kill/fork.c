#include <stdio.h>
#include <unistd.h>

/*
	fork()는 현재 실행되는 프로세스에 대하여 복사본 프로세스를 생성 
	하나의 프로세스가 실행중 fork()를 만나면 자식프로세스를 생성합니다.
        반환값 : fork() 호출 후 성공시 child process id, child에서는 0: 
                 실패 : -1,
*/


int main(){
	int counter = 0;
	pid_t pid;
	pid = fork();
	switch(pid){
		case -1 : 
		{	
			printf("생성 실패 \n ");
			return -1;
			break;
		}
		case 0 :
		{
			printf("자식 프로세스 생성 성공\n");
		}
	}
	
	return 0;
}
		
		
