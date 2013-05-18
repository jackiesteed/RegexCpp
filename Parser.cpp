#include <iostream>
#include "Parser.h"


using namespace std;


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

Node* Parser::Parse()
{
    return ParseAlt();
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
    Node* root = ParseMulti();
    char token = CurrentToken();

    if(isalnum(token) || token == '(')
    {
        Node* leftChild = root;
        Node* rightChild = ParseConcat();
        root = NewNode();

        root->Init(CONCAT, 0, leftChild, rightChild);
//        Dump(root, MID);
    }

    return root;
}

Node* Parser::ParseMulti()
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
            cout << "Error " <<  CurrentToken() << endl;
            return NULL;

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
        cout << "出现未能处理的token" << endl;
        return NULL;
    }


}

void Parser::DumpMidOrder(Node* root)
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
        cout << "(";
        DumpMidOrder(root->left);
        cout << ")+";
        break;
    }
    case STAR:
    {
        cout << "(";
        DumpMidOrder(root->left);
        cout << ")*";
        break;
    }
    case QUESTION:
    {
        cout << "(";
        DumpMidOrder(root->left);
        cout << ")?";
        break;
    }
    case CONCAT:
    {
        DumpMidOrder(root->left);
        DumpMidOrder(root->right);
        break;
    }
    case DOT:
    {
        cout << ".";
        break;
    }
    case ALT:
    {
        DumpMidOrder(root->left);
        cout << "|";
        DumpMidOrder(root->right);
        break;
    }
    default:
    {
        cout << "非法的符号: " << root->token <<  endl;
        return;
    }
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
        cout << "$"; //<By张方雪 2013-5-18>先用这个当连接符吧.
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
    default:
    {
        cout << "非法的符号: " << root->token <<  endl;
        return;
    }
    }
}

void Parser::Dump(Node* root, DumpOrder order)
{
    if(order == MID)
    {
        DumpMidOrder(root);
    }
    else if(order == POST)
    {
        DumpPostOrder(root);
    }

    cout << endl;
}






