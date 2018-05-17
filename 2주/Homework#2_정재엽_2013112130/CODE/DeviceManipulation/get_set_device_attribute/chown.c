/*
	소유권 변경시 사용 
	root 계정에서만 사용 가능
	int chown(const cahr* path, uid_t owner, gid_t group)
	path : 파일디스크립터(경로 파일이름), owner : 소유자 아이디(if -1, 실행취소), group : 소유자 그룹 ( if -1 , 실행 취소 )
	반환값 : 성공 0 실패 -1
*/

#include <stdio.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/stat.h>

int main(){
	struct passwd *root_pw;
	if(0!= getuid())
	{
		printf(" Access not authorized \n");
		return -1;
	}
	root_pw = getpwnam("jwjw");
	if(-1 == chown("./test.txt", root_pw->pw_uid, root_pw->pw_gid))
		printf("소유권 변경 실패\n");
}
