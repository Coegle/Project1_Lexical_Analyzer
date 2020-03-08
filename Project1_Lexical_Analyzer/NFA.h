#pragma once
#include <vector>
#include <map>
#include <string>
using namespace std;
class NFA
{

public:
	vector<string> beginState;
	map<string,string> endState;
	vector<string> stateList;
	vector<char> input;
	map<char, multimap<string, string>> adjList; //  ‰»Î ≥ıÃ¨ ÷’Ã¨
};

