/*
	int unlink(const char*path
	char *path : 삭제하려는 링크 이름
	반환값 : 성공 1, 실패 -1
*/

#include <stdio.h>
#include <unistd.h>

int main()
{
	unlink("sample.txt");

	return 0;
}
