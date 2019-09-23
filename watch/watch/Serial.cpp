#include"Serial.h"

#include<iostream>
#include<fstream>
#include <tchar.h>
//#include<algorithm>

//串口号查询
std::string Serial::GetComList_Reg()
{
	HKEY hkey;
	int result;
	int i = 0;
	result = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
		_T("Hardware\\DeviceMap\\SerialComm"),
		NULL,
		KEY_READ,
		&hkey);

	std::string serialnumber = "";

	if (ERROR_SUCCESS == result)   //   打开串口注册表   
	{

		TCHAR portName[0x100], commName[0x100];
		DWORD dwLong, dwSize;

		do
		{
			dwSize = sizeof(portName) / sizeof(TCHAR);//定义的数组个数，其实就是0x100个（256个）
			dwLong = dwSize;
			result = RegEnumValue(hkey, i, portName, &dwLong, NULL, NULL, (LPBYTE)commName, &dwSize);
			//std::cout << " i " << i << "  portName  " << portName << "  dwLong  " << dwLong << "  dwSize  " << dwSize << std::endl;
			if (ERROR_NO_MORE_ITEMS == result)   //ERROR_NO_MORE_ITEMS 没有更多项
			{
				//   枚举串口
				break;   //   commName就是串口名字
			}
			serialnumber = serialnumber + commName[3];
			i++;
		} while (1);
		RegCloseKey(hkey);
	}
	return serialnumber;
}

//打开串口
bool Serial::openport(char ComNum)
{
	if (m_ucom != INVALID_HANDLE_VALUE)
	{
		return true;
	}

	std::string prefixname = "\\\\.\\COM";
	std::string port_name = prefixname + ComNum;

	//std::cout << port_name << std::endl;

	m_ucom = CreateFileA(port_name.c_str(), 
		                 GENERIC_READ | GENERIC_WRITE, 
		                 0, 
		                 NULL, OPEN_EXISTING, 
		                 FILE_FLAG_OVERLAPPED, 
		                 NULL);

	if (m_ucom == INVALID_HANDLE_VALUE)
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool Serial::SetupDCB(int rate_arg)
{
	DCB  dcb;
	int rate = rate_arg;
	memset(&dcb, 0, sizeof(dcb));
	if (!GetCommState(m_ucom, &dcb))//获取当前DCB配置
	{
		return false;
	}
	/* -------------------------------------------------------------------- */
	// set DCB to configure the serial port
	dcb.DCBlength = sizeof(dcb);
	/* ---------- Serial Port Config ------- */
	dcb.BaudRate = rate;
	dcb.Parity = NOPARITY;
	dcb.fParity = 0;
	dcb.StopBits = ONESTOPBIT;
	dcb.ByteSize = 8;
	dcb.fOutxCtsFlow = 0;
	dcb.fOutxDsrFlow = 0;
	dcb.fDtrControl = DTR_CONTROL_DISABLE;
	dcb.fDsrSensitivity = 0;
	dcb.fRtsControl = RTS_CONTROL_DISABLE;
	dcb.fOutX = 0;
	dcb.fInX = 0;
	/* ----------------- misc parameters ----- */
	dcb.fErrorChar = 0;
	dcb.fBinary = 1;
	dcb.fNull = 0;
	dcb.fAbortOnError = 0;
	dcb.wReserved = 0;
	dcb.XonLim = 2;
	dcb.XoffLim = 4;
	dcb.XonChar = 0x13;
	dcb.XoffChar = 0x19;
	dcb.EvtChar = 0;
	/* -------------------------------------------------------------------- */
	// set DCB
	if (!SetCommState(m_ucom, &dcb))
	{
		return false;
	}
	else
		return true;
}

bool Serial::SetupTimeOut(DWORD ReadInterval, DWORD ReadTotalMultiplier, DWORD ReadTotalconstant, DWORD WriteTotalMultiplier, DWORD WriteTotalconstant)
{
	COMMTIMEOUTS timeouts;
	timeouts.ReadIntervalTimeout = ReadInterval;
	timeouts.ReadTotalTimeoutConstant = ReadTotalconstant;
	timeouts.ReadTotalTimeoutMultiplier = ReadTotalMultiplier;
	timeouts.WriteTotalTimeoutConstant = WriteTotalconstant;
	timeouts.WriteTotalTimeoutMultiplier = WriteTotalMultiplier;
	if (!SetCommTimeouts(m_ucom, &timeouts))
	{
		return false;
	}
	else
		return true;
}
//初始化
char Serial::initport(char ComNum, int rate_arg, DWORD ReadInterval, DWORD ReadTotalMultiplier, DWORD ReadTotalconstant, DWORD WriteTotalMultiplier, DWORD WriteTotalconstant)
{

	if (!openport(ComNum))
	{
		return 'C';
	}
	else if(!SetupDCB(rate_arg))
	{
		return 'D';
	}
	else if (!SetupTimeOut(ReadInterval, ReadTotalMultiplier, ReadTotalconstant, WriteTotalMultiplier, WriteTotalconstant))
	{
		return 'T';
	}
	else if (!SetupComm(m_ucom, 1024, 1024))
	{	
		/*SetupComm
     该函数初始化一个指定的通信设备的通信参数。
     BOOL SetupComm(
     HANDLE hFile，
     DWORD dwInQueue，
     DWORD dwOutQueue
     );
	 参数
	 hFile
	 [IN]通讯设备句柄。
	 CreateFile函数返回此句柄。
	 dwInQueue
	 [in]指定推荐的大小，以字节为单位，对设备的内部输入缓冲区。
	 dwOutQueue
	 [in]指定推荐的大小，以字节为单位，对设备的内部输出缓冲区。
	 返回值
	 非零表示成功。零表示失败。 要获得更多错误信息，调用GetLastError函数
	*/
		return 'Q';
	}
	else if(!PurgeComm(m_ucom, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT))
	{
		return 'P';
	 /*PurgeComm()函数--清空缓冲区
     该函数原型：
     BOOL PurgeComm(HANDLE hFile,DWORD dwFlags )
     HANDLE hFile //串口句柄
     DWORD dwFlags // 需要完成的操作
     参数dwFlags指定要完成的操作，可以是下列值的组合：
     PURGE_TXABORT 终止所有正在进行的字符输出操作,完成一个正处于等待状态的重叠i/o操作,他将产生一个事件,指明完成了写操作
     PURGE_RXABORT 终止所有正在进行的字符输入操作,完成一个正在进行中的重叠i/o操作,并带有已设置得适当事件
     PURGE_TXCLEAR 这个命令指导设备驱动程序清除输出缓冲区，经常与PURGE_TXABORT 命令标志一起使用
     PURGE_RXCLEAR 这个命令用于设备驱动程序清除输入缓冲区，经常与PURGE_RXABORT 命令标志一起使用
	 */
	}
	else
	{
		return '1';
	}
}
//读
int Serial::readport(std::string &buff)
{
	/*
	DWORD dwErrorFlags = 0;
	COMSTAT ComStat;
	lp->hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	ClearCommError(m_ucom, &dwErrorFlags, &ComStat);
	if (!ComStat.cbInQue) return 0;
	size = min(size, (DWORD)ComStat.cbInQue);



	bool readresult;
	readresult = ReadFile(m_ucom, buff, size, &dwReadBytes, lp);

	if (!readresult)
	//如果ReadFile函数返回FALSE
	{
	if (GetLastError() == ERROR_IO_PENDING)
	//GetLastError()函数返回ERROR_IO_PENDING,表明串口正在进行读操作
	{
	WaitForSingleObject(lp->hEvent, 2000);
	//使用WaitForSingleObject函数等待，直到读操作完成或延时已达到2秒钟
	//当串口读操作进行完毕后，m_osRead的hEvent事件会变为有信号
	PurgeComm(m_ucom, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
	return dwReadBytes;
	}
	return 0;
	}
	*/
	bool bRead = true;
	bool bResult = true;
	bool bloop = true;
	unsigned int loopbuff = 2000;
	DWORD dwError = 0;
	DWORD BytesRead = 0;
	char RXBuff;
	while (bloop)
	{

		bResult = ClearCommError(m_ucom, &dwError, &comstat);
		//std::cout << comstat.cbInQue << std::endl;
		if (comstat.cbInQue == 0)
		{
			loopbuff--;
			if (!loopbuff)
			{
				break;
			}
			continue;
		}
		if (bRead)
		{
			/*ReadFile函数 如果设定的读取字节个数大于缓存中的字节数，函数会阻塞，无法停止
			读串口时还是一个字节一个字节的读取比较安全，虽然有COMSTAT结构中的cbInQue（缓存中的字节数）
			通过ClearCommError函数得到COMSTAT结构，即得到cbInQue（缓存中的字节数），
			但是该字节数对于串口来说是不可信的，将要读取的字节数设定为cbInQue，还是会出现阻塞现象

			并且ClearCommError需要循环进行一段时间，comstat.cbInQue的值是跳动的，

			例如：串口传来60个字节，读取循环100
			下面是100次循环中的comstat.cbInQue值
			0000000000000000000000000000001      *
			0000000000000000001
			00000000000000001
			000000000000000001
			0000000000000000001
			0receive over                        *循环结束标志
			pleas                                *60个字节只收到5个

			size = min(size, (DWORD)comstat.cbInQue);
			std::cout << size << std::endl;
			*/
			bResult = ReadFile(m_ucom,	                // Handle to COMM port
				               &RXBuff,	                // RX Buffer Pointer
				               1,	                    // Read one byte
				               &BytesRead,	            // Stores number of bytes read
				               &m_ov);   	            // pointer to the m_ov structure	

			buff += RXBuff;   //

			if (!bResult)
			{
				switch (dwError = GetLastError())
				{
				case ERROR_IO_PENDING:
				{
					bRead = false;
					break;
				}
				default:
				{
					break;
				}
				}
			}
			else
			{
				bRead = true;
			}
		}  // close if (bRead)
		if (!bRead)
		{
			bRead = true;
			bResult = GetOverlappedResult(m_ucom,	    // Handle to COMM port
				                          &m_ov,	    // Overlapped structure
				                          &BytesRead,	// Stores number of bytes read
				                          true); 	    // Wait flag
		}
	}
	return buff.length();
}
//写
int Serial::writeport(BYTE* WriteBuffer, DWORD SendNum)
{
	
	BOOL bWrite = TRUE;
	BOOL bResult = TRUE;
	DWORD BytesSent = 0;
	HANDLE	m_hWriteEvent = 0;
	ResetEvent(m_hWriteEvent);
	if (bWrite)
	{
		m_ov.Offset = 0;
		m_ov.OffsetHigh = 0;
		// Clear buffer
		bResult = WriteFile(m_ucom,	// Handle to COMM Port
			WriteBuffer,	// Pointer to message buffer in calling finction
			SendNum,	// Length of message to send
			&BytesSent,	// Where to store the number of bytes sent
			&m_ov);	// Overlapped structure
		if (!bResult)
		{
			DWORD dwError = GetLastError();
			switch (dwError)
			{
			case ERROR_IO_PENDING:
			{
				// continue to GetOverlappedResults()
				BytesSent = 0;  //写入字节数置0
				bWrite = false;
				break;
			}
			default:
			{
				// all other error codes
				break;
			}
			}
		}
	} // end if(bWrite)
	if (!bWrite)
	{

		bWrite = TRUE;
		bResult = GetOverlappedResult(m_ucom,	// Handle to COMM port
			                          &m_ov,	// Overlapped structure
			                          &BytesSent,	// Stores number of bytes sent
			                          TRUE); 	// Wait flag
					//WaitForSingleObject(m_ov.hEvent,10000);
					//return m_nToSend;
					// deal with the error code
		if (!bResult)
		{
			printf("GetOverlappedResults() in WriteFile()");
		}

	} // end if (!bWrite)
	  // Verify that the data size send equals what we tried to send	
	return BytesSent;//写入字节数
}

//打开所有电脑串口，并通信，找到所需串口后，保持打开，如果没有找到，关闭所有串口
char Serial::WhichIsMySerial()
{
	using namespace std;
	string SNum = "";
	SNum = GetComList_Reg();

	char rcombuff;//com口记录

	unsigned int loopi;
	for (loopi = 0; loopi < SNum.length(); loopi++)
	{
		//串口初始化
		if (initport(SNum[loopi],115200,0,0,0,0,0) != '1')
		{
			return 'i';
			break;
		}
		else
		{
			rcombuff = SNum[loopi];
		}
		//string转char数组
		//string a1 = "AT\r";
		//string a2 = "AT+GSV\r";
		//string a3 = "AT+IPR?\r";
		
		string wchbuff = "AT\r";

		char *cbuff = new char[wchbuff.length()];
		unsigned int i = 0;
		for (i = 0; i < wchbuff.length(); i++)
		{
			cbuff[i] = wchbuff[i];
		}
		
		//写入AT,判断模块状态
		writeport((BYTE*)cbuff, wchbuff.length());

		string rch = "";
		readport(rch);
		//test
		//std::cout << rch << std::endl;
		if(rch.length()>3)
		{ 
		    for (unsigned int i = 0; i < rch.length(); i++)
		    {
				if (rch[i] == 'O' && rch[i + 1] == 'K')
			    {
					if (rch[i + 1] == 'K')
					{
						delete[] cbuff;
						return SNum[loopi];
					}
			    }
				if (rch[i] == 'E' && rch[i + 1] == 'R' && rch[i + 2] == 'R' && rch[i + 3] == 'O' && rch[i + 4] == 'R')
				{
					delete[] cbuff;
					closeport();
					return 'E';
				}

		    }
		}
		else
		{
			continue;
		}
		delete[] cbuff;
	}
	closeport();
	return 'n';
	
}
//使用前提：串口已打开 
bool Serial::DoesMyComWorking()
{
	using namespace std;
	string wchbuff = "AT\r";

	char *cbuff = new char[wchbuff.length()];
	unsigned int i = 0;
	for (i = 0; i < wchbuff.length(); i++)
	{
		cbuff[i] = wchbuff[i];
	}

	//写入AT,判断模块状态
	writeport((BYTE*)cbuff, wchbuff.length());

	string rch = "";
	readport(rch);
	//test
	//std::cout << rch << std::endl;
	if (rch.length() > 5)
	{
		for (unsigned int i = 0; i < rch.length(); i++)
		{
			if (rch[i] == 'O' && rch[i + 1] == 'K')
			{
				if (rch[i + 1] == 'K')
				{
					delete[] cbuff;
					return true;
				}
			}
			if (rch[i] == 'E' && rch[i + 1] == 'R' && rch[i + 2] == 'R' && rch[i + 3] == 'O' && rch[i + 4] == 'R')
			{
				delete[] cbuff;
				return false;
			}

		}
	}
}

//使用前提：正确的串口已打开 （内部取消了打开和关闭串口,初始化中没有关闭）
std::string Serial::SIM800A_AT(const char comname, const std::string ATorder,WORD read_time_s)
{
	//initport(comname,115200, 0, 0, 0, 0, 0);
	char *cbuff = new char[ATorder.length()];
	unsigned int i = 0;
	for (i = 0; i < ATorder.length(); i++)
	{
		cbuff[i] = ATorder[i];
	}
	//AT
	if (!writeport((BYTE*)cbuff, ATorder.length()))
	{
		delete[] cbuff;
		return "false";
	}

	std::string rch = "";
	std::string sumrch = "";
	//test
	//std::cout << 'q' << std::endl;
	//std::cout << readport(rch)<<std::endl;

	;
	while (1)
	{
		if (readport(rch))
		{
			//std::cout << rch << std::endl;
			if(sumrch != rch)
			sumrch = rch;
		}
		else
		{
			delete[] cbuff;
			return "false";
		}
		Sleep(1 * 1000);
		read_time_s--;
		if (read_time_s <= 0)
		{
			delete[] cbuff;
			return sumrch;
		}
	}

	//delete[] cbuff;//清理
	//closeport();
}

void Serial::testfunction(std::string phonenum)
{
	char mycom = WhichIsMySerial();
	if (mycom == 'E' || mycom == 'n')
	{
		std::cout << "串口设备出现问题,请检查......" << std::endl;
	}
	else
	{
		std::cout << SIM800A_AT(mycom, "AT\r",1)<< std::endl;
		//std::cout << SIM800A_AT(mycom, "AT+CSQ\r",1) << std::endl;
		std::cout << SIM800A_AT(mycom, phonenum,30) << std::endl;
		closeport();
	}
}

//关
bool Serial::closeport()
{
	if (m_ucom != INVALID_HANDLE_VALUE)
	{
		return CloseHandle(m_ucom);
	}
	else
	{
		return true;
	}
}
//  ?
Serial::Serial() :m_ucom(INVALID_HANDLE_VALUE)
{

}

//Serial::Serial(const Serial &)
//{

//}
/*
Serial & Serial::operator= (const Serial &)
{

}
*/

//析构函数
Serial::~Serial()
{

}

//Serial* Serial::instance = new Serial;