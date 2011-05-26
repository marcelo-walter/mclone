#ifndef SORTEDSET_H
#define SORTEDSET_H

#include "heapstruct.h"

#define Heapable MxHeapable

class SortedSet {
private:
    MxHeap Heap;
    
public:
    SortedSet();
    ~SortedSet();
	
    void insert(Heapable*);
    void insert(Heapable*, double);
    void remove(Heapable*);
    bool has(Heapable*);
    void clear();
    int size();
    bool empty();
    Heapable* smaller();
    void update(Heapable*);
	
	void Imprime(MxHeapable *tt);
};

#endif
