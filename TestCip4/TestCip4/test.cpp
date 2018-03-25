#include "snmp_pp/snmp_pp.h";
#include <iostream>
#include <string>
using namespace std;

/*
使用GET方式获取系统信息和网络接口信息
*/

void main() {
	Snmp_pp::Snmp::socket_startup(); //初始化Winsocket函数
	Snmp_pp::snmp_version version = Snmp_pp::version1;
	int status;
	Snmp_pp::Snmp snmp(status, 0, false); //创建一个Snmp对象；
	Snmp_pp::Pdu pdu;                    //创建一个Pdu对象；
	Snmp_pp::Vb vb;                      //创建一个Vb对象；
	vb.set_oid((Snmp_pp::Oid)"1.3.6.1.2.1.2.1.0");
	pdu += vb;             //设置Vb对象并将其添加到对象Pdu中；


	Snmp_pp::UdpAddress address("192.168.1.100");
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
		cout << "oid is : " << vb.get_printable_oid() << endl;
		cout << "value is : " << vb.get_printable_value() << endl;
	}
	//操作成功，将被管理对象的OID和值保存在reply_Oid和reply_value中；
	Snmp_pp::Snmp::socket_cleanup();
	getchar();
}




/*
#include <libsnmp.h>
#include <iostream>  
#include "snmp_pp/snmp_pp.h"

#ifdef WIN32
#define strcasecmp _stricmp
#endif

#ifdef SNMP_PP_NAMESPACE
using namespace Snmp_pp;
#endif

#define BULK_MAX 10

/*

使用WALK方式获取系统信息及网络接口信息

*/
/*
int main(int argc)
{
	int requests = 0;        // keep track of # of requests
	int objects = 0;

	char *ip_str = "192.168.1.100";  //ip地址
	//char *oid_str = "1.3.6.1.2.1.25.6.3.1.2";  //Oid地址  系统安装的软件列表
	//char *oid_str = "1.3.6.1.2.1.25.4.2.1.2";  //Oid地址  系统运行的进程列表
	//char *oid_str = "1.3.6.1.2.1.2.2.1.2";  //Oid地址  网络接口信息描述
	//char *oid_str = "1.3.6.1.2.1.2.2.1.3";  //Oid地址  网络接口类型
	//char *oid_str = "1.3.6.1.2.1.2.2.1.4";  //Oid地址  接口发送和接收的最大Ip数据报
	//char *oid_str = "1.3.6.1.2.1.2.2.1.5";  //Oid地址  接口当前带宽
	char *oid_str = "1.3.6.1.2.1.2.2.1.6";  //Oid地址  接口的物理地址


	Snmp::socket_startup();  // Initialize socket subsystem

	//---------[ make a GenAddress and Oid object to retrieve ]---------------
	
	UdpAddress address(ip_str);      // make a SNMP++ Generic address
	if (!address.valid()) {           // check validity of address
		std::cout << "Invalid Address or DNS Name, " << ip_str << "\n";
	
	}

	Oid oid("1");                      // default is beginning of MIB 
									  
	oid = oid_str;

	if (!oid.valid()) {            // check validity of user oid
		std::cout << "Invalid Oid, " << oid_str << "\n";
		
	}

	//---------[ determine options to use ]-----------------------------------
	snmp_version version = version1;                  // default is v1
	int retries = 1;                                  // default retries is 1
	int timeout = 100;                                // default is 1 second
	u_short port = 161;                               // default snmp port is 161
	OctetStr community("public");                   // community name
	bool subtree = false;


	//----------[ create a SNMP++ session ]-----------------------------------
	int status;
	// bind to any port and use IPv6 if needed
	Snmp snmp(status, 0, (address.get_ip_version() == Address::version_ipv6));

	if (status != SNMP_CLASS_SUCCESS) {
		std::cout << "SNMP++ Session Create Fail, " << snmp.error_msg(status) << "\n";

		return 1;
	}


	//--------[ build up SNMP++ object needed ]-------------------------------
	Pdu pdu;                               // construct a Pdu object
	Vb vb;                                 // construct a Vb object
	vb.set_oid(oid);                      // set the Oid portion of the Vb
	pdu += vb;                             // add the vb to the Pdu

	address.set_port(port);
	CTarget ctarget(address);             // make a target using the address

									
	ctarget.set_version(version);         // set the SNMP version SNMPV1 or V2
	ctarget.set_retry(retries);           // set the number of auto retries
	ctarget.set_timeout(timeout);         // set timeout
	ctarget.set_readcommunity(community); // set the read community name
										 
										  //-------[ issue the request, blocked mode ]-----------------------------
	std::cout << "SNMP++ snmpWalk to " << ip_str << " SNMPV"
		
		<< (version + 1)
		
		<< " Retries=" << retries
		<< " Timeout=" << timeout * 10 << "ms";
	
	std::cout << " Community=" << community.get_printable() << std::endl << std::flush;

	SnmpTarget *target;
	
	getchar();

	target = &ctarget;

	while ((status = snmp.get_bulk(pdu, *target, 0, BULK_MAX)) == SNMP_CLASS_SUCCESS) {
		//getchar();

		requests++;
	//	for (int z = 0;z<pdu.get_vb_count(); z++) {

			pdu.get_vb(vb, 0);
			
			Oid tmp;
			vb.get_oid(tmp);

		/*	if (subtree && (oid.nCompare(oid.len(), tmp) != 0))
			{
				std::cout << "End of SUBTREE Reached\n";
				std::cout << "Total # of Requests = " << requests << "\n";
				std::cout << "Total # of Objects  = " << objects << "\n";
				return -4;
			}
*/
	//		std::cout << "Oid::::  = " << oid.len() << "\n";
	//		getchar();

/*
			objects++;

			if ((oid.nCompare(oid.len(), tmp) == 0)) {
			// look for var bind exception, applies to v2 only   
			if (vb.get_syntax() != sNMP_SYNTAX_ENDOFMIBVIEW) {
				std::cout << vb.get_printable_oid() << " = ";
				std::cout << vb.get_printable_value() << "\n";

			}
			else {
				std::cout << "End of MIB Reached\n";
				std::cout << "Total # of Requests = " << requests << "\n";
				std::cout << "Total # of Objects  = " << objects << "\n";
				return -4;
			}

			}
			else {
				break;
			
			}


			
	//	}

	//	if (!strstr(vb.get_printable_oid(), "1.3.6.1.2.1.25.6.3.1.2")) {

	//		getchar();
	//		break;
	//	}

		// last vb becomes seed of next rquest
		pdu.set_vblist(&vb, 1);
		 
	}


	if (status != SNMP_ERROR_NO_SUCH_NAME)
		std::cout << "SNMP++ snmpWalk Error, " << snmp.error_msg(status) << "\n";
	std::cout << "Total # of Requests = " << requests << "\n";
	std::cout << "Total # of Objects  = " << objects << "\n";

	Snmp::socket_cleanup();  // Shut down socket subsystem
					
	getchar();

}*/