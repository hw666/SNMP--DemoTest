void main(){
	Snmp::socket_startup(); //初始化Winsocket函数
	snmp_version version = version1; 
	int status;
	Snmp snmp(status,0,false); //创建一个Snmp对象；
	Pdu pdu;                    //创建一个Pdu对象；
	Vb vb;                      //创建一个Vb对象；
	vb.set_oid((Oid)"1.3.6.1.2.1.1");
	pdu += vb;             //设置Vb对象并将其添加到对象Pdu中；
	
	UdpAddress address("127.0.0.1:161");
	Ctarget ctatget(address)；        //创建一个Ctarget对象；
	ctarget.set_version(version);
	ctarget.set_retey(1);
	ctarget.set_timout(100);
	ctarget.set_readcommunity("public"); //设置Ctarget对象；
	
	SnmpTarget *target;
	target = &ctarget;
	status = snmp.get(pdu,*target); //发起一个Get操作；

	if(status == SNMP_CLASS_SUCCESS)
	{
		pdu.get_vb(vb,0);
		CString reply_oid = vb.get_printable_oid();
		CString reply_value = vb.get_printable_value();
	}
	//操作成功，将被管理对象的OID和值保存在reply_Oid和reply_value中；
	Snmp::socket_cleanup;
}
	