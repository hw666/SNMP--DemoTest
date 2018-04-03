#include "scanhost.h"
//头文件

WSADATA wsaData;
SOCKET sockRaw;             //原始套接字
SOCKADDR_IN dest, from, End;
//dest：搜索目的IP，
//from：接收ICMP包的源IP
//end：搜索终止IP。
int IP1[4], IP2[4];
string Scanhost::result;

int fromlen = sizeof(from);                      //接收ICMP包长度
char *recvbuf = new char[MAX_PING_PACKET_SIZE];  //接受ICMP包缓冲区
unsigned int addr = 0;                           //IP地址
long ThreadNumCounter = 0, ThreadNumLimit = 20;     //线程数及最大允许线程数
long *aa = &ThreadNumCounter;					//全局变量的申明

Scanhost::Scanhost() {
}

Scanhost::~Scanhost() {
}

void fill_icmp_data(char *icmp_data, int datasize)
{
	IcmpHeader *icmp_hdr;
	char *datapart;
	icmp_hdr = (IcmpHeader*)icmp_data;
	icmp_hdr->type = ICMP_ECHO;//设置类型信息
	icmp_hdr->id = (USHORT)GetCurrentThreadId();//设置其ID号为当前线程ID号
	datapart = icmp_data + sizeof(IcmpHeader);//计算ICMP数据报的数据部分
	memset(datapart, 'A', datasize - sizeof(IcmpHeader));//填入数据
}
//ICMP数据包的填充
void decode_resp(char *buf, int bytes, struct sockaddr_in *from)
{
	IpHeader *iphdr;
	IcmpHeader *icmphdr;
	unsigned short iphdrlen;
	iphdr = (IpHeader *)buf;
	//跳过IP包头部
	iphdrlen = iphdr->headlen * 4;
	icmphdr = (IcmpHeader *)(buf + iphdrlen);


	if (bytes<iphdrlen + ICMP_MIN)
		return; //数据包太短，丢弃
	if (icmphdr->type != ICMP_ECHO_REPLY)
		return;   //不是会送相应、丢弃
	if (icmphdr->id != (USHORT)GetCurrentThreadId())
		return;  //ID号不符
	//cout << "活动主机：" << inet_ntoa(from->sin_addr) << endl;
	string str1(inet_ntoa(from->sin_addr));
	Scanhost::result = Scanhost::result + str1 + ";";
	//cout << Scanhost::result << endl;
}
//返回包的解析，以及输出
USHORT checksum(USHORT *buffer, int size)
{
	unsigned long cksum = 0;
	while (size>1)
	{
		cksum += *buffer++;
		size -= sizeof(USHORT);
	}
	if (size)
	{
		cksum += *(UCHAR*)buffer;
	}
	cksum = (cksum >> 16) + (cksum & 0xffff);
	cksum += (cksum >> 16);
	return (USHORT)(~cksum);
}
//效验和的计算
DWORD WINAPI FindIP(LPVOID pIPAddrTemp)
{
	InterlockedIncrement(aa);//线程数目+1
	char icmp_data[MAX_PACKET];
	memset(icmp_data, 0, MAX_PACKET);//数据报初始化
	int datasize = DEF_PACKET_SIZE;//数据报报文的缺省长度
	datasize += sizeof(IcmpHeader);//加上icmp头部长度
	fill_icmp_data(icmp_data, datasize);//填充包
	((IcmpHeader*)icmp_data)->checksum = 0;//效验和置零
	((IcmpHeader*)icmp_data)->seq = 0;//序列号置零
	((IcmpHeader*)icmp_data)->checksum = checksum((USHORT*)icmp_data, datasize);
	//计算效验和后填人
	int bwrote = sendto(sockRaw, icmp_data, datasize, 0, (struct sockaddr*)pIPAddrTemp, sizeof(dest));
	//发送数据报
	int n = 0;
	if (bwrote == SOCKET_ERROR)
	{
		if (WSAGetLastError() == WSAETIMEDOUT)
		{
			cout << "timed out" << endl;
		}
		cout << "sendto failed:" << WSAGetLastError() << endl;
		ExitProcess(STATUS_FAILED);
		n = 1;
	}
	if (WSAGetLastError() == WSAETIMEDOUT)
	{
		cout << "timed out" << endl;
		ExitProcess(STATUS_FAILED);
		n = 1;
	}
	if (bwrote<datasize)
	{
		cout << "Wrote" << bwrote << "bytes" << endl;
		ExitProcess(STATUS_FAILED);
		n = 1;
	}
	int bread = recvfrom(sockRaw, recvbuf, MAX_PING_PACKET_SIZE, 0, (struct sockaddr*)&from, &fromlen);
	//数据包的接收
	if (bread == SOCKET_ERROR)
	{
		if (WSAGetLastError() == WSAETIMEDOUT)
		{
			cout << "timed out" << endl;
		}
		cout << "recvfrom failed:" << WSAGetLastError() << endl;
		ExitProcess(STATUS_FAILED);
		n = 1;
	}
	if (n == 0)
		decode_resp(recvbuf, bread, &from);
	InterlockedDecrement(aa);
	return 0;
}

void Scanhost::setIP1(int a, int b, int c, int d) {
	IP1[0] = a;
	IP1[1] = b;
	IP1[2] = c;
	IP1[3] = d;
}

void Scanhost::setIP2(int a, int b, int c, int d) {
	IP2[0] = a;
	IP2[1] = b;
	IP2[2] = c;
	IP2[3] = d;
}

string CharArrayToIPString(int a, int b, int c, int d) {
	string str = to_string(a) + '.' + to_string(b) + '.' + to_string(c) + '.' + to_string(d);
	return str;
}

void Scanhost::make()
{
	if (WSAStartup(MAKEWORD(2, 1), &wsaData) != 0)
	{
		cout << "WSAStartup failed:" << GetLastError() << endl;
		ExitProcess(STATUS_FAILED);
	}
	//创建原始套接字
	sockRaw = WSASocket(AF_INET, SOCK_RAW, IPPROTO_ICMP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (sockRaw == INVALID_SOCKET)
	{
		cout << "WSASocket() failed:" << WSAGetLastError() << endl;
		ExitProcess(STATUS_FAILED);
	}
	int timeout = 1000;
	int bread = setsockopt(sockRaw, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
	if (bread == SOCKET_ERROR)
	{
		cout << "failed to set recv timeou:" << WSAGetLastError() << endl;
		ExitProcess(STATUS_FAILED);
	}
	timeout = 1000;
	bread = setsockopt(sockRaw, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout));
	if (bread == SOCKET_ERROR)
	{
		cout << "failed to set send timeout:" << WSAGetLastError() << endl;
		ExitProcess(STATUS_FAILED);
	}
	memset(&dest, 0, sizeof(dest)); //初始化dest结构
	memset(&End, 0, sizeof(End)); //初始化End结构

	//setIP1(192, 168, 1, 0); //初始化两个IP
	//setIP2(192, 168, 2, 255);

	//setIP1(10, 170, 16, 0); //初始化两个IP
	//setIP2(10, 170, 16, 255);

	string str1 = CharArrayToIPString(IP1[0], IP1[1], IP1[2], IP1[3]);
	string str2 = CharArrayToIPString(IP2[0], IP2[1], IP2[2], IP2[3]);

	unsigned long startIP, endIP;
	dest.sin_family = AF_INET;
	dest.sin_addr.s_addr = inet_addr(str1.c_str());  //填入开始搜索IP
	startIP = inet_addr(str1.c_str());
	End.sin_family = AF_INET;
	End.sin_addr.s_addr = inet_addr(str2.c_str());
	endIP = inet_addr(str2.c_str());    //填入结束搜索IP地址

	HANDLE hThread;
	while (htonl(startIP) <= htonl(endIP))   //起始IP比结束IP小
	{
		if (ThreadNumCounter>ThreadNumLimit) //判断线程数目，如果太多，休眠
		{
			Sleep(5000);
			continue;
		}
		DWORD ThreadID;
		sockaddr_in * pIPAddrTemp = new(sockaddr_in);
		if (!pIPAddrTemp)
		{
			cout << "memory alloc failed" << endl;
			//getchar();
			return;
		}
		*pIPAddrTemp = dest;
		//创建新进程
		clock_t start;
		start = clock();
		hThread = CreateThread(NULL, NULL, FindIP, (LPVOID)pIPAddrTemp, NULL, &ThreadID);
		long i = 60000000L;
		while (i--)
			;
		TerminateThread(hThread, 0);
		InterlockedDecrement(aa);
		memset(&from, 0, sizeof(from));
		startIP = htonl(htonl(startIP) + 1);
		dest.sin_addr.s_addr = startIP;
	}
	while (ThreadNumCounter != 0)
	{
		Sleep(2000);
		//getchar();
		return;
	}
}
