#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <string.h>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <set>
#include <map>
#include <sstream>
#include <algorithm>
#include "parameter.h"
using namespace std;

/*һ��״̬�궨�壬����ö���� or ������*/
#define TRUE		1
#define FALSE		0
#define OK		1
#define ERROR		0
#define INFEASIBLE	-1
typedef int Status;


//�������ӵ�block��
class BlockGroup {
public:
	int s_nfree;
	int s_free[BLOCK_FREE_NUM];
};


/*����Ŀ���Թ������ļ�ϵͳ�ǣ�
superblock����inode�����ļ������������δ洢��νṹ*/
/*superblock��ռ��0��1����block*/
class SuperBlock {
public:
	/*inodeΪ�ļ����������ƣ����ڼ򻯿��ǣ�������ʹ�ó������ӷ���д��Ϊ100*/
	int disk_block_size;//���̵�block����
	int s_isize;			//inodeռ�õ�block�̿���
	int s_ninode;			//ֱ�ӹ���Ŀ���inode��
	int s_inode[INODE_NUM];	//�����inodeջ���������

	/*ʵ��block�������϶࣬����ʹ�ó������ӷ�*/
	int s_fsize;				//������ʹ�õ�block����
	int s_nfree;				//ֱ�ӹ���Ŀ����̿���
	int s_free[BLOCK_FREE_NUM];	//ֱ�ӹ���Ŀ����̿�������

	/*���������*/
	int s_fmod;		//�ڴ���superblock���޸ģ�д��
	int s_ronly;	//�ļ�ϵͳֻ�ܶ���
	time_t s_time;	//���һ�θ���ʱ��
	int padding[66];//��䣬ʹsuperblock��СΪ1024
};

/*inode��ռ��10��block
����inode�ṹ��СΪ64��1��block�ܷ�8��inode��ϵͳ��80������inode�����ɴ����ļ�����Ϊ80*/
/*һ��inode����һ���ļ�*/
/*inodeĿǰ������superblock��ʹ��λʾͼ���й���*/

/*�ļ��Ĵ򿪽ṹ�����������˴�ֻ��¼ĳ���ļ���ϵͳ�еĴ��������л��û����� or �رգ�������Ŀǰ����д��Ӳ���У�ÿ�δ�cmd���ã�

*/
class Inode {
public:
	/*10λ��Чλ��drwxrwxrwx*/
	enum INODEMODE {
		OTHER_x = 0x1,
		OTHER_w = 0x2,
		OTHER_r = 0x4,
		GROUP_x = 0x8,
		GROUP_w = 0x10,
		GROUP_r = 0x20,
		OWNER_x = 0x40,
		OWNER_w = 0x80,
		OWNER_r = 0x100,
		DIRECTORY = 0x200
	};

	unsigned int i_mode;	//enum INODEMODE�ж�����ļ�Ȩ��(�ݶ�Ϊ10λ)
	int i_nlink;			//�ļ��ڲ�ͬĿ¼���е���������
	int i_uid;				//�ļ��������û�id
	char usergroup[12];		//�û�������ÿ���û������кܶ���û��飬��ֻ��һ�����û���
	int i_size;				//�ļ���С���ֽ�����
	int i_addr[10];			//�߼���ź�������ת��������
	int i_no;				//�ļ���inode��ţ������ڴ���inodeд�ش��̲���λ�ã�
	time_t  i_atime;		//������ʱ��
	time_t  i_mtime;		//����޸�ʱ��
	int padding[9];			//��䣬ʹinode�ڵ��СΪ128
};

/*���ļ���block�������д�����ļ�����
��Ŀ¼��block�������д����directory�ṹ */

/*�ļ��Ĵ򿪽ṹ��һ��file��Ӧ��ĳ���û���һ�δ�
���ڶ��û��򿪶���ļ������ʹ��������������
ע�⣬һ���û����ֻ�ܴ�һ����ͬ�ļ����ڴ��ļ�֮ǰ���Ȳ�ѯ
ʹ��һ���ڴ��е��ļ�����������й���
���δ�cmd�൱�ڿ������ر�cmd=�ػ�*/
class File {
public:
	string file_name;		//�ļ���
	string user_name;		//�û���
	int inode_num;			//�ļ���Ӧ��inode��
	int offset;				//�ļ�ָ�루ƫ����λ�ã���ÿ�δ򿪺�����
	int openuser_id;		//�򿪸��ļ����û�id
};


/*ע�⣬��Ҫд��block�е����ݲ���ʹ��string����Ϊ��С��ȷ����
��block��д�����Ƿǳ��ײ���ڴ��������string��һ���ܸ߼����Եķ�װ�࣬������Ȼ�������ͻ*/
/*Ŀ¼�������Ŀ¼��Ҳ�������ļ���ȡ����inode��mode����
DIRSIZ=28��DirectoryEntry=32*/
class DirectoryEntry {
public:
	int n_ino;			//Ŀ¼����Inode��Ų���
	char m_name[MAX_NAMELEN];//Ŀ¼�����ļ�������
};

/*Ŀ¼����Ŀ¼������ޣ���ʱδʵ��ʵʱ���䣩
MAX_SUBDIR=16��һ��DirectoryΪ32*16=512�ֽڣ�ռһ��block
�洢��̯ƽ�ģ�Ŀ¼�����εģ������Ҫ��������һ��ת��*/
class Directory {
public:
	DirectoryEntry child_filedir[MAX_SUBDIR];	//��Ŀ¼/�ļ�
	char m_name[MAX_NAMELEN];						//�Ľ�����Ŀ¼��д�Լ����ļ���
	int d_inode;								//���
};

/*�û������ݴ���һ���������ļ���*/
/*5�����ԣ���������*/
class UserInfo {


public:
	int id;				//�û���ţ�0Ϊroot
	char name[12];		//�û���
	char usergroup[12];	//�û�������ÿ���û������кܶ���û��飬��ֻ��һ�����û���
	char password[16];	//ע����md5�㷨���ܺ���û����룬private��װ,MD5�Ϊ128bit=16byte

	const char * Func_Encryption(const char password1[12]);//�洢���룺�������������Ϊ����

};

/*ϵͳ���û���������Ϣ������ĳ��block�У������ڴ��г�̬����*/
class Users {
public:
	int user_num;				//ϵͳ�洢���û�����������Ϊ10��һ��block�У�
	int user_now;				//��ǰ�û��ۼ������������һ���û���uid�ţ�
	UserInfo user[MAX_USER_NUM];//�洢�û���Ϣ
	int padding[16];			//����ֽ�
};


/*ϵͳ���壬ע��ʹ�õ���ģʽ������������*/
// �ֲ���̬����ʵ�ֵ�����ʽ������C++11���̰߳�ȫ
/*C++11�涨��local static�ڶ��߳������µĳ�ʼ����Ϊ��Ҫ���������֤���ڲ���̬�������̰߳�ȫ�ԡ�
��C++11��׼�£���Effective C++�������һ�ָ����ŵĵ���ģʽʵ�֣�ʹ�ú����ڵ� local static ����
������ֻ�е���һ�η���getInstance()����ʱ�Ŵ���ʵ�������ַ���Ҳ����ΪMeyers' Singleton��*/
class FileSystem
{
private:
	FileSystem() { };
	~FileSystem() { };
	FileSystem(const FileSystem&);
	FileSystem& operator=(const FileSystem&);

	/*����Ϊ�ļ�ϵͳ�Զ������ݣ��뵥���޹�*/
	UserInfo user_now;//��ǰϵͳ�е�¼���û�

	/*ָ�ʵ��ָ���Ŀ����Ϊ��ʵ��ָ���������Զ���*/
	/*ͨ������ָ��ķ�ʽ����һ�麯����string�Ķ�Ӧ��ϵ*/
	unordered_map<string, int(FileSystem::*)(vector<string>)> inst;	//ָ�
	SuperBlock my_superblock;										//�ڴ�superblock

	/*���ļ����*/
	int inode_open_num;						//�Ѿ��򿪵�inode��(ֻ���ļ�)
	Inode my_inode_map[MAX_OPEN_FILE_NUM];	//�ڴ�inode����
	File my_file_map[MAX_OPEN_FILE_NUM];	//�򿪵��ļ��ṹ

	/*·�����*/
	Directory now_dir;		//��ǰλ�ڵ�direction

public:

	/*һϵ�д�������������enum+����ָ�룿����*/
	/*����ÿһ��������˵������Ҫ��ǰ�û�ӵ�ж�Ӧ��Ȩ��
	����Ȩ����ʱ��д���ģ���rootר������ʱ��ȡ���ڶ�Ӧ�ļ�������������ļ�Ȩ���뵱ǰ�û���*/
	/*Ҫ��Զ�Ӧ�������������һ��궨�壨�����ö���ࣩ��������ֱ�ӷ��ؾ��ж�Ӧ����ı�����ż��ɣ���ͬʱά��errno����������ߴ���ɶ��Լ�ϵͳ�ȶ��ԣ����û���˵��*/

	/*rootȨ�޲��ܲ�����һ�麯���������û����û����ά��*/
	int Func_CheckUser(string name);						//���û��ļ��м���û��Ƿ����
	int Func_UserToken(string name, string passwd);			//��¼ʱ����û����������Ƿ�ƥ��
	int Func_AddUser(vector<string> str);					//�½��û�
	int Func_DelUser(vector<string> str);					//ɾ���û�
	int Func_AddUserToGroup(vector<string> str);			//����û����û���
	int Func_DelUserFromGroup(vector<string> str);			//���û�����ɾ���û�

	/*�û���Ϣ���л�һ�麯�������û�ע�⿼���̰߳�ȫ*/
	int Func_Whoami(vector<string> str);				//��ӡ��ǰ�û���Ϣ
	int Func_ShowAllUser(vector<string> str);			//��ӡϵͳ�������û���Ϣ
	int Func_Su(vector<string> str);					//�л��û���root����Ҫ���룬������Ҫ
	int Func_UserExit(vector<string> str);				//�˳���ǰ�û����ص���ʼ���û����棩
	int Func_UpdateUserPassword(vector<string> str);	//�޸��û����룬root���޸������û�����ͨ�û�ֻ���޸��Լ�


	/*Ŀ¼��ز���*/
	int Func_Mkdir(vector<string> str);						//����Ŀ¼
	int Func_Rm(vector<string> str);						//ɾ��Ŀ¼���������������ݣ����ļ�
	int Func_Rm_plus(int inode_num, int father_inode_num);	//ɾ����ǰinode��Ӧ��λ��
	int Func_Ls(vector<string> str);						//ls�������г���ǰĿ¼�������ļ��������޸�ʱ�������Ϣ��
	int Func_Cd(vector<string> str);						//cd������ĳ��·��
	int Func_Chmod(vector<string> str);						//�޸�·�������ļ���Ȩ��
	int Func_Help(vector<string> str);						//��ӡȫ�ֻ��Ӧָ���help��Ϣ

	/*������ļ�������������ɾ�������յ�*/
	int Func_CreateFile(vector<string> str);	//�������ļ�
	int Func_OpenFile(vector<string> str);		//���ļ�
	int Func_CloseFile(vector<string> str);		//�ر��ļ�
	int Func_CatFile(vector<string> str);		//��ʾ�ļ����������ݣ�����Ҫ�򿪣��������ļ�ָ�룩

	/*��ĳ���ļ����޸Ĳ��������ļ�ָ����أ�*/
	int Func_ReadFile(vector<string> str);			//��ĳ���ļ�
	int Func_WriteFile(vector<string> str);			//дĳ���ļ�
	int Func_LseekFile(vector<string> str);			//�ƶ��ļ�ָ�룬���ݵ�ǰ�ļ����ļ�ָ����ж�д
	int Func_WriteFileFromMine(vector<string> str);	//�ӱ���ϵͳ�е����ļ�
	int Func_WriteFileToMine(vector<string> str);	//�����ļ�������ϵͳ
	int Func_OpenFileState(vector<string> str);		//���ļ�״̬

	/*ϵͳ��غ����������ʼ�����߸�ʽ�����󲿷ֲ������û����*/
	int Func_CreateNewFileSystem();		//���㿪ʼ�����ļ�ϵͳ
	int Func_MountExistedFileSystem();	//���������ļ�ϵͳ
	int Func_CheckExistedFileSystem();	//��������ļ�ϵͳ�Ƿ��𻵣�����ѡ���Ƿ��½�
	int Func_FileSystemExit();			//�˳���ǰ�ļ�ϵͳ

	/*��¼��shell��أ�ϵͳ������ɲ���*/
	int Func_LoginLoop();									//ѭ����¼
	int Func_IsUserExist(string user_name, string password);//���Ҹ��û��Ƿ����
	int Func_ShellLoop();									//����shell��֮�����shell��ȡѭ��
	int Func_InitInstructions();							//��ʼ��ָ�����Ӧ��䣩
	int Func_CheckParaNum(vector<string> str, int para_num);//ͳһ���ڼ����������Ƿ����Ԥ��

	/*���ڴ��̺��ڴ��ͬ�����£������޸��Ը��¶�Ҫ����ִ��*/
	int Func_RenewMemory();		//�������ж�Ӧλ�����ݶ����ڴ��У���ʼ��ʱ���������superblock�͸�Ŀ¼
	int Func_RenewDisk();		//���ڴ��ж�Ӧλ�����ݶ�������У����ڴ��ִ��
	int Func_SaveFileInode();	//�洢�ڴ��еĴ�file���inode

	/*��ӳ�䡢ת�������ݵ�����*/
	int Func_AddrMapping(int inode, int logic_addr);	//����inode��addr�����߼��̿��ת��Ϊ�����̿�ţ������ļ�ʱҲ�����߼�����д���
	int Func_AnalysePath(string str);					//�ֽ�·��������·����Ӧ��inode��Ų�����
	bool Func_CheckDirExist(int inode, string str);		//�����жϸ�inode��Ӧdir���Ƿ����ͬ���ļ�/�ļ���

	/*��superblock�����ݿ�ṹ��ά��*/
	int Func_ReleaseBlock(int block_num);	//�����ݿ�blockͳһ�ͷ�
	int Func_AllocateBlock();				//�����ݿ�blockͳһ����

	/*·�����*/
	int Func_GetFatherInode(string str);					//��ȡ����Ŀ¼��inode��
	int Func_OpenedFileAddBlock(int now_pos, int new_size);	//���ڴ����Ѿ��򿪵��ļ���inode���в���
	string Func_LastPath(string str);						//��ȡpath�����һ������λ��

	/*Ȩ�����*/
	int Func_AuthCheck(int file_inode, string str);		//�Ե�ǰ�û��Ƿ��ܹ����в�����Ȩ�޽�����֤
	const char* Func_FromUseridGetGroup(int user_id);	//����uid��ȡgroupname

	static FileSystem& getInstance()//����ģʽΨһ�����á�����ȡʵ��
	{
		static FileSystem instance;
		return instance;
	}
};









