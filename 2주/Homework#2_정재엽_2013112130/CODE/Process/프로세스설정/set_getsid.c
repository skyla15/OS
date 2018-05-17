#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

/*
	pid_t getsid(void)
	현재 프로세스가 속한 세션의 id를 가져옵니다.
	
	pid_t setsid(void)
	만약 setsid()를 호출하는 프로세스(:12)가 프로세스 그룹의 리더가 아니라면 
	새로운 세션을 생성하고 해당 세션에 대한 그룹의 리더가 된다.
	이렇게 setsid()를 호출해서 세션을 생성하고 그룹의 리더가 되었다면 
	세션아이디와 그룹아이디는 프로세스아이디(PID)와 동일하게 된다. 
	이후 이 프로세스에서 생성되는 모든 (자식)프로세스들은 
	이 세션 아이디와 그룹아이디를 가지게 된다. 

	반환값 : 호출한 프로세스의 세션 id
*/
int main(){
	pid_t pid;
	if((pid=fork()) > 0 ){ //부모프로세스가 수행하는 부분
		sleep(1);
		exit(1);
	}
	else if ( pid == 0 ) { //fork 성공시
		printf("old session id : %d \n", getsid(0));
			//getsid(0) => 현재 프로세스의 세션id를 가져온다
		printf("new session id : %d \n:", setsid());
			//새로운 세션의 번호를 생성한다.
		sleep(600);
	}
	return 0 ;
}

