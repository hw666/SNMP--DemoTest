
void main(){

	UdpAddress address("127.0.0.1");
	snmp_version version = version1;
	int retries=1;
	int timeout=100;
	u_short port=161;
	OctetStr community("public");
	Ctarget ctarget(address);
	ctarget.set_version(version);
	ctarget.set_retry(timeout);
	ctarget.set_redcommunity(community);
}

