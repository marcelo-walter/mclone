/***************************************************************************
 set.cpp  -  description
 -------------------
 begin                : Sat Jun 7 2003
 copyright            : (C) 2003 by Dimas Martinez Morera
 email                : dimasmm@impa.br
 ***************************************************************************/
#include <iostream>

#include "set.h"

Set::Set(char* name, int n)
{
    Name = new char[10];
    Name = name;
    max_nodes = n;
    sz = 0;
    alocate(max_nodes);
}

Set::Set()
{
    Name = new char[10];
    Name = "";
    max_nodes = 0;
    sz = 0;
    alocate(max_nodes);
}

Set::~Set()
{
    delete [] keys;
    delete [] Name;
}

void Set::insert(int n)
{
    if (n < max_nodes)
    {
		if (keys[n])
			cout << "Warning:  inserting an existing point in the set --> " << Name << endl;
		else
		{
			keys[n] = true;
			sz++;
		}
    }
    else
		cout << "Error: tempting to insert a node out of range --> " << Name << endl;
}

void Set::remove(int n)
{
	if (n < max_nodes)
    {
		if (!keys[n])
			cout << "Error:  tempting to remove a non-existing point in the set  --> " << Name << endl;
		else
		{
			keys[n] = false;
			sz--;
		}
    }
    else
		cout << "Error: tempting to remove a node out of range!!" << endl;
}

bool Set::has(int n)
{
    return keys[n];
}


void Set::clear()
{
	for (int i = 0; i < max_nodes; i++)
		keys[i] = false;
}


int Set::size()
{
	return  sz;
}

bool Set::empty()
{
	return (sz == 0);
}


void Set::foreach(void (*fun)(int , void *), void* user_data)
{
	for (int i = 0; i < max_nodes; i++)
		if (keys[i])
		{
			fun(i, user_data);
		}
}


void Set::alocate(int n)
{
    if (max_nodes != 0)
        delete keys;
	
    keys = new bool[n];
    for (int i = 0; i < n; i++)
        keys[i] = false;
    max_nodes = n;
}

void Set::setName(char* name)
{
	Name = name;
}

