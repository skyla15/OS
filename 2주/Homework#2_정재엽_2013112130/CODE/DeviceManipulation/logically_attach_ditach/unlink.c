/*
	int unlink(const char*path
	char *path : �����Ϸ��� ��ũ �̸�
	��ȯ�� : ���� 1, ���� -1
*/

#include <stdio.h>
#include <unistd.h>

int main()
{
	unlink("sample.txt");

	return 0;
}
