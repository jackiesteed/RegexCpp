#ifndef __PARSER_H__
#define __PARSER_H__

#include <cstdlib>
#include <ctype.h>
#include <cstring>
#include <stack>
#include "Symbol.h"

using namespace std;

#define MAXN 1000000

typedef struct Node
{
    TokenType token;
    int subType; //<By张方雪 2013-5-18>用于处理一种token会对应多种的情形.
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
    void Dump(Node* root, DumpOrder order);

private:
    Node* _buffer;
    int _used;
    int _pos;
    char* _regex;
    int _len;

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

    Node* Parse();// recursive implementation
    Node* ParseAlt();
    Node* ParseConcat();
    Node* ParsePlus();
    Node* ParseBracket();
    Node* ParseDot();
    Node* ParseAtom();
    Node* ParseMulti();

    void DumpMidOrder(Node* root);
    void DumpPostOrder(Node* root);

};















#endif // __PARSER_H__
