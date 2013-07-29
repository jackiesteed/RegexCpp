#include <iostream>
#include "Parser.h"
#include "NFA.h"

using namespace std;

int main(int argc, char* argv[])
{
	bool cmdline = true;
	char* regexStr = NULL;
	char* text = NULL;
	if(cmdline)
	{
		if(argc != 3)
		{
			cout << "RegexCpp <regexStr> <text>" << endl;
			return -1;
		}
		regexStr = argv[1];
		text = argv[2];
	}
	else
	{
		regexStr = new char[MAXN];
		strcpy(regexStr, "\\Db?c");
		text = new char[MAXN];
		strcpy(text, "cbc");
	}


	Parser parser;

	Node* root = parser.Parse(regexStr);

	parser.Dump(root, POST);
	NFA nfa;
	State* start = nfa.CreateNFA(root);

	int pos =  nfa.Match(start, text);
	char res[MAXN] = {0};
	if(pos == -1) cout << "Not matched!" << endl;
	else 
	{
		cout << "Matched : ";
		for(int i = 0; i <= pos; i++) cout << text[i];
		cout << endl;
	}
	if(!cmdline)
	{
		system("pause");
	}
	return 0;
}
