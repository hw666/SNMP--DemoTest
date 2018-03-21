
#include<iostream>
#include "snmp_pp/snmp_pp.h"  //"snmp_pp.h"

using namespace std;

void main() {

	Snmp_pp::IpAddress ip("127.0.0.1");
	cout<< "ip address is: " << ip << endl;
	Snmp_pp::UdpAddress udp("127.0.0.1:8888");
	cout << "udp address is: " << udp << endl;
	cout << "udp address port is:" << udp.get_port() << endl;
	udp = "127.0.0.1";
	udp.set_port(9999);
	cout << udp << endl;
	ip = "258.0.0.1";
	if (!ip.valid())
		cout << "bad ip:258.0.0.1" << endl;
	else
		cout << "good ip:258.0.0.1" << endl;
	ip = "255.0.0.1";
	if (!ip.valid())
		cout << "error ip:255.0.0.1" << endl;
	else
		cout << "good ip:255.0.0.1" << endl;
	ip = "0.0.0.0";
	if (!ip.valid())
		cout << "error ip:0.0.0.0" << endl;
	else
		cout << "good ip:0.0.0.0" << endl;
	ip = "255.255.255.255";
	if (!ip.valid())
		cout << "error ip:255.255.255.255" << endl;
	else
		cout << "good ip:255.255.255.255" << endl;


	getchar();

}