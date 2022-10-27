#define _CRT_SECURE_NO_WARNINGS
#include "filesystem.h"

/*shell��أ���Ű�����ʼ�����ڵ�shell��ز���*/

//�ֽ�һ��string����������ת��Ϊ�����б�
//stringstream�ַ����������ַ�����ƴ�Ӽ��ѱ�
vector<string> split(string str)
{
	string buf;
	stringstream ss(str);
	vector<string> v;
	// �ַ���ss 
	while (ss >> buf) {
		//תСд
		transform(buf.begin(), buf.end(), buf.begin(), ::tolower);
		v.push_back(buf);
	}

	return v;
}


int FileSystem::Func_LoginLoop()//ѭ����¼
{
	Func_InitInstructions();//��ʼ��ָ�
	//user_num = 1;
	//user[0].Func_FillUser("root", "root", 0);

	char whe_init;
	cout << "�Ƿ����¹������̣���y�����¹�����n�������¹�������";
	cin >> whe_init;
	if (whe_init == 'y')
		Func_CreateNewFileSystem();//���¼���
	else
		Func_RenewMemory();//�����¼���

	while (1)
	{
		string user_name;
		string password;
		cout << "��¼" << endl;
		cout << "�û���: ";
		cin >> user_name;
		cout << "����: ";
		cin >> password;

		vector <string> str_login(3);
		str_login[0] = "su";
		str_login[1] = user_name;
		str_login[2] = password;
		int ret = Func_Su(str_login);
		if (ret == 1)//��¼�ɹ�
		{
			break;
		}
	}

	return 1;
}

int FileSystem::Func_ShellLoop()
{
	inode_open_num = 0;
	cout << "��ӭ��¼������ͨ������help�鿴���ļ�ϵͳ֧�ֵ�ָ��" << endl;


	char c; 
	while ((c = getchar()) != '\n');//������뻺����

	while (1)
	{
		string para_string;//�����б�
		cout << "[" << user_now.name <<" "<< now_dir.m_name<< "]" << "#";

		getline(cin, para_string);
		vector<string> para_list = split(para_string);//��ȡ�����б�

		if (para_list.empty())//ֻ�лس������¿�ʼѭ��
			continue;
		/*��Ч����ʼ���в�������*/
		if (inst.find(para_list[0]) == inst.end())//��ָ��в���ָ���Ƿ�������
		{
			cout << "����" << para_list[0] << "���Ϸ��������¿���" << endl;
			continue;
		}

		//�����Ϸ�
		int k = (this->*inst[para_list[0]])(para_list);//������庯�����д���
		if (k == EXIT_SHELL)
		{
			break;
		}
	}
	return 1;
}

//�����µ��ļ�ϵͳ�������ļ���׼��ʽ����
int FileSystem::Func_CreateNewFileSystem()
{
	/*���ȴ�����Ӧ��С�Ĵ����ļ����ڽ��뺯��֮ǰ�Ѿ�ȷ��û���ļ�����*/
	/*�����ļ���񣺣�.h�ļ��к궨��涨��
	superblock��512*2
	inode:512*10
	���ݴ洢����512*100
	*/

	//�ڵ�ǰĿ�½��ļ���Ϊ�ļ���
	fstream fd(MYDISK_NAME, ios::out);//c++��ʽ���ļ������̳���iostream
	fd.close();
	fd.open(MYDISK_NAME, ios::out  | ios::in | ios::binary);//�������򴴽���ʹ��ios::_Nocreate����������openʧ��
	//���û�д��ļ��������ʾ��Ϣ��throw����
	if (!fd.is_open()) //������
	{
		cout << "�����ļ���ʼ��ʧ��" << endl;
		exit(-1);
	}

	//��SuperBlock���г�ʼ��
	SuperBlock superblock;
	superblock.disk_block_size = SUPERBLOCK_BLOCK_NUM + INODE_BLOCK_NUM + BLOCK_NUM;

	//��ʼ��superblock��inode�����
	superblock.s_isize = INODE_BLOCK_NUM;
	superblock.s_ninode = INODE_NUM - 1;//��ʼ������Ŀ¼���û��ļ�
	for (int i = 0; i < INODE_NUM; i++)
	{
		superblock.s_inode[i] = (INODE_NUM - i - 1);//ջʽ��������--����
	}

	//��ʼ��superblock��block����ʹ�ó�������
	superblock.s_fsize = 2;
	superblock.s_nfree = BLOCK_FREE_NUM - 2;
	//���ݿ�block��������,ÿ��[0]λ�õ�ָ����һ����
	/*������block���Ϊ���࣬���ӿ�����ݿ飬���ӿ�ÿ��100��һ��*/
	int now_pos = 0;//��ǰ���λ��
	int* my_list = superblock.s_free;//ѭ�������е���
	int last_write_pos = BLOCK_START;//д��λ��Ϊ��һ����ʼ��
	BlockGroup BG;
	int whe_super = 0;//ֻ��һ������
	for (int i = 0; ; i++)
	{
		if (i == BLOCK_NUM)//�˴��Ѿ�����ĩβ��û������Ҫ���������
		{
			BG.s_nfree = now_pos;
			fd.seekg(BLOCK_SIZE * last_write_pos, ios::beg);
			fd.write((char*)&BG, sizeof(BG));
			break;
		}
		if (now_pos == BLOCK_FREE_NUM)//��ǰ�����ˣ�д��
		{
			//д��
			if (whe_super == 0)//superblock������д�룬����list
			{
				whe_super = 1;
				my_list = BG.s_free;
			}
			else//��superblock��ֱ��д�룬������list�����Ǽǵ���գ�
			{
				fd.seekg(BLOCK_SIZE * last_write_pos, ios::beg);
				last_write_pos += BLOCK_FREE_NUM;//??????
				BG.s_nfree = BLOCK_FREE_NUM;
				fd.write((char*)&BG, sizeof(BG));//ת����char*ֱ��д���ļ�����Ҫ��ȡʱֱ�Ӵ��ļ���ͬ����ʽ����ת�������ڴ�
				memset(my_list, 0, sizeof(my_list));
			}
			now_pos = 0;
			i--;//����δ����
			continue;
		}
		my_list[now_pos] = i;
		now_pos++;
	}

	//��ʼ��superblock������λ��
	superblock.s_fmod = ~SUPERBLOCK_WRITEBACK;
	superblock.s_ronly = SUPERBLOCK_READWRITE;
	superblock.s_time = time(NULL);//time.h�ж���time����


	//��inode������ز�������ʱ��֧�ֳ������ӣ������Ҫ�����������޸ģ�
	Inode inode1;//��Ŀ¼
	//Inode inode2;//�û��ļ�


	//��Ŀ¼inode��ʼ��
	inode1.i_mode = 0x3FF;					//Ŀ¼������Ȩ��֮����
	inode1.i_nlink = 0;						//�ļ��ڲ�ͬĿ¼���е���������
	inode1.i_uid = 0;						//�ļ��������û�id
	//inode1.i_gid = 0;						//�ļ��������û���id
	inode1.i_size = DIRECTORY_SIZE;			//�ļ���С��Ŀ¼�ļ�Ϊ512
	inode1.i_addr[0] = superblock.s_free[BLOCK_FREE_NUM-1];//�ӵ�����һ�����ݿ�[99]��ʼ
	inode1.i_no = 0;						//�ļ���inode��ţ������ڴ���inodeд�ش��̲���λ�ã�
	inode1.i_atime = time(NULL);			//������ʱ��
	inode1.i_mtime = time(NULL);			//����޸�ʱ��
	//inode����inode����Ӧλ����
	fd.seekg(INODE_START * BLOCK_SIZE + inode1.i_no * sizeof(Inode), ios::beg);
	fd.write((char*)&inode1, sizeof(Inode));

	//��ʼ��DirectoryĿ¼�ṹ������Ŀ¼д��block
	//��Ҫע����ǣ�������Ŀ¼�ж���"."��".."����Ŀ¼����һ��Ŀ¼������Ŀ¼���⣬�������Լ�
	Directory root_directory;
	strcpy(root_directory.m_name, "~");//��Ŀ¼��~������
	root_directory.d_inode = 0;
	//��Ŀ¼.��..��Ϊ�Լ�����ָ��0��inode
	strcpy(root_directory.child_filedir[0].m_name, ".");
	root_directory.child_filedir[0].n_ino = 0;
	strcpy(root_directory.child_filedir[1].m_name, "..");
	root_directory.child_filedir[1].n_ino = 0;
	//strcpy(root_directory.child_filedir[2].m_name, "user_info");//�û��ļ�
	//root_directory.child_filedir[2].n_ino = 1;
	//��ʼ������λ��Ϊ0
	for (int i = 2; i < MAX_SUBDIR; i++) {
		root_directory.child_filedir[i].m_name[0] = '\0';
		root_directory.child_filedir[i].n_ino = -1;//-1����û�ж�Ӧ��inode���ڲ���ʱ�ǵ����жϣ�
	}
	//directory����block��������
	fd.seekg((BLOCK_START + inode1.i_addr[0]) * BLOCK_SIZE, ios::beg);
	fd.write((char*)&root_directory, sizeof(root_directory));

	//�û��ļ�inode��ʼ��
	/*inode2.i_mode = 0x1FF;					//Ŀ¼������Ȩ��֮����
	inode2.i_nlink = 0;						//�ļ��ڲ�ͬĿ¼���е���������
	inode2.i_uid = 0;						//�ļ��������û�id
	inode2.i_gid = 0;						//�ļ��������û���id
	const char userfile[40] = "uid\tusername\tpassword\r\n0\troot\troot123\r\n";//��ʼroot�û�
	inode2.i_size = sizeof(userfile);		//�ļ���С
	inode2.i_addr[0] = superblock.s_free[BLOCK_FREE_NUM-2];//�ӵ����ڶ������ݿ�[98]��ʼ
	inode2.i_no = 1;						//�ļ���inode��ţ������ڴ���inodeд�ش��̲���λ�ã�
	inode2.i_atime = time(NULL);			//������ʱ��
	inode2.i_mtime = time(NULL);			//����޸�ʱ��
	//inodeд��inode����Ӧλ����
	fd.seekg(INODE_START * BLOCK_SIZE + inode2.i_no * sizeof(Inode), ios::beg);
	fd.write((char*)&inode2, sizeof(Inode));
	//���ļ�����ʵ��д��block
	fd.seekg((BLOCK_START + inode2.i_addr[0]) * BLOCK_SIZE, ios::beg);
	fd.write((char*)userfile, sizeof(userfile));*/

	//��Ŀ¼��root�û���ʼ����ɣ�׼����������Ҫ��Ŀ¼���ļ��Ĵ���
	//��ʱ�Ѿ�ʵ������صĶ�Ӧ������ֱ�ӵ������api����
	

	//superblockд���ļ���
	fd.seekg(SUPERBLOCK_START * BLOCK_SIZE, ios::beg);
	fd.write((char*)&superblock, sizeof(superblock));//ת����char*ֱ��д���ļ�����Ҫ��ȡʱֱ�Ӵ��ļ���ͬ����ʽ����ת�������ڴ�
	//superblock�����ڴ�
	fd.seekg(SUPERBLOCK_START * BLOCK_SIZE, ios::beg);
	fd.read((char*)&my_superblock, sizeof(SuperBlock));//ת����char*ֱ��д���ļ�����Ҫ��ȡʱֱ�Ӵ��ļ���ͬ����ʽ����ת�������ڴ�

	//�����Ŀ¼
	//now_path = "~";
	fd.seekg((BLOCK_START + inode1.i_addr[0])* BLOCK_SIZE, ios::beg);
	fd.read((char*)&now_dir, sizeof(now_dir));
	//path_inode = 0;

	//����root�û�
	vector <string> root_init;
	root_init.push_back("adduser");
	root_init.push_back("root");
	root_init.push_back("root123");
	Func_AddUser(root_init);

	//����Ŀ¼
	vector <string> init_dir;
	init_dir.push_back("mkdir");
	init_dir.push_back("/bin");
	Func_Mkdir(init_dir);
	init_dir[1] = "/etc";
	Func_Mkdir(init_dir);
	init_dir[1] = "/home";
	Func_Mkdir(init_dir);
	init_dir[1] = "/dev";
	Func_Mkdir(init_dir);
	init_dir[1] = "/home/texts";
	Func_Mkdir(init_dir);
	init_dir[1] = "/home/reports";
	Func_Mkdir(init_dir);
	init_dir[1] = "/home/photos";
	Func_Mkdir(init_dir);



	fd.close();//���ļ�
	return 0;
}



