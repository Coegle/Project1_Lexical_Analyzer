#include <iostream>
#include <fstream>
#include <algorithm>
#include <set>
#include <queue>
#include "NFA.h"
#include "DFA.h"
using namespace std;
class analyzer {

public:
	NFA nfa;
	DFA dfa;
	char blank;
	string reservedWordLabel;
	vector<string> reservedWord;
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

// 输出
void append_output(const char* resultPath, int lineNum, vector<pair<string, string>>& tokenList) {
	ofstream fresult(resultPath, fstream::out|fstream::app);
	for (auto it = tokenList.begin(); it != tokenList.end(); it++) {
		fresult << lineNum << " " << it->first << " " << it->second << endl;
	}
	fresult.close();
}

// 对源代码处理
void test(analyzer& analysis, const char* sourcePath, const char* resultPath) {
	ifstream fsource(sourcePath);

	if (fsource.fail()) {
		cout << "Could not open source file!" << endl;
		exit(0);
	}

	int lineNum = 1;
	string line;
	while (getline(fsource, line)) {//每行
		vector<string> substr = split(line, " ");

		for (int i = 0; i < substr.size(); i++) { // 每个「单词」

			vector<pair<string, string>> tokenList; // tokenType token
			bool isSuccess = true;

			int beginIndex = 0;
			int endIndex = 0;

			int state = analysis.dfa.beginState;

			while (endIndex < substr[i].length()) { 
				auto it_input = analysis.dfa.adjList.find(substr[i][endIndex]);
				auto it_prestate = it_input->second.find(state);
				// 找到下一个状态
				if (it_input != analysis.dfa.adjList.end() && it_prestate != it_input->second.end()) {
					state = (it_input->second)[state];
					endIndex++;
					continue;
				}
				
				auto it_isEndState = analysis.dfa.endState.find(state);

				// 当前处于结束状态且无法继续匹配，则保存
				if (it_isEndState != analysis.dfa.endState.end()) { 
					tokenList.push_back(make_pair(it_isEndState->second, substr[i].substr(beginIndex, endIndex - beginIndex)));
					beginIndex = endIndex;
					endIndex++;
					continue;
				}

				// 失败
				tokenList.clear();
				tokenList.push_back(make_pair(string("ERRORTYPE"), substr[i]));
				break;
			}
			// 整个单词都匹配
			if (analysis.dfa.endState.find(state) != analysis.dfa.endState.end() && tokenList.size() == 0) {

				auto it_reservedWord = find(analysis.reservedWord.begin(), analysis.reservedWord.end(), substr[i].substr(beginIndex, endIndex - beginIndex));
				
				if (it_reservedWord != analysis.reservedWord.end()) {
					tokenList.push_back(make_pair(analysis.reservedWordLabel, *it_reservedWord));
				} else {
					tokenList.push_back(make_pair(analysis.dfa.endState[state], substr[i].substr(beginIndex, endIndex - beginIndex)));
				}
			}
			append_output(resultPath,lineNum, tokenList);
		}
		lineNum++;
	}

	fsource.close();
}

// 读入 NFA
void input(analyzer& analysis, const char* path) {
	ifstream fin;

	fin.open(path);

	if(fin.fail()){
		cout << "Could not open Chomsky file!" << endl;
		exit(0);
	}
	string line;
	fin >> analysis.blank >> analysis.reservedWordLabel;

	// 关键字
	fin.ignore();
	getline(fin, line);
	analysis.reservedWord = split(line, " ");

	int startNodeNum; //开始结点数目
	fin >> startNodeNum;

	// 读入开始结点
	for (int i = 0; i < startNodeNum; i++) {
		string tmp;
		fin >> tmp;
		analysis.nfa.stateList.push_back(tmp);
		analysis.nfa.beginState.push_back(tmp);
	}
	fin.ignore();

	// 读入文法
	while(getline(fin, line)) {		
		if (line.size() == 0)continue;

		vector<string> substr = split(line, " ");
		
		// 将文法左侧未添加的状态添加到结点列表
		if (find(analysis.nfa.stateList.begin(), analysis.nfa.stateList.end(), substr[0]) == analysis.nfa.stateList.end()) {
			analysis.nfa.stateList.push_back(substr[0]);
		}
		// 添加邻边、输入符号集、文法右侧未添加的状态
		if (substr.size() == 4) {
			(analysis.nfa.adjList[substr[2][0]]).insert(make_pair(substr[0], substr[3]));
			if (find(analysis.nfa.input.begin(), analysis.nfa.input.end(), substr[2][0]) == analysis.nfa.input.end()) {
				analysis.nfa.input.push_back(substr[2][0]);
			}
			if (find(analysis.nfa.stateList.begin(), analysis.nfa.stateList.end(), substr[3]) == analysis.nfa.stateList.end()) {
				analysis.nfa.stateList.push_back(substr[3]);
			}
			
		} else if (substr[2][0] == analysis.blank) {
			analysis.nfa.endState[substr[0]] = substr[0];
			// TODO: 下一个 else 语句应该不存在
		} else {
			if (analysis.nfa.endState.find("Z") == analysis.nfa.endState.end()) {
				analysis.nfa.endState["Z"] = "Z";
			}
			(analysis.nfa.adjList[substr[2][0]]).insert(make_pair(substr[0], string("Z")));
		}
	}
	fin.close();
}

// move
set<string> move(analyzer& analysis, set<string>& state, char a) {
	auto it_in = analysis.nfa.adjList.find(a);
	if (it_in == analysis.nfa.adjList.end()) return state;
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

// 求闭包
void enClousure(analyzer& analysis, set<string>& state) {
	auto it_en = analysis.nfa.adjList.find(analysis.blank);
	if (it_en == analysis.nfa.adjList.end()) return;

	vector<string> v;
	int index = 0;
	for (auto it = state.begin(); it != state.end(); it++) {
		v.push_back(*it);
	}
	while (index < v.size()) {
		string tmp = v[index];
		index++;
		for (auto it_pre = it_en->second.begin(); it_pre != it_en->second.end(); it_pre++) {
			if (it_pre->first == tmp && find(v.begin(),v.end(),it_pre->second) == v.end()) {
				v.push_back(it_pre->second);
				state.insert(it_pre->second);
			}
		}
	}
}

// 最小化 DFA
void NFAtoDFA(analyzer& analysis) {
	vector<set<string>> subSet;
	// 所有开始结点
	set<string> beginState;
	for (auto it = analysis.nfa.beginState.begin(); it != analysis.nfa.beginState.end(); it++) {
		beginState.insert(*it);	
	}
	enClousure(analysis, beginState);
	analysis.dfa.beginState = 0;
	analysis.dfa.stateList.push_back(0);
	subSet.push_back(beginState);

	vector<int> isTested(subSet.size(), 0);
	for (int i = 0; i < subSet.size(); i++) {
		
		if (isTested[i]) continue;
		isTested[i] = 1;
		set<string> tmp;
		for (auto it_input = analysis.nfa.input.begin(); it_input != analysis.nfa.input.end(); it_input++) { // 对于每个输入字符
			if (*it_input == analysis.blank)continue;// 去掉空白输入

			if (i == 0)analysis.dfa.input.push_back(*it_input);
	
			tmp = move(analysis, subSet[i], *it_input);
			enClousure(analysis, tmp);
			if (tmp.size() == 0)continue;

			auto it_tmp = find(subSet.begin(), subSet.end(), tmp);
			int index = it_tmp - subSet.begin();
			if (it_tmp == subSet.end()) { // 未在 subSet 中
				subSet.push_back(tmp);
				isTested.push_back(0);

				index = subSet.size() - 1;
				// 是否是终态
				for (auto it = tmp.begin(); it != tmp.end(); it++) {
					auto it_key = analysis.nfa.endState.find(*it);
					if (it_key != analysis.nfa.endState.end()) {
						analysis.dfa.endState[index] = it_key->second;
						break;
					}
				}			
				analysis.dfa.stateList.push_back(index);
			}

			analysis.dfa.adjList[*it_input][i] = index;
		}
	}
}

int main() {
	analyzer analysis;
	input(analysis, "NFAtest.txt");
	NFAtoDFA(analysis);
	test(analysis, "source.txt", "result.txt");
	return 0;
}