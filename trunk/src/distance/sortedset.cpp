#include "sortedset.h"

SortedSet::SortedSet()
{
}

SortedSet::~SortedSet()
{
}

void SortedSet::Imprime(MxHeapable *tt)
{
	Heap.p(tt);
}

void SortedSet::insert(Heapable* t)
{
    Heap.insert(t);
}

void SortedSet::insert(Heapable* t, double k)
{
    Heap.insert(t, (float) k);
}

void SortedSet::remove(Heapable* t)
{
    Heap.remove(t);
}

bool SortedSet::has(Heapable* t)
{
    return t->is_in_heap() ;
}

void SortedSet::clear()
{
    Heap.clear();
}

int SortedSet::size()
{
	return Heap.size();
}

bool SortedSet::empty()
{
	return (Heap.size() == 0);
}

Heapable* SortedSet::smaller()
{
    return Heap.extract();
}

void SortedSet::update(Heapable* t)
{
    Heap.update(t);
}

