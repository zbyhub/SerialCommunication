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

	string GSV = "AT+GSV\r";//��ʾģ�������̣����ƺͰ汾��Ϣ
	string CSQ = "AT+CSQ\r";//
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