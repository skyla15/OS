/*
	open() 
	C라이브러리 fopen() => Sys call에서는 open()
	open시에는 읽기 전용 / 쓰기 전용 / 읽기 쓰기 전용 옵션 
	=> 옵션은 fcntl.h에 정의 되어있음 
	O_RDONLY / O_WRONLY / RONLY  
	O_CREAT : 만약에 파일이 없다면 생성.
	O_EXCL : O_CREAT을 사용하면서 같이 사용시 
		 덮어쓰기를 방지할 수 있음 
	ex) fd = open("./test.txt", O_WRONLY | O_CREAT | O_EXCL, authority)
	
	int open(cont char *FILENAME, int FLAGS[,mode_t MODE]) 
	파일이름 / 열기옵션 및 O_CREAT을 사용하였을 때 지정되는 접근 권한
	정상으로 open 하였을 경우 fd > 0 , 실패시 -1 반환 	 
*/

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{
	char * temp = "OS Assignment- open";
	int fd; // 파일디스크립터 

	if( 0 < (fd = open("./open.txt", O_WRONLY | O_CREAT | O_EXCL, 0644)))
	{
		write(fd , temp, strlen(temp));
		close(fd);
	}
	else
	{
		printf("파일 열기 실패\n");
	}
	return 0;
}
