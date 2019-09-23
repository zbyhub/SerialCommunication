#include"Serial.h"
#include<iostream>
#include<stdio.h>
#include<iostream>
#include<fstream>

int main()
{
	using namespace std;
	Serial *test = Serial::getInstance();
	Serial *test1 = Serial::getInstance();

	cout << test << endl;
	cout << test1 << endl;

	string zby1 = "ATD17615182814;\r";
	string zby2 = "ATD18840844369;\r";

	string GSV = "AT+GSV\r";//显示模块制造商，名称和版本信息
	string CSQ = "AT+CSQ\r";//
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

	char MyCom;
	MyCom = test->WhichIsMySerial();
	cout << MyCom << endl;
	
	if (MyCom != 'n' && MyCom != 'i')
	{ 
		cout << test->SIM800A_AT(MyCom, GSV) << endl;
		cout << test->SIM800A_AT(MyCom, CSQ) << endl;
	}
	else
	{
		cout << '0' << endl;
	}

	cin.get();
	cin.get();
	return 1;
}