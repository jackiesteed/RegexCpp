/************************************************************************/
/* <By Jackie 2013-7-25>
利用正则表达式的解析树, 创建出一个NFA自动机, 能处理基本的正则表达式匹配
/************************************************************************/


#ifndef __NFA_H__
#define __NFA_H__

namespace RegexCpp
{
	typedef struct State
	{
		int code;		//状态码, 一般是自动机中边上的字符, 特殊情况是S(开始), T(结束), FAKE(伪节点,导出空转移的边)
		State* out;		//出边1
		State* out1;	//出边2
		int lastList;	//记录上一次所在的链表, 判重用的.
		int id;			// <By张方雪 2013-5-27>这个id用来提示, 表示是哪个节点.

		void Init(int code, State* out, State* out1)
		{
			this->code = code;
			this->out = out;
			this->out1 = out1;
			this->lastList = -1;
		}

		void Init()
		{
			code = FAKE;
			out = out1 = NULL;
			lastList = -1;
		}
		void Init(int code)
		{
			this->code = code;
			out = out1 = NULL;
			lastList = -1;
		}

	}State;

	typedef struct Fragment
	{
		State* start; //子NFA的进入节点
		State* end;   //子NFA的出节点

		void Init()
		{
			start = end = NULL;
		}
		void Init(State* start, State* end)
		{
			this->start = start;
			this->end = end;
		}
	}Fragment;

	typedef struct StateList
	{
		State* list[MAXN];
		int nStates;
	}StateList;

	class Automata
	{
	public:
		Automata();
		~Automata();
		Fragment* CreateAutomataWithFakeStates(Node* root); //从表达式树转化成NFA的图, 图只有一个入口, 所以用一个State* 表示即可
		State* CreateAutomata(Node* root);
		bool Match(State* start, char* text);
		void Dump(State* state);
		void Dump(bool showFakeStates = true);
		void SetDebug(bool debug){_debug = debug;}

	private:
		State* _state_buffer;
		Fragment* _frag_buffer;
		int _used_state;
		int _used_frag;
		int _list_id;
		bool _debug;
		int _text_start_pos; //By Jackie 2013-12-2  记录当前是从text的什么位置开始匹配, 给^符号使用
		int _text_end_pos;
		char* _text;
		StateList* _current_list;
		StateList* _next_list;
		State* NewState(){return &_state_buffer[_used_state++];}
		Fragment* NewFragment(){return &_frag_buffer[_used_frag++];}
		void ResetStates(State* start);
		void DumpStateList(StateList* list);
		bool IsMatch(StateList* list);
		void Step(int code);
		void AddState(StateList* list, State* state);
		void PrintCode(int code);

	};
}

#endif