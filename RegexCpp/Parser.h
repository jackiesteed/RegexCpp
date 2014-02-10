/************************************************************************
<By Jackie 2013-7-25>
语法解析实现逻辑, 主要用途, 1, 为构造NFA提供语法解析树, 2, 检查pattern语法是否正确
/************************************************************************/

#ifndef __PARSER_H__
#define __PARSER_H__

#include <cstdlib>
#include <ctype.h>
#include <cstring>
#include "Symbol.h"

using namespace std;

#define MAXN 1000000

namespace RegexCpp
{

	typedef struct Node
	{
		TokenType token;
		int subType;			//<By张方雪 2013-5-18>用于处理一种token会对应多种的情形.
		Node* left;
		Node* right;

		void Init(TokenType tok, int subT, Node* l, Node* r)
		{
			token = tok;
			subType = subT;
			left = l;
			right = r;
		}
	}Node;


	class Parser
	{
	public:
		Parser();
		~Parser();
		Node* Parse(char* regexStr);
		void Dump(Node* root);
		void SetDebug(bool debug){_debug = debug;}
		bool GetDebug(){return _debug;}

	private:
		Node* _buffer;
		int _used;
		int _pos;
		char* _regex;
		int _len;
		bool _debug;
		bool _error;

		Node* NewNode()
		{
			return &_buffer[_used++];
		}
		char CurrentToken()
		{
			if(_pos >= _len) return -1;
			return _regex[_pos];
		}
		void NextToken()
		{
			_pos++;
		}

		bool Validate(); //对于正则表达式的正确性合法性进行校验, 比如括号是否匹配

		Node* Parse();				// recursive implementation
		Node* ParseAlt();			// 解析|号
		Node* ParseConcat();		// 连接符号解析, 连接符号是隐式的
		Node* ParsePlus();			// + 符号解析
		Node* ParseBracket();		//解析括号
		Node* ParseDot();			//解析.号
		Node* ParseAtom();			//解析一个元素
		Node* ParseMulti();			//解析*好
		Node* ParseSlash();			//解析\符号, 支持\d\D\w\W
		
	};
}

#endif // __PARSER_H__
