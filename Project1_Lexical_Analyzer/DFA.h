#pragma once
#include <string>
#include <vector>
#include <map>
using namespace std;
class DFA {

public:
	string token;
	vector<int> stateList;
	map<int, string> endState;
	int beginState;
	vector<char> input;
	map<char, map<int, int>> adjList;
};
