// 프로그램 실행 후 현재까지의 시간을 받아옵니다.
//clock_t clock(void) 
#include <time.h>
#include <stdio.h>
#include <unistd.h>

int main()
{
	clock_t clk_start;
	double diff;
	
	clk_start = clock();
	while(1)
	{
		diff = (double) (clock() - clk_start) / CLOCKS_PER_SEC;
		printf("%lf", diff);
		sleep(3); 
	}
	return 0;
}
