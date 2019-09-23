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
	unsigned int i_t = 60;//每隔i_t分钟监视一次
	char testmark = '0';
	bool errormark = true;

	cout << "请输入监视的文件夹路径" << endl;
	getline(cin, file_dir);
	cout << file_dir << endl;
	cout << "------OK" << endl;

	cout << "每隔多少秒查看一次:" << endl;
	getline(cin, gettime);
	i_t = atoi(gettime.c_str());
	cout << i_t << endl;
	cout << "------OK" << endl;

	string phone_mark = "1";
	string phone_write[2] = { "" };
	string phone[5] = { "ATD176xxxxxxxx;\r","ATD188xxxxxxxx;\r" ,"ATD158xxxxxxxx;\r" ,"ATD152xxxxxxxx;\r" ,"ATD130xxxxxxxx;\r" };
	string phone_cout[6] = { "0/手动输入","1/张176xxxxxxxx","2/郭188xxxxxxxx" ,"3/闫158xxxxxxxx" ,"4/宋152xxxxxxxx" ,"5/崔130xxxxxxxx" };
	unsigned int which_phone_first = 1;//默认
	unsigned int which_phone_second = 1;//默认

	do
	{
		cout << "请选择号码:" << endl;
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
			cout << "请输入号码:" << endl;
			getline(cin, phonenum);
		} while (phonenum.length() != 11);

		phone_write[0] = "ATD" + phonenum + ";\r";
		cout << phone_write[0] << endl;
		cout << "------OK" << endl;
	}
	//**********************8
	do
	{
		cout << "请选择备选号码(可重复):" << endl;
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
			cout << "请输入备选号码:" << endl;
			getline(cin, phonenum);
		} while (phonenum.length() != 11);

		phone_write[1] = "ATD" + phonenum + ";\r";
		cout << phone_write[1] << endl;
		cout << "------OK" << endl;
	}


	string GSV = "AT+GSV\r";//显示模块制造商，名称和版本信息
	string CSQ = "AT+CSQ\r";//检查网络信号强度和 SIM 卡情况命令格式
	string IPR = "AT+IPR\r";//查询当前串口波特率 返回：IPR:0 说明当前的波特率为自适应；
	string CPBS = "AT+CPBS=?\r";//列出可用电话薄
	string SM = "AT+CPBS=\"SM\"\r";//选择SIM电话簿：AT+CPBS="SM"
	string CPBR = "AT+CPBR=?\r";//显示电话薄条目范围：AT+CPBR=?
	string ATDL = "ATDL\r";//重播上一次电话号：ATDL
	string hangup = "ATH\r";//挂断呼叫：ATH
							//string IPR = "AT+CSQ\r";//
							//string IPR = "AT+CSQ\r";//
							//string IPR = "AT+CSQ\r";//
							//string IPR = "AT+CSQ\r";//
							//string IPR = "AT+CSQ\r";//
	//cout << "拨号测试中..." << endl;
	//test->SIM800A_AT(MyCom, zby1);//拨号测试
	//test->SIM800A_AT(MyCom, action);
	do
	{
		cout << "测试(请输入Y)/直接启动监控(请输入M):" << endl;
		testmark = cin.get();
		cin.get();
		if (testmark == 'Y' || testmark == 'y')
		{
			cout << "开始测试......" << endl;
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
		file_Watch(file_dir, i_t, phone_write[0], phone_write[2]);//输入两个号码
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

	cout << "发生硬件错误,监控停止,10秒后关闭程序" << endl;
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
		MyCom = test->WhichIsMySerial();//寻找串口并初始化  在这里串口已经打开
		Sleep(2 * 1000);
		cout << ".";
	}
	while (MyCom == 'n' || MyCom == 'i' || MyCom == 'E');
	cout << "串口号：" << MyCom << endl;

	//时间
	SYSTEMTIME st = { 0 };
	__time32_t time = 0;

	WORD old_second = 60;
	WORD new_second = 60;
	//拨号
	int record = -1;
	bool call = false;
	//监视文件夹
	//bool whilemark = true;
	

	string dir = filedir + "\\*.*";
	int file_amount = 0;
	string new_filename = "";
	string filename_buff = "";

	file_dir_message(dir, file_amount, filename_buff, time);
	new_filename = filename_buff;

	//
	cout << "开始监控 Start Monitor..." << endl;
	while(1)
	{
		//运行中硬件断开处理函数
		if (!test->DoesMyComWorking())
		{
			cout << "循环中断" << endl;
			break;
		}
		//时间处理函数
		GetLocalTime(&st);
		if (old_second != st.wSecond)
		{
			cout << ".";
			record++;
			old_second = st.wSecond;
		}
		if (record == interval_time)
		{
			cout << "时间: " << st.wHour << ':' << st.wMinute << ':' <<st.wSecond << endl;
			file_dir_message(dir, file_amount, new_filename,time);

			cout << "文件数量:        " <<file_amount << endl;
			cout << "最近创建的文件名:" << new_filename << endl;
			cout << "创建时间:        " << time << endl;

			if (filename_buff == new_filename)
			{
				filename_buff = new_filename;
				cout << "***************************警告*******************************" << endl;
				cout << "***"<<"发现"<<record<<"秒内没有出现文件更新***" << endl;
				//switch(action):

				cout << test->SIM800A_AT(MyCom, action_1,40) << endl;

				cout << test->SIM800A_AT(MyCom, action_2, 40) << endl;


				/*
				string ReplyMessage = "等待回复......";
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


				cout << "***************************警告*******************************" << endl;
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
		if (!(fileInfo.attrib & _A_SUBDIR))//查找到的不是文件夹才能加一
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
	char file_name[MAX_PATH]; //设置文件名;
	char file_rename[MAX_PATH]; //设置文件重命名后的名字;
	char notify[1024];
	int count = 0; //文件数量。可能同时拷贝、删除多个文件，可以进行更友好的提示;
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

	if (dirHandle == INVALID_HANDLE_VALUE) //若网络重定向或目标文件系统不支持该操作，函数失败，同时调用GetLastError()返回ERROR_INVALID_FUNCTION
	{
		cout << "error" + GetLastError() << endl;
	}

	memset(notify, 0, strlen(notify));
	FILE_NOTIFY_INFORMATION *pnotify = (FILE_NOTIFY_INFORMATION*)notify;  //

	//cout << "开始监控 Start Monitor..." << endl;

	while (1)
	{
		if (ReadDirectoryChangesW(dirHandle, &notify, 1024, true,
			FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_SIZE,
			&cbBytes, NULL, NULL))
		{
			//转换文件名为多字节字符串;
			if (pnotify->FileName)
			{ 
				memset(file_name, 0, strlen(file_name));
				//int nlen;
				//WideCharToMultiByte(CP_ACP, 0, pnotify->FileName, -1, file_name, 99, NULL, NULL);
				WideCharToMultiByte(CP_ACP, 0, pnotify->FileName, pnotify->FileNameLength / sizeof(wchar_t), file_name, 99, NULL, NULL);
				

				//cout << sizeof(pnotify) << endl;
				//cout <<  pnotify->FileNameLength / 2 << endl;
			}


			//获取重命名的文件名;
			if (pnotify->NextEntryOffset != 0 && (pnotify->FileNameLength > 0 && pnotify->FileNameLength < MAX_PATH))
			{

				PFILE_NOTIFY_INFORMATION p = (PFILE_NOTIFY_INFORMATION)((char*)pnotify + pnotify->NextEntryOffset);//结构指针pnotify被强制转换成(char*)(字符指针)

				memset(file_rename, 0, sizeof(file_rename));
				//WideCharToMultiByte(CP_ACP, 0, p->FileName, -1, file_rename, 99, NULL, NULL);
				WideCharToMultiByte(CP_ACP, 0, p->FileName, p->FileNameLength / sizeof(wchar_t), file_rename, 99, NULL, NULL);
			}

			//设置类型过滤器,监听文件创建、更改、删除、重命名等;
			switch (pnotify->Action)
			{
			case FILE_ACTION_ADDED:
				
				/*
				buff = fdir + '\\'+ file_name;
				str_new_mark = buff.rfind('\\', buff.length());
				if(str_old_mark != str_new_mark)//检查文件夹是否发生变动，有变动就运行一次
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
				cout << "文件数量: " << count << endl;
				cout << setw(5) << "动作:新增文件  文件名: " << setw(5) << file_name << endl;
				break;

			case FILE_ACTION_MODIFIED:
				cout << "file modified:" << setw(5) << file_name << endl;
				break;

			case FILE_ACTION_REMOVED:

				/*
				buff = fdir + '\\' + file_name;
				str_new_mark = buff.rfind('\\', buff.length());
				if (str_old_mark != str_new_mark)//检查文件夹是否发生变动，有变动就运行一次
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
