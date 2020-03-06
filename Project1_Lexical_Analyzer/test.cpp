#include <iostream>
#include <fstream>
#include <algorithm>
#include "NFA.h"
#include "DFA.h"
using namespace std;
class analyzer {

public:
	analyzer(int n) {
		num = n;
		nfa = new NFA[n];
	}
	int num;
	NFA* nfa;
	string blank;
	string reserveWord;
	~analyzer() {
		delete[] nfa;
	}
};

//字符串分割函数
std::vector<std::string> split(std::string str, std::string pattern)
{
	std::string::size_type pos;
	std::vector<std::string> result;
	str += pattern;//扩展字符串以方便操作
	int size = str.size();

	for (int i = 0; i < size; i++)
	{
		pos = str.find(pattern, i);
		if (pos < size)
		{
			std::string s = str.substr(i, pos - i);
			result.push_back(s);
			i = pos + pattern.size() - 1;
		}
	}
	return result;
}
void test(analyzer*& analysis, const char* sourcePath, const char* resultPath) {
	ifstream fsource(sourcePath);
	ofstream fresult(resultPath, fstream::out);
	if (fsource.fail()) {
		cout << "Could not open source file!" << endl;
		exit(0);
	}

	int linenum = 1;
	string line;
	while (getline(fsource, line)) {
		
	}


	fsource.close();
	fresult.close();
}
void input(analyzer*& analysis, const char* path) {
	ifstream fin;
	fin.open(path);

	if(fin.fail()){
		cout << "Could not open Chomsky file!" << endl;
		exit(0);
	}

	int num;
	fin >> num;
	analysis = new analyzer(num);
	fin >> analysis->blank >> analysis->reserveWord;
	fin.ignore();
	for (int i = 0; i < num; i++) {
		int size;
		string line;
		// 除掉注释行
		getline(fin, line);
		// 读入结点
		fin >> size;
		for (int j = 0; j < size; j++) {
			string tmp;
			fin >> tmp;
			analysis->nfa[i].beginState.push_back(tmp);
			analysis->nfa[i].stateList.push_back(tmp);
		}
		fin >> size;
		for (int j = 0; j < size; j++) {
			string tmp;
			fin >> tmp;
			analysis->nfa[i].stateList.push_back(tmp);

		}
		
		// 建立关系
		fin >> size;
		fin.ignore();
		for (int j = 0; j < size; j++) {
			getline(fin, line);
			if (line.length == 0) {
				j--;
			}
			vector<string> substr = split(line, " ");
			if (substr.size() == 4) {
				analysis->nfa[i].adjList[substr[2]][substr[0]] = substr[3];
				if (find(analysis->nfa[i].input.begin(), analysis->nfa[i].input.end(), substr[2]) == analysis->nfa[i].input.end()) {
					analysis->nfa[i].input.push_back(substr[2]);
				}
				
			}
			else {
				if (substr[2] == "$") {
					analysis->nfa[i].endState.push_back(substr[0]);
				}
				else {
					if (find(analysis->nfa[i].endState.begin(), analysis->nfa[i].endState.end(), "Z") == analysis->nfa[i].endState.end()) {
						analysis->nfa[i].endState.push_back("Z");
					}
					analysis->nfa[i].adjList[substr[2]][substr[0]] = "Z";
				}
			}
		}
	}
	fin.close();
}

int main() {
	analyzer* analysis = nullptr;
	input(analysis, "Chomsky_III.txt");
	return 0;
}