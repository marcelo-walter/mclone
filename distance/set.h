/***************************************************************************
 set.h  -  description
 -------------------
 begin                : Sat Jun 7 2003
 copyright            : (C) 2003 by Dimas Martinez Morera
 email                : dimasmm@impa.br
 ***************************************************************************/

#ifndef SET_H
#define SET_H

using namespace std;

class Set
	{
	private:
		bool* keys;
		int max_nodes;
		int sz;
		char* Name;
	public:
		Set(char* name, int n);
		Set();
		~Set();
		
		void insert(int n);
		void remove(int n);
		bool has(int n);
		void clear();
		int size();
		bool empty();
		void foreach(void (*fun)(int, void *), void* user_data);
		void alocate(int n);
		void setName(char* name);
		
	};

void function(void* user_data);

#endif

