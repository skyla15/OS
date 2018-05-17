#include <stdio.h>
#include <signal.h>
#include <unistd.h>


/*
	int pause(void);
	시그널을 받을 때까지 대기합니다.
	반환값은 항상 -1
*/
void sigint_handler(int signo)
{
	printf("날 깨웠구나?\n");
 
}

int main()
{
	printf("No Signal, No talk. please press CTRL +  C\n");
	signal(SIGINT, sigint_handler);
	// CTRL + C로 바로 종료된느 것을 막기 위해 SIGINT를 사용자 핸들러로 지정 
	pause();
	printf("I don't wanna see your face anymore so bye~\n");
}
