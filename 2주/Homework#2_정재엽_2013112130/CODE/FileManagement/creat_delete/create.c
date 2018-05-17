#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#define BUFF_SIZE 5

/*
	creat : 이미 같은 이름의 파일이 있다면 파일의 내용을 삭제 후 열기
	해당 헤더 : hcntl.h 
	반환값 : int 파일 디스크립터  
	int creat(cont char* file, mode_t mode) 
		  파일 경로       /  생성파일 접근 권한 
*/

int main(){
	char *temp = "OS Assignment-from create.c";//생성할 파일에 들어갈 string
	int fd; //file describer
	
	if ( 0 < (fd = creat("./creat.txt", 0644)))
	{
		write(fd, temp, strlen(temp));
		close(fd);
	}	
	else
	{
		printf("파일생성 실패\n");
	}
	return 0;
}
	
