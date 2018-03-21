#include "snmp_pp/snmp_pp.h"
#include <iostream>
using namespace std;

void main() {
	Snmp_pp::Pdu pdu;
	Snmp_pp::Vb vb((Snmp_pp::Oid)"1.3.6.1.2.1.1.0");
	pdu += vb;
	pdu.get_vb(vb, 0);
	cout << vb.get_printable_value();
	getchar();
}