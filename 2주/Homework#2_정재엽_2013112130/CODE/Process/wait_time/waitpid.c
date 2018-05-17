#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>

/*
	waitpid(pid, status, option) : 
	자식 프로세스가 종료되는 것을 기다림, 반환값은 종료시킨 자식 pid입니다. 
    ⇨ wait과 다른 것은 모든 자식프로세스가 종료되야 값을 반환합니다. 
    ⇨ 입력값의 pid는 체크할 자식 pid이며 status는 5와 같습니다.
*/

int main(){
	pid_t pid1, pid2;
	int status;
	pid1 = fork();
	if(pid1 > 0){
		waitpid(pid1, &status, 0);
			//부모프로세스는 자식프로세스가 끝나기를 기다림
		printf("parent : child1 - exit (%d) \n", status);
			//자식프로세스 종료 후 메세지 출력 
	}
	else if(pid1 == 0 ){
		sleep(1); //fork 한뒤에 잠깐 쉬게만듬 -> 부모프로세스 실행
		exit(1);
	}
	else printf("failed to fork\n");
	return 0;
}
	
	
