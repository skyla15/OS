/*
	int iotcl(int d, int request, .... );
	d : 파일 핸들러, request : 디바이스 종속 명령 
	
*/

#include <fcntl.h>
#include <linux/cdrom.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
	//쉘에서 해당 디바이스에 대한 파일 디스크립터를 입력
	//해당 디바이스에 대한 device number를 인자로 전달 
	// => ./iotcl device#
	int fd = open (argv[1], O_RDONLY, 0);
	//Ejecting the cd-ROM
	ioctl(fd, CDROMEJECT);
	//Release device
	close(fd);
	
	return 0;
}

