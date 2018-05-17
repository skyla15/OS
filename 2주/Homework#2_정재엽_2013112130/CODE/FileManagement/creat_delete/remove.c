/* 
	remove : 파일 및 디렉토리 삭제 
	헤더 : stdio.h
	int remove(cont char* pathname)
	반환값 : 성공 0 / 실패 -1
	pathname : 삭제할 파일 혹은 디렉토리 	
*/

#include <stdio.h>

int main(){
	//삭제할 파일을 현재 디렉토리에 같이 넣어놨습니다.
	//test.txt
	if ( -1 == remove("test.txt"))
		printf("파일삭제 실패\n");
	else 
		printf("파일삭제 성공\n");

	return 0;
}
