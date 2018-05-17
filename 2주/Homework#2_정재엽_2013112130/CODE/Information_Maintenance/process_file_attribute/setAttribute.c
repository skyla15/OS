/*
	chmod
	헤더 : sys/stat.h
	int chmod (const char * file , mode_t mode)
	file : 권한을 바꿀 파일, mode : 접근권한
	반환값 : 성공 : 0 / 실패 : -1
*/

#include <sys/stat.h>
#include <stdio.h>

int main()
{
	if(-1 == chmod("test.txt", 0644)){
		printf("접근 권한 변경 실패\n");
	}
	else{
		printf("접근 권한 변경 성공\n");
	}
	return 0;
}
