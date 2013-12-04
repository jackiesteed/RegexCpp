#include "StdAfx.h"
#include <iostream>
#include <Windows.h>
#include <fstream>
#include <cstdio>
#include "Parser.h"
#include "Automata.h"
#include "BruteForce.h"

using namespace std;

using namespace RegexCpp;



int main(int argc, char* argv[])
{
	//cout << match("a*", "aaaaaa") << endl;
	//system("pause");

	cout << "=============================================================================="<< endl;
	cout << "				 RegexCpp" << endl;
	cout << "  0. exit: Quit the program" << endl;
	cout << "  1. match <pattern> <text>: Do regex string matching" << endl;
	cout << "  2. dump <pattern>: Do the syntax parsing for the regex string" << endl;
	cout << "  3. debug <on|off>: Show running log or not" << endl;
	cout << "  4. cls: Clear the screen" << endl;
	cout << "==============================================================================" << endl;
	char* cmdline = new char[MAXN];
	char* cmd;
	char* pattern;
	char* text;
	char* para;
	Parser parser;
	Automata automata;

	while(true)
	{
		cout << ">>>>";
		cin.getline(cmdline, MAXN);

		cmd = strtok(cmdline, " ");

		if(0 == strcmp(cmd, "exit")) 
		{
			return 0;
		}
		if(0 == strcmp(cmd, "match"))
		{
			pattern = strtok(NULL, " ");
			text = strtok(NULL, " ");
			if(pattern == NULL || text == NULL)
			{
				cout << "Incomplete parameters for [match] command " << endl;
				continue;
			}
			Node* root = parser.Parse(pattern); //½âÎöÊ÷
			if(root == NULL)
			{
				cout << "Error parsing the pattern!!" << endl;
				continue;
			}
			
			State* start = automata.CreateAutomata(root);
			bool bingo = automata.Match(start, text);
		}
		else if(0 == strcmp(cmd, "dump"))
		{
			para = strtok(NULL, " ");
			if(para == NULL)
			{
				cout << "Incomplete parameters for [dump] command " << endl;
			}
			Node* root = parser.Parse(para); //½âÎöÊ÷
			cout << "====================== Dump the syntax tree ======================" << endl;
			parser.Dump(root);
			cout << "==================================================================" << endl;
			State* start = automata.CreateAutomata(root);
			cout << "============= Dump the automata with fake states =================" << endl;
			automata.Dump();
			cout << "==================================================================" << endl;
		}
		else if(0 == strcmp(cmd, "debug"))
		{
			para = strtok(NULL, " ");
			if(para == NULL)
			{
				cout << "Incomplete parameters for [debug] command " << endl;
			}
			if(0 == strcmp(para, "on"))
			{
				parser.SetDebug(true);
				automata.SetDebug(true);

			}
			else if(0 == strcmp(para, "off"))
			{
				parser.SetDebug(false);
				automata.SetDebug(false);
			}
			else
			{
				cout << "Unknown command" << endl;
			}
		}
		else if(0 == strcmp(cmd, "cls"))
		{
			system("cls");
		}
		else
		{
			cout << "Unknown command" << endl;
		}
		
	}

	return 0;
}
