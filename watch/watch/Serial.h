#ifndef SERIAL_H_
#define SERIAL_H_

#include<Windows.h>
#include<string>

class Serial
{
public:
	//析构函数
	~Serial();

	//获取串口对象，单例模式下，每个串口只有一个类对象  饿汉
	static Serial *getInstance()
	{
		static Serial instance;
		return &instance;
	}
	//获取所有串口号
	std::string GetComList_Reg();
	/*打开串口
    char ComNum 串口号*/
	bool openport(char ComNum);
	/*设置串口*/
	bool SetupDCB(int rate_arg);
	/*超时设置*/
	bool SetupTimeOut(DWORD ReadInterval, DWORD ReadTotalMultiplier, DWORD ReadTotalconstant, DWORD WriteTotalMultiplier, DWORD WriteTotalconstant);
	/*初始化串口*/
	char initport(char ComNum, int rate_arg, DWORD ReadInterval, DWORD ReadTotalMultiplier, DWORD ReadTotalconstant, DWORD WriteTotalMultiplier, DWORD WriteTotalconstant);
	int readport(std::string &buff);
	int writeport(BYTE* m_szWriteBuffer, DWORD m_nToSend);

	bool DoesMyComWorking();

	std::string SIM800A_AT(const char comname, const std::string ATorder,WORD read_time_s);
	

	char WhichIsMySerial();
    //功能测试
	void testfunction(std::string phonenum);
	bool closeport();

private:
	//私有构造函数
	Serial();
	//句柄
	HANDLE m_ucom;

	OVERLAPPED m_ov;
	COMSTAT comstat;

	//Serial(const Serial &); //只声明不定义，防止调用隐式复制构造函数
	//Serial & operator= (const Serial &);//只声明不定义，防止调用隐式赋值函数
};   //类后面有分号
#endif
