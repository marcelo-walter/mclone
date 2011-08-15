#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include "edge.h"
#include "edgelist.h"

using namespace std;

class HashTable {
private:
    unsigned int hash_size;
    EdgeList *hash_data;
public:
	HashTable(unsigned int);
    ~HashTable();
	
    EdgeDistance *insert(EdgeDistance *, FaceDistance *);
    void CleanEdges();
	
private:
    unsigned int Key(EdgeDistance *);
};

#endif
