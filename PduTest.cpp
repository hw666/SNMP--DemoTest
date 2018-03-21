#include "snmp_pp.h"
#include "Oid.h"
#include <iostream>

void main(){
	Pdu pdu;
	Vb vb((Oid)"1.3.6.1.2.1.1.0");
	pdu+=vb;
	pdu.get_vb(vb,0);
	cout<<vb.get_printable_value();
}