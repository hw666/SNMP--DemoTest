#include "snmp_pp/snmp_pp.h"
#include <iostream>
//#include "Oid.h"
using namespace std;

void main() {
	Snmp_pp::Oid oid;
	oid = "1.3.6.1.2.1";
	Snmp_pp::Vb vb((Snmp_pp::Oid)"1.2.3.4.5.6");
	cout<< "Vb's oid is: " << vb.get_printable_oid() << endl;
	vb.set_oid(oid);
	cout << "Vb's oid is: " << vb.get_printable_oid() << endl;
	cout << "Vb's value is: " << vb.get_printable_value() << endl;
	int i = 1000;
	vb.set_value(i);
	cout << "Vb's value is: " << vb.get_printable_value() << endl;
	int value;
	vb.get_value(value);
	cout << "Vb's value is: " << value << endl;
	vb.set_null();
	cout << "Vb's value is: " << vb.get_printable_value() << endl;

	getchar();
}