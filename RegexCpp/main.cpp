#include "StdAfx.h"
#include <iostream>
#include <Windows.h>
#include <fstream>
#include <cstdio>
#include "Parser.h"
#include "Automata.h"
#include "BruteForce.h"
#include "ConsoleColor.h"

using namespace std;

using namespace RegexCpp;



int main(int argc, char* argv[])
{

	cout << "=============================================================================="<< endl;
	cout << "				 RegexCpp" << endl;
	cout << "  0. exit: Quit the program" << endl;
	cout << "  1. match <pattern> <text>: Do regex string matching" << endl;
	cout << "  2. debug <on|off>: Show running log or not, default not" << endl;
	cout << "  3. clear: Clear the screen" << endl;
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
				cout << red << "Error parsing the pattern!!" << flushcolor;
				continue;
			}
			
			State* start = automata.CreateAutomata(root);
			bool bingo = automata.Match(start, text);
		}
		else if(0 == strcmp(cmd, "debug"))
		{
			para = strtok(NULL, " ");
			if(para == NULL)
			{
				if(parser.GetDebug())
					cout << yellow << "Debug mode is on " << flushcolor;
				else 
					cout << yellow << "Debug mode is off " << flushcolor;
				continue;
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
				cout << red << "---------------------- Invalid parameters for [debug] command-------------------" << flushcolor;	
			}
		}
		else if(0 == strcmp(cmd, "clear"))
		{
			system("cls");
		}
		else
		{
			cout << red << "--------------------------------Unknown command-------------------------------" << flushcolor;	
		}
	}

	return 0;
}
