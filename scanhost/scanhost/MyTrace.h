#pragma pack(1)//定义对齐方式
#include <iostream>
#include <iomanip>
#include <winsock2.h>
#include <ws2tcpip.h>
using namespace std;

//IP数据报头
typedef struct
{
	unsigned char hdr_len : 4;//头部长度 4位
	unsigned char version : 4;//版本号 4位
	unsigned char tos;//服务类型 8位
	unsigned short total_len;//总长度 16位
	unsigned short identifier;//标示符 16位
	unsigned short frag_flag;//3位标志+13位片便宜
	unsigned char ttl;//生存时间 8位
	unsigned char protocol;//上层协议 8位
	unsigned short checksum;//校验和 16位
	unsigned long source_ip;//源ip地址 32位
	unsigned long des_ip;//目的ip地址 32位

} IP_HEADER;

//ICMP数据报头
typedef struct
{
	BYTE type;//类型字段8位
	BYTE code;//代码字段8位
	USHORT cksum;//校验和16位
	USHORT id;//标示符16位
	USHORT seq;//序列号16位
} ICMP_HEADER;

//记录分析结果
typedef struct
{
	USHORT seq;			//序列号
	DWORD round_time;	//往返时间
	in_addr ip_address;	//对端IP地址

} RESULT;
#pragma pack()

const BYTE ICMP_REQUEST = 8;//ICMP请求回显
const BYTE ICMP_REPLY = 0;//ICMP回显应答
const BYTE ICMP_TIMEOUT = 11;//ICMP超时

const DWORD DEF_ICMP_TIMEOUT = 3000;	//默认超时时间，单位ms
const int DEF_ICMP_DATA_SIZE = 32;	//默认ICMP数据字段的长度
const int MAX_ICMP_PACKET_SIZE = 1024;	//最大ICMP数据报大小
const int MAX_HOP = 30;				//最大跳数

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