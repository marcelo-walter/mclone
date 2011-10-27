#ifndef VLIST_H
#define VLIST_H

#include "surface.h"

struct vNode;
class vIt;

class vList
{
        friend class vIt;
    private:
        vNode* head;
        vNode* end;

    public:
        vList();
        ~vList();

        void insert(VertexDistance*, vNode* pos);
        void clear();
        void remove(vNode*);
        vIt* makeIt(vNode* vn);
        void destroyIt(vIt*);

};

struct vNode
{
        VertexDistance* v;
        vNode* next;
};

class vIt
{
    private:
        vList* L;
        vNode* curr;

    public:
        vIt(vList*, vNode* vn);
        ~vIt();

        vNode* current();
        vNode* next();
        vNode* operator++();
};

#endif
