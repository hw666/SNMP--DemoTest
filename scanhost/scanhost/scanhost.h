#pragma comment(lib,"WS2_32.LIB")
#include<winsock2.h>
#include<iostream>
#include<time.h>
#include<string>
using namespace std;

typedef struct iphdr {                  //IPͷ
	unsigned int headlen : 4;            //IPͷ����
	unsigned int version : 4;            //IP�汾��
	unsigned char tos;                 //��������
	unsigned short totallen;            //�ܳ���
	unsigned short id;                 //ID��
	unsigned short flag;               //���
	unsigned char ttl;                 //����ʱ��
	unsigned char prot;                //Э��
	unsigned short checksum;           //Ч���
	unsigned int sourceIP;             //ԴIP
	unsigned int destIP;               //Ŀ��IP
}IpHeader;
//IPͷ��

typedef struct icmphdr    //ICMPͷ
{
	BYTE type;                         //ICMP������
	BYTE code;                         //��������
	USHORT checksum;                   //Ч���
	USHORT id;                         //ID��
	USHORT seq;                        //ICMP���ݱ������к�
}IcmpHeader;
//ICMP��ͷ��

#define ICMP_ECHO 8                    //�������
#define ICMP_ECHO_REPLY 0              //�����Ӧ
#define ICMP_MIN 8                     //ICMP��ͷ���ȣ���СICMP�����ȣ�
#define STATUS_FAILED 0xFFFF           //������
#define DEF_PACKET_SIZE 32             //ȱʡ���ݿ鳤��
#define MAX_PACKET 1024                //������ݿ鳤��
#define MAX_PING_PACKET_SIZE MAX_PACKET+sizeof(IpHeader)
//���������ݰ�����

class Scanhost
{
public:
	Scanhost();
	~Scanhost();
public:
	void setIP1(int a, int b, int c, int d);
	void setIP2(int a, int b, int c, int d);
public:
	void make();
public:
	static string result;
};

string getMyIP();