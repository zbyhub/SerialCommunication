#include <iostream>
#include <windows.h>
#include <tchar.h>
#include <iomanip>
#include<io.h>
#include<string>
#include"Serial.h"
#include<stdio.h>
#include<fstream>
#include<istream>


using namespace std;

void fileWatcher();
bool file_dir_message(string filedir, int &fileNum, string &file_name, __time32_t &newfile_time);
void file_Watch(string filedir, unsigned int interval_time = 1, string action_1 = "ATD176xxxxxxxx;\r", string action_2 = "ATD176xxxxxxxx;\r");

int main()
{

	string file_dir = "";
	string phonenum = "";
	string gettime = "";
	unsigned int i_t = 60;//ÿ��i_t���Ӽ���һ��
	char testmark = '0';
	bool errormark = true;

	cout << "��������ӵ��ļ���·��" << endl;
	getline(cin, file_dir);
	cout << file_dir << endl;
	cout << "------OK" << endl;

	cout << "ÿ��������鿴һ��:" << endl;
	getline(cin, gettime);
	i_t = atoi(gettime.c_str());
	cout << i_t << endl;
	cout << "------OK" << endl;

	string phone_mark = "1";
	string phone_write[2] = { "" };
	string phone[5] = { "ATD176xxxxxxxx;\r","ATD188xxxxxxxx;\r" ,"ATD158xxxxxxxx;\r" ,"ATD152xxxxxxxx;\r" ,"ATD130xxxxxxxx;\r" };
	string phone_cout[6] = { "0/�ֶ�����","1/��176xxxxxxxx","2/��188xxxxxxxx" ,"3/��158xxxxxxxx" ,"4/��152xxxxxxxx" ,"5/��130xxxxxxxx" };
	unsigned int which_phone_first = 1;//Ĭ��
	unsigned int which_phone_second = 1;//Ĭ��

	do
	{
		cout << "��ѡ�����:" << endl;
		cout << "---------------------" << endl;
		for (int phone_i = 0; phone_i < 6; phone_i++)
		{
			cout << phone_cout[phone_i] << endl;
		}
		cout << "---------------------" << endl;
		getline(cin, phone_mark);
		which_phone_first = atoi(phone_mark.c_str());
		//test
		cout << which_phone_first << endl;

	} while (which_phone_first > 5 && which_phone_first < 0);

	if (which_phone_first == 0)
	{
		do
		{
			cout << "���������:" << endl;
			getline(cin, phonenum);
		} while (phonenum.length() != 11);

		phone_write[0] = "ATD" + phonenum + ";\r";
		cout << phone_write[0] << endl;
		cout << "------OK" << endl;
	}
	//**********************8
	do
	{
		cout << "��ѡ��ѡ����(���ظ�):" << endl;
		cout << "---------------------" << endl;
		for (int phone_i = 0; phone_i < 6; phone_i++)
		{
		    cout << phone_cout[phone_i] << endl;
		}
		cout << "---------------------" << endl;
		getline(cin, phone_mark);
		which_phone_second = atoi(phone_mark.c_str());
	} while (which_phone_second < 0 && which_phone_second > 5);

	if (which_phone_second == 0)
	{
		do
		{
			cout << "�����뱸ѡ����:" << endl;
			getline(cin, phonenum);
		} while (phonenum.length() != 11);

		phone_write[1] = "ATD" + phonenum + ";\r";
		cout << phone_write[1] << endl;
		cout << "------OK" << endl;
	}


	string GSV = "AT+GSV\r";//��ʾģ�������̣����ƺͰ汾��Ϣ
	string CSQ = "AT+CSQ\r";//��������ź�ǿ�Ⱥ� SIM ����������ʽ
	string IPR = "AT+IPR\r";//��ѯ��ǰ���ڲ����� ���أ�IPR:0 ˵����ǰ�Ĳ�����Ϊ����Ӧ��
	string CPBS = "AT+CPBS=?\r";//�г����õ绰��
	string SM = "AT+CPBS=\"SM\"\r";//ѡ��SIM�绰����AT+CPBS="SM"
	string CPBR = "AT+CPBR=?\r";//��ʾ�绰����Ŀ��Χ��AT+CPBR=?
	string ATDL = "ATDL\r";//�ز���һ�ε绰�ţ�ATDL
	string hangup = "ATH\r";//�ҶϺ��У�ATH
							//string IPR = "AT+CSQ\r";//
							//string IPR = "AT+CSQ\r";//
							//string IPR = "AT+CSQ\r";//
							//string IPR = "AT+CSQ\r";//
							//string IPR = "AT+CSQ\r";//
	//cout << "���Ų�����..." << endl;
	//test->SIM800A_AT(MyCom, zby1);//���Ų���
	//test->SIM800A_AT(MyCom, action);
	do
	{
		cout << "����(������Y)/ֱ���������(������M):" << endl;
		testmark = cin.get();
		cin.get();
		if (testmark == 'Y' || testmark == 'y')
		{
			cout << "��ʼ����......" << endl;
			Serial *test = Serial::getInstance();

			if (which_phone_first == 0)
			{
				test->testfunction(phone_write[0]);
			}
			else
			{
				test->testfunction(phone[which_phone_first - 1]);
			}
		}
		else
		{
			errormark = false;
		}
		
	} while (errormark);

	if (which_phone_first == 0 && which_phone_second == 0)
	{
		file_Watch(file_dir, i_t, phone_write[0], phone_write[2]);//������������
	}
	else
	{
		if (which_phone_first == 0)
		{
			file_Watch(file_dir, i_t, phone_write[0], phone[which_phone_second - 1]);
		}

		if (which_phone_second == 0)
		{
			file_Watch(file_dir, i_t, phone[which_phone_first - 1], phone_write[2]);
		}

		file_Watch(file_dir, i_t, phone[which_phone_first - 1], phone[which_phone_second - 1]);
	}

	cout << "����Ӳ������,���ֹͣ,10���رճ���" << endl;
	Sleep(10 * 1000);
	exit(0);
	return 1;
}









void file_Watch(string filedir,unsigned int interval_time, string action_1,string action_2)
{

	using namespace std;
	Serial *test = Serial::getInstance();
	//Serial *test1 = Serial::getInstance();

	//cout << test << endl;
	//cout << test1 << endl;
	cout << "waiting";

	char MyCom = 'n';
	do
	{
		MyCom = test->WhichIsMySerial();//Ѱ�Ҵ��ڲ���ʼ��  �����ﴮ���Ѿ���
		Sleep(2 * 1000);
		cout << ".";
	}
	while (MyCom == 'n' || MyCom == 'i' || MyCom == 'E');
	cout << "���ںţ�" << MyCom << endl;

	//ʱ��
	SYSTEMTIME st = { 0 };
	__time32_t time = 0;

	WORD old_second = 60;
	WORD new_second = 60;
	//����
	int record = -1;
	bool call = false;
	//�����ļ���
	//bool whilemark = true;
	

	string dir = filedir + "\\*.*";
	int file_amount = 0;
	string new_filename = "";
	string filename_buff = "";

	file_dir_message(dir, file_amount, filename_buff, time);
	new_filename = filename_buff;

	//
	cout << "��ʼ��� Start Monitor..." << endl;
	while(1)
	{
		//������Ӳ���Ͽ�������
		if (!test->DoesMyComWorking())
		{
			cout << "ѭ���ж�" << endl;
			break;
		}
		//ʱ�䴦����
		GetLocalTime(&st);
		if (old_second != st.wSecond)
		{
			cout << ".";
			record++;
			old_second = st.wSecond;
		}
		if (record == interval_time)
		{
			cout << "ʱ��: " << st.wHour << ':' << st.wMinute << ':' <<st.wSecond << endl;
			file_dir_message(dir, file_amount, new_filename,time);

			cout << "�ļ�����:        " <<file_amount << endl;
			cout << "����������ļ���:" << new_filename << endl;
			cout << "����ʱ��:        " << time << endl;

			if (filename_buff == new_filename)
			{
				filename_buff = new_filename;
				cout << "***************************����*******************************" << endl;
				cout << "***"<<"����"<<record<<"����û�г����ļ�����***" << endl;
				//switch(action):

				cout << test->SIM800A_AT(MyCom, action_1,40) << endl;

				cout << test->SIM800A_AT(MyCom, action_2, 40) << endl;


				/*
				string ReplyMessage = "�ȴ��ظ�......";
				unsigned int sleep_time = 6;
				while (1)
				{
					Sleep(2 * 1000);
					sleep_time--;
					if (test->readport(ReplyMessage))
					{
						cout << ReplyMessage << endl;
					}
                    
				    if (sleep_time <= 0)
					{
						break;
					}	
				}*/


				cout << "***************************����*******************************" << endl;
			}
			record = 0;
		}
	}

	//fileWatcher();
	//cin.get();
	//cin.get();
}
bool Is_NewFile_OK()
{
	return 1;
}



bool file_dir_message(string filedir, int &fileNum, string &file_name, __time32_t &newfile_time)
{
	_finddata_t fileInfo;
	fileNum = 0;
	long handle = _findfirst(filedir.c_str(), &fileInfo);

	if (handle == -1L)
	{
		cerr << "failed to transfer files" << endl;
		return false;
	}

	do
	{
		if (!(fileInfo.attrib & _A_SUBDIR))//���ҵ��Ĳ����ļ��в��ܼ�һ
		{
			fileNum++;
		}
		if (fileInfo.time_create > newfile_time)
		{
			newfile_time = fileInfo.time_create;
			file_name = fileInfo.name;
		}
	} while (_findnext(handle, &fileInfo) == 0);

	//cout << "files number:  " << fileNum << endl;

	return true;
}
/**/
void fileWatcher()
{
	DWORD cbBytes;
	char file_name[MAX_PATH]; //�����ļ���;
	char file_rename[MAX_PATH]; //�����ļ��������������;
	char notify[1024];
	int count = 0; //�ļ�����������ͬʱ������ɾ������ļ������Խ��и��Ѻõ���ʾ;
	int str_old_mark = 0;
	int str_new_mark = 0;
	string fdir = "D:\\FTP";
	TCHAR *dir = (char*)("D:\\FTP");
    string buff;
	
	HANDLE dirHandle = CreateFile(dir,
		GENERIC_READ | GENERIC_WRITE | FILE_LIST_DIRECTORY,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
		NULL);

	if (dirHandle == INVALID_HANDLE_VALUE) //�������ض����Ŀ���ļ�ϵͳ��֧�ָò���������ʧ�ܣ�ͬʱ����GetLastError()����ERROR_INVALID_FUNCTION
	{
		cout << "error" + GetLastError() << endl;
	}

	memset(notify, 0, strlen(notify));
	FILE_NOTIFY_INFORMATION *pnotify = (FILE_NOTIFY_INFORMATION*)notify;  //

	//cout << "��ʼ��� Start Monitor..." << endl;

	while (1)
	{
		if (ReadDirectoryChangesW(dirHandle, &notify, 1024, true,
			FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_SIZE,
			&cbBytes, NULL, NULL))
		{
			//ת���ļ���Ϊ���ֽ��ַ���;
			if (pnotify->FileName)
			{ 
				memset(file_name, 0, strlen(file_name));
				//int nlen;
				//WideCharToMultiByte(CP_ACP, 0, pnotify->FileName, -1, file_name, 99, NULL, NULL);
				WideCharToMultiByte(CP_ACP, 0, pnotify->FileName, pnotify->FileNameLength / sizeof(wchar_t), file_name, 99, NULL, NULL);
				

				//cout << sizeof(pnotify) << endl;
				//cout <<  pnotify->FileNameLength / 2 << endl;
			}


			//��ȡ���������ļ���;
			if (pnotify->NextEntryOffset != 0 && (pnotify->FileNameLength > 0 && pnotify->FileNameLength < MAX_PATH))
			{

				PFILE_NOTIFY_INFORMATION p = (PFILE_NOTIFY_INFORMATION)((char*)pnotify + pnotify->NextEntryOffset);//�ṹָ��pnotify��ǿ��ת����(char*)(�ַ�ָ��)

				memset(file_rename, 0, sizeof(file_rename));
				//WideCharToMultiByte(CP_ACP, 0, p->FileName, -1, file_rename, 99, NULL, NULL);
				WideCharToMultiByte(CP_ACP, 0, p->FileName, p->FileNameLength / sizeof(wchar_t), file_rename, 99, NULL, NULL);
			}

			//�������͹�����,�����ļ����������ġ�ɾ������������;
			switch (pnotify->Action)
			{
			case FILE_ACTION_ADDED:
				
				/*
				buff = fdir + '\\'+ file_name;
				str_new_mark = buff.rfind('\\', buff.length());
				if(str_old_mark != str_new_mark)//����ļ����Ƿ����䶯���б䶯������һ��
				{
					str_old_mark = str_new_mark;
					buff.erase(str_new_mark + 1);
					buff = buff + "*.*";
				    //cout << buff << endl;
					only_file_amount(buff.c_str(), count);
					count--;
				}
                */
				count++;
				cout << "�ļ�����: " << count << endl;
				cout << setw(5) << "����:�����ļ�  �ļ���: " << setw(5) << file_name << endl;
				break;

			case FILE_ACTION_MODIFIED:
				cout << "file modified:" << setw(5) << file_name << endl;
				break;

			case FILE_ACTION_REMOVED:

				/*
				buff = fdir + '\\' + file_name;
				str_new_mark = buff.rfind('\\', buff.length());
				if (str_old_mark != str_new_mark)//����ļ����Ƿ����䶯���б䶯������һ��
				{
					str_old_mark = str_new_mark;
					buff.erase(str_new_mark + 1);
					buff = buff + "*.*";
					//cout << buff << endl;
					only_file_amount(buff.c_str(), count);
					count++;
				}
                */
				count--;
				cout << "count  = " << count << endl;
				
				cout << setw(5) << "file removed:" << setw(5) << file_name << endl;
				break;

			case FILE_ACTION_RENAMED_OLD_NAME:
				//cout << pnotify->NextEntryOffset << endl;
				cout << "file renamed:" << setw(5) << file_name << "->" << file_rename << endl;
				break;

			default:
				cout << "UNknow command!" << endl;
			}
		}
	}

	CloseHandle(dirHandle);
}
