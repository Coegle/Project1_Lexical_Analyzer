#include "Lexer.h"
#include <fstream>
#include <iostream>
using namespace std;

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
			i = pos + pattern.size() - 1;
			if (s.size() != 0) {
				result.push_back(s);
			}
		}
	}
	return result;
}

void Lexer::append_output(const char* resultPath, int lineNum, vector<pair<string, string>>& tokenList) {
	ofstream fresult(resultPath, fstream::out | fstream::app);
	for (auto it = tokenList.begin(); it != tokenList.end(); it++)
	{
		fresult << lineNum << " " << it->first << " " << it->second << endl;
	}
	fresult.close();
}

/*��Դ���봦��
�� sourcePath ��ȡԴ��������� resultPath */
void Lexer::analyze(const char* sourcePath, const char* resultPath) {
	ifstream fsource(sourcePath);

	if (fsource.fail()) {
		cout << "Could not open source file!" << endl;
		exit(0);
	}

	int lineNum = 1;
	string line;
	while (getline(fsource, line)) {//ÿ��
		vector<string> tmp_substr = split(line, " ");
		vector<string> substr;
		// ���ո��Ʊ����ע��ȥ��

		for (int i = 0; i < tmp_substr.size(); i++) {
			vector<string> tmp = split(tmp_substr[i], "\t");
			int j;
			for (j = 0; j < tmp.size(); j++) {
				if (tmp[j].find(annotation) != string::npos) {
					substr.insert(substr.end(), tmp.begin(), tmp.begin() + j);
					break;
				}
			}
			if (j < tmp.size())break;
			substr.insert(substr.end(), tmp.begin(), tmp.end());
		}

		for (int i = 0; i < substr.size(); i++) { // ÿ�������ʡ�

			vector<pair<string, string>> tokenList; // tokenType token
			bool isSuccess = true;

			int beginIndex = 0;
			int endIndex = 0;

			int state = dfa.beginState;

			while (endIndex < substr[i].length()) {
				auto it_input = dfa.adjList.find(substr[i][endIndex]);
				map<int, int>::iterator it_prestate;
				if (it_input != dfa.adjList.end()) {
					it_prestate = it_input->second.find(state);
				}
				// �ҵ���һ��״̬
				if (it_input != dfa.adjList.end() && it_prestate != it_input->second.end()) {
					state = (it_input->second)[state];
					endIndex++;
					if (endIndex != substr[i].length()) {
						continue;
					}
				}

				auto it_isEndState = dfa.endState.find(state);

				// ��ǰ���ڽ���״̬���޷�����ƥ�䣬�򱣴�
				if (it_isEndState != dfa.endState.end()) {
					tokenList.push_back(make_pair(it_isEndState->second, substr[i].substr(beginIndex, endIndex - beginIndex)));
					beginIndex = endIndex;
					state = dfa.beginState;
					continue;
				}

				// ʧ��
				tokenList.clear();
				tokenList.push_back(make_pair(string("ERRORTYPE"), substr[i]));
				break;
			}
			// ���ƥ��ɹ���Ѱ�ҹؼ���
			if (state == dfa.beginState) {
				for (auto it_token = tokenList.begin(); it_token != tokenList.end(); it_token++) {
					auto it_reservedWord = find(reservedWord.begin(), reservedWord.end(), it_token->second);
					if (it_reservedWord != reservedWord.end()) {
						*it_token = make_pair(*it_reservedWord, *it_reservedWord);
					}
				}

			}
			append_output(resultPath, lineNum, tokenList);
		}
		lineNum++;
	}
	vector<pair<string, string>> tmp_end;
	tmp_end.push_back(make_pair(endtoken, endtoken));
	append_output(resultPath, 0, tmp_end);
	fsource.close();
}

// ���� NFA
void Lexer::inputNFA(const char* path) {
	ifstream fin;

	fin.open(path);

	if (fin.fail()) {
		cout << "Could not open Chomsky file!" << endl;
		exit(0);
	}
	string line;
	fin >> blank >> endtoken;
	fin >> annotation;
	// �ؼ���
	fin.ignore();
	getline(fin, line);
	reservedWord = split(line, " ");

	int startNodeNum; //��ʼ�����Ŀ
	fin >> startNodeNum;

	// ���뿪ʼ���
	for (int i = 0; i < startNodeNum; i++) {
		string tmp;
		fin >> tmp;
		nfa.stateList.push_back(tmp);
		nfa.beginState.push_back(tmp);
	}
	int endNodeNum;
	fin >> endNodeNum;
	fin.ignore();
	for (int i = 0; i < endNodeNum; i++) {
		getline(fin, line);
		vector<string> s = split(line, " ");
		if (s.size() == 2) {
			nfa.endState[s[0]] = s[1];
		}
	}

	// �����ķ�
	while (getline(fin, line)) {
		if (line.size() == 0)continue;

		vector<string> substr = split(line, " ");

		// ���ķ����δ��ӵ�״̬��ӵ�����б�
		if (find(nfa.stateList.begin(), nfa.stateList.end(), substr[0]) == nfa.stateList.end()) {
			nfa.stateList.push_back(substr[0]);
		}
		// ����ڱߡ�������ż����ķ��Ҳ�δ��ӵ�״̬
		if (substr.size() == 4) {
			(nfa.adjList[substr[2][0]]).insert(make_pair(substr[0], substr[3]));
			if (find(nfa.input.begin(), nfa.input.end(), substr[2][0]) == nfa.input.end()) {
				nfa.input.push_back(substr[2][0]);
			}
			if (find(nfa.stateList.begin(), nfa.stateList.end(), substr[3]) == nfa.stateList.end()) {
				nfa.stateList.push_back(substr[3]);
			}
			// TODO: ��һ�� else ���Ӧ�ò�����
		}
		//else if (substr[2][0] == blank) {
		//	nfa.endState[substr[0]] = substr[0];
		//	
		//} else {
		//	if (nfa.endState.find("Z") == nfa.endState.end()) {
		//		nfa.endState["Z"] = "Z";
		//	}
		//	(nfa.adjList[substr[2][0]]).insert(make_pair(substr[0], string("Z")));
		//}
	}
	fin.close();
}

// move
set<string> Lexer::move(set<string>& state, char a) {
	auto it_in = nfa.adjList.find(a);
	if (it_in == nfa.adjList.end()) return state;
	set<string> nextState;
	for (auto it_ele = state.begin(); it_ele != state.end(); it_ele++) {
		for (auto it_pre = it_in->second.begin(); it_pre != it_in->second.end(); it_pre++) {
			if (it_pre->first == *it_ele && find(nextState.begin(), nextState.end(), it_pre->second) == nextState.end()) {
				nextState.insert(it_pre->second);
			}
		}
	}
	return nextState;
}

// ��հ�
void Lexer::enClousure(set<string>& state) {
	auto it_en = nfa.adjList.find(blank);
	if (it_en == nfa.adjList.end()) return;

	vector<string> v;
	int index = 0;
	for (auto it = state.begin(); it != state.end(); it++) {
		v.push_back(*it);
	}
	while (index < v.size()) {
		string tmp = v[index];
		index++;
		for (auto it_pre = it_en->second.begin(); it_pre != it_en->second.end(); it_pre++) {
			if (it_pre->first == tmp && find(v.begin(), v.end(), it_pre->second) == v.end()) {
				v.push_back(it_pre->second);
				state.insert(it_pre->second);
			}
		}
	}
}

// ��С�� DFA
void Lexer::NFAtoDFA() {
	vector<set<string>> subSet;
	// ���п�ʼ���
	set<string> beginState;
	for (auto it = nfa.beginState.begin(); it != nfa.beginState.end(); it++) {
		beginState.insert(*it);
	}
	enClousure(beginState);
	dfa.beginState = 0;
	dfa.stateList.push_back(0);
	subSet.push_back(beginState);

	vector<int> isTested(subSet.size(), 0);
	for (int i = 0; i < subSet.size(); i++) {

		if (isTested[i]) continue;
		isTested[i] = 1;
		set<string> tmp;
		for (auto it_input = nfa.input.begin(); it_input != nfa.input.end(); it_input++) { // ����ÿ�������ַ�
			if (*it_input == blank)continue;// ȥ���հ�����

			if (i == 0) dfa.input.push_back(*it_input);

			tmp = move(subSet[i], *it_input);
			enClousure(tmp);
			if (tmp.size() == 0)continue;

			auto it_tmp = find(subSet.begin(), subSet.end(), tmp);
			int index = it_tmp - subSet.begin();
			if (it_tmp == subSet.end()) { // δ�� subSet ��
				subSet.push_back(tmp);
				isTested.push_back(0);

				index = subSet.size() - 1;
				// �Ƿ�����̬
				for (auto it = tmp.begin(); it != tmp.end(); it++) {
					auto it_key = nfa.endState.find(*it);
					if (it_key != nfa.endState.end()) {
						dfa.endState[index] = it_key->second;
						break;
					}
				}
				dfa.stateList.push_back(index);
			}

			dfa.adjList[*it_input][i] = index;
		}
	}
}
