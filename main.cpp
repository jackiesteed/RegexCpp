#include <iostream>
#include "Parser.h"

using namespace std;

int main()
{
    cout << "Hello world!" << endl;
    Parser parser;
    Node* root = parser.Parse("p|a(c*|.f++)+(cd|F|W|C)**");
    parser.Dump(root, MID);
    root = parser.Parse("a|b|c|d+c*");
    parser.Dump(root, POST);
    return 0;
}


