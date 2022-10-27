#define _CRT_SECURE_NO_WARNINGS
#include "filesystem.h"

/*����⣬��ſ�����������Ҫ�õ��Ĺ��ߺ���*/

//Instructions���캯������ʼ��ָ�map
int FileSystem::Func_InitInstructions()
{
	inst["help"] = &FileSystem::Func_Help;//����ָ��д��
	inst["mkdir"] = &FileSystem::Func_Mkdir;
	inst["ls"] = &FileSystem::Func_Ls;
	inst["chmod"] = &FileSystem::Func_Chmod;
	inst["cd"] = &FileSystem::Func_Cd;
	inst["rm"] = &FileSystem::Func_Rm;

	//�ļ�����
	inst["create"] = &FileSystem::Func_CreateFile;
	inst["open"] = &FileSystem::Func_OpenFile;
	inst["close"] = &FileSystem::Func_CloseFile;
	inst["cat"] = &FileSystem::Func_CatFile;
	inst["read"] = &FileSystem::Func_ReadFile;
	inst["write"] = &FileSystem::Func_WriteFile;
	inst["lseek"] = &FileSystem::Func_LseekFile;
	inst["writein"] = &FileSystem::Func_WriteFileFromMine;
	inst["writeout"] = &FileSystem::Func_WriteFileToMine;
	inst["opened"] = &FileSystem::Func_OpenFileState;

	//���û���ز���
	inst["adduser"] = &FileSystem::Func_AddUser;
	inst["deluser"] = &FileSystem::Func_DelUser;
	inst["addusertogroup"] = &FileSystem::Func_AddUserToGroup;
	inst["deluserfromgroup"] = &FileSystem::Func_DelUserFromGroup;
	inst["whoami"] = &FileSystem::Func_Whoami;
	inst["showalluser"] = &FileSystem::Func_ShowAllUser;
	inst["su"] = &FileSystem::Func_Su;
	inst["exit"] = &FileSystem::Func_UserExit;
	inst["updatepassword"] = &FileSystem::Func_UpdateUserPassword;
	

	return 1;
}

//����ͬ�����ڴ�
int FileSystem::Func_RenewMemory()
{
	fstream fd;//c++��ʽ���ļ������̳���iostream
	fd.open(MYDISK_NAME, ios::out | ios::in | ios::binary);

	//����my_superblock
	fd.seekg(SUPERBLOCK_START * BLOCK_SIZE, ios::beg);
	fd.read((char*)&my_superblock, sizeof(my_superblock));//ת����char*ֱ��д���ļ�����Ҫ��ȡʱֱ�Ӵ��ļ���ͬ����ʽ����ת�������ڴ�

	//���µ�ǰ��directory

	//�������ݿ�block
	fd.seekg((BLOCK_START + ROOT_DIRECTORY_LOAD) * BLOCK_SIZE, ios::beg);
	fd.read((char*)&now_dir, sizeof(now_dir));

	fd.close();
	return 1;
}

//�ڴ�ͬ��������
int FileSystem::Func_RenewDisk()
{
	fstream fd;//c++��ʽ���ļ������̳���iostream
	fd.open(MYDISK_NAME, ios::out | ios::in | ios::binary);

	//����my_superblock
	fd.seekg(SUPERBLOCK_START * BLOCK_SIZE, ios::beg);
	fd.write((char*)&my_superblock, sizeof(my_superblock));//ת����char*ֱ��д���ļ�����Ҫ��ȡʱֱ�Ӵ��ļ���ͬ����ʽ����ת�������ڴ�

	//���µ�ǰ��directory

	//����inode��Ӧ�Ĵ���λ��
	Inode inode;
	fd.seekg(INODE_START * BLOCK_SIZE + now_dir.d_inode * sizeof(Inode), ios::beg);
	fd.read((char*)&inode, sizeof(Inode));

	//�����޸ĺͷ���ʱ��
	inode.i_mtime = time(NULL);
	inode.i_atime = time(NULL);

	//�������ݿ�block
	fd.seekg((BLOCK_START + inode.i_addr[0]) * BLOCK_SIZE, ios::beg);
	fd.write((char*)&now_dir, sizeof(now_dir));

	//���µ�ǰĿ¼inode
	fd.seekg(INODE_START * BLOCK_SIZE + now_dir.d_inode * sizeof(Inode), ios::beg);
	fd.write((char*)&inode, sizeof(Inode));

	fd.close();
	return 1;
}

//�洢�ڴ��еĴ�file���inode
int FileSystem::Func_SaveFileInode()
{
	fstream fd;//c++��ʽ���ļ������̳���iostream
	fd.open(MYDISK_NAME, ios::out | ios::in | ios::binary);

	//���ڴ�Ĵ��ļ�inodeд�����
	for (int i = 0; i < inode_open_num; i++)
	{
		fd.seekg(INODE_START * BLOCK_SIZE + my_inode_map[i].i_no * sizeof(Inode), ios::beg);
		fd.write((char*)&my_inode_map[i], sizeof(Inode));
	}


	fd.close();
	return 1;
}

//���·��������-1����·�������ڣ�
int FileSystem::Func_AnalysePath(string str)
{
	fstream fd;//c++��ʽ���ļ������̳���iostream
	fd.open(MYDISK_NAME, ios::out | ios::in | ios::binary);

	vector <string> path_list;//��vector��ʽ�ֽ��·��
	int pos;
	string sub_path = "";
	int start_inode;//��ʼλ�õ�inode
	if (str[0] == '/')//����·�����Ӹ�Ŀ¼��ʼ��
	{
		//path_list.push_back("~");//�Ȳ����Ŀ¼
		start_inode = 0;//��Ŀ¼inode
		for (pos = 1; pos <= str.size(); pos++)
		{
			if (pos == str.size())
			{
				if (sub_path == "")
					break;
				path_list.push_back(sub_path);
			}
			if (str[pos] == '/')//�ֽ��
			{
				path_list.push_back(sub_path);
				sub_path = "";
			}
			else
			{
				sub_path += str[pos];
			}
		}
	}
	else//���·�����ӵ�ǰ��ʼ��
	{
		//path_list.push_back(now_dir.m_name);//�Ȳ��뵱ǰĿ¼
		start_inode = now_dir.d_inode;//��ǰ·��
		for (pos = 0; pos <= str.size(); pos++)
		{
			if (pos == str.size())
			{
				if (sub_path == "")
					break;
				path_list.push_back(sub_path);
			}
			if (str[pos] == '/')//�ֽ��
			{
				path_list.push_back(sub_path);
				sub_path = "";
			}
			else
			{
				sub_path += str[pos];
			}
		}
	}

	/*·���ֽ���ɺ�Ĵ�����ͬ������vector˳����path��*/
	for (int i = 0; i < path_list.size(); i++)
	{
		//����name��inode��
		Inode inode;
		Directory dir;
		//���ݵ�ǰstart_inode�Ų��Ӧinode�ṹ
		fd.seekg(INODE_START * BLOCK_SIZE + start_inode * sizeof(Inode), ios::beg);
		fd.read((char*)&inode, sizeof(Inode));
		//����inode�ṹ��directory�ṹ
		fd.seekg((BLOCK_START + inode.i_addr[0]) * BLOCK_SIZE, ios::beg);
		fd.read((char*)&dir, sizeof(dir));
		//����directory�ṹ����һ���Ƿ��ж�Ӧname
		int over = 0;
		for (int j = 0; j < MAX_SUBDIR; j++)
		{
			if (strcmp(dir.child_filedir[j].m_name, path_list[i].c_str()) == 0)//�ҵ�
			{
				start_inode = dir.child_filedir[j].n_ino;
				over = 1;
				break;
			}
		}
		if (over == 0)
		{
			fd.close();
			//cout << "·�������ڣ��������룡" << endl;
			return -1;
		}
	}
	fd.close();
	return start_inode;
}

//�����߼����Ӧ�������ļ�ָ��������
//inode��Ӧ���ļ����߼���ӳ��Ϊ�����
int FileSystem::Func_AddrMapping(int inode_num, int logic_addr)
{
	fstream fd;//c++��ʽ���ļ������̳���iostream
	fd.open(MYDISK_NAME, ios::out | ios::in | ios::binary);
	Inode inode;
	fd.seekg(INODE_START * BLOCK_SIZE + inode_num * sizeof(Inode), ios::beg);
	fd.read((char*)&inode, sizeof(Inode));

	if (logic_addr >= 0 && logic_addr < ZERO_INDEX)//ֱ��ӳ��
	{
		fd.close();
		return inode.i_addr[logic_addr];
	}
	else if (logic_addr >= ZERO_INDEX && logic_addr < ONE_INDEX)//һ������
	{
		int index0 = (logic_addr - ZERO_INDEX) / ONE_INDEX_NUM + 6;//6 or 7���㼶����
		int index1 = (logic_addr - ZERO_INDEX) % ONE_INDEX_NUM;//һ������
		int block_num1 = inode.i_addr[index0];//һ������block��
		int block1[BLOCK_INT_NUM];
		//��һ������blockλ��
		fd.seekg((BLOCK_START + block_num1) * BLOCK_SIZE, ios::beg);
		fd.read((char*)&block1, sizeof(block1));

		fd.close();
		return block1[index1];
	}
	else if (logic_addr >= ONE_INDEX && logic_addr < TWO_INDEX)//��������
	{
		int index0 = (logic_addr - ONE_INDEX) / TWO_INDEX_NUM + 8;//8 or 9���㼶����
		int index1 = ((logic_addr - ONE_INDEX)% TWO_INDEX_NUM) / ONE_INDEX_NUM;//һ������ 0-127
		int index2 = ((logic_addr - ONE_INDEX) % TWO_INDEX_NUM) % ONE_INDEX_NUM;//�������� 0-127
		int block_num1 = inode.i_addr[index0];//һ������block��
		int block1[BLOCK_INT_NUM];
		//��һ������blockλ��
		fd.seekg((BLOCK_START + block_num1) * BLOCK_SIZE, ios::beg);
		fd.read((char*)&block1, sizeof(block1));
		int block_num2 = block1[index1];
		//���������blockλ��
		int block2[BLOCK_INT_NUM];
		fd.seekg((BLOCK_START + block_num2) * BLOCK_SIZE, ios::beg);
		fd.read((char*)&block2, sizeof(block2));

		fd.close();
		return block2[index2];
	}
	else//����
	{
		cout << "ӳ��ת�����󣬴�С��������" << endl;
		fd.close();
		return -1;
	}
	return -1;
}


int FileSystem::Func_CheckParaNum(vector<string> str, int para_num)
{
	if (str.size() < para_num + 1)
	{
		cout << "�������٣�����������" << endl;
		return -1;
	}
	if (str.size() > para_num + 1)
	{
		cout << "�������࣬����������" << endl;
		return -1;
	}
	return 1;
}

//�����жϸ�inode��Ӧdir���Ƿ����ͬ���ļ�/�ļ���
bool FileSystem::Func_CheckDirExist(int inode, string str)
{
	fstream fd;//c++��ʽ���ļ������̳���iostream
	fd.open(MYDISK_NAME, ios::out | ios::in | ios::binary);

	int block_num = Func_AddrMapping(inode, 0);
	Directory dir;
	fd.seekg((BLOCK_START + block_num) * BLOCK_SIZE, ios::beg);
	fd.read((char*)&dir, sizeof(dir));
	bool result = false;
	for (int i = 0; i < MAX_SUBDIR; i++)
	{
		if (dir.child_filedir[i].m_name[0] == '\0')
		{
			break;
		}
		if (strcmp(dir.child_filedir[i].m_name, str.c_str()) == 0)//�ҵ�
		{
			result = true;
			break;
		}

	}

	fd.close();
	return result;
}


//���̻����ļ��ͷŵĿ�
int FileSystem::Func_ReleaseBlock(int block_num)
{
	if (my_superblock.s_fsize == 0)
	{
		cout << "�Ѿ��ﵽ���̴�С����" << endl;
		return -1;
	}

	my_superblock.s_fsize--;//ά����ǰ����ռ�����

	fstream fd;//c++��ʽ���ļ������̳���iostream
	fd.open(MYDISK_NAME, ios::out | ios::in | ios::binary);

	//��ʱ���ܵ�Ϊ1-100
	if (my_superblock.s_nfree == BLOCK_GROUP_NUM)//superblock���Ѿ����ˣ�������block���洢���ӿ�
	{
		BlockGroup newBG;
		newBG.s_nfree = my_superblock.s_nfree;
		memcpy(newBG.s_free, my_superblock.s_free, BLOCK_FREE_NUM * sizeof(int));
		
		//��superblock��������д��������
		fd.seekg((BLOCK_START + block_num) * BLOCK_SIZE, ios::beg);
		fd.write((char*)&newBG, sizeof(BlockGroup));//����ɹ�

		//����superblock
		my_superblock.s_nfree = 1;
		my_superblock.s_free[0] = block_num;
	}
	else
	{
		my_superblock.s_free[my_superblock.s_nfree] = block_num;
		my_superblock.s_nfree++;
	}

	fd.close();
	return 1;
}

//�Ӵ����и�superblock����һ����
int FileSystem::Func_AllocateBlock()
{
	if (my_superblock.s_fsize== BLOCK_NUM)
	{
		cout << "�Ѿ��ﵽ���̴�С����" << endl;
		return -1;
	}

	my_superblock.s_fsize++;//ά����ǰ����ռ�����
	
	fstream fd;//c++��ʽ���ļ������̳���iostream
	fd.open(MYDISK_NAME, ios::out | ios::in | ios::binary);
	int result = my_superblock.s_free[--my_superblock.s_nfree];//Ŀ¼����һλ����

	/*�����һ�����п鱻���䣬s_nfree��ֵΪ1��һ��ֻ��99�������ã�[0]������һ�飩
	s_nfree=0ʱ�����������Ҳ�����ȥ�������ڷ������ӿ�ǰ�Ƚ��������ݷ���superblock��*/
	if (my_superblock.s_nfree == 0)//superblock��ʱֱ�ӹ�����п�Ϊ0������
	{
		int numx = my_superblock.s_free[0];//��һ����п�λ��
		my_superblock.s_nfree = 100;//����
		//������һ��block��superblock��
		fd.seekg((BLOCK_START + numx) * BLOCK_SIZE + sizeof(int), ios::beg);
		fd.read((char*)my_superblock.s_free, sizeof(int) * BLOCK_FREE_NUM);//����ɹ�
	}

	fd.close();
	return result;

}




int FileSystem::Func_GetFatherInode(string str)
{
	fstream fd;//c++��ʽ���ļ������̳���iostream
	fd.open(MYDISK_NAME, ios::out | ios::in | ios::binary);

	vector <string> path_list;//��vector��ʽ�ֽ��·��
	int pos;
	string sub_path = "";
	int start_inode;//��ʼλ�õ�inode
	if (str[0] == '/')//����·�����Ӹ�Ŀ¼��ʼ��
	{
		//path_list.push_back("~");//�Ȳ����Ŀ¼
		start_inode = 0;//��Ŀ¼inode
		for (pos = 1; pos <= str.size(); pos++)
		{
			if (pos == str.size())
			{
				if (sub_path == "")
					break;
				path_list.push_back(sub_path);
			}
			if (str[pos] == '/')//�ֽ��
			{
				path_list.push_back(sub_path);
				sub_path = "";
			}
			else
			{
				sub_path += str[pos];
			}
		}
	}
	else//���·�����ӵ�ǰ��ʼ��
	{
		//path_list.push_back(now_dir.m_name);//�Ȳ��뵱ǰĿ¼
		start_inode = now_dir.d_inode;//��ǰ·��
		for (pos = 0; pos <= str.size(); pos++)
		{
			if (pos == str.size())
			{
				if (sub_path == "")
					break;
				path_list.push_back(sub_path);
			}
			if (str[pos] == '/')//�ֽ��
			{
				path_list.push_back(sub_path);
				sub_path = "";
			}
			else
			{
				sub_path += str[pos];
			}
		}
	}

	/*·���ֽ���ɺ�Ĵ�����ͬ������vector˳����path��*/
	for (int i = 0; i < path_list.size()-1; i++)
	{
		//����name��inode��
		Inode inode;
		Directory dir;
		//���ݵ�ǰstart_inode�Ų��Ӧinode�ṹ
		fd.seekg(INODE_START * BLOCK_SIZE + start_inode * sizeof(Inode), ios::beg);
		fd.read((char*)&inode, sizeof(Inode));
		//����inode�ṹ��directory�ṹ
		fd.seekg((BLOCK_START + inode.i_addr[0]) * BLOCK_SIZE, ios::beg);
		fd.read((char*)&dir, sizeof(dir));
		//����directory�ṹ����һ���Ƿ��ж�Ӧname
		int over = 0;
		for (int j = 0; j < MAX_SUBDIR; j++)
		{
			if (strcmp(dir.child_filedir[j].m_name, path_list[i].c_str()) == 0)//�ҵ�
			{
				start_inode = dir.child_filedir[j].n_ino;
				over = 1;
				break;
			}
		}
		if (over == 0)
		{
			fd.close();
			//cout << "·�������ڣ��������룡" << endl;
			return -1;
		}
	}
	fd.close();
	return start_inode;
}

//���ڴ����Ѿ��򿪵��ļ���inode���в���addblock(���������ڴ�������)
int FileSystem::Func_OpenedFileAddBlock(int now_pos, int new_size)
{
	fstream fd;//c++��ʽ���ļ������̳���iostream
	fd.open(MYDISK_NAME, ios::out | ios::in | ios::binary);

	Inode* write_inode = &(my_inode_map[now_pos]);

	int old_block_sum;
	int new_block_sum;
	if (write_inode->i_size == 0)
		old_block_sum = 0;
	else
		old_block_sum = (write_inode->i_size - 1) / BLOCK_SIZE + 1;
	write_inode->i_size = new_size;
	if (write_inode->i_size == 0)
		new_block_sum = 0;
	else
		new_block_sum = (write_inode->i_size - 1) / BLOCK_SIZE + 1;

	/*��Ҫע����ǣ��״ν���128��Ҫ�������*/
	/*�����൱�ڽ�����������߼���*/
	for (int logic_addr = old_block_sum; logic_addr < new_block_sum; logic_addr++)
	{
		if (logic_addr < ZERO_INDEX)//ֱ������
		{
			write_inode->i_addr[logic_addr] = Func_AllocateBlock();//ֱ�ӷ���block
		}
		else if (logic_addr < ONE_INDEX)
		{
			int index0 = (logic_addr - ZERO_INDEX) / ONE_INDEX_NUM + 6;//6 or 7���㼶����
			int index1 = (logic_addr - ZERO_INDEX) % ONE_INDEX_NUM;//һ������

			if (index1 == 0)//�����һ��������
			{
				write_inode->i_addr[index0] = Func_AllocateBlock();//1������
			}
			int block_num1 = write_inode->i_addr[index0];
			int block1[BLOCK_INT_NUM];
			/*�ȶ��룬�޸ģ���д*/
			fd.seekg((BLOCK_START + block_num1) * BLOCK_SIZE, ios::beg);
			fd.read((char*)&block1, sizeof(block1));
			block1[index1] = Func_AllocateBlock();//���ݿ�
			//дһ������blockλ��
			fd.seekg((BLOCK_START + block_num1) * BLOCK_SIZE, ios::beg);
			fd.write((char*)&block1, sizeof(block1));
		}
		else if (logic_addr < TWO_INDEX)
		{
			int index0 = (logic_addr - ONE_INDEX) / TWO_INDEX_NUM + 8;//8 or 9���㼶����
			int index1 = ((logic_addr - ONE_INDEX) % TWO_INDEX_NUM) / ONE_INDEX_NUM;//һ������ 0-127
			int index2 = ((logic_addr - ONE_INDEX) % TWO_INDEX_NUM) % ONE_INDEX_NUM;//�������� 0-127

			if (index1 == 0 && index2 == 0)//һ����������������
			{
				write_inode->i_addr[index0] = Func_AllocateBlock();//1������
				int block_num1 = write_inode->i_addr[index0];
				int block1[BLOCK_INT_NUM];
				/*�ȶ��룬�޸ģ���д*/
				fd.seekg((BLOCK_START + block_num1) * BLOCK_SIZE, ios::beg);
				fd.read((char*)&block1, sizeof(block1));
				block1[index1] = Func_AllocateBlock();//2������
				//дһ������blockλ��
				fd.seekg((BLOCK_START + block_num1) * BLOCK_SIZE, ios::beg);
				fd.write((char*)&block1, sizeof(block1));
				//д��������blockλ��
				int block_num2 = block1[index1];
				int block2[BLOCK_INT_NUM];
				/*�ȶ��룬�޸ģ���д*/
				fd.seekg((BLOCK_START + block_num2) * BLOCK_SIZE, ios::beg);
				fd.read((char*)&block2, sizeof(block2));
				block2[index2] = Func_AllocateBlock();//���ݿ�
				fd.seekg((BLOCK_START + block_num2) * BLOCK_SIZE, ios::beg);
				fd.write((char*)&block2, sizeof(block2));
			}
			else if (index2 == 0)//��������
			{
				int block_num1 = write_inode->i_addr[index0];
				int block1[BLOCK_INT_NUM];
				/*�ȶ��룬�޸ģ���д*/
				fd.seekg((BLOCK_START + block_num1) * BLOCK_SIZE, ios::beg);
				fd.read((char*)&block1, sizeof(block1));
				block1[index1] = Func_AllocateBlock();//2������
				//дһ������blockλ��
				fd.seekg((BLOCK_START + block_num1) * BLOCK_SIZE, ios::beg);
				fd.write((char*)&block1, sizeof(block1));
				//д��������blockλ��
				int block_num2 = block1[index1];
				int block2[BLOCK_INT_NUM];
				/*�ȶ��룬�޸ģ���д*/
				fd.seekg((BLOCK_START + block_num2) * BLOCK_SIZE, ios::beg);
				fd.read((char*)&block2, sizeof(block2));
				block2[index2] = Func_AllocateBlock();//���ݿ�
				fd.seekg((BLOCK_START + block_num2) * BLOCK_SIZE, ios::beg);
				fd.write((char*)&block2, sizeof(block2));
			}
			else
			{
				int block_num1 = write_inode->i_addr[index0];
				int block1[BLOCK_INT_NUM];
				/*�ȶ���*/
				fd.seekg((BLOCK_START + block_num1) * BLOCK_SIZE, ios::beg);
				fd.read((char*)&block1, sizeof(block1));
				//д��������blockλ��
				int block_num2 = block1[index1];
				int block2[BLOCK_INT_NUM];
				/*�ȶ��룬�޸ģ���д*/
				fd.seekg((BLOCK_START + block_num2) * BLOCK_SIZE, ios::beg);
				fd.read((char*)&block2, sizeof(block2));
				block2[index2] = Func_AllocateBlock();//���ݿ�
				fd.seekg((BLOCK_START + block_num2) * BLOCK_SIZE, ios::beg);
				fd.write((char*)&block2, sizeof(block2));
			}
		}
		else//error
		{
			//cout << "error!!!" << endl;
			return -1;
		}
	}

	fd.close();
	return 1;
}


/*����ֵ�������ڷ��ض�Ӧ�����±꣬�������򷵻�-1*/
int FileSystem::Func_CheckUser(string name)
{
	fstream fd;//c++��ʽ���ļ������̳���iostream
	fd.open(MYDISK_NAME, ios::out | ios::in | ios::binary);

	Users user_list;
	fd.seekg((BLOCK_START + USER_INFO_BLOCK) * BLOCK_SIZE, ios::beg);
	fd.read((char*)&user_list, BLOCK_SIZE);

	for (int i = 0; i < user_list.user_num; i++)
	{
		if (strcmp(user_list.user[i].name, name.c_str()) == 0)//���û����ļ��д���
		{
			fd.close();//���ļ�
			return i;
		}
	}
	fd.close();//���ļ�
	return -1;
}

/*����ֵ�����û���������ƥ���򷵻�posλ�ã���ƥ���򷵻�-1*/
int FileSystem::Func_UserToken(string name, string passwd)
{
	int pos = Func_CheckUser(name);
	if (pos == -1)
	{
		return -1;
	}

	fstream fd;//c++��ʽ���ļ������̳���iostream
	fd.open(MYDISK_NAME, ios::out | ios::in | ios::binary);

	Users user_list;
	fd.seekg((BLOCK_START + USER_INFO_BLOCK) * BLOCK_SIZE, ios::beg);
	fd.read((char*)&user_list, BLOCK_SIZE);

	if (strcmp(user_list.user[pos].name, name.c_str()) == 0 && strcmp(user_list.user[pos].password, passwd.c_str()) == 0)
	{
		fd.close();//���ļ�
		return pos;
	}
	fd.close();//���ļ�
	return -1;
}


string FileSystem::Func_LastPath(string str)
{
	fstream fd;//c++��ʽ���ļ������̳���iostream
	fd.open(MYDISK_NAME, ios::out | ios::in | ios::binary);

	vector <string> path_list;//��vector��ʽ�ֽ��·��
	int pos;
	string sub_path = "";
	int start_inode;//��ʼλ�õ�inode
	if (str[0] == '/')//����·�����Ӹ�Ŀ¼��ʼ��
	{
		//path_list.push_back("~");//�Ȳ����Ŀ¼
		start_inode = 0;//��Ŀ¼inode
		for (pos = 1; pos <= str.size(); pos++)
		{
			if (pos == str.size())
			{
				if (sub_path == "")
					break;
				path_list.push_back(sub_path);
			}
			if (str[pos] == '/')//�ֽ��
			{
				path_list.push_back(sub_path);
				sub_path = "";
			}
			else
			{
				sub_path += str[pos];
			}
		}
	}
	else//���·�����ӵ�ǰ��ʼ��
	{
		//path_list.push_back(now_dir.m_name);//�Ȳ��뵱ǰĿ¼
		start_inode = now_dir.d_inode;//��ǰ·��
		for (pos = 0; pos <= str.size(); pos++)
		{
			if (pos == str.size())
			{
				if (sub_path == "")
					break;
				path_list.push_back(sub_path);
			}
			if (str[pos] == '/')//�ֽ��
			{
				path_list.push_back(sub_path);
				sub_path = "";
			}
			else
			{
				sub_path += str[pos];
			}
		}
	}
	fd.close();
	return path_list[path_list.size() - 1];
}

/*�Ȳ��Լ����ٲ�ͬһ�û��飬�ٲ�����
����ֵ��-1�����ԣ�1����*/
int FileSystem::Func_AuthCheck(int file_inode, string str)
{
	fstream fd;//c++��ʽ���ļ������̳���iostream
	fd.open(MYDISK_NAME, ios::out | ios::in | ios::binary);

	Inode inode;
	fd.seekg(INODE_START * BLOCK_SIZE + file_inode * sizeof(Inode), ios::beg);
	fd.read((char*)&inode, sizeof(Inode));

	char file_group[12];
	strcpy(file_group, Func_FromUseridGetGroup(inode.i_uid));

	int ret = -1;
	if (inode.i_uid == user_now.id)//���ļ�������
	{
		if (str == "read")
		{
			if ((inode.i_mode & Inode::INODEMODE::OWNER_r) == Inode::INODEMODE::OWNER_r)
				ret = 1;
		}
		else//write
		{
			if ((inode.i_mode & Inode::INODEMODE::OWNER_w) == Inode::INODEMODE::OWNER_w)
				ret = 1;
		}
	}
	else if(strcmp(user_now.usergroup, file_group)==0)//��ͬһ�û���
	{
		if (str == "read")
		{
			if ((inode.i_mode & Inode::INODEMODE::GROUP_r) == Inode::INODEMODE::GROUP_r)
				ret = 1;
		}
		else//write
		{
			if ((inode.i_mode & Inode::INODEMODE::GROUP_w) == Inode::INODEMODE::GROUP_w)
				ret = 1;
		}
	}
	else//������
	{
		if (str == "read")
		{
			if ((inode.i_mode & Inode::INODEMODE::OTHER_r) == Inode::INODEMODE::OTHER_r)
				ret = 1;
		}
		else//write
		{
			if ((inode.i_mode & Inode::INODEMODE::OTHER_w) == Inode::INODEMODE::OTHER_w)
				ret = 1;
		}
	}

	fd.close();//���ļ�
	return ret;
}

/*����uid��ȡgroupname*/
const char* FileSystem::Func_FromUseridGetGroup(int user_id)
{
	fstream fd;//c++��ʽ���ļ������̳���iostream
	fd.open(MYDISK_NAME, ios::out | ios::in | ios::binary);
	Users user_list;
	fd.seekg((BLOCK_START + USER_INFO_BLOCK) * BLOCK_SIZE, ios::beg);
	fd.read((char*)&user_list, BLOCK_SIZE);
	for (int i = 0; i < user_list.user_num; i++)
	{
		if (user_list.user[i].id == user_id)
		{
			fd.close();//���ļ�
			return user_list.user[i].usergroup;
		}
	}

	fd.close();//���ļ�
	return "error";
}
