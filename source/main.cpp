#include "filesystem.h"

int main()
{
	FileSystem::getInstance();//����ģʽ��ʼ��

	/*��¼ѭ��*/
	FileSystem::getInstance().Func_LoginLoop();//ѭ����¼

	/*����shell����ʼѭ������*/
	FileSystem::getInstance().Func_ShellLoop();

	return 0;
}



