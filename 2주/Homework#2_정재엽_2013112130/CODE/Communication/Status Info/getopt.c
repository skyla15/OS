#include <stdio.h>
#include <unistd.h>
/*
	프로그램 실행시 받은 인수 분석, 권한 확인 가능 
	optarg : 인수를 필요하는 옵션처리시 사용, test.txt 이용
	optpot : 지정되지 않은 옵션을 경우, 시스템 지정 옵션 출력

	int getopt(int argc, char * const *argv, const char *shotopts)
	반환 : 지정되지 않은 옵션일 경우 '?' 반환
               더이상 옵션이 없을 경우 -1 반환 
	argc : 인수의 갯수
	argv : 인수의 내용(받아오는 것)
	shortopts : 검색하려는 옵션의 문자열 / 이 예시에서는 o, r
               
*/

int main( int argc, char** argv){
	int is_f = 0;
	int is_o = 0;
	int is_r = 0;
	char *file_name = NULL;
	int param_opt;

	while ( -1 != ( param_opt = getopt(argc, argv, "f:or")))
	{
		switch(param_opt)
		{
			case 'f' : is_f = 1;
				   file_name = optarg;
				   break;

			case 'o' : is_o = 1;
				   break;
		

			case 'r' : is_r = 1;
				   break;
			
			case '?' : printf("알수 없는 옵션 : %c\n", optopt);
		}
	}

	if(is_f&&(NULL != file_name)) printf("옵션 -f 파일 이름 : %s\n", file_name);
	if(is_o) printf("옵션 -o 있음\n");
	if(is_r) printf("옵션 -r 있음\n");
	return 0;
}
