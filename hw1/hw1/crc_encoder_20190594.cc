#include <iostream>
#include <fstream>
#include <cstdlib>
#include <bitset>
#include <string>
#include <cstring>

using namespace std;

string get_remainder(string dividend, string generator);
string do_crc(string dataword, string generator, int generator_size);

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

string do_crc(string dataword, string generator, int generator_size) {

	string remainder, codeword;

	remainder = get_remainder(dataword + string(generator_size - 1, '0'), generator);

	codeword = dataword + remainder;

	return codeword;
}

int main(int argc, char* argv[]) {
	
	string generator, codewords;
	ifstream fp;
	ofstream fq;
	int data_s, data_cnt, generator_s, codeword_s, f_size, padding_s;

	if (argc != 5) {
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

	data_s = atoi(argv[4]);
	if (data_s != 4 && data_s != 8) {
		cout << "dataword size must be 4 or 8" << endl;
		return 1;
	}

	generator_s = strlen(argv[3]);
	generator = argv[3];
	codeword_s = data_s + generator_s - 1;

	char c;
	codewords = "";

	while (fp.get(c)) {
		bitset<8> data(c);
		string str = data.to_string();
		for (int i = 0; i < (8 / data_s); i++)
			codewords += do_crc(str.substr(4 * i, data_s), generator, generator_s);
	}

	padding_s = (8 - codewords.size() % 8) % 8;
	bitset<8> tmp(padding_s);
	if (padding_s != 0)
		codewords = tmp.to_string() + string(padding_s, '0') + codewords;
	else
		codewords = tmp.to_string() + codewords;

	for (int i = 0; i < codewords.size(); i += 8) {
		char w = stoi(codewords.substr(i, 8), 0, 2);
		fq << w;
	}

	fp.close();
	fq.close();

	return 0;
}