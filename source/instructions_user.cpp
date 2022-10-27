#define _CRT_SECURE_NO_WARNINGS
#include "filesystem.h"


/*md5����ʹ��*/
const char* UserInfo::Func_Encryption(const char password1[12])
{
	return password1;
}



/*���û���ش���*/
//���û�block�������û�����
int FileSystem::Func_AddUser(vector<string> str)
{
	//username��password��������
	if (Func_CheckParaNum(str, 2) == -1)
	{
		return -1;
	}
	if (str[1].size() > 11)
	{
		cout << "�û���������������11���ֽ���" << endl;
		return - 1;
	}
	if (Func_CheckUser(str[1]) != -1)
	{
		cout << "�û����Ѵ��ڣ�����ʧ��" << endl;
		return -1;
	}
	/*if (strcmp(user_now.name, "root") != 0)
	{
		cout << "ֻ��root�û����ܴ����û�������ʧ��" << endl;
		return -1;
	}*/


	fstream fd;//c++��ʽ���ļ������̳���iostream
	fd.open(MYDISK_NAME, ios::out | ios::in | ios::binary);

	Users user_list;
	fd.seekg((BLOCK_START + USER_INFO_BLOCK) * BLOCK_SIZE, ios::beg);
	fd.read((char*)&user_list, BLOCK_SIZE);

	if (user_list.user_num == MAX_USER_NUM)
	{
		cout << "��ǰϵͳ�û������Ѵ�����" << endl;
		fd.close();//���ļ�
		return -1;
	}


	UserInfo new_user;
	new_user.id = user_list.user_now;
	user_list.user_now++;
	strcpy(new_user.name, str[1].c_str());

	const char* crypassword = new_user.Func_Encryption(str[2].c_str());
	strcpy(new_user.password, crypassword);

	new_user.usergroup[0] = '\0';

	memcpy(&user_list.user[user_list.user_num], &new_user, sizeof(UserInfo));
	user_list.user_num++;
	fd.seekg((BLOCK_START + USER_INFO_BLOCK) * BLOCK_SIZE, ios::beg);
	fd.write((char*)&user_list, BLOCK_SIZE);


	fd.close();//���ļ�
	cout << "�������û��ɹ�" << endl;
	return 1;
}


//���û�block��ɾ���û�
int FileSystem::Func_DelUser(vector<string> str)
{
	//username����
	if (Func_CheckParaNum(str, 1) == -1)
	{
		return -1;
	}

	/*ɾ��*/
	int pos = Func_CheckUser(str[1]);
	if (pos == -1)
	{
		cout << "�û������ڣ�ɾ��ʧ��" << endl;
		return -1;
	}
	if (strcmp(user_now.name, str[1].c_str()) == 0)
	{
		cout << "�û����ڵ�¼״̬��ɾ��ʧ��" << endl;
		return -1;
	}
	if (strcmp(user_now.name, "root") != 0)
	{
		cout << "ֻ��root�û�����ɾ�������û���ɾ��ʧ��" << endl;
		return -1;
	}


	fstream fd;//c++��ʽ���ļ������̳���iostream
	fd.open(MYDISK_NAME, ios::out | ios::in | ios::binary);

	Users user_list;
	fd.seekg((BLOCK_START + USER_INFO_BLOCK) * BLOCK_SIZE, ios::beg);
	fd.read((char*)&user_list, BLOCK_SIZE);
	user_list.user_num--;//���Լ������Լ������
	memcpy(&user_list.user[pos], &user_list.user[user_list.user_num], sizeof(UserInfo));


	fd.seekg((BLOCK_START + USER_INFO_BLOCK) * BLOCK_SIZE, ios::beg);
	fd.write((char*)&user_list, BLOCK_SIZE);
	fd.close();//���ļ�
	cout << "ɾ���û��ɹ�" << endl;
	return 1;
}


//���û�block�����/�޸��û���
int FileSystem::Func_AddUserToGroup(vector<string> str)
{
	//username����
	if (Func_CheckParaNum(str, 2) == -1)
	{
		return -1;
	}

	/*ɾ��*/
	int pos = Func_CheckUser(str[1]);
	if (pos == -1)
	{
		cout << "�û������ڣ�����û���ʧ��" << endl;
		return -1;
	}
	if (str[2].size() > 11)
	{
		cout << "�û�����������������11���ֽ���" << endl;
		return -1;
	}


	fstream fd;//c++��ʽ���ļ������̳���iostream
	fd.open(MYDISK_NAME, ios::out | ios::in | ios::binary);

	Users user_list;
	fd.seekg((BLOCK_START + USER_INFO_BLOCK) * BLOCK_SIZE, ios::beg);
	fd.read((char*)&user_list, BLOCK_SIZE);

	strcpy(user_list.user[pos].usergroup, str[2].c_str());
	if (strcmp(user_now.name, str[1].c_str()) == 0)
	{
		strcpy(user_now.usergroup, str[2].c_str());
	}

	fd.seekg((BLOCK_START + USER_INFO_BLOCK) * BLOCK_SIZE, ios::beg);
	fd.write((char*)&user_list, BLOCK_SIZE);
	fd.close();//���ļ�
	cout << "��ӳɹ�" << endl;
	return 1;
}


//���û�block��ɾ���û����е��û�
int FileSystem::Func_DelUserFromGroup(vector<string> str)
{
	//username����
	if (Func_CheckParaNum(str, 2) == -1)
	{
		return -1;
	}

	/*ɾ��*/
	int pos = Func_CheckUser(str[1]);
	if (pos == -1)
	{
		cout << "�û������ڣ�ɾ��ʧ��" << endl;
		return -1;
	}

	fstream fd;//c++��ʽ���ļ������̳���iostream
	fd.open(MYDISK_NAME, ios::out | ios::in | ios::binary);

	Users user_list;
	fd.seekg((BLOCK_START + USER_INFO_BLOCK) * BLOCK_SIZE, ios::beg);
	fd.read((char*)&user_list, BLOCK_SIZE);


	if (strcmp(user_list.user[pos].usergroup, str[2].c_str()) == 0)
	{
		user_list.user[pos].usergroup[0] = '\0';
		if (strcmp(user_now.name, str[1].c_str()) == 0)
		{
			user_now.usergroup[0] = '\0';
		}
	}
	else
	{
		fd.close();//���ļ�
		cout << "ɾ��ʧ�ܣ��û������ڸ��û���" << endl;
		return -1;
	}

	fd.seekg((BLOCK_START + USER_INFO_BLOCK) * BLOCK_SIZE, ios::beg);
	fd.write((char*)&user_list, BLOCK_SIZE);
	fd.close();//���ļ�
	cout << "ɾ���ɹ�" << endl;
	return 1;
}


/*��ӡ��ǰ�û��û���*/
int FileSystem::Func_Whoami(vector<string> str)
{
	if (Func_CheckParaNum(str, 0) == -1)
	{
		return -1;
	}

	cout << user_now.name << endl;
	return 1;
}


/*��ӡ�����û�*/
int FileSystem::Func_ShowAllUser(vector<string> str)
{
	if (Func_CheckParaNum(str, 0) == -1)
	{
		return -1;
	}


	fstream fd;//c++��ʽ���ļ������̳���iostream
	fd.open(MYDISK_NAME, ios::out | ios::in | ios::binary);

	Users user_list;
	fd.seekg((BLOCK_START + USER_INFO_BLOCK) * BLOCK_SIZE, ios::beg);
	fd.read((char*)&user_list, BLOCK_SIZE);

	cout << setw(16) << left << "user_id";
	cout << setw(16) << left << "user_name";
	cout << setw(16) << left << "user_group";
	cout << endl;

	for (int i = 0; i < user_list.user_num; i++)
	{
		user_list.user[i].id;
		//��ӡid
		cout << setw(16) << left << user_list.user[i].id;

		//��ӡ����
		cout << setw(16) << left << user_list.user[i].name;

		//��ӡgroup
		if (user_list.user[i].usergroup[0] != '\0')
			cout << setw(16) << left << user_list.user[i].usergroup;
		else
			cout << setw(16) << left << "null";
		cout << right;

		cout << endl;
	}
	
	fd.close();//���ļ�
	return 1;
}

/*��Ҫͬʱ�����˺ź�����*/
int FileSystem::Func_Su(vector<string> str)
{
	if (Func_CheckParaNum(str, 2) == -1)
	{
		return -1;
	}

	int pos = Func_UserToken(str[1], str[2]);
	if (pos == -1)
	{
		cout << "�˺Ż�������������������" << endl;
		return -1;
	}

	fstream fd;//c++��ʽ���ļ������̳���iostream
	fd.open(MYDISK_NAME, ios::out | ios::in | ios::binary);

	Users user_list;
	fd.seekg((BLOCK_START + USER_INFO_BLOCK) * BLOCK_SIZE, ios::beg);
	fd.read((char*)&user_list, BLOCK_SIZE);

	memcpy(&user_now, &user_list.user[pos], sizeof(UserInfo));


	fd.close();//���ļ�
	return 1;

}

int FileSystem::Func_UserExit(vector<string> str)
{
	if (Func_CheckParaNum(str, 0) == -1)
	{
		return -1;
	}

	cout << "���˳��ļ�ϵͳ" << endl;
	return EXIT_SHELL;
}

int FileSystem::Func_UpdateUserPassword(vector<string> str)
{
	if (Func_CheckParaNum(str, 1) == -1)
	{
		return -1;
	}
	/*ɾ��*/
	int pos = Func_CheckUser(str[1]);
	if (pos == -1)
	{
		cout << "�û������ڣ��޸�ʧ��" << endl;
		return -1;
	}

	string old_password;
	string new_password;
	cout << "������: ";
	cin >> old_password;
	cout << "������: ";
	cin >> new_password;

	fstream fd;//c++��ʽ���ļ������̳���iostream
	fd.open(MYDISK_NAME, ios::out | ios::in | ios::binary);

	Users user_list;
	fd.seekg((BLOCK_START + USER_INFO_BLOCK) * BLOCK_SIZE, ios::beg);
	fd.read((char*)&user_list, BLOCK_SIZE);


	if (strcmp(user_list.user[pos].password, user_list.user[pos].Func_Encryption(old_password.c_str())) == 0)
	{
		strcpy(user_list.user[pos].password, user_list.user[pos].Func_Encryption(new_password.c_str()));
		strcpy(user_now.password, user_list.user[pos].Func_Encryption(new_password.c_str()));
		fd.seekg((BLOCK_START + USER_INFO_BLOCK) * BLOCK_SIZE, ios::beg);
		fd.write((char*)&user_list, BLOCK_SIZE);
	}
	else
	{
		cout << "����������" << endl;
	}

	fd.close();//���ļ�
	return 1;
}