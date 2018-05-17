/*
	access() : 파일 유무 및 접근 권한 확인
	헤더 ; unistd.h
	int access(cont char* pathname, int mode)
	pathname : 파일 혹은 디렉토리 전체 이름
	mode : 체크하려는 권한
	반환값 : 0 -파일 존재 / -1 : 체크하려는 권한이 거부됨

	mode : R_OK (read ok?), W_OK (write ok?), X_OK (execute ok?)
	       F_OK (is file?)
*/

#include <stdio.h>
#include <unistd.h>

int main(void)
{
	char *file_name = "./test.txt";
	if (0 == access(file_name, F_OK))
	{
		printf("%s 파일이 있습니다.\n", file_name);

		if (0 == access(file_name, R_OK | W_OK))
		{
			printf("읽거나 쓰기가 가능합니다.\n");
		}
		else
		{
			printf("읽지 못하거나 내용을 변경할 수 없습니다.\n");
		}
	}
}
