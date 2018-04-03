#include "MyTrace.h"
string TraceRT::route;

TraceRT::TraceRT() {
}

TraceRT::~TraceRT() {
}

void TraceRT::tracert(char* ip)
{
	//初始化Windows Socket DLL
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		cout << "Initialize Windows Socket DLL Error!" << endl;
		cout << "Error code: " << WSAGetLastError() << endl;
		exit(0);
	}

	u_long des_ip = inet_addr(ip);//目的主机ip地址
	if (des_ip == INADDR_NONE)//如果输入的是域名
	{
		//进行域名到ip的转换
		hostent* host = gethostbyname(ip);
		if (host)
		{
			des_ip = (*(in_addr*)host->h_addr).s_addr;
			cout << "Tracing route to " << ip << " [" << inet_ntoa(*(in_addr*)(&des_ip)) << "] " << " ";
		}
		else //解析主机名失败
		{
			cout << "Can not decode the host name." << endl;
			WSACleanup();
			exit(0);
		}
	}
	else
		cout << "Tracing route to " <<ip<< " ";

	cout << "over a maximum of " << MAX_HOP << " hops" << endl;

	sockaddr_in des_sock_add;//存储目标地址
	ZeroMemory(&des_sock_add, sizeof(des_sock_add));
	des_sock_add.sin_family = AF_INET;
	des_sock_add.sin_addr.s_addr = des_ip;

	//创建socket
	SOCKET my_socket = WSASocket(AF_INET, SOCK_RAW, IPPROTO_ICMP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (my_socket == INVALID_SOCKET)
	{
		cout << "Set up the socket error!" << endl;
		cout << "Error Code：" << WSAGetLastError() << endl;
		exit(0);
	}

	int time_out = DEF_ICMP_TIMEOUT;//超时时间

	//对socket设置接受和发送的最大时间
	if (setsockopt(my_socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&time_out, sizeof(time_out)) == SOCKET_ERROR)
	{
		cout << "Set the rectimeout error!" << endl;
		closesocket(my_socket);
		WSACleanup();
		exit(0);
	}
	if (setsockopt(my_socket, SOL_SOCKET, SO_SNDTIMEO, (char*)&time_out, sizeof(time_out)) == SOCKET_ERROR)
	{
		cout << "Set the sndtimeout error!" << endl;
		closesocket(my_socket);
		WSACleanup();
		exit(0);
	}

	char send_buffer[sizeof(ICMP_HEADER) + DEF_ICMP_DATA_SIZE];//发送数据缓存
	char reveive_buffer[MAX_ICMP_PACKET_SIZE];//接受数据缓存
	//初始化，将缓存全部置零
	memset(send_buffer, 0, sizeof(send_buffer));
	memset(reveive_buffer, 0, sizeof(reveive_buffer));

	ICMP_HEADER* send_icmp_hd = (ICMP_HEADER*)send_buffer;//ICMP头
	//填充各个字段
	send_icmp_hd->type = ICMP_REQUEST;
	send_icmp_hd->code = 0;
	send_icmp_hd->id = (USHORT)GetCurrentProcessId();
	memset(send_buffer + sizeof(ICMP_HEADER), 'E', DEF_ICMP_DATA_SIZE);

	RESULT result;//存储解析结果
	bool reach = false;//标志是否到达目的主机
	USHORT seq_number = 0;//序列号
	int max_hop = MAX_HOP;//最大跳数
	int ttl = 1;//ttl，初始为1
	while (!reach&&max_hop--)
	{
		//设置ttl
		setsockopt(my_socket, IPPROTO_IP, IP_TTL, (char*)&ttl, sizeof(ttl));
		cout << setw(3) << ttl << flush;

		//填充发送的icmp包的其他字段
		((ICMP_HEADER*)send_buffer)->cksum = 0;
		((ICMP_HEADER*)send_buffer)->seq = htons(seq_number);
		seq_number++;
		((ICMP_HEADER*)send_buffer)->cksum = chechsum((USHORT*)send_buffer, sizeof(ICMP_HEADER) + DEF_ICMP_DATA_SIZE);

		//初始化分析结果中的字段
		result.seq = ((ICMP_HEADER*)send_buffer)->seq;
		result.round_time = GetTickCount();

		//发包
		if (sendto(my_socket, send_buffer, sizeof(send_buffer), 0, (sockaddr*)&des_sock_add, sizeof(des_sock_add)) == SOCKET_ERROR)
		{
			if (WSAGetLastError() == WSAEHOSTUNREACH)
				cout << "Can not reach the destination!" << endl;
			closesocket(my_socket);
			WSACleanup();
			exit(0);
		}

		sockaddr_in from;
		int from_len = sizeof(from);
		int read_len;

		while (true)
		{
			//接包
			read_len = recvfrom(my_socket, reveive_buffer, MAX_ICMP_PACKET_SIZE, 0, (sockaddr*)&from, &from_len);
			if (read_len != SOCKET_ERROR)
			{
				//解析数据包
				if (decode(reveive_buffer, read_len, result))
				{
					if (result.ip_address.s_addr == des_sock_add.sin_addr.s_addr)//是否到达目标主机
						reach = TRUE;
					cout << '\t' << inet_ntoa(result.ip_address) << endl;
					route = route + inet_ntoa(result.ip_address) + "-";
					break;
				}
			}
			//接收超时
			else if (WSAGetLastError() == WSAETIMEDOUT)
			{
				cout << setw(9) << '*' << '\t' << "Request timed out." << endl;
				break;
			}
			else
			{
				cout << "Receive data error!" << endl;
				cout << "Error Code: " << WSAGetLastError() << endl;
				closesocket(my_socket);
				WSACleanup();
				exit(0);
			}
		}
		//ttl递增，发送下一个包
		ttl++;
	}

	cout << "Trace complete." << endl;
	closesocket(my_socket);
	WSACleanup();
	return;
}
/*********************************************************
功能：计算校验和
参数：input_buf:待校验的部分
size:缓冲区的大小
*********************************************************/
USHORT TraceRT::chechsum(USHORT* input_buf, int size)
{
	unsigned long cksum = 0;
	while (size > 1)
	{
		cksum += *input_buf++;
		size -= sizeof(USHORT);
	}
	if (size)
		cksum += *(UCHAR*)input_buf;

	cksum = (cksum >> 16) + (cksum & 0xffff);
	cksum += (cksum >> 16);

	return (USHORT)(~cksum);
}

/*********************************************************
功能：对接受到的数据包进行分析，并将结果存入result中
参数：buffer:接受的数据缓冲区
size:缓冲区大小
result:分析结果
*********************************************************/
bool TraceRT::decode(char* buffer, int size, RESULT& result)
{
	//检查包大小是否合法
	IP_HEADER* pIpHdr = (IP_HEADER*)buffer;
	int iIpHdrLen = pIpHdr->hdr_len * 4;
	if (size < (int)(iIpHdrLen + sizeof(ICMP_HEADER)))
		return false;

	ICMP_HEADER* pIcmpHdr = (ICMP_HEADER*)(buffer + iIpHdrLen);//ICMP头
	USHORT id, seq;//存储从数据包中读入的标示符和序列号
				   //如果是回显应答则直接读取
	if (pIcmpHdr->type == ICMP_REPLY)
	{
		id = pIcmpHdr->id;
		seq = pIcmpHdr->seq;
	}
	//如果是接受超时，则需要进一步分析后面的数据，获得标示符和序列号
	else if (pIcmpHdr->type == ICMP_TIMEOUT)
	{
		char* pInnerIpHdr = buffer + iIpHdrLen + sizeof(ICMP_HEADER);//IP头
		int iInnerIPHdrLen = ((IP_HEADER*)pInnerIpHdr)->hdr_len * 4;//IP头长
		ICMP_HEADER* pInnerIcmpHdr = (ICMP_HEADER*)(pInnerIpHdr + iInnerIPHdrLen);//内部的ICMP头
		id = pInnerIcmpHdr->id;
		seq = pInnerIcmpHdr->seq;
	}
	else
		return false;
	//判断是否对应于本程序发送的icmp应答
	if (id != (USHORT)GetCurrentProcessId() || seq != result.seq)
		return false;

	//处理正确收到的ICMP数据报
	if (pIcmpHdr->type == ICMP_REPLY || pIcmpHdr->type == ICMP_TIMEOUT)
	{
		//存储解码结果
		result.ip_address.s_addr = pIpHdr->source_ip;
		result.round_time = GetTickCount() - result.round_time;

		if (result.round_time)
			cout << setw(6) << result.round_time << " ms" << flush;
		else
			cout << setw(6) << "<1" << " ms" << flush;
		return true;
	}

	return false;
}