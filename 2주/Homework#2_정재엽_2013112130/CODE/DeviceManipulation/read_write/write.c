/*
	헤더 : unistd.h
	ssize_t write(int fd, const void *buf, size_t n)
	fd : 파일 디스크립터, buf : 쓰기할 내용을 담을 버퍼
	size_t n : 쓰기할 바이트의 수 
	반환값 : size_t : 정상 -> 쓰기한 바이트의 갯수 / 실패 : -1

	표준출력함수 fgets()는 첫째 항씩 읽어서 반환하지만 read의 경우
		     버퍼의 크기를 조절하여 한번에 읽는 게 가능
		     파일 시스템 직접 접근 by systemcall
*/

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#define BUFF_SIZE 100


int main(int argc, char** argv)
{
	int fd;
	char * temp = "OS Assignment - Written by filedecripter" ;

	if (0 < (fd = open(argv[1], O_WRONLY | O_CREAT | O_EXCL, 0644)))
	{
		write(fd, temp, BUFF_SIZE);
	}
	else
	{
		printf("파일 열기 실패\n");
	}
	return 0;
}
