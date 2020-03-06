#pragma once
#include <vector>
#include <map>
#include <string>
using namespace std;
class NFA
{

public:
	vector<string> beginState;
	vector<string> endState;
	vector<string> stateList;
	vector<string> input;
	map<string, map<string, string>> adjList; //  ‰»Î ≥ıÃ¨ ÷’Ã¨
};

