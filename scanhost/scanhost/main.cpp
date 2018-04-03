#include "scanhost.h"
#include "MyTrace.h"
#include "StringSpilt.h"

#define max 1000

Scanhost sc = Scanhost::Scanhost();
TraceRT tr = TraceRT::TraceRT();

int main(void) {

	// 获取活动主机，已完成，一点小BUG，有时候不能发现自己所以需要调用getMyIP获得本机IP
	// 后续处理中可以通过消除重复项的方法去掉活动主机中重复部分，再进行处理
	/*sc.setIP1(192, 168, 1, 0);
	sc.setIP2(192, 168, 2, 255);
	sc.make();
	cout << "活动主机: "<< sc.result.c_str()  << endl; //<< getMyIP().c_str()*/

	// 获取拓扑图，目前想法是通过TRACE方法获得每次ping活动主机的路径，然后根据路径生成图的邻接矩阵
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