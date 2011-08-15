#include "surface.h"

#include <fstream>
#include <string>

#include <stdio.h>

#include <GL/gl.h>

#include "vector.h"
#include "vlist.h"

#define SQR(x) (x*x)
#define MIN min

double Surface::GeodesicDistanceCalculated(void)
{
	//Calcula distancia 29/03/2005
	point PointActual, PointBefore;
	Vector Aux;
	double Distance = 0.0;
	
	double t;
	geoNode* gn;
	
	geoIt* It = geodesicpath.makeIt();
	gn = It->data();
	t = gn->t;
	
	if (gn->V2 != NULL)
	{
		PointActual.x = t*gn->V1->X() + (1-t)*gn->V2->X();
		PointActual.y = t*gn->V1->Y() + (1-t)*gn->V2->Y();
		PointActual.z = t*gn->V1->Z() + (1-t)*gn->V2->Z();
	}
	else {
		PointActual.x = gn->V1->X();
		PointActual.y = gn->V1->Y();
		PointActual.z = gn->V1->Z();
	}
	
	PointBefore = PointActual;
	++(*It);
	
	for(;It->more_elem(); ++(*It))
    {
		gn = It->data();
		t = gn->t;
		
		if (gn->V2 != NULL)
		{
			PointActual.x = t*gn->V1->X() + (1-t)*gn->V2->X();
			PointActual.y = t*gn->V1->Y() + (1-t)*gn->V2->Y();
			PointActual.z = t*gn->V1->Z() + (1-t)*gn->V2->Z();
		}
		else {
			PointActual.x = gn->V1->X();
			PointActual.y = gn->V1->Y();
			PointActual.z = gn->V1->Z();
		}
		
		Aux = PointActual-PointBefore;
		
		Distance += sqrt((Aux.x*Aux.x)+(Aux.y*Aux.y)+(Aux.z*Aux.z));
		
		PointBefore = PointActual;
	}
	
	return Distance;
}

Surface::Surface(int numVerts, int numFaces)
{
	this->Edges = NULL;
	this->Faces = NULL;
	this->geodLength = 0.0;
	
	this->NumVertices = numVerts;
	this->NumFaces = numFaces;
	
	Faces = new FaceVector(numFaces);
	Edges = new HashTable(numVerts);
}

void Surface::FinishToConstruct(int numVertices)
{
    Edges->CleanEdges();
	
    if (!sinfo.hasnormal)
		ComputeNormals();
	
    geodLength = 0.0;
    
    far.alocate(numVertices);
    alive.alocate(numVertices);
    
    far.setName("far");
    alive.setName("alive");
}

Surface::~Surface()
{
	int Indice, Total;
	
    delete Faces;
    delete Edges;
	
	Total = Vertices.size();
	for (Indice = 0; Indice < Total; Indice++)
	{
		delete Vertices[Indice];
	}
	Vertices.clear();
}

// getVertexStar:
// Retorna los vertices en la estrella del 
// vertice v.
vector < pVertex > Surface::getVertexStar(unsigned int v)
{
    VertexDistance *V = Vertices[v];
    vector < pVertex > out, aux, aux1;
	
    HalfEdge *h = V->getHalfEdge();
    VertexDistance *vv = h->Next()->vertex();
    HalfEdge *hh = h;
	
    aux.push_back(h->Next()->vertex());
	
    h = next_cw(h);
    if (h != NULL) 
	{
		aux.push_back(h->vertex());
		h = h->Partner();
    }
    while ((h != NULL) && (h != hh))
	{
		h = next_cw(h);
		if (h != NULL) 
		{
			aux.push_back(h->vertex());	// ver bien esta linea!!
			h = h->Partner();
		}
    }
    if (h == NULL) 
	{
		h = V->getHalfEdge()->Partner();
		while (h != NULL) 
		{
			aux1.push_back(h->Next()->Next()->vertex());
			h = next_acw(h);
		}
    } else {
		vector < pVertex >::iterator vit = aux.end();
		vit--;
		aux.erase(vit, aux.end());
    }
	
    // orden en contra de las Manecillas del Reloj
    for (int i = (int)aux.size() - 1; i > -1; i--)
	{
		out.push_back(aux[i]);
	}
	
    for (unsigned int i = 0; i < aux1.size(); i++)
	{
		out.push_back(aux1[i]);
	}
	
	//aux.clear();
	//aux1.clear();
	
	//vector <int>::size_type i; printf("V[%d] [%d] [%d]\n", out.size(), aux.size(), aux1.size());
	
    return out;
}


vector < pFace > Surface::getFaceStar(unsigned int v)
{
    VertexDistance *V = Vertices[v];
    vector < pFace > out, aux, aux1;
	
    HalfEdge *h = V->getHalfEdge();
    HalfEdge *hp = h;
    if (h == NULL) 
	{
		// cout << "error: superfice mal construida at vertex " << v << endl;
		return out;		// retorna un vector vacio
    }
    h = next_cw(h);
    if (h != NULL) 
	{
		aux.push_back(h->face());
		h = h->Partner();
    }
	
    while ((h != NULL) && (h != hp)) 
	{
		h = next_cw(h);
		if (h != NULL) 
		{
			aux.push_back(h->face());
			h = h->Partner();
		}
    }
    if (h == NULL)
	{
		h = hp->Partner();
		while (h != NULL) 
		{
			aux1.push_back(h->face());
			h = next_acw(h);
		}
    }
    for (int i = (int)aux.size() - 1; i > -1; i--)
	{
		out.push_back(aux[i]);
	}
    
	for (unsigned int i = 0; i < aux1.size(); i++)
	{
		out.push_back(aux1[i]);
	}
	
	//aux.clear();
	//aux1.clear();
	
    return out;
}

vector < pVertex > Surface::getFaceVertices(unsigned int f)
{
    vector < pVertex > vv;
	
    HalfEdge *he = (*Faces)[f].getHalfEdge();
    vv.push_back(he->vertex ());
    for (HalfEdge * h = (*Faces)[f].getHalfEdge()->Next(); h != he; h = h->Next()) 
	{
		vv.push_back (h->vertex());
    }
    return vv;
}

vector < pVertex > Surface::getVertices()
{
    return Vertices;
}


HalfEdge *Surface::next_cw(HalfEdge * h)
{
    HalfEdge *h2 = h;
    for (HalfEdge * h1 = h; h1 != NULL;) 
	{
		if (h1->Next() != NULL)
	   		if (*(h1->Next()->vertex()) == *(h->vertex())) 
			{
				h2 = h1;
				h1 = NULL;
			}
		h1 = (h1 != NULL) ? h1->Next() : NULL;
    }
    return (h2 == h) ? NULL : h2;
}


HalfEdge *Surface::next_acw(HalfEdge * h)
{
    if (h->Next () != NULL) 
	{
		return h->Next()->Partner();
    }
    return NULL;
}

void Surface::addVertex(double x, double y, double z)
{
    VertexDistance *pV = new VertexDistance(x, y, z, Vertices.size());
    Vertices.push_back(pV);
}

double Surface::returnXVertice(int Index)
{
	return Vertices[Index]->X();
}

double Surface::returnYVertice(int Index)
{
	return Vertices[Index]->Y();
}

double Surface::returnZVertice(int Index)
{
	return Vertices[Index]->Z();
}

void Surface::addColor(double r, double g, double b)
{
    int ind = Vertices.size() - 1;
    if (ind >= 0)
		Vertices[ind]->setColor(r, g, b);
	else {
		cout << "Warning: Trying to set a vertex color ";
		cout << "without vertices ..." << endl;
	}
}

void Surface::addNormal(double x, double y, double z)
{
    int ind = Vertices.size() - 1;
    if (ind >= 0)
		Vertices[ind]->setNormal(x, y, z);
    else {
		cout << "Warning: Trying to set a vertex normal ";
		cout << "without vertices ..." << endl;
	}
}

void Surface::addFace(int *fvert, int nvert)
{
    FaceDistance F;
    Faces->insert(&F);
	
    HalfEdge *aux;
    HalfEdge *aux1;
    HalfEdge *aux2;
	
    FaceDistance *f = &((*Faces)[Faces->size() - 1]);
    EdgeDistance *E = new EdgeDistance(Vertices[fvert[0]], Vertices[fvert[1]], f);
	
    EdgeDistance *Ed = Edges->insert(E, f);
    HalfEdge **laux = Ed->getHalfEdges();
	
    aux = (laux[0]->face() == f) ? laux[0] : laux[1];
    
	aux1 = aux;
    for (int i = 2; i < nvert; i++) 
	{
		EdgeDistance *E = new EdgeDistance(Vertices[fvert[i - 1]], Vertices[fvert[i]], f);
		
		Ed = Edges->insert(E, f);
		
		laux = Ed->getHalfEdges();
		aux2 = (laux[0]->face() == f) ? laux[0] : laux[1];
		aux1->setNext(aux2);
		aux1 = aux2;
    }
    EdgeDistance *E1 = new EdgeDistance(Vertices[fvert[nvert - 1]], Vertices[fvert[0]], f);
    Ed = Edges->insert(E1, f);
    aux2 = ((Ed->getHalfEdges())[0]->face() == f) ? (Ed->getHalfEdges())[0] : (Ed->getHalfEdges())[1];
	(*Faces)[Faces->size() - 1].setHalfEdge(aux2);
	
    aux1->setNext(aux2);
    aux2->setNext(aux);
}

FaceVector *Surface::getFaces()
{
    return Faces;
}

int Surface::NumberOfFaces()
{
    return Faces->size();
}

void Surface::Draw()
{
    //glColor3f(0.4, 0.7, 0.1);
    glColor3f (0.7, 0.7, 0.7);
    //glColor3f(0.7,0.4,0.1);
    glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    glColor3f(0.3,0.3,0.3);
    glDepthRange(0.0,0.99);
    for (unsigned int i = 0; i < Faces->size(); i++) 
	{
		(*Faces)[i].Draw (sinfo.hascolor, sinfo.drawdistance, sinfo.max_dist);
    }
    glDepthRange(0.0,1.0);
    glColor3f (0.7, 0.7, 0.7);
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    for (unsigned int i = 0; i < Faces->size(); i++) 
	{
		(*Faces)[i].Draw (sinfo.hascolor, sinfo.drawdistance, sinfo.max_dist);
    }
}

void Surface::ShowGeodesic(bool showfirst)
{
    point p, inicio;
	/* Inicio */
	glColor3f(1.0,1.0,0.0);
	glBegin(GL_POINTS);
	for(geoIt* It = geodesicpath.makeIt();It->more_elem(); ++(*It))
	{
		geoNode* gn = It->data();  
		double t = gn->t;
		if (gn->V2 != NULL) 
		{
			p.x = t*gn->V1->X() + (1-t)*gn->V2->X();
			p.y = t*gn->V1->Y() + (1-t)*gn->V2->Y();
			p.z = t*gn->V1->Z() + (1-t)*gn->V2->Z();
		}
		else
		{
			p.x = gn->V1->X();
			p.y = gn->V1->Y();
			p.z = gn->V1->Z();
		}
		glVertex3f(p.x,p.y,p.z);
	}
	glEnd();
	/* Fim */
	int first = 1;
	
    glColor3f (1.0, 0.0, 0.0);
    glLineWidth (5.0);
    glBegin (GL_LINE_STRIP);
	for (geoIt * It = geodesicpath.makeIt(); It->more_elem(); ++(*It))
	{
		geoNode *gn = It->data();
		double t = gn->t;
		if (gn->V2 != NULL)
		{
			p.x = t * gn->V1->X() + (1 - t) * gn->V2->X();
			p.y = t * gn->V1->Y() + (1 - t) * gn->V2->Y();
			p.z = t * gn->V1->Z() + (1 - t) * gn->V2->Z();
		}
		else {
			p.x = gn->V1->X();
			p.y = gn->V1->Y();
			p.z = gn->V1->Z();
		}
		if (first)
		{
			inicio.x = p.x;
			inicio.y = p.y;
			inicio.z = p.z;
			first = 0;
		}
		glVertex3f (p.x, p.y, p.z);
	}
    glEnd();
	
	glColor3f(0.0, 0.0, 1.0);
    glBegin(GL_LINE_STRIP);
	glVertex3f(inicio.x, inicio.y, inicio.z);
	glVertex3f(p.x, p.y, p.z);
    glEnd();
	
    if (showfirst) {
		/* Inicio */
	    glColor3f(0.0,1.0,0.0);
	    glBegin(GL_POINTS);
	    for(geoIt* It = firstaprox.makeIt();It->more_elem(); ++(*It))
	    {
			geoNode* gn = It->data();
			double t = gn->t;
			if (gn->V2 != NULL)
			{
				p.x = t*gn->V1->X() + (1-t)*gn->V2->X();
				p.y = t*gn->V1->Y() + (1-t)*gn->V2->Y();
				p.z = t*gn->V1->Z() + (1-t)*gn->V2->Z();
			}
			else
			{
				p.x = gn->V1->X();
				p.y = gn->V1->Y();
				p.z = gn->V1->Z();
			}
			glVertex3f(p.x,p.y,p.z);
	    }
	    glEnd();
		/* Fim */
		glLineWidth(3.0);
		glColor3f (0.5, 0.0, 0.5);
		glBegin (GL_LINE_STRIP);
		for (geoIt * It = firstaprox.makeIt (); It->more_elem (); ++(*It)) {
			geoNode *gn = It->data ();
			double t = gn->t;
			if (gn->V2 != NULL) {
				p.x = t * gn->V1->X () + (1 - t) * gn->V2->X ();
				p.y = t * gn->V1->Y () + (1 - t) * gn->V2->Y ();
				p.z = t * gn->V1->Z () + (1 - t) * gn->V2->Z ();
			} else {
				p.x = gn->V1->X ();
				p.y = gn->V1->Y ();
				p.z = gn->V1->Z ();
			}
			glVertex3f (p.x, p.y, p.z);
		}
		glEnd ();
    }
    glLineWidth (1.0);
}

void Surface::ComputeNormals()
{
    for (unsigned int i = 0; i < Faces->size(); i++)
		(*Faces)[i].ComputeNormal();
	
    for (unsigned int i = 0; i < Vertices.size(); i++)
	{
		vector < pFace > Fstar = getFaceStar(i);
		Vector V = { 0.0, 0.0, 0.0 };
		
		for (unsigned int j = 0; j < Fstar.size(); j++)
		{
			double x, y, z;
			(Fstar[j])->getNormal(&x, &y, &z);
			Vector V1 = { x, y, z };
			V += V1;
		}
		V = (1.0 / V.norm()) * V;
		Vertices[i]->setNormal(V.x, V.y, V.z);
		
		//Fstar.clear();
    }
}

void Surface::ComputeGeodesic(int vi, int ve)
{
	unsigned int i;
	unsigned int j;
	
    geodesicpath.clear();
    firstaprox.clear();
	
    if (vi < 0 || vi > Faces->size() || ve < 0 || ve > Faces->size()) 
	{
		cout << "Warning: Trying to construct a geodesic path " << endl;
		cout << "from (or to) a non-existent point" << endl;
		return;
    }
    // **** Compute edge path: **** //
    close.clear();
    far.clear();
    alive.clear();
    sinfo.max_dist = 0.0;
    
    // 1. Define la funcion distancia
    //    y inserta todos en far 
    unsigned int sz = Vertices.size();
    for (i = 0; i < sz; i++) 
	{
		Vertices[i]->heap_key(-HUGE_VAL);
		far.insert(i);
    }
	
    // 2. Define la distancia en vi y ponlo en alive
    far.remove(vi);
    alive.insert(vi);
    Vertices[vi]->heap_key(0.0);
	
    // 3. Busca los vertices cercanos y ponlos en alive
    vector < pVertex > st = getVertexStar(vi);
    for (i = 0; i < st.size(); i++) 
	{
		// 3.1 define la distancia
		Vector v = { st[i]->X() - Vertices[vi]->X(),  st[i]->Y() - Vertices[vi]->Y(),
		st[i]->Z() - Vertices[vi]->Z()};
		
		far.remove(st[i]->Pos());
		alive.insert(st[i]->Pos());
		st[i]->heap_key(-v.norm());
		sinfo.max_dist = max(sinfo.max_dist,(double)-st[i]->heap_key());
    }
	
    // 4. Construir el conjunto close 
    for (i = 0; i < st.size(); i++) 
	{
		vector < pVertex > star = getVertexStar(st[i]->Pos());
		for (j = 0; j < star.size(); j++) 
		{
			if (far.has(star[j]->Pos())) 
			{
				far.remove(star[j]->Pos());
				dupdate(star[j]);
				
				close.insert(star[j]);
				//close.Imprime(star[j]);
			}
		}
		//star.clear();
    }
	
	//st.clear();
	
	// 5. Iterar
    while (!alive.has(ve))
	{
		//while (!close.empty()) {
		VertexDistance* pV = (VertexDistance*) close.smaller();
		if (pV == NULL) break;
		alive.insert(pV->Pos());
		sinfo.max_dist = max(sinfo.max_dist,(double)-pV->heap_key());
		
		// agregar a close los nuevos miembros
		vector < pVertex > vv = getVertexStar(pV->Pos());
		
		for (i = 0; i < vv.size(); i++)
		{
			if ( far.has(vv[i]->Pos()) )
			{
				close.insert(vv[i]);
				far.remove(vv[i]->Pos());
			}
			
			if (close.has(vv[i]))
			{ 
				dupdate(vv[i]);
				close.update(vv[i]);
			}
		}
		
		//vv.clear();
    }
    
    alive.clear();
    close.clear();
    far.clear();
    
    // 6. Primera aproximacion ...
    geodesicpath.insert(Vertices[ve]);
    firstaprox.insert(Vertices[ve]);
    int ind = ve;
    int last = -1;
    int l1 = ve;
    while (ind != vi) 
	{
		vector < pVertex > vv = getVertexStar(ind);
		
		ind = (vv[0]->Pos() == last) ? vv[1]->Pos() : vv[0]->Pos();
		double dd = -Vertices[ind]->heap_key();
		for (i = 1; i < vv.size(); i++) 
		{
			int k = vv[i]->Pos();
			double dd1 = -Vertices[k]->heap_key();
			if (dd1 < dd && k != last)
			{
				dd = dd1;
				ind = k;
			}
		}
		last = l1;
		l1 = ind;
		geodesicpath.insert(Vertices[ind]);
		firstaprox.insert(Vertices[ind]);
		
		//vv.clear();
    }
	
    //**** Correct edge path ****//
	/* int i;
	 for (i = 0; i < 20; i++) {
	 for (int j = 0; j < 100; j++)
	 CorrectGeodesic ();
	 // criterio de parada ... 
	 if (geodesicerror () < EPS) {
	 cout << "iterations: " << i;
	 i = 2000;		// parar 
	 }
	 }
	 cout << " error: " << geodesicerror () << endl;*/
}


void Surface::dupdate(VertexDistance* VV)
{
    vector < pVertex > vv = getVertexStar(VV->Pos());
    //Esto es casi Dijstrass
    /*for(unsigned int i = 0; i < vv.size(); i++)
	 {
	 int k = vv[i]->Pos();
	 Vector V = {vv[i]->X()-Vertices[vert]->X(),
	 vv[i]->Y()-Vertices[vert]->Y(),
	 vv[i]->Z()-Vertices[vert]->Z()};
	 
	 dist[vert] = (alive.has(k)) ? min(dist[vert],dist[k]+V.norm()) : dist[vert];
	 } */
	
    // Esto es casi Fast Marching Method (Sethian & Kimmel)
    for (unsigned int i = 1; i < vv.size(); i++) 
	{
		if (alive.has (vv[i - 1]->Pos()) && alive.has (vv[i]->Pos())) 
		{
			int ia, ib;
			//if (-vv[i-1]->heap_key () < -vv[i]->heap_key ()) {
			if (vv[i-1]->heap_key() > vv[i]->heap_key()) 
			{	
				ia = i - 1;
				ib = i;
			} else {
				ia = i;
				ib = i - 1;
			}
			point A = { vv[ia]->X(), vv[ia]->Y(), vv[ia]->Z() };
			point B = { vv[ib]->X(), vv[ib]->Y(), vv[ib]->Z() };
			point C = { VV->X(), VV->Y(),VV->Z() };
			
			double a = (B - C).norm();
			double b = (A - C).norm();
			//double u = -vv[ib]->heap_key () - (-vv[ia]->heap_key ());
			double u = vv[ia]->heap_key() - vv[ib]->heap_key();
			//double sen = cross (B - C, A - C).norm () / (a * b);
			//double sen2 = sen * sen;
			double cos = (B - C) * (A - C) / (a * b);
			double cos2 = cos * cos;
			
			double sen = sin(acos(cos));
			double sen2 = sen * sen;
			
			double AA = SQR(a) + SQR(b) - 2.0 * a * b * cos;
			double BB = 2.0 * b * u * (a * cos - b);
			double CC = SQR(b) * (SQR(u) - SQR(a) * SQR(sen));
			double disc = SQR(BB) - 4.0 * AA * CC;
			double t, t0, t1;
			if (disc > 0) 
			{
				disc = sqrt(disc);
				t0 = (AA == 0) ? 0 : (-BB + disc) / (2.0 * AA);
				t1 = (AA == 0) ? 0 : (-BB - disc) / (2.0 * AA);
			} else {
				t0 = (-BB) / (2.0 * AA);
				t1 = (-BB) / (2.0 * AA);
			}
			if (u < t0)
				if (u < t1)
					t = MIN (t0, t1);
				else
					t = t0;
				else if (u < t1)
					t = t1;
				else
					t = u;
			double f = (b * (t - u)) / t;
			
			/*double t = 0.5 * (2.0 * b * u - 2 * a * cos * u + 2 *
			 sqrt (a * a * cos2 * u * u - a * a * u * u +
			 a * a * a * a * sen2 -
			 2 * a * a * a * b * cos * sen2 +
			 b * b * a * a * sen2))
			 * b / (a * a - 2 * a * b * cos + b * b); */
			
			if (  (u < t && a * cos < b * (t - u) / t)  &&   (b * (t - u) / t < a / cos)  )
			    //VV->heap_key( min( -(double)VV->heap_key(), t + (- (double) vv[ia]->heap_key() ) ));
		   		VV->heap_key( max( (double)VV->heap_key(), - t + (double) vv[ia]->heap_key() ) );
			else
				//VV->heap_key( min((double) VV->heap_key(),
				//min (b + (double) vv[ia]->heap_key (), a + (double) vv[ib]->heap_key ())));	    
				VV->heap_key( max((double) VV->heap_key(),
								  max (-b + (double) vv[ia]->heap_key (), -a + (double) vv[ib]->heap_key ())));	    			   
		}
    }
    if (alive.has(vv[vv.size() - 1]->Pos()) && alive.has (vv[0]->Pos())) 
	{
		int ia, ib;
		if (vv[vv.size() - 1]->heap_key() > vv[0]->heap_key() ) 
		{
			ia = vv.size() - 1;
			ib = 0;
		} else {
			ia = 0;
			ib = vv.size() - 1;
		}
		point A = { vv[ia]->X(), vv[ia]->Y(), vv[ia]->Z() };
		point B = { vv[ib]->X(), vv[ib]->Y(), vv[ib]->Z() };
		point C = { VV->X(),VV->Y(), VV->Z()};
		double a = (B - C).norm();
		double b = (A - C).norm();
		double u = vv[ia]->heap_key() - vv[ib]->heap_key();
		double sen = cross(B - C, A - C).norm() / (a * b);
		double sen2 = sen * sen;
		double cos = (B - C) * (A - C) / (a * b);
		double cos2 = cos * cos;
		
		double t = 0.5 * (2.0 * b * u - 2 * a * cos * u + 2 *
						  sqrt (a * a * cos2 * u * u - a * a * u * u +
								a * a * a * a * sen2 -
								2 * a * a * a * b * cos * sen2 +
								b * b * a * a * sen2))
		* b / (a * a - 2 * a * b * cos + b * b);
		
		if (u < t && a * cos < b * (t - u) / t && b * (t - u) / t < a / cos)
			VV->heap_key(max ((double)VV->heap_key(), -t + (double)vv[ia]->heap_key() ));
		else
			VV->heap_key( max ((double)VV->heap_key(),
							   max (-b + (double)vv[ia]->heap_key(), -a + (double)vv[ib]->heap_key() )));
    }
	
	vv.clear();
}


void Surface::SelectDraw()
{
    glInitNames ();
    glPushName (0);
    for (unsigned int i = 0; i < Vertices.size(); i++) {
		glLoadName (i + 1);
		glBegin (GL_POINTS);
		glVertex3f (Vertices[i]->X(), Vertices[i]->Y(),
					Vertices[i]->Z());
		glEnd ();
    }
	
}

void Surface::CorrectGeodesic()
{
    if (geodesicpath.head == NULL) 
    {
		cout << "Warning: empty geodesic path" << endl;
		return;
    }
    geoNode *gn0 = geodesicpath.head;
    geoNode *gn1 = gn0->next;
    geoNode *gn2 = NULL;
    if (gn1 != NULL)
		gn2 = gn1->next;
    if (gn1 == NULL || gn2 == NULL) 
    {
		//cout << "Geodesic with less than three points" << endl;
		return;
    }
    while (gn2 != NULL) 
    {
		// actualiza geodesica en gn1
	    if (gn1->V2 != NULL)	// no es un vertice
	    {
			point V1 = { gn1->V1->X(), gn1->V1->Y(), gn1->V1->Z() };
			point V2 = { gn1->V2->X(), gn1->V2->Y(), gn1->V2->Z() };
			point A, B;
			// Calcular A: punto gn0         
			if (gn0->V2 != NULL) 
			{
				double tt = gn0->t;
				A.x = tt * gn0->V1->X() + (1 - tt) * gn0->V2->X();
				A.y = tt * gn0->V1->Y() + (1 - tt) * gn0->V2->Y();
				A.z = tt * gn0->V1->Z() + (1 - tt) * gn0->V2->Z();
			} 
			else 
			{
				A.x = gn0->V1->X();
				A.y = gn0->V1->Y();
				A.z = gn0->V1->Z();
			}
			// Calcular B: punto gn2
			if (gn2->V2 != NULL) 
			{
				double tt = gn2->t;
				B.x = tt * gn2->V1->X() + (1 - tt) * gn2->V2->X();
				B.y = tt * gn2->V1->Y() + (1 - tt) * gn2->V2->Y();
				B.z = tt * gn2->V1->Z() + (1 - tt) * gn2->V2->Z();
			} 
			else 
			{
				B.x = gn2->V1->X();
				B.y = gn2->V1->Y();
				B.z = gn2->V1->Z();
			}
			// Calcular las proyecciones PA y PB de A y B en V1-V2              
			Vector v = V2 - V1;
			double vnorm = v.norm();
			v = (1.0 / vnorm) * v;
			Vector a = A - V1;
			Vector b = B - V1;
			if (b.norm() == 0.0 || cross (b, v).norm() == 0.0)	// B = V1 o B = V2
			{
				geodesicpath.remove(gn1);
			} 
			else 
			{
				point PA = V1 + (a * v) * v;
				point PB = V1 + (b * v) * v;
				// Calcular interseccion -Q- de A-B con V1-V2
				double h = cross(a, v).norm() / cross(b, v).norm();
				point Q = PA + (h / (1.0 + h)) * (PB - PA);
				// Calcular el parametro de interpolacion
				double par = 1.0 - (Q - V1).norm() / vnorm;
				//if (par <= TOL * vnorm)
				if (par <= TOL) {
					gn1->V1 = gn1->V2;
					gn1->V2 = NULL;
					gn1->t = 1.0;
				}
				//else if (par >= 1.0 - TOL * vnorm)
				else if (par >= 1.0 - TOL) 
				{
					gn1->V2 = NULL;
					gn1->t = 1.0;
				}
				else
					gn1->t = par;
				gn0 = gn1;
			}
		} 
		else			// se trata de un vertice
		{
			// 0.1 obtener la "estrella" del vertice gn1->V1
			vector < pVertex > vs = getVertexStar(gn1->V1->Pos ());
			
			// 0.2 Verificar que gn0 no esta' en el interior de la estrella
			bool flag = (gn0->V1 == gn1->V1) || (gn0->V2 == gn1->V1);
			while (flag) 
			{
				if (gn0->prev != NULL) 
				{
					geodesicpath.remove (gn0);
					gn0 = gn1->prev;
					flag = (gn0->V1 == gn1->V1) || (gn0->V2 == gn1->V1);
				}
				else
					flag = false;
			}
			// 0.3 Verificar que gn2 no esta' en el interior de la estrella
			flag = (gn2->V1 == gn1->V1) || (gn2->V2 == gn1->V1);
			while (flag) 
			{
				if (gn2->next != NULL) 
				{
					geodesicpath.remove(gn2);
					gn2 = gn1->next;
					flag = (gn2->V1 == gn1->V1) || (gn2->V2 == gn1->V1);
				} 
				else
					flag = false;
			}
			
			// 0.4 Encontrar pto inicial en vs
			// y Encontrar pto final en vs
			int pini = -1;
			int pend = -1;
			int pini2 = -1;
			int pend2 = -1;
			int s = vs.size();
			for (unsigned int i = 0; i < s; i++) 
			{
				if (gn0->V1 == vs[i])
					pini = i;
				if (gn2->V1 == vs[i])
					pend = i;
			}
			
			point Pini, Pend;
			if (gn0->V2 == NULL)	// gn0 es un vertice
			{
				Pini.x = gn0->V1->X ();
				Pini.y = gn0->V1->Y ();
				Pini.z = gn0->V1->Z ();
			}
			else		// gn0 esta' sobre la frontera de la estrella del vertice
			{
				if (pini == 0)
					pini2 = (gn0->V2 == vs[1]) ? 1 : s - 1;
				else if (pini == s - 1)
					pini2 = (gn0->V2 == vs[0]) ? 0 : s - 2;
				else
					pini2 =(gn0->V2 == vs[pini - 1]) ? pini - 1 : pini + 1;
				
				Pini.x = gn0->t * gn0->V1->X () + (1.0 - gn0->t) * gn0->V2->X ();
				Pini.y = gn0->t * gn0->V1->Y () + (1.0 - gn0->t) * gn0->V2->Y ();
				Pini.z = gn0->t * gn0->V1->Z () + (1.0 - gn0->t) * gn0->V2->Z ();
			}
			if (gn2->V2 == NULL) 
			{
				Pend.x = gn2->V1->X ();
				Pend.y = gn2->V1->Y ();
				Pend.z = gn2->V1->Z ();
			} 
			else 
			{
				if (pend == 0)
					pend2 = (gn2->V2 == vs[1]) ? 1 : s - 1;
				else if (pend == s - 1)
					pend2 = (gn2->V2 == vs[0]) ? 0 : s - 2;
				else
					pend2 = (gn2->V2 == vs[pend - 1]) ? pend - 1 : pend + 1;
				
				Pend.x = gn2->t * gn2->V1->X () + (1.0 - gn2->t) * gn2->V2->X ();
				Pend.y = gn2->t * gn2->V1->Y () + (1.0 - gn2->t) * gn2->V2->Y ();
				Pend.z = gn2->t * gn2->V1->Z () + (1.0 - gn2->t) * gn2->V2->Z ();
				
			}
			
			int si1, si2, se1, se2;
			
			if (pini2 == -1)	// el pto inicial es un vertice
			{
				si1 = (pini == s - 1) ? 0 : pini + 1;
				si2 = (pini == 0) ? s - 1 : pini - 1;
			}
			else		// el punto inicial esta sobre una arista
			{
				if (pini == 0) 
				{	
					si1 = (pini2 == 1) ? 1 : 0;
					si2 = (pini2 == 1) ? 0 : s - 1;
				}
				else if (pini == s - 1) 
				{
					si1 = (pini2 == 0) ? 0 : s - 1;
					si2 = (pini2 == 0) ? s - 1 : s - 2;
				}
				else 
				{
					si1 = (pini2 == pini + 1) ? pini2 : pini;
					si2 = (pini2 == pini + 1) ? pini : pini2;
				}
			}
			if (pend2 == -1)	// el pto final es un vertice
			{
				se1 = (pend == s - 1) ? 0 : pend + 1;
				se2 = (pend == 0) ? s - 1 : pend - 1;
			}
			else		// el punto inicial esta sobre una arista
			{
				if (pend == 0) 
				{
					se1 = (pend2 == 1) ? 1 : 0;
					se2 = (pend2 == 1) ? 0 : s - 1;
				}
				else if (pend == s - 1) 
				{
					se1 = (pend2 == 0) ? 0 : s - 1;
					se2 = (pend2 == 0) ? s - 1 : s - 2;
				}
				else 
				{
					se1 = (pend2 == pend + 1) ? pend2 : pend;
					se2 = (pend2 == pend + 1) ? pend : pend2;
				}
			}
			
			
			// 0.5 Crear listas de puntos
			vector < Vector > Right, Left;
			vector < double > Rnorm, Lnorm;
			vector < pVertex > RVS, LVS;
			point Center = { gn1->V1->X (), gn1->V1->Y (), gn1->V1->Z () };
			Vector V = Pini - Center;
			Rnorm.push_back (V.norm ());
			Lnorm.push_back (V.norm ());
			V = (1.0 / V.norm ()) * V;
			Right.push_back (V);
			Left.push_back (V);
			RVS.push_back (NULL);
			LVS.push_back (NULL);
			
			// 0.6 Recorrer la estrella en sentido positivo (Right)    
			int k;
			if (si1 <= se2)  
				for (k = si1; k <= se2; k++) 
				{
					point pp = { vs[k]->X (), vs[k]->Y (), vs[k]->Z () };
					V = pp - Center;
					Rnorm.push_back (V.norm ());
					V = (1.0 / V.norm ()) * V;
					Right.push_back (V);
					RVS.push_back (vs[k]);
				} 
			else 
			{
				for (k = si1; k < s; k++) 
				{
					point pp = { vs[k]->X (), vs[k]->Y (), vs[k]->Z () };
					V = pp - Center;
					Rnorm.push_back (V.norm ());
					V = (1.0 / V.norm ()) * V;
					Right.push_back (V);
					RVS.push_back (vs[k]);
				}
				for (k = 0; k <= se2; k++) 
				{
					point pp = { vs[k]->X (), vs[k]->Y (), vs[k]->Z () };
					V = pp - Center;
					Rnorm.push_back (V.norm ());
					V = (1.0 / V.norm ()) * V;
					Right.push_back (V);
					RVS.push_back (vs[k]);
				}
			}
			
			// 0.7 Recorrer la estrella en sentido negativo   
			if (si2 >= se1)
				for (k = si2; k >= se1; k--) 
				{
					point pp = { vs[k]->X (), vs[k]->Y (), vs[k]->Z () };
					V = pp - Center;
					Lnorm.push_back (V.norm ());
					V = (1.0 / V.norm ()) * V;
					Left.push_back (V);
					LVS.push_back (vs[k]);
				}
			else 
			{
				for (k = si2; k >= 0; k--) 
				{
					point pp = { vs[k]->X (), vs[k]->Y (), vs[k]->Z () };
					V = pp - Center;
					Lnorm.push_back (V.norm ());
					V = (1.0 / V.norm ()) * V;
					Left.push_back (V);
					LVS.push_back (vs[k]);
				}
				for (k = s - 1; k >= se1; k--) 
				{
					point pp = { vs[k]->X (), vs[k]->Y (), vs[k]->Z () };
					V = pp - Center;
					Lnorm.push_back (V.norm ());
					V = (1.0 / V.norm ()) * V;
					Left.push_back (V);
					LVS.push_back (vs[k]);
				}
			}
			// poner punto final
			V = Pend - Center;
			Rnorm.push_back (V.norm ());
			Lnorm.push_back (V.norm ());
			V = (1.0 / V.norm ()) * V;
			Right.push_back (V);
			Left.push_back (V);
			RVS.push_back (NULL);
			LVS.push_back (NULL);
			
			// 1. Calcular angulos a la derecha
			double Rangle = 0.0;
			vector < double >Rangs;
			Rangs.push_back (0.0);
			for (unsigned int i = 1; i < Right.size (); i++) 
			{
				double theta = fabs (acos (Right[i] * Right[i - 1]));
				Rangle += theta;
				Rangs.push_back (Rangle);
			}
			
			// 2. Calcular angulos a la izquierda
			double Langle = 0.0;
			vector < double >Langs;
			Langs.push_back (0.0);
			for (unsigned int i = 1; i < Left.size (); i++) 
			{
				double theta = fabs (acos (Left[i] * Left[i - 1]));
				Langle += theta;
				Langs.push_back (Langle);
			}
			
			// 3. Clasificar el vertice para decidir que
			//    procedimiento seguir para su correccion
			double sumangles = Rangle + Langle;
			int VertexType = (sumangles > 2 * pi + TOL) ? 
			HYPERBOLIC : (sumangles < 2 * pi - TOL) ?
			SPHERICAL : EUCLIDEAN;
			
			// 3.1 Decidir que calculos hacer
			int side = NOSIDE;
			switch (VertexType) 
			{
				case EUCLIDEAN:
					if (Langle < Rangle) 
					{
						side = LEFT;
					} 
					else if (Rangle < Langle) 
					{
						side = RIGHT;
					}
					else 
					{
						//nada que hacer
						side = NOSIDE;
					}
					break; 
				case SPHERICAL: 
					if (Langle < Rangle) 
					{
						side = LEFT;
					}
					else 
					{
						side = RIGHT;
					}
					break;
				case HYPERBOLIC:
					if (Langle > pi && Rangle > pi) 
					{ 
						// nada que hacer
						side = NOSIDE;
					} 
					else if (Langle < pi) 
					{
						side = LEFT;
					}
					else 
					{
						side = RIGHT;
					}
					
			}
			
			// 3.5 Si el punto esta en la frontera 
			//     y el lado derecho es menor que pi
			//     side = RIGHT, si no side = NOSIDE
			
			if (isInBorder(gn1->V1) )
				switch (side)
			{
				case LEFT:
					if (si2 > se1) // pasa por la frontera
						side = (Rangle < pi) ? RIGHT : NOSIDE; 
					break;
				case RIGHT:
					if (si1 > se2) // pasa por la frontera
						side = (Langle < pi) ? LEFT : NOSIDE;
					break;
			}
			
			vector < double >norms, angs;
			vector < pVertex > VS;
			switch (side) 
			{
				case RIGHT:
					norms = Rnorm;
					angs = Rangs;
					VS = RVS;
					break;
				case LEFT:
					norms = Lnorm;
					angs = Langs;
					VS = LVS;
					break;
				case NOSIDE:
					// nada que hacer
					;
			}
			
			// 4. Aplanar    
			if (norms.size () > 2) 
			{
				vector < point > planarstar;
				point pp = { norms[0], 0.0, 0.0 };
				planarstar.push_back (pp);
				for (unsigned int i = 1; i < norms.size (); i++) 
				{
					point pn = { norms[i] * cos (angs[i]), 
						norms[i] * sin (angs[i]), 
					0.0 };
					planarstar.push_back (pn);
				}
				
				// 5. Calcular la nueva aproximacion de la geodesica
				// 5.1 Encontrar ecuacion implicita de la recta
				Pini = planarstar[0];
				Pend = planarstar[planarstar.size () - 1];
				point recta;
				if (Pini.x == Pend.x)	// x = cte
				{
					recta.x = 1.0;
					recta.y = 0.0;
					recta.z = -Pini.x;
				} 
				else		// y = mx+n
				{
					recta.x = (Pend.y - Pini.y) / (Pend.x - Pini.x);	//m
					recta.y = -1.0;
					recta.z = Pini.y - recta.x * Pini.x;	//n
				}
				
				
				// 5.2. construir la aproximacion
				//    insertar en la lista
				VertexDistance *pV = gn1->V1;
				geodesicpath.remove (gn1);
				gn1 = NULL;
				for (unsigned int k = 1; k < planarstar.size () - 1; k++) 
				{
					// calcular interseccion y insertar
					// actualizando gn0 y gn1 si es preciso 
					double tpar = -recta.z / (recta.x * planarstar[k].x +
											  recta.y * planarstar[k].y);
					if (tpar > TOL * norms[k])	//if (tpar > 0.0)                                         
					{
						if (tpar < 1.0 - TOL * norms[k])	//if (tpar < 1.0 )                              
							geodesicpath.insert (VS[k], pV, tpar, gn0);
						else 
						{
							geodesicpath.insert (VS[k], NULL, 1.0, gn0);
						}
					} 
					else	// tpar < 0.0
					{
						geodesicpath.insert (pV, NULL, 1.0, gn0);
					}
					gn0 = gn0->next;
				}
			}
			
		}
		gn1 = gn2;
		gn0 = gn1->prev;
		gn2 = gn2->next;
    }
}


double Surface::geodesicerror ()
{
    double error = 0.0;
    //    double length = 0.0;
    geoIt *It = geodesicpath.makeIt ();
    // geoNode* g1;   
    // g1 = It->data();       
    ++(*It);
    // point P1,P2;
    // if (g1->V2 == NULL)
    // {
    // P1.x = g1->V1->X();
    // P1.y = g1->V1->Y();
    // P1.z = g1->V1->Z();
    // }
    // else
    // {
    // double tt = g1->t;
    // P1.x = tt*g1->V1->X() + (1.0-tt)*g1->V2->X();
    // P1.y = tt*g1->V1->Y() + (1.0-tt)*g1->V2->Y();
    // P1.z = tt*g1->V1->Z() + (1.0-tt)*g1->V2->Z();
    // 
    // }
    for (; It->more_elem (); ++(*It)) {
		geoNode *gn = It->data ();
		// 1. Calcula err
		double err = LocalError (gn);
		error = (err > error) ? err : error;
		
		//g1 = It->data();
		// if (g1->V2 == NULL)
		// {
		// P2.x = g1->V1->X();
		// P2.y = g1->V1->Y();
		// P2.z = g1->V1->Z();
		// }
		// else
		// {
		// double tt = g1->t;
		// P2.x = tt*g1->V1->X() + (1.0-tt)*g1->V2->X();
		// P2.y = tt*g1->V1->Y() + (1.0-tt)*g1->V2->Y();
		// P2.z = tt*g1->V1->Z() + (1.0-tt)*g1->V2->Z();
		// 
		// }
		//        Vector P1P2 = P2-P1;
		//        length += P1P2.norm();
		//        P1 = P2;
    }
    //    double error = fabs(geodLength - length);
    //   geodLength = length;
    return error;
}

double Surface::LocalError (geoNode * gn)
{
    if (gn == NULL) {
		return 0.0;
    }
    geoNode *ga = gn->prev;	// nodo anterior
    geoNode *gp = gn->next;	// nodo posterior      
    if (ga == NULL || gp == NULL) {
		return 0.0;
    }
    // 0. Localizar gn, ga y gp     
    point Pn;
    if (gn->V2 == NULL) {
		Pn.x = gn->V1->X ();
		Pn.y = gn->V1->Y ();
		Pn.z = gn->V1->Z ();
    } else {
		double tt = gn->t;
		Pn.x = tt * gn->V1->X () + (1 - tt) * gn->V2->X ();
		Pn.y = tt * gn->V1->Y () + (1 - tt) * gn->V2->Y ();
		Pn.z = tt * gn->V1->Z () + (1 - tt) * gn->V2->Z ();
    }
	
    point Pa;
    if (ga->V2 == NULL) {
		Pa.x = ga->V1->X ();
		Pa.y = ga->V1->Y ();
		Pa.z = ga->V1->Z ();
    } else {
		double tt = ga->t;
		Pa.x = tt * ga->V1->X () + (1 - tt) * ga->V2->X ();
		Pa.y = tt * ga->V1->Y () + (1 - tt) * ga->V2->Y ();
		Pa.z = tt * ga->V1->Z () + (1 - tt) * ga->V2->Z ();
		
    }
    point Pp;
    if (gp->V2 == NULL) {
		Pp.x = gp->V1->X ();
		Pp.y = gp->V1->Y ();
		Pp.z = gp->V1->Z ();
    } else {
		double tt = gp->t;
		Pp.x = tt * gp->V1->X () + (1 - tt) * gp->V2->X ();
		Pp.y = tt * gp->V1->Y () + (1 - tt) * gp->V2->Y ();
		Pp.z = tt * gp->V1->Z () + (1 - tt) * gp->V2->Z ();
    }
	
	
    if (gn->V2 != NULL)		// arista
    {
		// 1. Calcular angulos a la derecha
		point V1 = { gn->V1->X (), gn->V1->Y (), gn->V1->Z () };
		Vector va = Pa - Pn;
		Vector vp = Pp - Pn;
		Vector vn = V1 - Pn;
		if (va.norm () == 0 || vp.norm () == 0) {
			return 0.0;
		} else {
			va = (1.0 / va.norm ()) * va;
			vp = (1.0 / vp.norm ()) * vp;
			vn = (1.0 / vn.norm ()) * vn;
		}
		
		double alpha = acos (va * vn) + acos (vn * vp);
		
		// 2. Calcular angulos a la izquierda
		//    no es necesario -- vea coment. en 3.
		// 3. Retornar el valor absoluto de la 
		//    diferencia |alpha-beta|=2|pi-alpha|=2|pi-beta|
		return 2 * fabs (pi - alpha);
    } else			//vertice
    {
		// 0.1 obtener la "estrella" del vertice gn->V1
		vector < pVertex > vs = getVertexStar (gn->V1->Pos ());
		
		// 0.2 Verificar que ga no esta' en el interior de la estrella
		bool flag = (ga->V1 == gn->V1) || (ga->V2 == gn->V1);
		while (flag) {
			if (ga->prev != NULL) {
				geodesicpath.remove (ga);
				ga = gn->prev;
				flag = (ga->V1 == gn->V1) || (ga->V2 == gn->V1);
			} else
				flag = false;
		}
		// 0.3 Verificar que gp no esta' en el interior de la estrella
		flag = (gp->V1 == gn->V1) || (gp->V2 == gn->V1);
		while (flag) {
			if (gp->next != NULL) {
				geodesicpath.remove (gp);
				gp = gn->next;
				flag = (gp->V1 == gn->V1) || (gp->V2 == gn->V1);
			} else
				flag = false;
		}
		
		// 0.4 Encontrar pto inicial en vs
		// y Encontrar pto final en vs
		int pini = -1;
		int pend = -1;
		int pini2 = -1;
		int pend2 = -1;
		int s = vs.size ();
		for (unsigned int i = 0; i < s; i++) {
			if (ga->V1 == vs[i])
				pini = i;
			if (gp->V1 == vs[i])
				pend = i;
		}
		
		point Pini, Pend;
		if (ga->V2 == NULL)	// ga es un vertice
		{
			Pini.x = ga->V1->X ();
			Pini.y = ga->V1->Y ();
			Pini.z = ga->V1->Z ();
		} else			// ga esta' sobre la frontera de la estrella del vertice
		{
			if (pini == 0)
				pini2 = (ga->V2 == vs[1]) ? 1 : s - 1;
			else if (pini == s - 1)
				pini2 = (ga->V2 == vs[0]) ? 0 : s - 2;
			else
				pini2 = (ga->V2 == vs[pini - 1]) ? pini - 1 : pini + 1;
			
			Pini.x = ga->t * ga->V1->X () + (1.0 - ga->t) * ga->V2->X ();
			Pini.y = ga->t * ga->V1->Y () + (1.0 - ga->t) * ga->V2->Y ();
			Pini.z = ga->t * ga->V1->Z () + (1.0 - ga->t) * ga->V2->Z ();
		}
		if (gp->V2 == NULL) {
			Pend.x = gp->V1->X ();
			Pend.y = gp->V1->Y ();
			Pend.z = gp->V1->Z ();
		} else {
			if (pend == 0)
				pend2 = (gp->V2 == vs[1]) ? 1 : s - 1;
			else if (pend == s - 1)
				pend2 = (gp->V2 == vs[0]) ? 0 : s - 2;
			else
				pend2 = (gp->V2 == vs[pend - 1]) ? pend - 1 : pend + 1;
			
			Pend.x = gp->t * gp->V1->X () + (1.0 - gp->t) * gp->V2->X ();
			Pend.y = gp->t * gp->V1->Y () + (1.0 - gp->t) * gp->V2->Y ();
			Pend.z = gp->t * gp->V1->Z () + (1.0 - gp->t) * gp->V2->Z ();
			
		}
		
		int si1, si2, se1, se2;
		
		if (pini2 == -1)	// el pto inicial es un vertice
		{
			si1 = (pini == s - 1) ? 0 : pini + 1;
			si2 = (pini == 0) ? s - 1 : pini - 1;
		} else			// el punto inicial esta sobre una arista
		{
			if (pini == 0) {
				si1 = (pini2 == 1) ? 1 : 0;
				si2 = (pini2 == 1) ? 0 : s - 1;
			} else if (pini == s - 1) {
				si1 = (pini2 == 0) ? 0 : s - 1;
				si2 = (pini2 == 0) ? s - 1 : s - 2;
			} else {
				si1 = (pini2 == pini + 1) ? pini2 : pini;
				si2 = (pini2 == pini + 1) ? pini : pini2;
			}
		}
		if (pend2 == -1)	// el pto final es un vertice
		{
			se1 = (pend == s - 1) ? 0 : pend + 1;
			se2 = (pend == 0) ? s - 1 : pend - 1;
		} else			// el punto inicial esta sobre una arista
		{
			if (pend == 0) {
				se1 = (pend2 == 1) ? 1 : 0;
				se2 = (pend2 == 1) ? 0 : s - 1;
			} else if (pend == s - 1) {
				se1 = (pend2 == 0) ? 0 : s - 1;
				se2 = (pend2 == 0) ? s - 1 : s - 2;
			} else {
				se1 = (pend2 == pend + 1) ? pend2 : pend;
				se2 = (pend2 == pend + 1) ? pend : pend2;
			}
		}
		
		
		// 0.5 Crear listas de puntos
		vector < Vector > Right, Left;
		vector < double >Rnorm, Lnorm;
		vector < pVertex > RVS, LVS;
		point Center = { gn->V1->X (), gn->V1->Y (), gn->V1->Z () };
		Vector V = Pini - Center;
		Rnorm.push_back (V.norm ());
		Lnorm.push_back (V.norm ());
		V = (1.0 / V.norm ()) * V;
		Right.push_back (V);
		Left.push_back (V);
		RVS.push_back (NULL);
		LVS.push_back (NULL);
		
		// 0.6 Recorrer la estrella en sentido positivo (Right)    
		int k;
		if (si1 <= se2)
			for (k = si1; k <= se2; k++) {
				point pp = { vs[k]->X (), vs[k]->Y (), vs[k]->Z () };
				V = pp - Center;
				Rnorm.push_back (V.norm ());
				V = (1.0 / V.norm ()) * V;
				Right.push_back (V);
				RVS.push_back (vs[k]);
			} else {
				for (k = si1; k < s; k++) {
					point pp = { vs[k]->X (), vs[k]->Y (), vs[k]->Z () };
					V = pp - Center;
					Rnorm.push_back (V.norm ());
					V = (1.0 / V.norm ()) * V;
					Right.push_back (V);
					RVS.push_back (vs[k]);
				}
				for (k = 0; k <= se2; k++) {
					point pp = { vs[k]->X (), vs[k]->Y (), vs[k]->Z () };
					V = pp - Center;
					Rnorm.push_back (V.norm ());
					V = (1.0 / V.norm ()) * V;
					Right.push_back (V);
					RVS.push_back (vs[k]);
				}
			}
		
		// 0.7 Recorrer la estrella en sentido negativo   
		if (si2 >= se1)
			for (k = si2; k >= se1; k--) {
				point pp = { vs[k]->X (), vs[k]->Y (), vs[k]->Z () };
				V = pp - Center;
				Lnorm.push_back (V.norm ());
				V = (1.0 / V.norm ()) * V;
				Left.push_back (V);
				LVS.push_back (vs[k]);
			} else {
				for (k = si2; k >= 0; k--) {
					point pp = { vs[k]->X (), vs[k]->Y (), vs[k]->Z () };
					V = pp - Center;
					Lnorm.push_back (V.norm ());
					V = (1.0 / V.norm ()) * V;
					Left.push_back (V);
					LVS.push_back (vs[k]);
				}
				for (k = s - 1; k >= se1; k--) {
					point pp = { vs[k]->X (), vs[k]->Y (), vs[k]->Z () };
					V = pp - Center;
					Lnorm.push_back (V.norm ());
					V = (1.0 / V.norm ()) * V;
					Left.push_back (V);
					LVS.push_back (vs[k]);
				}
			}
		// poner punto final
		V = Pend - Center;
		Rnorm.push_back (V.norm ());
		Lnorm.push_back (V.norm ());
		V = (1.0 / V.norm ()) * V;
		Right.push_back (V);
		Left.push_back (V);
		RVS.push_back (NULL);
		LVS.push_back (NULL);
		
		// 1. Calcular angulos a la derecha
		double Rangle = 0.0;
		vector < double >Rangs;
		Rangs.push_back (0.0);
		for (unsigned int i = 1; i < Right.size (); i++) {
			double theta = fabs (acos (Right[i] * Right[i - 1]));
			Rangle += theta;
			Rangs.push_back (Rangle);
		}
		
		// 2. Calcular angulos a la izquierda
		double Langle = 0.0;
		vector < double >Langs;
		Langs.push_back (0.0);
		for (unsigned int i = 1; i < Left.size (); i++) {
			double theta = fabs (acos (Left[i] * Left[i - 1]));
			Langle += theta;
			Langs.push_back (Langle);
		}
		
		// 3. Clasificar el vertice para decidir que
		//    procedimiento seguir para su correccion
		double sumangles = Rangle + Langle;
		int VertexType = (sumangles > 2 * pi + TOL) ?
	    HYPERBOLIC : (sumangles < 2 * pi - TOL) ?
	    SPHERICAL : EUCLIDEAN;
		
		// 4. calcular error
		switch (VertexType) {
			case EUCLIDEAN:
				return fabs (Langle - Rangle);
				break;
			case HYPERBOLIC:
				if (Rangle > 2 * pi && Langle > 2 * pi)
					return 0.0;
				else
					return EPS;
				break;
			case SPHERICAL:
				return EPS;
				break;
		}
		
    }
}

bool Surface::isInBorder(VertexDistance* V)
{
	HalfEdge *h = V->getHalfEdge ();
	HalfEdge *hh = h;
	h = next_cw (h);
	if (h != NULL) 
	{
		h = h->Partner ();
	}
	
	while ((h != NULL) && (h != hh)) 
	{
		h = next_cw (h);
		if (h != NULL) 
		{
			h = h->Partner ();
		}
	}
	
	if (h == NULL)
	{
		printf("border vertex \n");
		return true;
	}
	else
		return false;
}

