/************************************************************************/
/* <By Jackie 2013-7-25>
Parse解析正则表达式的字符串, 最终生成一个节点树	
目前支持的符号有 |.?*+()\d\D\w\W等
*/
/************************************************************************/
#include "StdAfx.h"
#include <iostream>
#include "Parser.h"

using namespace std;

namespace RegexCpp{

	Parser::Parser()
	{
		_used = 0;
		_buffer = new Node[MAXN];
		_pos = 0;
		_regex = new char[MAXN];
	}
	Parser::~Parser()
	{
		delete []_buffer;
	}


	Node* Parser::Parse(char* regexStr)
	{
		strcpy(_regex, regexStr);
		_len = strlen(regexStr);
		_pos = 0;
		_used = 0;
		return Parse();
	}

	//Parse基本就是一个设定好优先级的递归调用
	Node* Parser::Parse()
	{
		Node* root =  ParseAlt();
		if(CurrentToken() != -1)
		{
			cout << "Parsing failure, exit unexpectedly at position " << _pos << endl;
			return NULL;
		}
		return root;
	}
	Node* Parser::ParseStart()
	{
		if(CurrentToken() != '^') return ParseConcat();
		NextToken();
		Node* root = ParseConcat();
		Node* leftChild = root;
		root = NewNode();
		root->Init(START, 0, leftChild, NULL);
		return root;
	}

	Node* Parser::ParseAlt()
	{
		Node* root = ParseConcat();
		char token = CurrentToken();

		if(token == '|')
		{
			NextToken();
			Node* leftChild = root;
			Node* rightChild = ParseAlt();
			root = NewNode();
			root->Init(ALT, 0, leftChild, rightChild);
		}

		return root;
	}

	Node* Parser::ParseConcat()
	{
		char token = CurrentToken();

		//ParseStart here
		if(token == '^')
		{
			return ParseStart();
		}

		Node* root = ParseMulti();
		token = CurrentToken();
		if(isalnum(token) || token == '(' || token == '.')
		{
			Node* leftChild = root;
			Node* rightChild = ParseConcat();
			root = NewNode();
			root->Init(CONCAT, 0, leftChild, rightChild);
		}
		return root;
	}

	Node* Parser::ParseMulti() //单目运算符, 算子是左儿子
	{
		Node* root = ParseBracket();
		char token = CurrentToken();
		while(token == '+' || token == '*' || token == '?')
		{
			Node* leftChild = root;
			root = NewNode();
			switch(token)
			{
			case '+':
				root->Init(PLUS, 0, leftChild, NULL);
				break;
			case '?':
				root->Init(QUESTION, 0, leftChild, NULL);
				break;
			case '*':
				root->Init(STAR, 0, leftChild, NULL);
				break;
			}
			NextToken();
			token = CurrentToken();
		}
		return root;
	}

	Node* Parser::ParseBracket()
	{
		Node* root = NULL;
		char token = CurrentToken();
		if(token == '(')
		{
			NextToken();
			root = ParseAlt();
			if(CurrentToken() != ')')
			{
				cout << "Error parsing  bracket at position " << _pos << endl;
				return NULL;
			}
			NextToken();
		}
		else
		{
			root = ParseSlash();
		}
		return root;
	}


	//<By张方雪 2013-5-27>目前先, 只支持\D \d \w \W
	Node* Parser::ParseSlash()
	{
		Node* root = NULL;
		char token = CurrentToken();

		root = NewNode();
		if(token == '\\')
		{
			NextToken();
			token = CurrentToken();

			switch(token)
			{
			case 'd':
				{
					root->Init(ALL_NUM, 0, NULL, NULL);
					break;
				}
			case 'D':
				{
					root->Init(NON_NUM, 0, NULL, NULL);
					break;
				}
			case 'w':
				{
					root->Init(ALL_ALNUM, 0, NULL, NULL);
					break;
				}
			case 'W':
				{
					root->Init(NON_ALNUM, 0, NULL, NULL);
					break;
				}
			default:
				{
					cout << "Invalid operator at position " << _pos << endl;
					return NULL;
				}
			}
			NextToken();
		}
		else
		{
			root = ParseAtom();
		}

		return root;
	}

	Node* Parser::ParseAtom()
	{
		Node* root = NULL;
		char token = CurrentToken();

		if(isalpha(token))
		{
			root = NewNode();
			root->Init(ALPHA, token, NULL, NULL);
			NextToken();
			return root;
		}
		else if(isalnum(token))
		{
			root = NewNode();
			root->Init(NUM, token, NULL, NULL);
			NextToken();
			return root;
		}
		else if(token == '.')
		{
			root = NewNode();
			root->Init(DOT, 0, NULL, NULL);
			NextToken();
			return root;
		}
		else
		{
			cout << "Unknown token at position " << _pos << endl;
			return NULL;
		}


	}

	void Parser::DumpPostOrder(Node* root)
	{
		if(root == NULL) return;
		switch(root->token)
		{
		case ALPHA:
		case NUM:
			{
				cout << char(root->subType) ;
				break;
			}
		case PLUS:
			{
				DumpPostOrder(root->left);
				cout << "+";
				break;
			}
		case STAR:
			{
				DumpPostOrder(root->left);
				cout << "*";
				break;
			}
		case QUESTION:
			{
				DumpPostOrder(root->left);
				cout << "?";
				break;
			}
		case CONCAT:
			{
				DumpPostOrder(root->left);
				DumpPostOrder(root->right);
				cout << "'"; //<By张方雪 2013-5-18>先用这个当连接符吧.
				break;
			}
		case DOT:
			{
				cout << ".";
				break;
			}
		case ALT:
			{
				DumpPostOrder(root->left);
				DumpPostOrder(root->right);
				cout << "|";
				break;
			}
		case START:
			{
				DumpPostOrder(root->left);
				cout << "^";
				break;
			}
		default:
			{
				cout << "{Invalid token: " << root->token << "}" << endl;
				return;
			}
		}
	}

	void Parser::Dump(Node* root)
	{
		DumpPostOrder(root);
		cout << endl;
	}
}

