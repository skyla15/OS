#include <stdio.h>
#include <unistd.h>
#include <signal.h>

int main(int argc, char* argv[])
{
	if ( argc < 2)
	{
		printf("사용법 : ./killer p_pid \n");
	}
	else
	{
		kill(atoi(argv[1]), SIGKILL); 
		//프로세스 아이디를 인수로 받아들여서 SIGKILL 시그널을 보냅니다.
	}
}
