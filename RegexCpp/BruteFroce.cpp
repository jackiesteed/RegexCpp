#include "StdAfx.h"
#include "BruteForce.h"


//By Jackie 2013-11-30  如果匹配到了就反馈1, 否则0
//不支持括号, 不涉及换行问题, 仅作为暴力实现算法的一个demonstration.
//支持的符号如下:
//^ 开始符号
//* 可以匹配0至多个
//$ 结束符号, 就是pattern串必须匹配到文本的末尾才行.
//. 可以匹配任意一个符号
int match(char* regexp, char* text)
{
	if(regexp[0] == '^') return matchhere(regexp, text)  ;
	do
	{
		if(matchhere(regexp, text))
			return 1;
	}while(*text++ != '\0');

	return 0;
}

int matchhere(char* regexp, char* text)
{
	if(regexp[0] == '\0') return 1;
	if(regexp[1] == '*') return matchstar(regexp[0], regexp + 2, text)       ;
	if(regexp[0] == '$' && regexp[1] == '\0' ) return text[0] == '\0';
	if(text[0] != '\0' && (regexp[0] == '.' || regexp[0] == text[0])) return matchhere(regexp + 1, text + 1);

	return 0;
}

int matchstar(int c, char* regexp, char* text)
{
	do
	{
		if(matchhere(regexp, text)) return 1;

	}   while(*text != '\0' && (*text++ == c || c == '.'));

	return 0;
}

int matchplus(int c, char* regexp, char* text)
{
	while(*text != '\0' && (*text++ == c || c == '.'))
	{
		if(matchhere(regexp, text)) return 1;
	}
	return 0;
}