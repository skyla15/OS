#include <stdio.h>
#include <time.h>

/*
	헤더 : time.h
	int time(time_t *t)
	int : 1970년 1월 1일 0시부터 함수를 호출할 때까지의 초 카운트
	time_t* t : 시간을 담을 변수 
*/

int main()
{
	time_t current_time;
	time(&current_time);
	printf("%ld\n", current_time); //현재까지 지난 시간을 보여줌 from 1970 0101 0000
	printf( ctime(&current_time));
	return 0;
}

