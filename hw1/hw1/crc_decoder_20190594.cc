#include <iostream>
#include <fstream>
#include <cstdlib>
#include <bitset>
#include <string>
#include <cstring>

using namespace std;
string get_remainder(string dividend, string generator);
string do_crc(string codeword, string generator, int dataword_size);
int error_cnt = 0;

string get_remainder(string dividend, string generator) {
	int idx = generator.size();
	string tmp = dividend.substr(0, idx);
	string tmp2;

	while (idx <= dividend.size()) {
		tmp2 = "";
		if (tmp[0] == '0') {
			for (int i = 1; i < tmp.size(); i++)
				tmp2 += to_string('0' ^ tmp[i]);
			if (idx != dividend.size())
				tmp2 += dividend[idx];
		}
		else {
			for (int i = 1; i < tmp.size(); i++)
				tmp2 += to_string(generator[i] ^ tmp[i]);
			if (idx != dividend.size())
				tmp2 += dividend[idx];
		}
		tmp = tmp2;
		idx++;
	}

	return tmp;
}

string do_crc(string codeword, string generator, int dataword_size) {
	string remainder;

	remainder = get_remainder(codeword, generator);
	for (int i = 0; i < remainder.size(); i++) {
		if (remainder[i] == '1') {
			error_cnt++;
			break;
		}
	}

	return codeword.substr(0, dataword_size);
}

int main(int argc, char* argv[]) {

	string generator, codewords, datawords;
	ifstream fp;
	ofstream fq, fr;
	int data_s, generator_s, codeword_s, codeword_n, padding_s;
	char padding, c;

	if (argc != 6) {
		cout << "usage: ./crc_encoder input_file output_file generator dataword_size" << endl;
		return 1;
	}

	fp.open(argv[1]);
	if (!fp.is_open()) {
		cout << "input file open error." << endl;
		fp.close();
		return 1;
	}

	fq.open(argv[2]);
	if (!fq.is_open()) {
		cout << "output file open error." << endl;
		fq.close();
		return 1;
	}

	fr.open(argv[3]);
	if (!fr.is_open()) {
		cout << "result file open error." << endl;
		fr.close();
		return 1;
	}

	data_s = atoi(argv[5]);
	if (data_s != 4 && data_s != 8) {
		cout << "dataword size must be 4 or 8" << endl;
		return 1;
	}

	generator_s = strlen(argv[4]);
	generator = argv[4];

	fp.get(padding);
	padding_s = (int)padding;
	codewords = "";
	while (fp.get(c)) {
		bitset<8> b(c);
		codewords += b.to_string();
	}

	codewords = codewords.substr(padding_s, codewords.size());
	codeword_s = data_s + generator_s - 1;
	codeword_n = codewords.size() / codeword_s; //codeword °³¼ö

	datawords = "";
	for (int i = 0; i < codeword_n; i++) {
		datawords += do_crc(codewords.substr(i * codeword_s, codeword_s), generator, data_s);
	}

	for (int i = 0; i < datawords.size(); i += 8) {
		char w = stoi(datawords.substr(i, 8), 0, 2);
		fq << w;
	}

	fr << codeword_n << ' ' << error_cnt << '\n';

	fp.close();
	fq.close();
	fr.close();

	return 0;
}
