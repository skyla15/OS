#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <wait.h>

int testing(int x, int *y);

int main(){
	int result;
	int x, *y;
	y = (int*)malloc(sizeof(int));
	x = 10;
	*y = 20;
	printf("메인에서 할당된 y주소 : %p\n", y);
	result = testing(x,y);
	printf("%d\n",result);
}





int testing(int x, int *y){
	pid_t pid;
	int status;	
	
	printf("P : X = %d, Y = %d\n", x, *y);
	pid = fork();
	wait(&status);
	printf("\n\n현재 프로세스 %d\n",pid);
	if(pid==0){ //자식프로세스 실행
		printf("자식프로세스에서 pid 반환값 : %d\n",pid);
		printf("자식프로세스 : %d\n", getpid());
		printf("자식프로세스 y주소 : %p, y값 : %d \n", y, *y);
		printf("X : X = %d, Y= %d\n", x, *y);
		x = x + 10;
		*y = *y + 10;
		printf("X : X = %d, Y = %d, y주소 : %p\n", x, *y, y);
		sleep(3);
		printf("자식프로세서에서 주는 y : ");
		return *y;
		
	}

	else if(pid > 0){ //부모프로세스 
		wait(&status);
		printf("부모프로세서 pid 반환값 : %d\n", pid);
		printf("부모프로세스 %d\n", getpid());
		printf("부모프로세스 y주소 : %p, y값: %d \n", y, *y);
		printf("P : X = %d, Y = %d\n", x, *y);
		x=  x+15;
		*y = *y + 5;
		printf("P : X = %d, Y = %d, y주소 : %p\n", x, *y, y);
		printf("부모프로세스에서 주는 y : ");
		return *y;
	}
	else 
		printf("Fork error\n");
}

