#ifndef VECT_FACES_H
#define VECT_FACES_H

#include "face.h"

class FaceVector
	{
	private:
		int sz;
		int current;
		FaceDistance* data;
	public:
		FaceVector(); 
		FaceVector(int);
		~FaceVector();
		
		void insert(FaceDistance*);
		FaceDistance& operator[](int);
		int size();
		
	private:
		bool isfull();
	};

#endif
