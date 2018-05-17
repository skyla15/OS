/*
	read()
	open()으로 열기를 한ㅍ ㅏ일의 내용을 읽습니다.
	헤더 : unistd.h
	ssize_t read (int fd, void *buf, size_t nbytes)
	fd : 파일디스크립터 // open으로 받아옵니다.
	buf : 읽어들일 버퍼의 크기 
	nbytes : 읽어 올 때 사용하는 버퍼의 크기
	반환값 :
	정상적으로 실행되었다면 읽어들인 바이트 크기 반환 
	실패시 -1 반환 
*/

#include <stdio.h>         // puts()
#include <string.h>        // strlen()
#include <fcntl.h>         // O_WRONLY
#include <unistd.h>        // write(), close()

#define  BUFF_SIZE   5     // 버퍼의 크기가 작습니다.

int main()
{
	char     buff[BUFF_SIZE];
	int      fd;
	ssize_t  rd_size;

	if (0 < (fd = open("./write.txt", O_RDONLY)))
	{
		while (0 < (rd_size = read(fd, buff, BUFF_SIZE - 1)))   //  4 byte씩 읽씁니다.
		{
			buff[rd_size] = '\0';      // puts()를 위해 NULL을 대입
			puts(buff);
		}
		close(fd);
	}
	else
	{
		printf("파일 열기에 실패했습니다.\n");
	}
	return 0;
}
