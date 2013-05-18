#ifndef __SYMBOL_H__
#define __SYMBOL_H__


enum TokenType
{
    EOF = -1,
    START = 0,//^
    PLUS, //+
    STAR,//*
    BRACKET, //()
    SQUARE, //[]
    QUESTION, // ?
    END, //$
    CONCAT, //.
    ALT, //|
    DOT, //
    ALPHA, //
    NUM,
};

enum DumpOrder
{
    PRE, MID, POST
};


#endif // __SYMBOL_H__
