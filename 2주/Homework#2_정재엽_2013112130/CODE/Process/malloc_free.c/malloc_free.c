#include <stdio.h>
#include <stdlib.h>

/*
	malloc : c에서 힙에 메모리 동적할 때 사용하는 것과 동일합니다.
	반환값은 void 포인터입니다.
	
	free : 동적할당된 메모리를 해제합니다.
*/
int main(){
	int *pi;
	pi = (int*)malloc(sizeof(int));
	*pi = 1234567890;
	printf("값 : %d, 주소 : %x\n", *pi, pi);
	free(pi);
	printf("pi의 메모리 해제 완료\n");
}
