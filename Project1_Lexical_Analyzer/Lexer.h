#pragma once
#include "DFA_NFA.h"
#include <set>
class Lexer {

public:
	NFA nfa;
	DFA dfa;
	char blank; // 表示文法中的空
	string endtoken;
	string annotation; // 表示注释开始
	vector<string> reservedWord;
	set<string> move(set<string>& state, char a);
	void enClousure(set<string>& state);
	void NFAtoDFA();
	void analyze(const char* sourcePath, const char* resultPath);
	void inputNFA(const char* path);
	void append_output(const char* resultPath, int lineNum, vector<pair<string, string>>& tokenList);
};

