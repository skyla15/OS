/*
	close()
	open()함수로 열기한 파일 사용 중지 => fcntl.h
	write / read / close => std.h에 정의되어있음 
	
	int close(int fd)
	fd : 파일 디스크립터 / int : 정상적으로 close : 0 , 비정상 : -1	
*/

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

int main(){
	char* temp = "OS Assignmet - close example";
	int fd;
	
	if( 0 < (fd = open("./close.txt", O_WRONLY | O_CREAT | O_EXCL, 0644)))
	{
		write(fd, temp, strlen(temp));
		close(fd);
	}
	else
	{
		printf("파일 열기 실패");
	}
	return 0;
}
