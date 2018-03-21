#include "snmp_pp/snmp_pp.h"
#include <iostream>
using namespace std;

void main() {
	Snmp_pp::UdpAddress address("127.0.0.1");
	Snmp_pp::snmp_version version = Snmp_pp::version1;
	int retries = 1;
	int timeout = 100;
	u_short port = 161;
	Snmp_pp::OctetStr community("public");
	Snmp_pp::CTarget ctarget(address);
	ctarget.set_version(version);
	ctarget.set_retry(timeout);
	ctarget.set_readcommunity(community);
	getchar();
}