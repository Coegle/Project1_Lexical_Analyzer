#pragma once
#include <string>
#include <vector>
#include <map>
using namespace std;
class DFA {

public:
	string token;
	vector<string> stateList;
	vector<string> endState;
	string beginState;
	vector<char> input;
	map<string, map<string, string>> adjList;
};
