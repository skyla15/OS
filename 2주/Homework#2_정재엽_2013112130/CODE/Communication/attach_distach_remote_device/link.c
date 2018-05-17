/*
	int link(const char* Old_path, const char* New_path)
	반환값 : 성공 0, 실패 -1
	old path 원래 있는 파일 
	new path 새롭게 만들려는 파일 
*/

#include <stdio.h>
#include <unistd.h>

int main()
{
	if ( -1 == link("link_a.txt", "sample.txt"))
		printf("링크생성 실패");
}
