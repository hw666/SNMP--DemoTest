#include "MyTrace.h"
string TraceRT::route;

TraceRT::TraceRT() {
}

TraceRT::~TraceRT() {
}

void TraceRT::tracert(char* ip)
{
	//��ʼ��Windows Socket DLL
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		cout << "Initialize Windows Socket DLL Error!" << endl;
		cout << "Error code: " << WSAGetLastError() << endl;
		exit(0);
	}

	u_long des_ip = inet_addr(ip);//Ŀ������ip��ַ
	if (des_ip == INADDR_NONE)//��������������
	{
		//����������ip��ת��
		hostent* host = gethostbyname(ip);
		if (host)
		{
			des_ip = (*(in_addr*)host->h_addr).s_addr;
			cout << "Tracing route to " << ip << " [" << inet_ntoa(*(in_addr*)(&des_ip)) << "] " << " ";
		}
		else //����������ʧ��
		{
			cout << "Can not decode the host name." << endl;
			WSACleanup();
			exit(0);
		}
	}
	else
		cout << "Tracing route to " <<ip<< " ";

	cout << "over a maximum of " << MAX_HOP << " hops" << endl;

	sockaddr_in des_sock_add;//�洢Ŀ���ַ
	ZeroMemory(&des_sock_add, sizeof(des_sock_add));
	des_sock_add.sin_family = AF_INET;
	des_sock_add.sin_addr.s_addr = des_ip;

	//����socket
	SOCKET my_socket = WSASocket(AF_INET, SOCK_RAW, IPPROTO_ICMP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (my_socket == INVALID_SOCKET)
	{
		cout << "Set up the socket error!" << endl;
		cout << "Error Code��" << WSAGetLastError() << endl;
		exit(0);
	}

	int time_out = DEF_ICMP_TIMEOUT;//��ʱʱ��

	//��socket���ý��ܺͷ��͵����ʱ��
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

	char send_buffer[sizeof(ICMP_HEADER) + DEF_ICMP_DATA_SIZE];//�������ݻ���
	char reveive_buffer[MAX_ICMP_PACKET_SIZE];//�������ݻ���
	//��ʼ����������ȫ������
	memset(send_buffer, 0, sizeof(send_buffer));
	memset(reveive_buffer, 0, sizeof(reveive_buffer));

	ICMP_HEADER* send_icmp_hd = (ICMP_HEADER*)send_buffer;//ICMPͷ
	//�������ֶ�
	send_icmp_hd->type = ICMP_REQUEST;
	send_icmp_hd->code = 0;
	send_icmp_hd->id = (USHORT)GetCurrentProcessId();
	memset(send_buffer + sizeof(ICMP_HEADER), 'E', DEF_ICMP_DATA_SIZE);

	RESULT result;//�洢�������
	bool reach = false;//��־�Ƿ񵽴�Ŀ������
	USHORT seq_number = 0;//���к�
	int max_hop = MAX_HOP;//�������
	int ttl = 1;//ttl����ʼΪ1
	while (!reach&&max_hop--)
	{
		//����ttl
		setsockopt(my_socket, IPPROTO_IP, IP_TTL, (char*)&ttl, sizeof(ttl));
		cout << setw(3) << ttl << flush;

		//��䷢�͵�icmp���������ֶ�
		((ICMP_HEADER*)send_buffer)->cksum = 0;
		((ICMP_HEADER*)send_buffer)->seq = htons(seq_number);
		seq_number++;
		((ICMP_HEADER*)send_buffer)->cksum = chechsum((USHORT*)send_buffer, sizeof(ICMP_HEADER) + DEF_ICMP_DATA_SIZE);

		//��ʼ����������е��ֶ�
		result.seq = ((ICMP_HEADER*)send_buffer)->seq;
		result.round_time = GetTickCount();

		//����
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
			//�Ӱ�
			read_len = recvfrom(my_socket, reveive_buffer, MAX_ICMP_PACKET_SIZE, 0, (sockaddr*)&from, &from_len);
			if (read_len != SOCKET_ERROR)
			{
				//�������ݰ�
				if (decode(reveive_buffer, read_len, result))
				{
					if (result.ip_address.s_addr == des_sock_add.sin_addr.s_addr)//�Ƿ񵽴�Ŀ������
						reach = TRUE;
					cout << '\t' << inet_ntoa(result.ip_address) << endl;
					route = route + inet_ntoa(result.ip_address) + "-";
					break;
				}
			}
			//���ճ�ʱ
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
		//ttl������������һ����
		ttl++;
	}

	cout << "Trace complete." << endl;
	closesocket(my_socket);
	WSACleanup();
	return;
}
/*********************************************************
���ܣ�����У���
������input_buf:��У��Ĳ���
size:�������Ĵ�С
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
���ܣ��Խ��ܵ������ݰ����з����������������result��
������buffer:���ܵ����ݻ�����
size:��������С
result:�������
*********************************************************/
bool TraceRT::decode(char* buffer, int size, RESULT& result)
{
	//������С�Ƿ�Ϸ�
	IP_HEADER* pIpHdr = (IP_HEADER*)buffer;
	int iIpHdrLen = pIpHdr->hdr_len * 4;
	if (size < (int)(iIpHdrLen + sizeof(ICMP_HEADER)))
		return false;

	ICMP_HEADER* pIcmpHdr = (ICMP_HEADER*)(buffer + iIpHdrLen);//ICMPͷ
	USHORT id, seq;//�洢�����ݰ��ж���ı�ʾ�������к�
				   //����ǻ���Ӧ����ֱ�Ӷ�ȡ
	if (pIcmpHdr->type == ICMP_REPLY)
	{
		id = pIcmpHdr->id;
		seq = pIcmpHdr->seq;
	}
	//����ǽ��ܳ�ʱ������Ҫ��һ��������������ݣ���ñ�ʾ�������к�
	else if (pIcmpHdr->type == ICMP_TIMEOUT)
	{
		char* pInnerIpHdr = buffer + iIpHdrLen + sizeof(ICMP_HEADER);//IPͷ
		int iInnerIPHdrLen = ((IP_HEADER*)pInnerIpHdr)->hdr_len * 4;//IPͷ��
		ICMP_HEADER* pInnerIcmpHdr = (ICMP_HEADER*)(pInnerIpHdr + iInnerIPHdrLen);//�ڲ���ICMPͷ
		id = pInnerIcmpHdr->id;
		seq = pInnerIcmpHdr->seq;
	}
	else
		return false;
	//�ж��Ƿ��Ӧ�ڱ������͵�icmpӦ��
	if (id != (USHORT)GetCurrentProcessId() || seq != result.seq)
		return false;

	//������ȷ�յ���ICMP���ݱ�
	if (pIcmpHdr->type == ICMP_REPLY || pIcmpHdr->type == ICMP_TIMEOUT)
	{
		//�洢������
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