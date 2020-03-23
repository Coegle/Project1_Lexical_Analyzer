#include "DFA_NFA.h"
#include "Lexer.h"
using namespace std;

int main() {
	Lexer analysis;
	analysis.inputNFA("Chomsky_III.txt");
	analysis.NFAtoDFA();
	analysis.analyze("testsource.txt", "testresult.txt");
	//analysis.analyze("source.txt", "..\\..\\.\\Project2_Syntactic_Analyzer\\Project2_Syntactic_Analyzer\\Project2_Syntactic_Analyzer\\result.txt");
	return 0;
}