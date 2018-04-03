#pragma pack(1)//������뷽ʽ
#include <iostream>
#include <iomanip>
#include <winsock2.h>
#include <ws2tcpip.h>
using namespace std;

//IP���ݱ�ͷ
typedef struct
{
	unsigned char hdr_len : 4;//ͷ������ 4λ
	unsigned char version : 4;//�汾�� 4λ
	unsigned char tos;//�������� 8λ
	unsigned short total_len;//�ܳ��� 16λ
	unsigned short identifier;//��ʾ�� 16λ
	unsigned short frag_flag;//3λ��־+13λƬ����
	unsigned char ttl;//����ʱ�� 8λ
	unsigned char protocol;//�ϲ�Э�� 8λ
	unsigned short checksum;//У��� 16λ
	unsigned long source_ip;//Դip��ַ 32λ
	unsigned long des_ip;//Ŀ��ip��ַ 32λ

} IP_HEADER;

//ICMP���ݱ�ͷ
typedef struct
{
	BYTE type;//�����ֶ�8λ
	BYTE code;//�����ֶ�8λ
	USHORT cksum;//У���16λ
	USHORT id;//��ʾ��16λ
	USHORT seq;//���к�16λ
} ICMP_HEADER;

//��¼�������
typedef struct
{
	USHORT seq;			//���к�
	DWORD round_time;	//����ʱ��
	in_addr ip_address;	//�Զ�IP��ַ

} RESULT;
#pragma pack()

const BYTE ICMP_REQUEST = 8;//ICMP�������
const BYTE ICMP_REPLY = 0;//ICMP����Ӧ��
const BYTE ICMP_TIMEOUT = 11;//ICMP��ʱ

const DWORD DEF_ICMP_TIMEOUT = 3000;	//Ĭ�ϳ�ʱʱ�䣬��λms
const int DEF_ICMP_DATA_SIZE = 32;	//Ĭ��ICMP�����ֶεĳ���
const int MAX_ICMP_PACKET_SIZE = 1024;	//���ICMP���ݱ���С
const int MAX_HOP = 30;				//�������

class TraceRT {
public:
	TraceRT();
	~TraceRT();
public:
	void tracert(char* ip);
	USHORT chechsum(USHORT* input_buf, int size);
	bool decode(char* buffer, int size, RESULT& result);
public:
	static string route;
};