#include <iostream>
#include <fstream>
#include<vector>
#include<algorithm>
#include<string>

#define INF 1e9
#define NONE -999

using namespace std;

ifstream fp1, fp2, fp3;
ofstream fp_out;

int graph[101][101];
int d[101][101];
int p[101][101];
vector<pair<pair<int, int>, string> > message_c;

int find_parent(int s, int e) {
	int c = e;
	if (d[s][c] == INF)
		return -1;
	while (p[s][c] != s)
		c = p[s][c];

	return c;
}

void bellmanFord(int n) {
	
	for (int i = 0; i < n; i++) { //i: ��� ���
		for (int j = 0; j < 101; j++) {
			d[i][j] = INF;
			p[i][j] = -1;
		}

		d[i][i] = 0; //�ڱ� �ڽ�
		p[i][i] = i; //�ڱ� �ڽ�

		for (int j = 0; j < n; j++) { //���� ������ V�� �ݺ�
			for (int k = 0; k < n; k++) { //��� ���� �ϳ��� Ȯ��
				for (int m = 0; m < n; m++) { //�� ����(k)�� ���� �ٸ� ���(m)�� ���� ��� ���
					if (graph[k][m] + d[i][k] < d[i][m] && graph[k][m] > 0) { //graph[k][m]>0: k�� m�� �̿��� �������� Ȯ��
						d[i][m] = d[i][k] + graph[k][m];
						p[i][m] = k;
					}
					else if (graph[k][m] + d[i][k] == d[i][m] && graph[k][m] > 0) { //tie-breaking rule 1
						int cur_p = p[i][m];
						int p1 = find_parent(i, m);
						p[i][m] = k;
						int p2 = find_parent(i, m);

						if (p2 > p1)
							p[i][m] = cur_p;
					}

				}
			}
		}
	}
}

void print_routing_table(int n) {

	int next;
	
	/*
	for (int s = 0; s < n; s++) {
		for (int d = 0; d < n; d++) {
			next = d;
			while (true) {
				tmp = next;
				next = p[s][next];
				if (next == s)
					break;
			}
			fp_out << d << " " << tmp << " " << d[s][d] << '\n';
		}
	}
	*/

	for (int s = 0; s < n; s++) {
		for (int e = 0; e < n; e++) {
			if (d[s][e] == INF) //����� ��ΰ� ���� ���
				continue;
			else {
				next = e;
				while (p[s][next] != s)
					next = p[s][next];
				fp_out << e << " " << next << " " << d[s][e] << '\n';
			}
		}
		fp_out << '\n';
	}
}

void print_messages() {

	for (int i = 0; i < (int)message_c.size(); i++) {
		int s = message_c[i].first.first;
		int e = message_c[i].first.second;
		string message = message_c[i].second;

		if (d[s][e] == INF) { //�� ��� ������ ��ΰ� ���� ��
			fp_out << "from "<< s <<" to "<< e <<" cost infinite hops unreachable message " << message << '\n';
		}
		else {
			fp_out << "from " << s << " to " << e << " cost " << d[s][e] << " hops ";
			vector<int> parent;
			int next = e;

			while (p[s][next] != s) {
				parent.push_back(p[s][next]);
				next = p[s][next];
			}

			parent.push_back(p[s][next]);
			reverse(parent.begin(), parent.end());
			for (int j = 0; j < (int)parent.size(); j++)
				fp_out << parent[j] << " ";

			fp_out << "message " << message << '\n';
		}
	}
	fp_out << '\n';
}

int main(int argc, char* argv[]) {

	int n; //node�� ����

	if (argc != 4) {
		cout << "usage: linkstate topologyfile messagesfile changesfile" << endl;
		return 1;
	}

	fp1.open(argv[1]);
	fp2.open(argv[2]);
	fp3.open(argv[3]);

	if (!fp1.is_open()) {
		cout << "Error: open input file." << endl;
		fp1.close();
		return 1;
	}

	else if (!fp2.is_open()) {
		cout << "Error: open input file." << endl;
		fp2.close();
		return 1;
	}

	else if (!fp3.is_open()) {
		cout << "Error: open input file." << endl;
		fp3.close();
		return 1;
	}

	fp1 >> n;

	while (true) { //read topology file
		int s, e, c;
		fp1 >> s >> e >> c;
		if (fp1.eof())
			break;

		graph[s][e] = c;
		graph[e][s] = c;
	}

	while (true) { //read message file
		string s, m;
		int n[2], tmp2 = 0, tmp1;
		getline(fp2, s);
		if (fp2.eof())
			break;

		for (int i = 0; i < 2; i++) {
			tmp1 = tmp2;
			while(tmp2 < (int)s.size()) {
				if (s[tmp2] == ' ') {
					n[i] = atoi(s.substr(tmp1, tmp2 - tmp1).c_str());
					tmp2++;
					break;
				}
				tmp2++;
			}
		}
		m = s.substr(tmp2);
		message_c.push_back(make_pair(make_pair(n[0], n[1]), m));
	}

	fp_out.open("output_dv.txt");

	bellmanFord(n);
	print_routing_table(n);
	print_messages();

	while (true) {
		int s, e, c;
		fp3 >> s >> e >> c;
		if (fp3.eof())
			break;

		graph[s][e] = c; //change cost
		graph[e][s] = c;

		bellmanFord(n);
		print_routing_table(n);
		print_messages();
	}

	cout << "Complete. Output file written to output_dv.txt" << endl;

	return 0;
}