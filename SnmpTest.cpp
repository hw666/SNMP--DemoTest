#include "snmp_pp/snmp_pp.h";
#include <iostream>
#include <string>
using namespace std;

void main() {
	Snmp_pp::Snmp::socket_startup(); //初始化Winsocket函数
	Snmp_pp::snmp_version version = Snmp_pp::version1;
	int status;
	Snmp_pp::Snmp snmp(status, 0, false); //创建一个Snmp对象；
	Snmp_pp::Pdu pdu;                    //创建一个Pdu对象；
	Snmp_pp::Vb vb;                      //创建一个Vb对象；
	vb.set_oid((Snmp_pp::Oid)"1.3.6.1.2.1.1.1.0");
	pdu += vb;             //设置Vb对象并将其添加到对象Pdu中；

	Snmp_pp::UdpAddress address("192.168.2.100");
	Snmp_pp::CTarget ctarget(address);        //创建一个Ctarget对象；
	ctarget.set_version(version);
	ctarget.set_retry(1);
	ctarget.set_timeout(100);
	ctarget.set_readcommunity("public"); //设置Ctarget对象；	

	Snmp_pp::SnmpTarget *target;
	target = &ctarget;
	status = snmp.get(pdu, *target); //发起一个Get操作；


	if (status == SNMP_CLASS_SUCCESS)
	{
		cout << "in" << endl;
		pdu.get_vb(vb, 0);
		string reply_oid = vb.get_printable_oid();
		string reply_value = vb.get_printable_value();
		cout << "oid is : "<< vb.get_printable_oid() << endl;
		cout << "value is : "<< vb.get_printable_value() << endl;
	}
	//操作成功，将被管理对象的OID和值保存在reply_Oid和reply_value中；
	Snmp_pp::Snmp::socket_cleanup();
	getchar();
}