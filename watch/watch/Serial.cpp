#include"Serial.h"

#include<iostream>
#include<fstream>
#include <tchar.h>
//#include<algorithm>

//���ںŲ�ѯ
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

	if (ERROR_SUCCESS == result)   //   �򿪴���ע���   
	{

		TCHAR portName[0x100], commName[0x100];
		DWORD dwLong, dwSize;

		do
		{
			dwSize = sizeof(portName) / sizeof(TCHAR);//����������������ʵ����0x100����256����
			dwLong = dwSize;
			result = RegEnumValue(hkey, i, portName, &dwLong, NULL, NULL, (LPBYTE)commName, &dwSize);
			//std::cout << " i " << i << "  portName  " << portName << "  dwLong  " << dwLong << "  dwSize  " << dwSize << std::endl;
			if (ERROR_NO_MORE_ITEMS == result)   //ERROR_NO_MORE_ITEMS û�и�����
			{
				//   ö�ٴ���
				break;   //   commName���Ǵ�������
			}
			serialnumber = serialnumber + commName[3];
			i++;
		} while (1);
		RegCloseKey(hkey);
	}
	return serialnumber;
}

//�򿪴���
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
	if (!GetCommState(m_ucom, &dcb))//��ȡ��ǰDCB����
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
//��ʼ��
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
     �ú�����ʼ��һ��ָ����ͨ���豸��ͨ�Ų�����
     BOOL SetupComm(
     HANDLE hFile��
     DWORD dwInQueue��
     DWORD dwOutQueue
     );
	 ����
	 hFile
	 [IN]ͨѶ�豸�����
	 CreateFile�������ش˾����
	 dwInQueue
	 [in]ָ���Ƽ��Ĵ�С�����ֽ�Ϊ��λ�����豸���ڲ����뻺������
	 dwOutQueue
	 [in]ָ���Ƽ��Ĵ�С�����ֽ�Ϊ��λ�����豸���ڲ������������
	 ����ֵ
	 �����ʾ�ɹ������ʾʧ�ܡ� Ҫ��ø��������Ϣ������GetLastError����
	*/
		return 'Q';
	}
	else if(!PurgeComm(m_ucom, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT))
	{
		return 'P';
	 /*PurgeComm()����--��ջ�����
     �ú���ԭ�ͣ�
     BOOL PurgeComm(HANDLE hFile,DWORD dwFlags )
     HANDLE hFile //���ھ��
     DWORD dwFlags // ��Ҫ��ɵĲ���
     ����dwFlagsָ��Ҫ��ɵĲ���������������ֵ����ϣ�
     PURGE_TXABORT ��ֹ�������ڽ��е��ַ��������,���һ�������ڵȴ�״̬���ص�i/o����,��������һ���¼�,ָ�������д����
     PURGE_RXABORT ��ֹ�������ڽ��е��ַ��������,���һ�����ڽ����е��ص�i/o����,�����������õ��ʵ��¼�
     PURGE_TXCLEAR �������ָ���豸����������������������������PURGE_TXABORT �����־һ��ʹ��
     PURGE_RXCLEAR ������������豸��������������뻺������������PURGE_RXABORT �����־һ��ʹ��
	 */
	}
	else
	{
		return '1';
	}
}
//��
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
	//���ReadFile��������FALSE
	{
	if (GetLastError() == ERROR_IO_PENDING)
	//GetLastError()��������ERROR_IO_PENDING,�����������ڽ��ж�����
	{
	WaitForSingleObject(lp->hEvent, 2000);
	//ʹ��WaitForSingleObject�����ȴ���ֱ����������ɻ���ʱ�Ѵﵽ2����
	//�����ڶ�����������Ϻ�m_osRead��hEvent�¼����Ϊ���ź�
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
			/*ReadFile���� ����趨�Ķ�ȡ�ֽڸ������ڻ����е��ֽ������������������޷�ֹͣ
			������ʱ����һ���ֽ�һ���ֽڵĶ�ȡ�Ƚϰ�ȫ����Ȼ��COMSTAT�ṹ�е�cbInQue�������е��ֽ�����
			ͨ��ClearCommError�����õ�COMSTAT�ṹ�����õ�cbInQue�������е��ֽ�������
			���Ǹ��ֽ������ڴ�����˵�ǲ����ŵģ���Ҫ��ȡ���ֽ����趨ΪcbInQue�����ǻ������������

			����ClearCommError��Ҫѭ������һ��ʱ�䣬comstat.cbInQue��ֵ�������ģ�

			���磺���ڴ���60���ֽڣ���ȡѭ��100
			������100��ѭ���е�comstat.cbInQueֵ
			0000000000000000000000000000001      *
			0000000000000000001
			00000000000000001
			000000000000000001
			0000000000000000001
			0receive over                        *ѭ��������־
			pleas                                *60���ֽ�ֻ�յ�5��

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
//д
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
				BytesSent = 0;  //д���ֽ�����0
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
	return BytesSent;//д���ֽ���
}

//�����е��Դ��ڣ���ͨ�ţ��ҵ����贮�ں󣬱��ִ򿪣����û���ҵ����ر����д���
char Serial::WhichIsMySerial()
{
	using namespace std;
	string SNum = "";
	SNum = GetComList_Reg();

	char rcombuff;//com�ڼ�¼

	unsigned int loopi;
	for (loopi = 0; loopi < SNum.length(); loopi++)
	{
		//���ڳ�ʼ��
		if (initport(SNum[loopi],115200,0,0,0,0,0) != '1')
		{
			return 'i';
			break;
		}
		else
		{
			rcombuff = SNum[loopi];
		}
		//stringתchar����
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
		
		//д��AT,�ж�ģ��״̬
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
//ʹ��ǰ�᣺�����Ѵ� 
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

	//д��AT,�ж�ģ��״̬
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

//ʹ��ǰ�᣺��ȷ�Ĵ����Ѵ� ���ڲ�ȡ���˴򿪺͹رմ���,��ʼ����û�йرգ�
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

	//delete[] cbuff;//����
	//closeport();
}

void Serial::testfunction(std::string phonenum)
{
	char mycom = WhichIsMySerial();
	if (mycom == 'E' || mycom == 'n')
	{
		std::cout << "�����豸��������,����......" << std::endl;
	}
	else
	{
		std::cout << SIM800A_AT(mycom, "AT\r",1)<< std::endl;
		//std::cout << SIM800A_AT(mycom, "AT+CSQ\r",1) << std::endl;
		std::cout << SIM800A_AT(mycom, phonenum,30) << std::endl;
		closeport();
	}
}

//��
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

//��������
Serial::~Serial()
{

}

//Serial* Serial::instance = new Serial;