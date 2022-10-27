#pragma once
# include <iostream>
# include <fstream>
# include <string>
# include <time.h>
# include <iomanip>
#include<bitset>
using namespace std;


/*��ͷ�ļ������洢�궨��*/
static const char* MYDISK_NAME = "FileDisk.img";
/*�ļ���Ŀ¼��Ȩ�޶�Ӧλ*/
static const char* PRIVILEGE = "drwxrwxrwx";



/*Superblock�������ݶ���*/
//����block��size��С���궨���������Ӧ��
#define BLOCK_SIZE 512 
//inode������/������
#define INODE_NUM 80 
//block��ֱ�ӹ�����
#define BLOCK_FREE_NUM 100

//superblock�ε�block��
#define SUPERBLOCK_BLOCK_NUM 2
//inode��ռ�õ�block��
#define INODE_BLOCK_NUM 20
//block���ݶε�block��������ļ����޴�Լ��70000���ң�
#define BLOCK_NUM 100000

//�������ӷ���ÿ���block
#define BLOCK_GROUP_NUM 100

//�������ݶε���ʼλ��
#define SUPERBLOCK_START 0
#define INODE_START 2
#define BLOCK_START 22

//�������ֶ���
#define SUPERBLOCK_WRITEBACK 1
#define SUPERBLOCK_READWRITE 0
#define SUPERBLOCK_READONLY 1


//Ŀ¼�ṹ��ض���
//Ŀ¼���ļ��������
#define MAX_NAMELEN 28
//ÿ��Ŀ¼��������Ŀ¼/�ļ���
#define MAX_SUBDIR 15
//Ŀ¼�Ĵ�С
#define DIRECTORY_SIZE 512 
//�������ظ�Ŀ¼λ��
#define ROOT_DIRECTORY_LOAD 99

//�߼��鵽����������ת��
//ֱ��ӳ������
#define ZERO_INDEX 6
//һ������ӳ������
#define ONE_INDEX 6+128*2
//��������ӳ������
#define TWO_INDEX 6+128*2+128*128*2

//���������У�ÿ���������ܹ��洢�����ݴ�С
#define ZERO_INDEX_NUM 1
#define ONE_INDEX_NUM 128
#define TWO_INDEX_NUM 128*128

//block�������д洢��ʽΪint[128]
#define BLOCK_INT_NUM 128

//���ͬʱ���ļ���
#define MAX_OPEN_FILE_NUM 20


//ϵͳ����û���
#define MAX_USER_NUM 10


//ϵͳ�û��ļ��洢�̿飨ֻ��һ������ʼ��ȷ����
#define USER_INFO_BLOCK 98

//�����˳���ǰshell�Ķ���
#define EXIT_SHELL -213704







