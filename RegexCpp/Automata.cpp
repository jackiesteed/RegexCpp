#include "StdAfx.h"
#include <Windows.h>
#include <iostream>
#include <cassert>
#include "Symbol.h"
#include "Parser.h"
#include "Automata.h"


using namespace std;

namespace RegexCpp
{

	Automata::Automata()
	{
		_state_buffer = new State[MAXN];
		_frag_buffer = new Fragment[MAXN];
		_used_state = 0;
		_used_frag = 0;
		_current_list = new StateList();
		_next_list = new StateList();
		_list_id = 0;
		_debug = false;

	}

	Automata::~Automata()
	{
		delete []_state_buffer;
		delete []_frag_buffer;
		delete _current_list;
		delete _next_list;
	}

	//<By张方雪 2013-5-20> 这里的NFA, 会使用许多FAKE的State节点, 为了方便, 使得一个Fragment 变成一入一出的.
	//最后, 通过一个缩图函数, 把图缩成原有的样子.


	Fragment* Automata::CreateAutomataWithFakeStates(Node* root)
	{
		switch(root->token)
		{
		case CONCAT:
			{
				Fragment* pre = CreateAutomataWithFakeStates(root->left);

				assert(!(pre->end->out != NULL && pre->end->out1 != NULL));

				Fragment* post = CreateAutomataWithFakeStates(root->right);

				if(pre->end->out == NULL)
					pre->end->out = post->start;
				else 
					pre->end->out1 = post->start;
				Fragment* frag = NewFragment();
				frag->start = pre->start;
				frag->end = post->end;
				return frag;
				break;
			}
		case ALT: //"|" 节点, 需要两个Fake State.
			{
				Fragment* up = CreateAutomataWithFakeStates(root->left);
				Fragment* down = CreateAutomataWithFakeStates(root->right);
				Fragment* frag = NewFragment();
				frag->start = NewState();
				frag->start->Init();
				frag->end = NewState();
				frag->end->Init();
				frag->start->out = up->start;
				frag->start->out1 = down->start;

				assert(!(up->end->out != NULL && up->end->out1 != NULL));
				assert(!(down->end->out != NULL && down->end->out1 != NULL));
				if(up->end->out ==NULL)
					up->end->out = frag->end;
				else 
					up->end->out1 = frag->end;
				if(down->end->out == NULL)
					down->end->out = frag->end;
				else 
					down->end->out1 = frag->end;
				return frag;
			}
		case QUESTION:
			{
				Fragment* core = CreateAutomataWithFakeStates(root->left);
				Fragment* frag = NewFragment();
				frag->start = NewState();
				frag->start->Init();
				frag->end = NewState();
				frag->end->Init();
				frag->start->out = core->start;
				frag->start->out1 = frag->end;
				assert(!(core->end->out != NULL && core->end->out1 != NULL));
				if(core->end->out == NULL)
					core->end->out = frag->end;
				else  
					core->end->out1 = frag->end;
				return frag;
				break;
			}
		case STAR:
			{
				Fragment* core = CreateAutomataWithFakeStates(root->left);
				Fragment* frag = NewFragment();
				frag->start = frag->end = NewState();
				frag->start->Init();
				frag->start->out = core->start;
				assert(!(core->end->out != NULL && core->end->out1 != NULL));
				if(core->end->out == NULL)
					core->end->out = frag->start;
				else 
					core->end->out1 = frag->start;
				return frag;
				break;
			}
		case PLUS:
			{
				Fragment* core = CreateAutomataWithFakeStates(root->left);
				Fragment* frag = NewFragment();
				frag->start = core->start;
				frag->end = NewState();
				frag->end->Init();
				frag->end->out = frag->start;
				frag->end->out1 = NULL;

				assert(!(core->end->out != NULL && core->end->out1 != NULL));
				if(core->end->out == NULL)
					core->end->out = frag->end;
				else 
					core->end->out1 = frag->end;
				return frag;
				break;
			}
		case NUM: 
		case ALPHA:
			{
				Fragment* frag = NewFragment();
				frag->Init();
				State* core = NewState();
				core->Init(root->subType);
				frag->start = core;
				frag->end = core;
				return frag;
				break;
			}
		case DOT:
		case ALL_NUM:
		case NON_NUM:
		case ALL_ALNUM:
		case NON_ALNUM:
			{
				Fragment* frag = NewFragment();
				frag->Init();
				State* core = NewState();
				core->Init(root->token);
				frag->start = core;
				frag->end = core;
				return frag;
			}
		case START:
			{
				Fragment* core = CreateAutomataWithFakeStates(root->left);
				State* start = NewState();
				start->Init(START);
				start->out = core->start;
				Fragment* frag = NewFragment();
				frag->Init();
				frag->start = start;
				frag->end = core->end;
				return frag;
			}
		}

		return NULL;
	}

	void Automata::ClearFakeStates(State* state)
	{
		if(state == NULL) return;
		if(state->cleared) return;
		state->cleared = true; //By Jackie 2013-12-3 为什么需要标记? 因为有向图中存在环

		State* out = state->out;
		State* out1 = state->out1;
		ClearFakeStates(out);
		ClearFakeStates(out1);
		int cnt = 2;
		if(NULL == out) cnt--;
		if(NULL == out1) cnt--;
		if(cnt > 1) return;
		if(out != NULL && out->code == FAKE)
		{
			state->out = out->out;
			state->out1 = out->out1;
			return;
		}
		if(out1 != NULL && out1->code == FAKE)
		{
			state->out = out1->out;
			state->out1 = out1->out1;
			return;
		}
	}

	//By Jackie 2013-12-1 主程入口.
	State* Automata::CreateAutomata(Node* root) 
	{
		_used_frag = 0;
		_used_state = 0;
		Fragment* frag = CreateAutomataWithFakeStates(root);
		State* start = NewState();
		start->Init(S);
		State* end = NewState();
		end->Init(T);
		start->out = frag->start;
		if(frag->end->out == NULL)
			frag->end->out = end;
		else 
			frag->end->out1 = end;
		
		if(_debug) 
		{
			cout << "============= Dump the automata with fake states =================" << endl;
			Dump();
			cout << "==================================================================" << endl;
		}
		/*ClearFakeStates(start);
		if(_debug) 
		{
			cout << "============ Dump the automata without fake states ===============" << endl;
			Dump(false);
			cout << "==================================================================" << endl;
		}*/
		return start;
	}

	bool Automata::IsMatch(StateList* list)
	{
		for(int i = 0; i < list->nStates; i++)
		{
			if(list->list[i]->code == T)
				return true;
		}
		return false;
	}

	void Automata::Step(int code)
	{
		_next_list->nStates = 0;
		_list_id++;

		for(int i = 0; i < _current_list->nStates; i++)
		{
			State* state = _current_list->list[i];
			bool bingo = false;
			if(state->code == code) bingo = true;
			else if(state->code == DOT) bingo = true;
			else if(state->code == ALL_NUM && code >= '0' && code <= '9') bingo = true;
			else if(state->code == ALL_ALNUM && isalnum(code)) bingo = true;
			else if(state->code == NON_NUM && !(code >= '0' && code <= '9')) bingo = true;
			else if(state->code == NON_ALNUM && !isalnum(code)) bingo = true;
			if(bingo)
			{
				AddState(_next_list, state->out);
				AddState(_next_list, state->out1);
			}
		}

	}

	//<By张方雪 2013-5-27>这个递归的添加状态, 保证每次都会把有效状态加进来.
	void Automata::AddState(StateList* list, State* state)
	{
		if(state == NULL || state->lastList == _list_id)
		{
			return;
		}
		state->lastList = _list_id;

		if(state->code == START && _text_pos != 0) return;

		if(state->code == FAKE || state->code == START)
		{
			AddState(list, state->out);
			AddState(list, state->out1);
			return ;
		}
		list->list[list->nStates++] = state	;
	}

	void Automata::ResetStates(State* start)
	{
		if(start->lastList == -1) return;
		start->lastList = -1;
		if(start->out != NULL) ResetStates(start->out);
		if(start->out1 != NULL) ResetStates(start->out1);
	}

	bool Automata::Match(State* start, char* text)
	{
		int len = strlen(text);
		bool bingo = false;
		for(int i = 0; i < len; i++)
		{
			_list_id = 0;
			_current_list->nStates = 0;
			_next_list->nStates = 0;
			start->lastList = 0;
			_text_pos = i;
			ResetStates(start);
			if(start->out != NULL)
				AddState(_current_list, start->out);
			if(start->out1 != NULL)
				AddState(_current_list, start->out1);
			StateList* t = NULL;
			int last = -1;
			
			for(int j = i; j < len; j++)
			{
				int code = text[j];
				Step(code);
				t = _current_list;
				_current_list = _next_list;
				_next_list = t;
				if(IsMatch(_current_list)) 
				{
					last = max(last, j);
				}
				
			}
			if(last != -1)
			{
				::SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY | FOREGROUND_GREEN);
				cout << "Match: ";
				for(int k = i; k <= last; k++) cout << text[k];
				cout << " [" << i << "," << last << "]";
				cout << endl;
				::SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED );
				bingo = true;
				i = last;
			}
		}
		
		if(!bingo) 
		{
			::SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY | FOREGROUND_RED);
			cout << "Not matched -_-#" << endl;
			::SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED );
		}
		return bingo;
	}

	void Automata::DumpStateList(StateList* list)
	{
		cout << "Count(" <<list->nStates << "): ";
		for(int i = 0; i < list->nStates; i++)
		{
			if(list->list[i]->code == FAKE)
			{
				cout << "FAKE ";
			}
			else if(list->list[i]->code == S)
			{
				cout << "S " ;
			}
			else  if(list->list[i]->code == T)
			{
				cout << "T ";
			}
			else if(list->list[i]->code == ALL_ALNUM || list->list[i]->code == NON_ALNUM || list->list[i]->code == ALL_NUM || list->list[i]->code == NON_NUM)
			{
				cout << TokenString[list->list[i]->code] << " " ;
			}
			else 
			{
				cout << char(list->list[i]->code) << " ";
			}
		}
		cout << endl;
	}

	void Automata::Dump(State* state)
	{
		if(state == NULL) return;
		cout << state->code;
		if(state->out) cout << " " << state->out->code;
		if(state->out1) cout << " " << state->out1->code;
		cout << endl;
		Dump(state->out);
		Dump(state->out1);

	}
	void Automata::PrintCode(int code)
	{
		if(code == -1) cout << "START" ;
		else if(code == -2) cout << "END";
		else if(code == FAKE) cout << "FAKE";
		else if(code < 0)
		{
			cout << "{Unknown state code in automata " << code << "}" << endl;
			return;
		}
		if(code < 0) return;
		if(isalnum(code)) 
		{
			cout << char(code);
			return;
		}
		else if(code <= 15 && code >= 0)
			cout << TokenString[code];
		else cout << "{Unknown state code in automata " << code << "}" << endl;
	}
	void Automata::Dump(bool showFakeStates)
	{
		
		for(int i = 0; i < _used_state; i++)
		{
			State* state = &_state_buffer[i];
			if(!showFakeStates && state->code == FAKE) continue;
			cout << i << " {";
			PrintCode(state->code);
			cout << ": ";
			
			if(state->out) 
			{
				cout << " ";
				PrintCode(state->out->code);
				
			}
			if(state->out1) 
			{
				cout << " ";
				PrintCode(state->out1->code);
			}
			cout << "} ";
			if(state->out) cout << state->out - _state_buffer << " ";
			if(state->out1) cout << state->out1 - _state_buffer;
			
			cout << endl;
		}
	}
}