#ifndef A48_H
#define A48_H

#include <cstdio>
#include <iostream>
#include <set>
#include <map>
#include <iterator>
#include "heapstruct.h"

namespace A48 {
	
	typedef class Mesh;
	typedef class Surface;
	typedef class FaceDistance;
	typedef class EdgeDistance;
	typedef class Hedge;
	typedef class VertexDistance;
	
	typedef class Markable;
	typedef class Error;
	
	typedef std::set<FaceDistance*>  FaceContainer; 
	typedef FaceContainer::iterator FaceIter;
	
	typedef std::set<EdgeDistance*>  EdgeContainer;
	typedef EdgeContainer::iterator EdgeIter;
	
	typedef std::set<VertexDistance*>  VertexContainer;
	typedef VertexContainer::iterator VertexIter;
	
	typedef class Ipair;
	typedef std::map<const Ipair, A48::Hedge*> HedgeMap;
	
	
	class Ipair {
		int i0; int i1;
	public:
		Ipair(int p0, int p1) { i0=p0; i1=p1; }
		bool operator<(const Ipair s) const {
			return (i0 == s.i0)? (i1 < s.i1) : (i0 < s.i0);
		}
	};
	
	class Markable {
		bool     mark_;
	public:
		Markable(void) : mark_(false) {};
		void set_mark(bool m) { mark_ = m; };
		bool is_marked(void) const { return mark_; };
	};
	
	class Error {
	public:
		Error(char *s=""){ 
			std::cerr<<"A48 error: "<<s<<std::endl; exit(0); 
		}
	};
	
}


#endif
