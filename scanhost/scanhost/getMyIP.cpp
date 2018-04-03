#include "scanhost.h"

string getMyIP() {
	char buf[256] = "";
	struct hostent *ph = 0;
	WSADATA w;
	WSAStartup(0x0101, &w);
	gethostname(buf, 256);
	string hostName = buf;//获得本机名称
	ph = gethostbyname(buf);
	char *IP = inet_ntoa(*((struct in_addr *)ph->h_addr_list[0]));//获得本机IP
	WSACleanup();
	string str(IP);
	return str;
}