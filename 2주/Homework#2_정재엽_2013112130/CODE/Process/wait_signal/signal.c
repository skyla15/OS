#include <stdio.h>
#include <signal.h>

/*
signal(SIGNUM, 시그널처리 핸들러)
⇨ SIGNUM = SIGKILL(프로세스 죽여라), SIGALARM(알람발생), SIGSTP(프로세스종료),                   SIGINT(프로세스 인터럽트, 잠시 차단)
⇨ 시그널핸들러 : 위의 시그널이 들어올경우 실행되는 함수
⇨ ex) ctrl-c를 누를 경우 임의의 함수 function이 실행되도록하기 위해서는
signal(SIGINT, function()) 이렇게 표현할 수가 있다.
*/
void (*old_fun)(int);

void sigint_handler(int signo){
	printf("Ctrl-C 누른거 알고있음\n");
	printf("한번 더 그러면 종료할거임\n");
	signal(SIGINT, old_fun);
}

int main(){
	old_fun = signal(SIGINT, sigint_handler);
	while(1)
	{
		printf("운영체제 과제중\n");
		printf("종료 : ctrl + c\n");
		sleep(8);
	}
}
