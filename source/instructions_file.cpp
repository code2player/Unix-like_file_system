#define _CRT_SECURE_NO_WARNINGS
#include "filesystem.h"


//�����ļ�
int FileSystem::Func_CreateFile(vector<string> str)
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

	string new_file_name = Func_LastPath(str[1]);

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
	inode1.i_mode = 0x1FF;					//Ŀ¼������Ȩ��֮����
	inode1.i_nlink = 0;						//�ļ��ڲ�ͬĿ¼���е���������
	inode1.i_uid = user_now.id;						//�ļ��������û�id
	//inode1.i_gid = user_now.groupid;						//�ļ��������û���id
	inode1.i_size = 0;			//�ļ���С����ͨ�ļ���ʼΪ0
	//inode1.i_addr[0] = Func_AllocateBlock();	//��ʱ����Ҫ����block��

	inode1.i_no = my_superblock.s_inode[--my_superblock.s_ninode];//ջ��inode��ţ�top����
	//my_superblock.s_ninode--;//�ȼ۳�ջ��pop����
	inode1.i_atime = time(NULL);			//������ʱ��
	inode1.i_mtime = time(NULL);			//����޸�ʱ��

	//inode����inode����Ӧλ����
	fd.seekg(INODE_START * BLOCK_SIZE + inode1.i_no * sizeof(Inode), ios::beg);
	fd.write((char*)&inode1, sizeof(Inode));



	//����fatherĿ¼
	strcpy(father_dir.child_filedir[father_dir_insert].m_name, new_file_name.c_str());
	father_dir.child_filedir[father_dir_insert].n_ino = inode1.i_no;

	if (now_dir.d_inode == father_inode_num)//�ǵø��µ�ǰ����Ŀ¼(�����ͬ)
	{
		memcpy(&now_dir, &father_dir, sizeof(Directory));
	}
	fd.seekg((BLOCK_START + father_inode.i_addr[0]) * BLOCK_SIZE, ios::beg);
	fd.write((char*)&father_dir, sizeof(father_dir));

	fd.close();//���ļ�

	Func_RenewDisk();//��Ҫд������򿪵��ļ����棬��������ظ��ر�

	cout << "�������ļ��ɹ�" << endl;
	return 1;
}


//���ļ��������ļ��Ĵ򿪽ṹ
int FileSystem::Func_OpenFile(vector<string> str)
{
	if (Func_CheckParaNum(str, 1) == -1)
	{
		return -1;
	}
	if (inode_open_num == MAX_OPEN_FILE_NUM)//�ļ�ϵͳ�д��ļ����Ѵ�����
	{
		cout << "�ļ�ϵͳ�д��ļ����Ѵ����ޣ����ļ�ʧ��" << endl;
		return -1;
	}

	int open_inode_num = Func_AnalysePath(str[1]);
	if (open_inode_num == -1)//��·��������
	{
		cout << "·�������ڣ����ļ�ʧ��" << endl;
		return -1;
	}

	for (int i = 0; i < inode_open_num; i++)
	{
		if (open_inode_num == my_inode_map[i].i_no)
		{
			cout << "���ļ��Ѵ򿪣���ֱ��ʹ��" << endl;
			return -1;
		}
	}

	fstream fd;//c++��ʽ���ļ������̳���iostream
	fd.open(MYDISK_NAME, ios::out | ios::in | ios::binary);

	//ά���ڴ���file_inode
	fd.seekg(INODE_START * BLOCK_SIZE + open_inode_num * sizeof(Inode), ios::beg);
	fd.read((char*)&my_inode_map[inode_open_num], sizeof(Inode));
	//ά���ڴ���my_file_map
	my_file_map[inode_open_num].inode_num = open_inode_num;
	my_file_map[inode_open_num].offset = 0;
	my_file_map[inode_open_num].openuser_id = user_now.id;
	my_file_map[inode_open_num].file_name = str[1];
	my_file_map[inode_open_num].user_name = user_now.name;
	//ά���ڴ���inode_open_num
	inode_open_num++;

	fd.close();//���ļ�
	Func_RenewDisk();//ͬ��������
	return 1;
}


/*���ļ���д����������˼·�ǣ���д����ֱ�ӶԴ���block�������ڴ��в��洢block��صĶ���
�����ļ�������ص�inode�ʹ򿪽ṹ�������ڴ��С�
�൱��ÿһ�ο�һ���µ�cmd���Ƕ��ڴ����¿�ʼ����ǰ�򿪵��ļ����ļ�ָ��ȶ�ʧ�������޸ı���*/

//�ر��ļ�������ļ��Ĵ򿪽ṹ
int FileSystem::Func_CloseFile(vector<string> str)
{
	if (Func_CheckParaNum(str, 1) == -1)
	{
		return -1;
	}

	int close_inode_num = Func_AnalysePath(str[1]);
	if (close_inode_num == -1)//��·��������
	{
		cout << "·�������ڣ��ر��ļ�ʧ��" << endl;
		return -1;
	}

	int now_pos = -1;//��ǰ�ļ����±�

	for (int i = 0; i < inode_open_num; i++)
	{
		if (close_inode_num == my_inode_map[i].i_no)
		{
			now_pos = i;
			break;
		}
	}

	//����λ�ã�ά����������
	if (now_pos == -1)
	{
		cout << "���ļ�δ�򿪣��ر��ļ�ʧ��" << endl;
		return -1;
	}

	//ɾ��now_posλ�ã������һλ�Ƶ��˴�
	memcpy(&my_inode_map[now_pos], &my_inode_map[inode_open_num - 1], sizeof(Inode));
	memcpy(&my_file_map[now_pos], &my_file_map[inode_open_num - 1], sizeof(File));

	//ֻ��Ҫά���ڴ���inode_open_num���ɣ�����Ҫ���
	inode_open_num--;
	
	return 1;
}


//�鿴�ļ�
int FileSystem::Func_CatFile(vector<string> str)
{
	if (Func_CheckParaNum(str, 1) == -1)
	{
		return -1;
	}

	int cat_inode_num = Func_AnalysePath(str[1]);
	if (cat_inode_num == -1)//��·��������
	{
		cout << "·�������ڣ����ļ�ʧ��" << endl;
		return -1;
	}
	if (Func_AuthCheck(cat_inode_num, "read") == -1)
	{
		cout << "û�ж�ӦȨ�ޣ����ļ�ʧ��" << endl;
		return -1;
	}

	fstream fd;//c++��ʽ���ļ������̳���iostream
	fd.open(MYDISK_NAME, ios::out | ios::in | ios::binary);

	Inode inode;
	fd.seekg(INODE_START * BLOCK_SIZE + cat_inode_num * sizeof(Inode), ios::beg);
	fd.read((char*)&inode, sizeof(Inode));

	if ((inode.i_mode & Inode::INODEMODE::DIRECTORY) == Inode::INODEMODE::DIRECTORY)//Ŀ¼���ݹ�ɾ����������(�ǵ����ɾ���Լ�)
	{
		cout << "·��ΪĿ¼�����ļ�ʧ��" << endl;
		fd.close();//���ļ�
		return -1;
	}


	char buf[BLOCK_SIZE + 1];//��ӡ������
	buf[BLOCK_SIZE] = '\0';
	//�����ļ���size����block_sum
	int block_sum;//���ļ�ռ�õ�������block��
	if (inode.i_size == 0)
		block_sum = 0;
	else
		block_sum = (inode.i_size - 1) / BLOCK_SIZE + 1;

	for (int i = 0; i < block_sum; i++)
	{
		int block_pos = Func_AddrMapping(cat_inode_num, i);
		fd.seekg((BLOCK_START + block_pos) * BLOCK_SIZE, ios::beg);
		fd.read((char*)buf, BLOCK_SIZE);
		if (i == block_sum - 1)
			buf[(inode.i_size - 1) % BLOCK_SIZE + 1] = '\0';
		cout << buf;

	}

	cout << endl;
	fd.close();//���ļ�
	return 1;
}


//���ļ�����ָ���ֽڴ�С
int FileSystem::Func_ReadFile(vector<string> str)
{
	if (Func_CheckParaNum(str, 2) == -1)
	{
		return -1;
	}

	int cat_inode_num = Func_AnalysePath(str[1]);
	if (cat_inode_num == -1)//��·��������
	{
		cout << "·�������ڣ����ļ�ʧ��" << endl;
		return -1;
	}

	if (Func_AuthCheck(cat_inode_num, "read") == -1)
	{
		cout << "û�ж�ӦȨ�ޣ����ļ�ʧ��" << endl;
		return -1;
	}

	int read_byte = atoi(str[2].c_str());

	fstream fd;//c++��ʽ���ļ������̳���iostream
	fd.open(MYDISK_NAME, ios::out | ios::in | ios::binary);

	Inode inode;
	fd.seekg(INODE_START * BLOCK_SIZE + cat_inode_num * sizeof(Inode), ios::beg);
	fd.read((char*)&inode, sizeof(Inode));

	if ((inode.i_mode & Inode::INODEMODE::DIRECTORY) == Inode::INODEMODE::DIRECTORY)//Ŀ¼���ݹ�ɾ����������(�ǵ����ɾ���Լ�)
	{
		cout << "·��ΪĿ¼�����ļ�ʧ��" << endl;
		fd.close();//���ļ�
		return -1;
	}
	int open_pos = -1;
	for (int i = 0; i < inode_open_num; i++)
	{
		if (cat_inode_num == my_inode_map[i].i_no)
		{
			open_pos = i;
			break;
		}
	}
	if (open_pos == -1)
	{
		cout << "���ļ�δ�򿪣������޸��ļ�ָ��λ��" << endl;
		return -1;
	}


	read_byte = min(read_byte, (inode.i_size - my_file_map[open_pos].offset));//����������޶ȣ��������󼴿�
	if (read_byte == 0)
	{
		cout << endl;
		fd.close();//���ļ�
		return 1;
	}

	int start_block = my_file_map[open_pos].offset / BLOCK_SIZE;
	int end_block = (my_file_map[open_pos].offset + read_byte - 1) / BLOCK_SIZE;
	char buf[BLOCK_SIZE + 1];//��ӡ������
	buf[BLOCK_SIZE] = '\0';

	//[start_pos,end_pos)
	int start_pos = my_file_map[open_pos].offset;
	int end_pos = my_file_map[open_pos].offset + read_byte;

	for (int i = start_block; i <= end_block; i++)
	{
		int block_pos = Func_AddrMapping(cat_inode_num, i);
		fd.seekg((BLOCK_START + block_pos) * BLOCK_SIZE, ios::beg);
		fd.read((char*)buf, BLOCK_SIZE);

		char read_buf[BLOCK_SIZE + 1];//��ӡ������
		memset(read_buf, 0, BLOCK_SIZE + 1);
		int this_start = start_pos % BLOCK_SIZE;
		int this_end;
		if (i == end_block)
		{
			this_end = (end_pos - 1) % BLOCK_SIZE;
			memcpy(read_buf, &buf[this_start], (this_end - this_start + 1));
		}
		else
		{
			this_end = BLOCK_SIZE - 1;
			memcpy(read_buf, &buf[this_start], (this_end - this_start + 1));
		}
		start_pos = i * BLOCK_SIZE;
		cout << read_buf;
	}
	cout << endl;
	my_file_map[open_pos].offset += read_byte;
	fd.close();//���ļ�
	return 1;
}





//�޸��ļ�ָ��λ��
int FileSystem::Func_LseekFile(vector<string> str)
{
	if (Func_CheckParaNum(str, 2) == -1)
	{
		return -1;
	}

	int new_seek_pos = atoi(str[2].c_str());

	int lseek_inode_num = Func_AnalysePath(str[1]);
	if (lseek_inode_num == -1)//��·��������
	{
		cout << "·�������ڣ����ļ�ʧ��" << endl;
		return -1;
	}




	int open_pos = -1;
	for (int i = 0; i < inode_open_num; i++)
	{
		if (lseek_inode_num == my_inode_map[i].i_no)
		{
			open_pos = i;
			break;
		}
	}
	if (open_pos == -1)
	{
		cout << "���ļ�δ�򿪣������޸��ļ�ָ��λ��" << endl;
		return -1;
	}
	if ((my_inode_map[open_pos].i_mode & Inode::INODEMODE::DIRECTORY) == Inode::INODEMODE::DIRECTORY)//Ŀ¼���ݹ�ɾ����������(�ǵ����ɾ���Լ�)
	{
		cout << "·��ΪĿ¼�����ļ�ʧ��" << endl;
		return -1;
	}
	if (new_seek_pos<0 || new_seek_pos>my_inode_map[open_pos].i_size)
	{
		cout << "ָ�볬���ļ���С��Χ" << endl;
		return -1;
	}

	my_file_map[open_pos].offset = new_seek_pos;
	return 1;
}



//д�ļ�
int FileSystem::Func_WriteFile(vector<string> str)
{
	/*if (Func_CheckParaNum(str, 2) == -1)
	{
		return -1;
	}*/
	int keep_offset = 0;
	int file_myoffset;
	if (str.size() == 4 && str[3] == "myin")//�ⲿ���룬�����ļ�ָ��
	{
		keep_offset = 1;
	}
	else if(str.size() == 3)
	{
		;
	}
	else
	{
		cout << "������������������" << endl;
		return -1;
	}

	int cat_inode_num = Func_AnalysePath(str[1]);
	if (cat_inode_num == -1)//��·��������
	{
		cout << "·�������ڣ����ļ�ʧ��" << endl;
		return -1;
	}
	if (Func_AuthCheck(cat_inode_num, "write") == -1)
	{
		cout << "û�ж�ӦȨ�ޣ����ļ�ʧ��" << endl;
		return -1;
	}

	int open_pos = -1;
	for (int i = 0; i < inode_open_num; i++)
	{
		if (cat_inode_num == my_inode_map[i].i_no)
		{
			open_pos = i;
			break;
		}
	}
	if (open_pos == -1)
	{
		cout << "���ļ�δ�򿪣������޸�" << endl;
		return -1;
	}
	if ((my_inode_map[open_pos].i_mode & Inode::INODEMODE::DIRECTORY) == Inode::INODEMODE::DIRECTORY)//Ŀ¼
	{
		cout << "·��ΪĿ¼��д���ļ�ʧ��" << endl;
		return -1;
	}

	fstream fd;//c++��ʽ���ļ������̳���iostream
	fd.open(MYDISK_NAME, ios::out | ios::in | ios::binary);

	Inode* write_inode = &(my_inode_map[open_pos]);
	File* write_file = &(my_file_map[open_pos]);

	if (keep_offset==1)
	{
		file_myoffset = write_file->offset;
		write_file->offset = 0;
	}

	//str[2].c_str();

	/*�ȸ�����������̿�
	�ȸ��ݵ�ǰ�ļ�ָ���str[2].sizeȷ����Ҫ�漰����block��
	��ʼ���ĩβ�������⴦���ȱ��������ٸ����ٱ��棩*/
	int old_size = write_inode->i_size;
	int new_size = max((unsigned int)(write_inode->i_size), write_file->offset + str[2].size());
	int start_block;
	int end_block;
	/*if (write_file->offset == 0)
		start_block = 0;
	else
		start_block = (write_file->offset - 1) / BLOCK_SIZE + 1;
	if (write_file->offset + str[2].size() == 0)
		end_block = 0;
	else
		end_block = (write_file->offset + str[2].size() - 1) / BLOCK_SIZE + 1;*/

	start_block = (write_file->offset) / BLOCK_SIZE;
	end_block = (write_file->offset + str[2].size()) / BLOCK_SIZE;


	int ret = Func_OpenedFileAddBlock(open_pos, new_size);//�����ļ�block�ɹ���ֱ��ת�����Ҽ���
	if (ret == -1)
	{
		write_inode->i_size = old_size;
		cout << "��С�������ޣ����ʧ��" << endl;
		fd.close();//���ļ�
		Func_RenewDisk();//ͬ��������
		return 1;
	}
	Func_SaveFileInode();//�洢�ڴ��еĴ�file���inode

	//Func_AddrMapping()

	
	int str_pos_start = 0;//
	int last_length = str[2].size();//ʣ��û��copy�ĳ���
	
	int buf_start = write_file->offset % BLOCK_SIZE;//memcpy��buf�е���ʼλ��
	int buf_len = min(512 - buf_start, last_length);//����memcpy��buf�ĳ���
	//��blockΪ��λ˳�δ������⴦����ʼλ��
	for (int logic_addr = start_block; logic_addr <= end_block; logic_addr++)
	{
		//�ȶ���
		char buf[BLOCK_SIZE];
		int block_pos = Func_AddrMapping(cat_inode_num, logic_addr);
		fd.seekg((BLOCK_START + block_pos) * BLOCK_SIZE, ios::beg);
		fd.read((char*)buf, BLOCK_SIZE);

		//������ά��
		memcpy(&buf[buf_start], &str[2].c_str()[str_pos_start], buf_len);

		fd.seekg((BLOCK_START + block_pos) * BLOCK_SIZE, ios::beg);
		fd.write((char*)buf, BLOCK_SIZE);

		str_pos_start += buf_len;
		last_length -= buf_len;
		buf_start = 0;
		buf_len = min(512 - buf_start, last_length);
	}

	write_file->offset += str[2].size();
	if (keep_offset == 1)
	{
		write_file->offset = file_myoffset;
	}

	fd.close();//���ļ�
	Func_RenewDisk();//ͬ��������
	return 1;
}


//�ӱ���ϵͳ�е����ļ�
int FileSystem::Func_WriteFileFromMine(vector<string> str)
{
	if (Func_CheckParaNum(str, 2) == -1)
	{
		return -1;
	}

	ifstream ifs(str[2].c_str(), ios::in | ios::binary);
	string content((istreambuf_iterator<char>(ifs)),(istreambuf_iterator<char>()));
	vector<string> str1(4);
	str1[0] = "write";
	str1[1] = str[1];
	str1[2] = content;
	str1[3] = "myin";

	Func_WriteFile(str1);
	ifs.close();



	return 1;
}



int FileSystem::Func_WriteFileToMine(vector<string> str)
{
	if (Func_CheckParaNum(str, 2) == -1)
	{
		return -1;
	}

	ofstream ifs(str[2].c_str(), ios::out| ios::trunc| ios::binary);


	int cat_inode_num = Func_AnalysePath(str[1]);
	if (cat_inode_num == -1)//��·��������
	{
		cout << "·�������ڣ����ļ�ʧ��" << endl;
		ifs.close();
		return -1;
	}
	if (Func_AuthCheck(cat_inode_num, "read") == -1)
	{
		cout << "û�ж�ӦȨ�ޣ����ļ�ʧ��" << endl;
		ifs.close();
		return -1;
	}

	fstream fd;//c++��ʽ���ļ������̳���iostream
	fd.open(MYDISK_NAME, ios::out | ios::in | ios::binary);

	Inode inode;
	fd.seekg(INODE_START * BLOCK_SIZE + cat_inode_num * sizeof(Inode), ios::beg);
	fd.read((char*)&inode, sizeof(Inode));

	if ((inode.i_mode & Inode::INODEMODE::DIRECTORY) == Inode::INODEMODE::DIRECTORY)//Ŀ¼���ݹ�ɾ����������(�ǵ����ɾ���Լ�)
	{
		cout << "·��ΪĿ¼�����ļ�ʧ��" << endl;
		ifs.close();
		fd.close();//���ļ�
		return -1;
	}


	char buf[BLOCK_SIZE];//��ӡ������
	
	//buf[BLOCK_SIZE] = '\0';
	//�����ļ���size����block_sum
	int block_sum;//���ļ�ռ�õ�������block��
	if (inode.i_size == 0)
		block_sum = 0;
	else
		block_sum = (inode.i_size - 1) / BLOCK_SIZE + 1;

	for (int i = 0; i < block_sum; i++)
	{
		int block_pos = Func_AddrMapping(cat_inode_num, i);
		fd.seekg((BLOCK_START + block_pos) * BLOCK_SIZE, ios::beg);
		fd.read((char*)buf, BLOCK_SIZE);
		int slen = BLOCK_SIZE;
		if (i == block_sum - 1)
		{
			slen = (inode.i_size - 1) % BLOCK_SIZE + 1;
		}

		ifs.write(buf, slen);
		ifs.flush();

	}


	fd.close();//���ļ�
	ifs.close();

	return 1;
}





int FileSystem::Func_OpenFileState(vector<string> str)
{
	if (Func_CheckParaNum(str, 0) == -1)
	{
		return -1;
	}
	cout << setw(16) << left << "file_name";
	cout << setw(16) << left << "user_name";
	cout << setw(16) << left << "offset";
	cout << endl;

	for (int i = 0; i < inode_open_num; i++)
	{
		//��ӡ����
		cout << setw(16) << left << my_file_map[i].file_name;
		cout << setw(16) << left << my_file_map[i].user_name;
		cout << setw(16) << left << my_file_map[i].offset;
		cout << right;
		cout << endl;
	}
	return 1;
}














