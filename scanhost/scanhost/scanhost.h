#pragma comment(lib,"WS2_32.LIB")
#include<winsock2.h>
#include<iostream>
#include<time.h>
#include<string>
using namespace std;

typedef struct iphdr {                  //IP头
	unsigned int headlen : 4;            //IP头长度
	unsigned int version : 4;            //IP版本号
	unsigned char tos;                 //服务类型
	unsigned short totallen;            //总长度
	unsigned short id;                 //ID号
	unsigned short flag;               //标记
	unsigned char ttl;                 //生存时间
	unsigned char prot;                //协议
	unsigned short checksum;           //效验和
	unsigned int sourceIP;             //源IP
	unsigned int destIP;               //目的IP
}IpHeader;
//IP头部

typedef struct icmphdr    //ICMP头
{
	BYTE type;                         //ICMP类型码
	BYTE code;                         //子类型码
	USHORT checksum;                   //效验和
	USHORT id;                         //ID号
	USHORT seq;                        //ICMP数据报的序列号
}IcmpHeader;
//ICMP包头部

#define ICMP_ECHO 8                    //请求回送
#define ICMP_ECHO_REPLY 0              //请求回应
#define ICMP_MIN 8                     //ICMP包头长度（最小ICMP包长度）
#define STATUS_FAILED 0xFFFF           //错误码
#define DEF_PACKET_SIZE 32             //缺省数据块长度
#define MAX_PACKET 1024                //最大数据块长度
#define MAX_PING_PACKET_SIZE MAX_PACKET+sizeof(IpHeader)
//最大接收数据包长度

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