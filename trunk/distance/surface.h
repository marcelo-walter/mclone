#ifndef SURFACE_H
#define SURFACE_H

#include "halfedge.h"
#include "hashtable.h"
#include "facevector.h"

#include "set.h"
#include "sortedset.h"
#include "geolist.h"

#include <vector>

#ifndef pi
#define pi 3.14159265358979
#endif

#define TOL 0.0001
#define EPS 0.005

#define CRECIENTE   0
#define DECRECIENTE 1

#define EUCLIDEAN  0
#define SPHERICAL  1
#define HYPERBOLIC 2

#define RIGHT   0
#define LEFT    1
#define NOSIDE  2

using namespace std;

struct surf_info {
    bool hasnormal;
    bool hascolor;
    bool hastexture;
    bool drawdistance;
    double max_dist;
};

class Surface {
private:
    HashTable *Edges;
	FaceVector *Faces;
    vector < pVertex > Vertices;
    //surf_info sinfo;
    Set alive, far;
	SortedSet close;
    geoList geodesicpath;
    geoList firstaprox;
    double geodLength;
    
public:	
    int ExitState;
    surf_info sinfo;
	int NumVertices, NumFaces; //Add by Vin?cius.
	
public:
    Surface(int numVerts, int numFaces);
    ~Surface();
	
    vector < pVertex > getVertexStar(unsigned int);
    vector < pFace > getFaceStar(unsigned int);
    vector < pVertex > getFaceVertices(unsigned int);
	vector < pVertex > getVertices();
	
    FaceVector *getFaces();
    int NumberOfFaces();
	
    void Draw();
    void SelectDraw();
	
    void ComputeGeodesic(int, int);
    void dupdate(VertexDistance*);
	
	//Before was private (Vin?cius)
	void addVertex(double, double, double);
    void addFace(int *, int);
	//-------
	
	void FinishToConstruct(int numVertices);
	double returnXVertice(int Index);
	double returnYVertice(int Index);
	double returnZVertice(int Index);
	double GeodesicDistanceCalculated(void);
private:
    void addColor(double, double, double);
    void addNormal(double, double, double);
    HalfEdge *next_cw(HalfEdge *);
    HalfEdge *next_acw(HalfEdge *);
    void ComputeNormals();
    //double geodesicerror ();
    double LocalError(geoNode *);
    bool isInBorder(VertexDistance*);
public:
    void CorrectGeodesic();
    void ShowGeodesic(bool);
	
    double geodesicerror();
};

#endif
