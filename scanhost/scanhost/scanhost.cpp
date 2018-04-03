#include "scanhost.h"
//ͷ�ļ�

WSADATA wsaData;
SOCKET sockRaw;             //ԭʼ�׽���
SOCKADDR_IN dest, from, End;
//dest������Ŀ��IP��
//from������ICMP����ԴIP
//end��������ֹIP��
int IP1[4], IP2[4];
string Scanhost::result;

int fromlen = sizeof(from);                      //����ICMP������
char *recvbuf = new char[MAX_PING_PACKET_SIZE];  //����ICMP��������
unsigned int addr = 0;                           //IP��ַ
long ThreadNumCounter = 0, ThreadNumLimit = 20;     //�߳�������������߳���
long *aa = &ThreadNumCounter;					//ȫ�ֱ���������

Scanhost::Scanhost() {
}

Scanhost::~Scanhost() {
}

void fill_icmp_data(char *icmp_data, int datasize)
{
	IcmpHeader *icmp_hdr;
	char *datapart;
	icmp_hdr = (IcmpHeader*)icmp_data;
	icmp_hdr->type = ICMP_ECHO;//����������Ϣ
	icmp_hdr->id = (USHORT)GetCurrentThreadId();//������ID��Ϊ��ǰ�߳�ID��
	datapart = icmp_data + sizeof(IcmpHeader);//����ICMP���ݱ������ݲ���
	memset(datapart, 'A', datasize - sizeof(IcmpHeader));//��������
}
//ICMP���ݰ������
void decode_resp(char *buf, int bytes, struct sockaddr_in *from)
{
	IpHeader *iphdr;
	IcmpHeader *icmphdr;
	unsigned short iphdrlen;
	iphdr = (IpHeader *)buf;
	//����IP��ͷ��
	iphdrlen = iphdr->headlen * 4;
	icmphdr = (IcmpHeader *)(buf + iphdrlen);


	if (bytes<iphdrlen + ICMP_MIN)
		return; //���ݰ�̫�̣�����
	if (icmphdr->type != ICMP_ECHO_REPLY)
		return;   //���ǻ�����Ӧ������
	if (icmphdr->id != (USHORT)GetCurrentThreadId())
		return;  //ID�Ų���
	//cout << "�������" << inet_ntoa(from->sin_addr) << endl;
	string str1(inet_ntoa(from->sin_addr));
	Scanhost::result = Scanhost::result + str1 + ";";
	//cout << Scanhost::result << endl;
}
//���ذ��Ľ������Լ����
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
//Ч��͵ļ���
DWORD WINAPI FindIP(LPVOID pIPAddrTemp)
{
	InterlockedIncrement(aa);//�߳���Ŀ+1
	char icmp_data[MAX_PACKET];
	memset(icmp_data, 0, MAX_PACKET);//���ݱ���ʼ��
	int datasize = DEF_PACKET_SIZE;//���ݱ����ĵ�ȱʡ����
	datasize += sizeof(IcmpHeader);//����icmpͷ������
	fill_icmp_data(icmp_data, datasize);//����
	((IcmpHeader*)icmp_data)->checksum = 0;//Ч�������
	((IcmpHeader*)icmp_data)->seq = 0;//���к�����
	((IcmpHeader*)icmp_data)->checksum = checksum((USHORT*)icmp_data, datasize);
	//����Ч��ͺ�����
	int bwrote = sendto(sockRaw, icmp_data, datasize, 0, (struct sockaddr*)pIPAddrTemp, sizeof(dest));
	//�������ݱ�
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
	//���ݰ��Ľ���
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
	//����ԭʼ�׽���
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
	memset(&dest, 0, sizeof(dest)); //��ʼ��dest�ṹ
	memset(&End, 0, sizeof(End)); //��ʼ��End�ṹ

	//setIP1(192, 168, 1, 0); //��ʼ������IP
	//setIP2(192, 168, 2, 255);

	//setIP1(10, 170, 16, 0); //��ʼ������IP
	//setIP2(10, 170, 16, 255);

	string str1 = CharArrayToIPString(IP1[0], IP1[1], IP1[2], IP1[3]);
	string str2 = CharArrayToIPString(IP2[0], IP2[1], IP2[2], IP2[3]);

	unsigned long startIP, endIP;
	dest.sin_family = AF_INET;
	dest.sin_addr.s_addr = inet_addr(str1.c_str());  //���뿪ʼ����IP
	startIP = inet_addr(str1.c_str());
	End.sin_family = AF_INET;
	End.sin_addr.s_addr = inet_addr(str2.c_str());
	endIP = inet_addr(str2.c_str());    //�����������IP��ַ

	HANDLE hThread;
	while (htonl(startIP) <= htonl(endIP))   //��ʼIP�Ƚ���IPС
	{
		if (ThreadNumCounter>ThreadNumLimit) //�ж��߳���Ŀ�����̫�࣬����
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
		//�����½���
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
