#include "scanhost.h"
#include "MyTrace.h"
#include "StringSpilt.h"

#define max 1000

Scanhost sc = Scanhost::Scanhost();
TraceRT tr = TraceRT::TraceRT();

int main(void) {

	// ��ȡ�����������ɣ�һ��СBUG����ʱ���ܷ����Լ�������Ҫ����getMyIP��ñ���IP
	// ���������п���ͨ�������ظ���ķ���ȥ����������ظ����֣��ٽ��д���
	/*sc.setIP1(192, 168, 1, 0);
	sc.setIP2(192, 168, 2, 255);
	sc.make();
	cout << "�����: "<< sc.result.c_str()  << endl; //<< getMyIP().c_str()*/

	// ��ȡ����ͼ��Ŀǰ�뷨��ͨ��TRACE�������ÿ��ping�������·����Ȼ�����·������ͼ���ڽӾ���
	tr.tracert("www.baidu.com");
	cout << tr.route.c_str() << endl;
	string a = getMyIP() + "-" + tr.route;
	vector<string> v = split(a, "-");
	for (int i = 0; i != v.size(); ++i)
		cout << v[i] << " ";
	cout << endl;



	getchar();
	return 0;
}