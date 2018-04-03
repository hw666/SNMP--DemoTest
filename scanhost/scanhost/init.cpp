#include "scanhost.h"
#include "MyTrace.h"
#include "StringSpilt.h"

void init() {
	string s = "10.173.67.75;192.168.1.100;192.168.1.1;192.168.2.1;192.168.2.100;";
	vector<string> v = split(s, ";");	//�ɰ�����ַ����ָ�;
	v.insert(v.begin(), getMyIP());	//������IP��ӽ�������б�
	for (int i = 1; i != v.size(); ++i)							//����������еĲ�����λ�ı���IPɾ��
		if (strcmp(v[i].c_str(), v[0].c_str()) == 0) {
			v.erase(v.begin() + i);
		}

	//��ӡ�����
	for (int i = 0; i != v.size(); ++i)
		cout << v[i] << " ";
	cout << endl;

	//����һ����־���飬�ֱ��ʶ���أ���������ͬһ���ε�����������ͬһ���ε�����
	vector<int> arr(v.size());
	arr[0] = 1;

	//��ӡ��־����
	for (int i = 0; i != arr.size(); ++i)
		cout << arr[i] << " ";
	cout << endl;

	//����һ���ڽӾ������ڱ�ʾ�������֮������ӹ�ϵ����ʼ��
	vector<vector<int> > topo(v.size(), vector<int>(v.size()));
	for (int i = 0; i != topo.size(); ++i) {
		for (int j = 0; j != topo[0].size(); ++j) {
			if (i == j) {
				topo[i][j] = 1;
			}
		}
	}

	//��ӡ�ڽӾ����
	for (int i = 0; i != topo.size(); i++)
	{
		for (int j = 0; j != topo[0].size(); j++)
			cout << setw(5) << topo[i][j];
		cout << endl;
	}
}
