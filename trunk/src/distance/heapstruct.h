#ifndef MXHEAP_INCLUDED
#define MXHEAP_INCLUDED

/************************************************************************
 
 Heap
 Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
 
 ************************************************************************/

#include <iostream>
#include <stdlib.h>

/*******************************************************/
// stdmix.h

#ifndef _STDMIX_H_
#define _STDMIX_H_

#include <cassert>

#if SAFETY > 0
#  define AssertBound(t)  assert(t)
#  define PRECAUTION(x) x
#else
#  define AssertBound(t)
#  define PRECAUTION(x)
#endif

#endif

//#ifdef ENABLE_CUDA
//    // for some reason, nvcc simply doesn't know the MIN macro
//    #define MIN(a,b) ((a < b)? a : b)
//#endif

/*******************************************************/

#define SanityCheck(t)	assert(t)

typedef unsigned int uint;

enum MxAllocPolicy { MX_ALLOC_INPLACE };

inline void *operator new(size_t, void *spot, MxAllocPolicy)
{
	return spot;  // MX_ALLOC_INPLACE
}

//
// Default equality class
//


class MxEQ {
public: bool operator==(const MxEQ& x) const { return this==&x; }
};




template <class T>
class MxBlock : public MxEQ
	{
	private:
		uint N;
		T *block;
		
	protected:
		MxBlock() { }
		void init_block(uint n)
		{
			// Allocate memory for block
			N=n; 
			block = (T *)malloc(sizeof(T)*n);
			// Initialize each entry
			for(uint i=0; i<n; i++)
			{
				//new((void *)&block[i], MX_ALLOC_INPLACE) T;
				block[i] = NULL;
			}
		}
		void resize_block(uint n) //Alloc more memory and construct new object of the type T (block)
		{
			T *old = block;
			
			// Allocate new block
			block = (T *)realloc(old, sizeof(T)*n);
			
			// Initialize all the newly allocated entries
			for(uint i=N; i<n; i++) 
			{
				//new( (void *)&block[i], MX_ALLOC_INPLACE) T;
				block[i] = NULL;
			}
			
			N = n;
		}
		void free_block()
		{
#ifdef WIN32
			// The Microsoft compiler has a problem with the 
			// standard syntax below.  For some reason,
			// expanding it into the following pointer-based
			// version makes it work.  Don't ask me why.
			//
			for(uint i=0; i<N; i++)  { T *p = &block[i]; p->~T(); }
			free(block);
#else
			// Call the relevant destructors for each element before
			// freeing the block.  Has now effect for types like 'int'.
			//
			
			for(uint i=0; i<N; i++)
			{
				//block[i].~T();
				//delete block[i];
				block[i] = NULL;
			}
			
			free(block);
#endif
		}
		
		T&       raw(uint i)       { return block[i]; } //Return the pointer in position i.
		const T& raw(uint i) const { return block[i]; }
		
		
	public:
		MxBlock(uint n) { init_block(n);}
		~MxBlock() { free_block(); }
		
		// Standard accessors
		//
		T&       ref(uint i)              { 
			AssertBound(i<N); //I guess that this verify is have enough space on teh heap.
			return block[i]; //Return the pointer of position i.
		}
		const T& ref(uint i) const        { 
			AssertBound(i<N);
			return block[i]; 
		}
		T&       operator()(uint i)       { return ref(i);   }
		const T& operator()(uint i) const { return ref(i);   }
#ifdef __GNUC__
		// Most compilers can apply the casting operators and then use the
		// standard array [].  GCC (2.7.2) seems to need explicit operators.
		//
		T&       operator[](uint i)       { return block[i]; }
		const T& operator[](uint i) const { return block[i]; }
#endif
		
		operator const T*() const { return block; }
		operator       T*()       { return block; }
		uint length() const { return N; }
		
		// Primitive methods for altering the data block
		//
		void resize(uint n) { resize_block(n); }
		void bitcopy(const T *array, uint n) // copy bits directly
		{ memcpy(block, array, this->MIN(n,N)*sizeof(T)); }

		void copy(const T *array, const uint n) // copy using assignment operator
		{ for(uint i=0; i<this->MIN(n,N); i++) block[i] = array[i]; }

		void bitcopy(const MxBlock<T>& b) { bitcopy(b, b.length()); }
		void copy(const MxBlock<T>& b) { copy(b, b.length()); }
	};


template<class T>
class MxDynBlock : public MxBlock<T>
{
private:
    uint fill;
	
public:
    MxDynBlock() { this->init_block(8); fill=0; }
    MxDynBlock(uint n) { this->init_block(n); fill=0; }
	
    T&       operator()(uint i)       { AssertBound(i<fill); return this->ref(i); }
    const T& operator()(uint i) const { AssertBound(i<fill); return this->ref(i); }
	
    uint length() const { return fill; } //Indicate how much have the heap "localy"
    uint total_space() const { return MxBlock<T>::length(); } //Return the number of position free on the heap.
    T& last() { AssertBound(fill>0); return this->raw(fill-1); }
    const T& last() const { AssertBound(fill>0); return this->raw(fill-1); }
	
    uint add()
	{
	    if( length()==total_space() ) //Show that doen't have free space on the heap.
		{
			resize(total_space() * 2);
		}
	    return fill++; //Add "logicaly" one position on the heap.
	}
    uint add(const T& t)
	{
	    uint i=add(); //Verify is have free space and then increment "logicaly" the number os postion filled.
	    this->raw(i) = t; //Put the pointer t on position i of the heap.
	    return i; //Return the position where the new nodo was inserted.
	}
	
    void reset() { fill = 0; }
    T& drop() { return this->raw(--fill); }
    void drop(uint d) { fill -= d; }
    bool find(const T& t, uint *index=NULL)
	{
	    for(uint i=0; i<fill; i++)
			if( this->raw(i) == t )
			{
				if( index ) *index = i;
				return true;
			}
	    return false;
	}
    void remove(uint i)
	{ AssertBound(i<fill); fill--; this->raw(i) = this->raw(fill); }
    void remove_inorder(uint i)
	{ memmove(&this->raw(i), &this->raw(i+1), (--fill - i)*sizeof(T)); }
};


template<class T, int T_SIZE>
class MxSizedDynBlock : public MxDynBlock<T>
{
public:   MxSizedDynBlock(uint n=T_SIZE) : MxDynBlock<T>(n) { }
};


class MxHeapable
	{
	private:
		float import;
		int token;
		char* mxheap;
		
	public:
		MxHeapable() { not_in_heap(); heap_key(0.0f); mxheap = ""; }
		
		inline bool is_in_heap() { return token != -47; } //If is -47, indicate that is a new nodo, and doesn't have on the heap.
		inline void not_in_heap() { token = -47; }
		inline int get_heap_pos() { return token; }
		inline void set_heap_pos(int position) { token=position; } //Fill token with the position where actual nodo was inserted at block.
		
		inline void  heap_key(float k) { import=k; }
		inline float heap_key() const  { return import; }
		
		inline void set_heap(char* s){mxheap = s;}    
		inline char* heap_name() { return mxheap; }
	};


class MxHeap : private MxDynBlock<MxHeapable *>
{
private:
    char* name;
    void place(MxHeapable *x, unsigned int i);
    void swap(unsigned int i, unsigned int j);
	
    unsigned int parent(unsigned int i) { return (i-1)/2; } 
    unsigned int left(unsigned int i) { return 2*i+1; }
    unsigned int right(unsigned int i) { return 2*i+2; }
	
    void upheap(unsigned int i);
    void downheap(unsigned int i);
	
public:
    MxHeap() : MxDynBlock<MxHeapable *>(8) { }
    MxHeap(unsigned int n) : MxDynBlock<MxHeapable *>(n) { }
	
    void insert(MxHeapable *t) 
	{ 
		insert(t, t->heap_key()); 
	}
    void insert(MxHeapable *, float);
    void update(MxHeapable *t) { 
		update(t, t->heap_key()); 
	}
    void update(MxHeapable *, float);
	
    unsigned int size() const { return length(); }
    MxHeapable       *item(uint i)       { return ref(i); }
    const MxHeapable *item(uint i) const { return ref(i); }
    MxHeapable *extract();
    MxHeapable *top() { return (length()<1 ? (MxHeapable *)NULL : raw(0)); }
    MxHeapable *remove(MxHeapable *);
    
    // Funciones agregadas por mi
    void clear();
    void setname(char* s){name = s;}
	
	void p(MxHeapable *t);
};

// MXHEAP_INCLUDED
#endif

