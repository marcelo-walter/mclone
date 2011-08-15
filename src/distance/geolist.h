#ifndef GEO_LIST
#define GEO_LIST

#include "vertex.h"

using namespace std;

struct geoNode;
class geoIt;

class geoList
	{
		friend class geoIt;
	public:
		geoNode* head;
		geoNode* end;	
		
	public:
		geoList();
		~geoList();
		
		void insert(VertexDistance* V1, VertexDistance* V2, double par, geoNode* pos);
		void insert(VertexDistance* V1);
		void clear();
		void remove(geoNode*);
		geoIt* makeIt(geoNode* gn);
		geoIt* makeIt();
		void destroyIt(geoIt*);
	};

struct geoNode
{
	VertexDistance* V1;
	VertexDistance* V2;
	double t;
	geoNode* next;
	geoNode* prev;
};

class geoIt
	{
	private:
		geoList* L;
		geoNode* curr;		
		
	public:
		geoIt(geoList*, geoNode* gn);
		~geoIt();
		
		void next();
		void operator++();
		void prev();
		void operator--();
		geoNode* data(); 
		bool more_elem();
	};

#endif
