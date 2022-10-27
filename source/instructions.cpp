#define _CRT_SECURE_NO_WARNINGS
#include "filesystem.h"


/*ָ�1�����Ŀ¼���ָ��*/

//helpָ����help��Ӧ����ʾ��Ϣ
int FileSystem::Func_Help(vector<string> str)
{
	if (str.size() == 1)//�հ�helpָ���ӡָ���Ҫ��Ϣ
	{
		//��������
		cout << "֧��ָ��         ָ���ʽ                       ָ���" << endl;
		cout << "help                                            ��ʾ��Ϣ" << endl;
		cout << "cd               <�ļ�·��>                     �л���ǰĿ¼��ָ��·��" << endl;
		cout << "ls                                              չʾ��ǰĿ¼������" << endl;
		cout << "mkdir            <�ļ�·��>                     ��ָ��·���´���Ŀ¼" << endl;
		cout << "rm               <�ļ�·��>                     ɾ��ָ��·���µ�Ŀ¼���ļ�" << endl;
		cout << "chmod            <Ȩ��>     <�ļ�·��>          �޸�Ŀ¼���ļ���Ȩ��" << endl;

		//�ļ�����
		cout << "create           <�ļ�·��>                     ��ָ��·���´����ļ�" << endl;
		cout << "open             <�ļ�·��>                     ��ָ���ļ�" << endl;
		cout << "close            <�ļ�·��>                     �ر�ָ���ļ�" << endl;
		cout << "cat              <�ļ�·��>                     ��ӡָ���ļ���������" << endl;
		cout << "read             <�ļ�·��> <������>            ���ļ�" << endl;
		cout << "write            <�ļ�·��> <д����>            д�ļ�" << endl;
		cout << "lseek            <�ļ�·��> <�µ��ļ�ָ��λ��>  �ƶ��ļ�ָ��" << endl;
		cout << "writein          <�ļ�·��> <�ⲿ�ļ�·��>      ���ⲿ�����ļ������ļ�ϵͳ" << endl;
		cout << "writeout         <�ļ�·��> <�ⲿ�ļ�·��>      ���ļ�ϵͳ���ⲿ�����ļ�" << endl;
		cout << "opened                                          �鿴���ļ���Ϣ" << endl;

		//���û���ز���
		cout << "adduser          <�û���>   <�û�����>          �����û�" << endl;
		cout << "deluser          <�û���>                       ɾ���û�" << endl;
		cout << "addusertogroup   <�û���>   <�û�����>          �����û����û���" << endl;
		cout << "deluserfromgroup <�û���>   <�û�����>          ���û�����ɾ���û�" << endl;
		cout << "whoami                                          ��ӡ��ǰ�û�" << endl;
		cout << "showalluser                                     ��ӡ�����û���Ϣ" << endl;
		cout << "updatepassword   <�û���>                       �޸��û�����" << endl;
		cout << "su               <�û���>                       �л���ǰ��¼�û�" << endl;
		cout << "exit                                            �˳��ļ�ϵͳ" << endl;

	}
	else
	{

	}
	return 1;
}


int FileSystem::Func_Mkdir(vector<string> str)
{
	if (Func_CheckParaNum(str, 1) == -1)
	{
		return -1;
	}

	if (my_superblock.s_ninode == 0)//�ļ�ϵͳ���ļ�/Ŀ¼���Ѵ�����
	{
		cout << "�ļ�ϵͳ���ļ�/Ŀ¼���Ѵ����ޣ�����Ŀ¼ʧ��" << endl;
		return -1;
	}

	int ret = Func_AnalysePath(str[1]);
	if (ret != -1)//��·���Ѵ���
	{
		cout << "�Ѵ���ͬ���ļ�/Ŀ¼������Ŀ¼ʧ��" << endl;
		return -1;
	}

	string new_dir_name = Func_LastPath(str[1]);

	fstream fd;//c++��ʽ���ļ������̳���iostream
	fd.open(MYDISK_NAME, ios::out | ios::in | ios::binary);

	int father_inode_num = Func_GetFatherInode(str[1]);
	Inode father_inode;
	Directory father_dir;
	fd.seekg(INODE_START * BLOCK_SIZE + father_inode_num * sizeof(Inode), ios::beg);
	fd.read((char*)&father_inode, sizeof(Inode));
	fd.seekg((BLOCK_START + father_inode.i_addr[0]) * BLOCK_SIZE, ios::beg);
	fd.read((char*)&father_dir, sizeof(father_dir));



	int father_dir_insert = -1;
	for (int i = 0; i < MAX_SUBDIR; i++)
		if (father_dir.child_filedir[i].m_name[0] == '\0')
		{
			father_dir_insert = i;
			break;
		}
	if (father_dir_insert == -1)
	{
		cout << "��ǰĿ¼���ļ�/Ŀ¼���Ѵ����ޣ�����Ŀ¼ʧ��" << endl;
		return -1;
	}

	Inode inode1;//��Ŀ¼
	//��Ŀ¼inode��ʼ��
	inode1.i_mode = 0x3FF;					//Ŀ¼������Ȩ��֮����
	inode1.i_nlink = 0;						//�ļ��ڲ�ͬĿ¼���е���������
	inode1.i_uid = user_now.id;						//�ļ��������û�id
	//inode1.i_gid = user_now.groupid;						//�ļ��������û���id
	inode1.i_size = DIRECTORY_SIZE;			//�ļ���С��Ŀ¼�ļ�Ϊ512
	inode1.i_addr[0] = Func_AllocateBlock();	//����block��

	inode1.i_no = my_superblock.s_inode[--my_superblock.s_ninode];//ջ��inode��ţ�top����
	//my_superblock.s_ninode--;//�ȼ۳�ջ��pop����
	inode1.i_atime = time(NULL);			//������ʱ��
	inode1.i_mtime = time(NULL);			//����޸�ʱ��

	//inode����inode����Ӧλ����
	fd.seekg(INODE_START * BLOCK_SIZE + inode1.i_no * sizeof(Inode), ios::beg);
	fd.write((char*)&inode1, sizeof(Inode));

	//��ʼ��DirectoryĿ¼�ṹ������Ŀ¼д��block
	//��Ҫע����ǣ�������Ŀ¼�ж���"."��".."����Ŀ¼����һ��Ŀ¼������Ŀ¼���⣬�������Լ�
	Directory new_directory;
	strcpy(new_directory.m_name, new_dir_name.c_str());//Ŀ¼��д��
	new_directory.d_inode = inode1.i_no;//inode���д��
	//Ŀ¼.Ϊ�Լ���ָ���Լ�inode
	strcpy(new_directory.child_filedir[0].m_name, ".");
	new_directory.child_filedir[0].n_ino = inode1.i_no;
	//..Ϊ��Ŀ¼��ָ��ǰĿ¼
	strcpy(new_directory.child_filedir[1].m_name, "..");
	new_directory.child_filedir[1].n_ino = father_dir.d_inode;
	//��ʼ������λ��Ϊ0
	for (int i = 2; i < MAX_SUBDIR; i++) {
		new_directory.child_filedir[i].m_name[0] = '\0';
		new_directory.child_filedir[i].n_ino = -1;//-1����û�ж�Ӧ��inode���ڲ���ʱ�ǵ����жϣ�
	}
	fd.seekg((BLOCK_START + inode1.i_addr[0]) * BLOCK_SIZE, ios::beg);
	fd.write((char*)&new_directory, sizeof(new_directory));

	//����fatherĿ¼
	strcpy(father_dir.child_filedir[father_dir_insert].m_name, new_dir_name.c_str());
	father_dir.child_filedir[father_dir_insert].n_ino = inode1.i_no;

	if (now_dir.d_inode == father_inode_num)//�ǵø��µ�ǰ����Ŀ¼(�����ͬ)
	{
		memcpy(&now_dir, &father_dir, sizeof(Directory));
	}

	fd.seekg((BLOCK_START + father_inode.i_addr[0]) * BLOCK_SIZE, ios::beg);
	fd.write((char*)&father_dir, sizeof(father_dir));

	fd.close();//���ļ�

	Func_RenewDisk();//��Ҫд������򿪵��ļ����棬��������ظ��ر�

	cout << "�������ļ��гɹ�" << endl;
	return 1;
}


int FileSystem::Func_Rm_plus(int inode_num, int father_inode_num)
{
	fstream fd;//c++��ʽ���ļ������̳���iostream
	fd.open(MYDISK_NAME, ios::out | ios::in | ios::binary);

	Inode inode;
	fd.seekg(INODE_START * BLOCK_SIZE + inode_num * sizeof(Inode), ios::beg);
	fd.read((char*)&inode, sizeof(Inode));
	if ((inode.i_mode & Inode::INODEMODE::DIRECTORY) == Inode::INODEMODE::DIRECTORY)//Ŀ¼���ݹ�ɾ����������(�ǵ����ɾ���Լ�)
	{
		Directory dir;
		fd.seekg((BLOCK_START + inode.i_addr[0]) * BLOCK_SIZE, ios::beg);
		fd.read((char*)&dir, sizeof(dir));
		//ɾ����Ŀ¼���ļ�
		for (int i = MAX_SUBDIR - 1; i >= 2; i--)//����ɾ����������Ϊ��������λ�õ���ǰ�滻
		{
			if (dir.child_filedir[i].n_ino != -1)
			{
				Func_Rm_plus(dir.child_filedir[i].n_ino, inode_num);
			}
		}
		//ɾ����Ŀ¼

	}
	else//�ļ���ɾ�������ļ���ͬʱɾ��inode��ά��dir��������
	{
		;//���������ͳһɾ��
	}

	//�����ļ���size����block_sum
	int block_sum;//���ļ�ռ�õ�������block��
	if (inode.i_size == 0)
		block_sum = 0;
	else
		block_sum = (inode.i_size - 1) / BLOCK_SIZE + 1;

	//һ��һ����ɾ��block
	for (int i = 0; i < block_sum; i++)
	{
		int del_block_addr = Func_AddrMapping(inode_num, i);
		Func_ReleaseBlock(del_block_addr);//delete
	}

	//ɾ������dir�еĶ�Ӧ��¼
	Inode father_inode;
	Directory father_dir;
	fd.seekg(INODE_START * BLOCK_SIZE + father_inode_num * sizeof(Inode), ios::beg);
	fd.read((char*)&father_inode, sizeof(Inode));
	fd.seekg((BLOCK_START + father_inode.i_addr[0]) * BLOCK_SIZE, ios::beg);
	fd.read((char*)&father_dir, sizeof(father_dir));

	int pos_now;//Ҫɾ����inode�ڸ�Ŀ¼λ��
	int pos_end = MAX_SUBDIR - 1;//��Ŀ¼���һ��inodeλ�ã������Ƶ�ǰ������
	for (int i = 0; i < MAX_SUBDIR; i++)
	{
		if (father_dir.child_filedir[i].n_ino == inode_num)
		{
			pos_now = i;
		}
		if (father_dir.child_filedir[i].n_ino == -1)
		{
			pos_end = i - 1;
			break;
		}
	}
	father_dir.child_filedir[pos_now].n_ino = father_dir.child_filedir[pos_end].n_ino;
	strcpy(father_dir.child_filedir[pos_now].m_name, father_dir.child_filedir[pos_end].m_name);
	father_dir.child_filedir[pos_end].n_ino = -1;
	strcpy(father_dir.child_filedir[pos_end].m_name, "");

	if (now_dir.d_inode == father_inode_num)//�ǵø��µ�ǰ����Ŀ¼
	{
		memcpy(&now_dir, &father_dir, sizeof(Directory));
	}
	//ɾ��inode
	my_superblock.s_inode[my_superblock.s_ninode] = inode_num;
	my_superblock.s_ninode++;

	fd.close();
	Func_RenewDisk();//����ȫ��
	return 1;
}


int FileSystem::Func_Rm(vector<string> str)
{
	if (Func_CheckParaNum(str, 1) == -1)
	{
		return -1;
	}
	fstream fd;//c++��ʽ���ļ������̳���iostream
	fd.open(MYDISK_NAME, ios::out | ios::in | ios::binary);

	/*�����ж��Ƿ��ǵ�ǰĿ¼*/
	/*��Ŀ¼����ɾ����root����ɾ�����˸�Ŀ¼����������κ��ļ�������ͨ�û�ֻ��ɾ���Լ��������ߵ��ļ�*/
	/*ע����ɾ���ļ�ʱ�ǵ�ͬʱɾ��inode*/
	/*ɾ��Ŀ¼ʱʹ�õݹ鷽��ɾ�����������ļ�*/
	int rm_inode_num = Func_AnalysePath(str[1]);
	if (rm_inode_num == -1)
	{
		cout << "·�������ڣ��������룡" << endl;
		fd.close();
		return 1;
	}
	if (rm_inode_num == now_dir.d_inode)
	{
		cout << "����ɾ����ǰĿ¼" << endl;
		fd.close();
		return 1;
	}
	if (rm_inode_num == 0)
	{
		cout << "����ɾ����Ŀ¼" << endl;
		fd.close();
		return 1;
	}
	if (rm_inode_num == -1)
	{
		cout << "Ҫɾ�����ļ���Ŀ¼������" << endl;
		fd.close();
		return 1;
	}

	int open_pos = -1;
	for (int i = 0; i < inode_open_num; i++)
	{
		if (rm_inode_num == my_inode_map[i].i_no)
		{
			open_pos = i;
			break;
		}
	}
	if (open_pos != -1)
	{
		cout << "���ļ��Ѵ򿪣����ȹر�" << endl;
		fd.close();
		return -1;
	}

	int father_inode_num = Func_GetFatherInode(str[1]);
	if (father_inode_num == -1)
	{
		cout << "·�������ڣ��������룡" << endl;
		fd.close();
		return 1;
	}
	Func_Rm_plus(rm_inode_num, father_inode_num);


	fd.close();
	Func_RenewDisk();//����ȫ��
	return 1;
}
int FileSystem::Func_Ls(vector<string> str)
{
	//Func_CheckParaNum(str, 0);
	/*��ӡ���ݰ���Ȩ�ޣ�10λ����������С������޸�ʱ��
	.��..��ʱ�����Ǵ�ӡ*/
	cout << "privilege\tname\t\tsize\tupdatetime" << endl;
	fstream fd;//c++��ʽ���ļ������̳���iostream
	fd.open(MYDISK_NAME, ios::out | ios::in | ios::binary);
	for (int i = 2; i < MAX_SUBDIR; i++)
	{
		if (now_dir.child_filedir[i].m_name[0] != '\0')
		{

			Inode inode;
			fd.seekg(INODE_START * BLOCK_SIZE + now_dir.child_filedir[i].n_ino * sizeof(Inode), ios::beg);
			fd.read((char*)&inode, sizeof(Inode));


			//��ӡȨ��
			for (int j = 0; j < 10; j++)
			{
				if ((inode.i_mode & (512 >> j)) != 0)//ʹ�ð�λ�����
					cout << PRIVILEGE[j];
				else
					cout << "-";
			}

			//��ӡ����
			cout << "\t";
			cout << setw(16) << left << now_dir.child_filedir[i].m_name;
			cout << right;


			//��ӡsize
			if ((inode.i_mode & 512) != 0)//ʹ�ð�λ�����
				cout << "-";
			else
				cout << inode.i_size;

			//��ӡ�޸�time
			cout << "\t" << ctime(&inode.i_mtime);

			//cout << endl;
		}
		else
		{
			break;
		}
	}

	fd.close();
	Func_RenewDisk();//��Ҫд������򿪵��ļ����棬��������ظ��ر�
	return 1;
}
int FileSystem::Func_Cd(vector<string> str)
{
	if (Func_CheckParaNum(str, 1) == -1)
	{
		return -1;
	}
	fstream fd;//c++��ʽ���ļ������̳���iostream
	fd.open(MYDISK_NAME, ios::out | ios::in | ios::binary);

	//now_dir.;
	int new_inode = Func_AnalysePath(str[1]);
	if (new_inode == -1)
	{
		cout << "·�������ڣ��������룡" << endl;
		fd.close();
		return 1;
	}
	Inode inode;
	fd.seekg(INODE_START * BLOCK_SIZE + new_inode * sizeof(Inode), ios::beg);
	fd.read((char*)&inode, sizeof(Inode));
	if ((inode.i_mode & (Inode::INODEMODE::DIRECTORY)) != Inode::INODEMODE::DIRECTORY)//����Ŀ¼���޷�����
	{
		cout << "����Ŀ¼���޷�����" << endl;
		fd.close();
		Func_RenewDisk();
		return 1;
	}

	int newdir_block_num = Func_AddrMapping(new_inode, 0);//dirֻ��[0]
	fd.seekg((BLOCK_START + newdir_block_num) * BLOCK_SIZE, ios::beg);
	fd.read((char*)&now_dir, sizeof(now_dir));


	fd.close();
	Func_RenewDisk();
	return 1;
}

int FileSystem::Func_Chmod(vector<string> str)
{
	if (Func_CheckParaNum(str, 2) == -1)
	{
		return -1;
	}
	fstream fd;//c++��ʽ���ļ������̳���iostream
	fd.open(MYDISK_NAME, ios::out | ios::in | ios::binary);

	/*�޸��ļ�Ȩ�ޣ�ֻ֧���޸��Լ���Ϊ�����ߵ��ļ�
	root�����������޸������ļ���Ȩ��
	���������3λ�˽�������777����*/
	int new_mode = 0;
	if (str[1].size() != 3)
	{
		cout << "�ļ�Ȩ�޲�����ʽ��������������" << endl;
		return -1;
	}

	for (int i = 2; i >=0 ;i--)
	{
		if (str[1][i] < '0' || str[1][i] > '7')
		{
			cout << "�ļ�Ȩ�޲�����ʽ��������������" << endl;
			return -1;
		}
		new_mode += (str[1][i] - '0') * (1 << (3 * (2 - i)));
	}
	//int new_mode;

	/*����·��*/
	int inode_num = Func_AnalysePath(str[2]);
	if (inode_num == -1)
	{
		cout << "·�������ڣ��������룡" << endl;
		fd.close();
		return -1;
	}

	Inode inode;
	fd.seekg(INODE_START * BLOCK_SIZE + inode_num * sizeof(Inode), ios::beg);
	fd.read((char*)&inode, sizeof(Inode));

	new_mode += (inode.i_mode & (Inode::INODEMODE::DIRECTORY));//dir���Ա���
	inode.i_mode = new_mode;

	fd.seekg(INODE_START * BLOCK_SIZE + inode_num * sizeof(Inode), ios::beg);
	fd.write((char*)&inode, sizeof(Inode));

	fd.close();
	return 1;
}







