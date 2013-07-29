/************************************************************************/
/* <By Jackie 2013-7-25>
	自动机里面需要用到的一些常量.*/
/************************************************************************/



#ifndef __SYMBOL_H__
#define __SYMBOL_H__


enum TokenType
{
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
	ALL_NUM,
	NON_NUM,
	ALL_ALNUM,
	NON_ALNUM,

};

extern char TokenString[100][100];


enum DumpOrder
{
	PRE, MID, POST
};

#define S -1
#define T -2
#define FAKE -3

#endif // __SYMBOL_H__
