#ifndef SERIAL_H_
#define SERIAL_H_

#include<Windows.h>
#include<string>

class Serial
{
public:
	//��������
	~Serial();

	//��ȡ���ڶ��󣬵���ģʽ�£�ÿ������ֻ��һ�������  ����
	static Serial *getInstance()
	{
		static Serial instance;
		return &instance;
	}
	//��ȡ���д��ں�
	std::string GetComList_Reg();
	/*�򿪴���
    char ComNum ���ں�*/
	bool openport(char ComNum);
	/*���ô���*/
	bool SetupDCB(int rate_arg);
	/*��ʱ����*/
	bool SetupTimeOut(DWORD ReadInterval, DWORD ReadTotalMultiplier, DWORD ReadTotalconstant, DWORD WriteTotalMultiplier, DWORD WriteTotalconstant);
	/*��ʼ������*/
	char initport(char ComNum, int rate_arg, DWORD ReadInterval, DWORD ReadTotalMultiplier, DWORD ReadTotalconstant, DWORD WriteTotalMultiplier, DWORD WriteTotalconstant);
	int readport(std::string &buff);
	int writeport(BYTE* m_szWriteBuffer, DWORD m_nToSend);

	bool DoesMyComWorking();

	std::string SIM800A_AT(const char comname, const std::string ATorder,WORD read_time_s);
	

	char WhichIsMySerial();
    //���ܲ���
	void testfunction(std::string phonenum);
	bool closeport();

private:
	//˽�й��캯��
	Serial();
	//���
	HANDLE m_ucom;

	OVERLAPPED m_ov;
	COMSTAT comstat;

	//Serial(const Serial &); //ֻ���������壬��ֹ������ʽ���ƹ��캯��
	//Serial & operator= (const Serial &);//ֻ���������壬��ֹ������ʽ��ֵ����
};   //������зֺ�
#endif
