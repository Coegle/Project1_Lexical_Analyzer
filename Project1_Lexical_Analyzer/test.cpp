#include <iostream>
#include <fstream>
#include <algorithm>
#include "NFA.h"
#include "DFA.h"
using namespace std;
class analyzer {

public:
	NFA nfa;
	DFA dfa;
	string blank;
	string reservedWordLabel;
	vector<string> reservedWord;
};

//�ַ����ָ��
std::vector<std::string> split(std::string str, std::string pattern)
{
	std::string::size_type pos;
	std::vector<std::string> result;
	str += pattern;//��չ�ַ����Է������
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

// ���
void append_output(const char* resultPath, int lineNum, vector<pair<string, string>>& tokenList) {
	ofstream fresult(resultPath, fstream::out|fstream::app);
	for (auto it = tokenList.begin(); it != tokenList.end(); it++) {
		fresult << lineNum << " " << it->first << " " << it->second << endl;
	}
	fresult.close();
}

// 
void test(analyzer& analysis, const char* sourcePath, const char* resultPath) {
	ifstream fsource(sourcePath);

	if (fsource.fail()) {
		cout << "Could not open source file!" << endl;
		exit(0);
	}

	int lineNum = 1;
	string line;
	while (getline(fsource, line)) {//ÿ��
		vector<string> substr = split(line, " ");

		for (int i = 0; i < substr.size(); i++) { // ÿ�������ʡ�

			vector<pair<string, string>> tokenList; // tokenType token
			bool isSuccess = true;

			int beginIndex = 0;
			int endIndex = 0;

			string state = analysis.nfa.beginState[0];

			while (endIndex < substr[i].length()) { 
				auto it_input = analysis.nfa.adjList.find(substr[i][endIndex]);
				auto it_prestate = it_input->second.find(state);
				// �ҵ���һ��״̬
				if (it_input != analysis.nfa.adjList.end() && it_prestate != it_input->second.end()) {
					state = (it_input->second)[state];
					endIndex++;
					continue;
				}
				
				auto it_isEndState = analysis.nfa.endState.find(state);

				// ��ǰ���ڽ���״̬���޷�����ƥ�䣬�򱣴�
				if (it_isEndState != analysis.nfa.endState.end()) { 
					tokenList.push_back(make_pair(it_isEndState->second, substr[i].substr(beginIndex, endIndex - beginIndex)));
					beginIndex = endIndex;
					endIndex++;
					continue;
				}

				// ʧ��
				tokenList.clear();
				tokenList.push_back(make_pair(string("ERRORTYPE"), substr[i]));
				break;
			}
			// �������ʶ�ƥ��
			if (analysis.nfa.endState.find(state) != analysis.nfa.endState.end() && tokenList.size() == 0) {
				tokenList.push_back(make_pair(state, substr[i].substr(beginIndex, endIndex - beginIndex)));
			}
			append_output(resultPath,lineNum, tokenList);
		}
		lineNum++;
	}

	fsource.close();
}

// ���� NFA
void input(analyzer& analysis, const char* path) {
	ifstream fin;

	fin.open(path);

	if(fin.fail()){
		cout << "Could not open Chomsky file!" << endl;
		exit(0);
	}
	string line;
	fin >> analysis.blank >> analysis.reservedWordLabel;
	int startNodeNum; //��ʼ�����Ŀ
	fin >> startNodeNum;

	// ���뿪ʼ���
	for (int i = 0; i < startNodeNum; i++) {
		string tmp;
		fin >> tmp;
		analysis.nfa.stateList.push_back(tmp);
		analysis.nfa.beginState.push_back(tmp);
	}
	fin.ignore();

	// �����ķ�
	while(getline(fin, line)) {		

		vector<string> substr = split(line, " ");
		
		// ���ķ����δ��ӵ�״̬��ӵ�����б�
		if (find(analysis.nfa.stateList.begin(), analysis.nfa.stateList.end(), substr[0]) == analysis.nfa.stateList.end()) {
			analysis.nfa.stateList.push_back(substr[0]);
		}
		// ����ڱߡ�������ż����ķ��Ҳ�δ��ӵ�״̬
		if (substr.size() == 4) {
			analysis.nfa.adjList[substr[2][0]][substr[0]] = substr[3];
			if (find(analysis.nfa.input.begin(), analysis.nfa.input.end(), substr[2][0]) == analysis.nfa.input.end()) {
				analysis.nfa.input.push_back(substr[2][0]);
			}
			if (find(analysis.nfa.stateList.begin(), analysis.nfa.stateList.end(), substr[3]) == analysis.nfa.stateList.end()) {
				analysis.nfa.stateList.push_back(substr[3]);
			}
			
		} else if (substr[2] == analysis.blank) {
			analysis.nfa.endState[substr[0]] = substr[0];
			// TODO: ��һ�� else ���Ӧ�ò�����
		} else {
			if (analysis.nfa.endState.find("Z") == analysis.nfa.endState.end()) {
				analysis.nfa.endState["Z"] = "Z";
			}
			analysis.nfa.adjList[substr[2][0]][substr[0]] = "Z";
		}
	}
	fin.close();
}

// ��С�� DFA
void NFAtoDFA(analyzer& analysis) {
	
}
int main() {
	analyzer analysis;
	input(analysis, "2.txt");
	test(analysis, "source.txt", "result.txt");
	return 0;
}