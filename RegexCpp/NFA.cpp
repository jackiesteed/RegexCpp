#include <iostream>
#include <cassert>
#include "Symbol.h"
#include "Parser.h"
#include "NFA.h"


using namespace std;

NFA::NFA()
{
	_state_buffer = new State[MAXN];
	_frag_buffer = new Frag[MAXN];
	_used_state = 0;
	_used_frag = 0;
	_current_list = new StateList();
	_next_list = new StateList();
	_list_id = 0;

}

NFA::~NFA()
{
	delete []_state_buffer;
	delete []_frag_buffer;
	delete _current_list;
	delete _next_list;
}

//<By张方雪 2013-5-20> 这里的NFA, 会使用许多FAKE的State节点, 为了方便, 使得一个Frag 变成一入一出的.
//最后, 通过一个缩图函数, 把图缩成原有的样子.


Frag* NFA::CreateFakeNFA(Node* root)
{
	switch(root->token)
	{
	case CONCAT:
		{
			//cout << "CONCAT" << endl;
			Frag* pre = CreateFakeNFA(root->left);

			assert(!(pre->end->out != NULL && pre->end->out1 != NULL));

			Frag* post = CreateFakeNFA(root->right);
			
			if(pre->end->out == NULL)
				pre->end->out = post->start;
			else 
				pre->end->out1 = post->start;
			Frag* frag = NewFrag();
			frag->start = pre->start;
			frag->end = post->end;
			return frag;
			break;
		}
	case ALT: //"|" 节点, 需要两个Fake State.
		{
			Frag* up = CreateFakeNFA(root->left);
			Frag* down = CreateFakeNFA(root->right);
			Frag* frag = NewFrag();
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
			Frag* core = CreateFakeNFA(root->left);
			Frag* frag = NewFrag();
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
			Frag* core = CreateFakeNFA(root->left);
			Frag* frag = NewFrag();
			frag->start = frag->end = NewState();
			//frag->end = NewState();
			frag->start->Init();
			//frag->end->Init();
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
			//cout << "PLUS" << endl;
			Frag* core = CreateFakeNFA(root->left);
			Frag* frag = NewFrag();
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
			//cout << "ALPHA " << char(root->subType) << endl;

			Frag* frag = NewFrag();
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
			//cout << "DOT" << endl;
			Frag* frag = NewFrag();
			frag->Init();
			State* core = NewState();
			core->Init(root->token);
			frag->start = core;
			frag->end = core;
			return frag;
		}
	}

	return NULL;
}


void NFA::ClearFakeNFA(State* state)
{
	if(state == NULL) return;

	State* out = state->out;
	State* out1 = state->out1;
	ClearFakeNFA(out);
	ClearFakeNFA(out1);
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

State* NFA::CreateNFA(Node* root) 
{
	_used_frag = 0;
	_used_state = 0;
	Frag* frag = CreateFakeNFA(root);
	State* start = NewState();
	start->Init(S);
	State* end = NewState();
	end->Init(T);
	start->out = frag->start;
	if(frag->end->out == NULL)
		frag->end->out = end;
	else 
		frag->end->out1 = end;
	//DumpNFA(start);
	DumpNFA();


	//ClearFakeNFA(start);

	return start;
}

bool NFA::IsMatch(StateList* list)
{
	for(int i = 0; i < list->nStates; i++)
	{
		if(list->list[i]->code == T)
			return true;
	}
	return false;
}

void NFA::Step(int code)
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
		}
	}

}

//<By张方雪 2013-5-27>这个递归的添加状态, 保证每次都会把有效状态加进来.
void NFA::AddState(StateList* list, State* state)
{
	if(state == NULL || state->lastList == _list_id) return;
	state->lastList = _list_id;

	if(state->code == FAKE)
	{
		AddState(list, state->out);
		AddState(list, state->out1);
		return ;
	}
	list->list[list->nStates++] = state	;
}

int NFA::Match(State* start, char* text)
{

	int len = strlen(text);
	_list_id = 0;
	_current_list->nStates = 0;
	_next_list->nStates = 0;
	if(start->out != NULL)
	AddState(_current_list, start->out);
	if(start->out1 != NULL)
	AddState(_current_list, start->out1);
	StateList* t = NULL;
	cout << "S: ";
	DumpStateList(_current_list);
	int matchPos = -1;
	for(int i = 0; i < len; i++)
	{
		
		int code = text[i];
		Step(code);
		t = _current_list;
		_current_list = _next_list;
		_next_list = t;

		cout << text[i] << ": ";
		DumpStateList(_current_list);
		if(IsMatch(_current_list)) matchPos = i;
	}
	DumpStateList(_current_list);
	return matchPos;
}

void NFA::DumpStateList(StateList* list)
{
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

void NFA::DumpNFA(State* state)
{
	if(state == NULL) return;
	cout << state->code;
	if(state->out) cout << " " << state->out->code;
	if(state->out1) cout << " " << state->out1->code;
	cout << endl;

	system("pause");

	DumpNFA(state->out);
	DumpNFA(state->out1);

}

void NFA::DumpNFA()
{
	for(int i = 0; i < _used_state; i++)
	{
		State* state = &_state_buffer[i];
		cout << state->code;
		if(state->out) cout << " " << state->out->code;
		if(state->out1) cout << " " << state->out1->code;
		cout << endl;
	}
}