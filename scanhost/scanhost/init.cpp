#include "scanhost.h"
#include "MyTrace.h"
#include "StringSpilt.h"

void init() {
	string s = "10.173.67.75;192.168.1.100;192.168.1.1;192.168.2.1;192.168.2.100;";
	vector<string> v = split(s, ";");	//可按多个字符来分隔;
	v.insert(v.begin(), getMyIP());	//将本地IP添加进活动主机列表
	for (int i = 1; i != v.size(); ++i)							//将活动主机当中的不是首位的本地IP删除
		if (strcmp(v[i].c_str(), v[0].c_str()) == 0) {
			v.erase(v.begin() + i);
		}

	//打印活动主机
	for (int i = 0; i != v.size(); ++i)
		cout << v[i] << " ";
	cout << endl;

	//定义一个标志数组，分别标识网关，本机，在同一网段的主机，不在同一网段的主机
	vector<int> arr(v.size());
	arr[0] = 1;

	//打印标志数组
	for (int i = 0; i != arr.size(); ++i)
		cout << arr[i] << " ";
	cout << endl;

	//定义一个邻接矩阵用于表示两活动主机之间的连接关系并初始化
	vector<vector<int> > topo(v.size(), vector<int>(v.size()));
	for (int i = 0; i != topo.size(); ++i) {
		for (int j = 0; j != topo[0].size(); ++j) {
			if (i == j) {
				topo[i][j] = 1;
			}
		}
	}

	//打印邻接矩阵表
	for (int i = 0; i != topo.size(); i++)
	{
		for (int j = 0; j != topo[0].size(); j++)
			cout << setw(5) << topo[i][j];
		cout << endl;
	}
}
