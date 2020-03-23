#pragma once
#include <string>
#include <vector>
#include <map>
using namespace std;
class DFA {

public:
	vector<int> stateList;
	map<int, string> endState;
	int beginState;
	vector<char> input;
	map<char, map<int, int>> adjList;
};
class NFA
{

public:
	vector<string> beginState;
	map<string, string> endState;
	vector<string> stateList;
	vector<char> input;
	map<char, multimap<string, string>> adjList; //  ‰»Î ≥ıÃ¨ ÷’Ã¨
};
