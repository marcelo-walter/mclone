#include "facevector.h"

#include <iostream>

using namespace std;

FaceVector::FaceVector() 
{
	sz = 0;
	current = 0;
	data = NULL;
}            

FaceVector::FaceVector(int s)
{
	sz = s;
	current = 0;
	data = new FaceDistance[sz];
}

FaceVector::~FaceVector()
{
	delete [] data;
}


void FaceVector::insert(FaceDistance* F)
{
	if (isfull())
		cout << "Error: vector overflow" << endl;
	else
	{
		data[current] = *F;
		current++;
	}
}

FaceDistance& FaceVector::operator[](int pos)
{
	return data[pos];
}

bool FaceVector::isfull()
{
	return (current == sz);
}

int FaceVector::size()
{
	return current;
}

