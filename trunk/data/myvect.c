/*
 *------------------------------------------------------------
 *		myvect.c
 *		Marcelo Walter
 *		may, 96
 *
 *		A limited 3D vector library
 *------------------------------------------------------------
 */

#include <math.h>
#include "../common.h"

#define DAMN_SMALL (1e-30)
#define L_EPSILON 1.0e-10
#define SMALL (1.0e-12)
void checkIdentity( Matrix4 *a );

/* return vector difference c = a-b */
Point3D *V3Sub(Point3D *a, Point3D *b, Point3D *c)
{
	c->x = a->x - b->x;
	c->y = a->y - b->y;
	c->z = a->z - b->z;
	return(c);
}

/* return the cross product c = a cross b */
Point3D *V3Cross(Point3D *a, Point3D *b, Point3D *c)
{
	c->x = (a->y*b->z) - (a->z*b->y);
	c->y = (a->z*b->x) - (a->x*b->z);
	c->z = (a->x*b->y) - (a->y*b->x);
	return(c);
}

/* returns squared length of input vector */	
double V3SquaredLength(Point3D *a) 
{
	return((a->x * a->x)+(a->y * a->y)+(a->z * a->z));
}

/* returns length of input vector */
double V3Length(Point3D *a) 
{
	return(sqrt(V3SquaredLength(a)));
}

/* zero the input vector and returns it */
Point3D *V3Zero(Point3D *v) 
{
	v->x = 0.0;
	v->y = 0.0;
	v->z = 0.0;
	return(v);
}

/* divides the input vector by divisor and returns it */
Point3D *V3Divide(Point3D *v,  double divisor) 
{
	v->x = v->x/divisor;
	v->y = v->y/divisor;
	v->z = v->z/divisor;
	return(v);
}

/* multiplies the input vector by mult and returns it */
Point3D *V3Multiply(Point3D *v,  double mult) 
{
	v->x = v->x * mult;
	v->y = v->y * mult;
	v->z = v->z * mult;
	return(v);
}

/* add vectors u and v into u */
Point3D *V3AddPlus(Point3D *u,  Point3D *v) 
{
	u->x += v->x;
	u->y += v->y;
	u->z += v->z;
	return(u);
}

/* initialize a vector */
Point3D *V3Init(Point3D *v, double value)
{
	v->x = value;
	v->y = value;
	v->z = value;
	return(v);
	
}

/* assign source vector component-wise into destiny vector */
void V3Assign(Point3D *destiny, Point3D source)
{
	destiny->x = source.x;
	destiny->y = source.y;
	destiny->z = source.z;
}

/* return the dot product of vectors a and b */
double V3Dot(Point3D *a, Point3D *b) 
{
	return((a->x*b->x)+(a->y*b->y)+(a->z*b->z));
}

/* return the vector a normalized */
Point3D *V3Normalize(Point3D *a)
{
	return(V3Divide( a, V3Length(a)));
}
/*
 *-------------------------------------------------------------
 * multiply together matrices c = ab
 * note that c must not point to either of the input matrices
 *-------------------------------------------------------------
 */
void MatMul(Matrix4 *a, Matrix4 *b, Matrix4 *c)
{
	register int i, j, k;
	
	for (i=0; i<4; i++) {
		for (j=0; j<4; j++) {
			c->element[i][j] = 0;
			for (k=0; k<4; k++) c->element[i][j] += 
				a->element[i][k] * b->element[k][j];
		}
	}
}

/*
 *-------------------------------------------------------------
 * 	multiply together matrices 'a' and 'b'
 *	and returns the result in b
 *-------------------------------------------------------------
 */
void MatMulCum(Matrix4 *a, Matrix4 *b)
{
	register int i, j, k;
	Matrix4 tmp;
	
	for (i=0; i<4; i++) {
		for (j=0; j<4; j++) {
			tmp.element[i][j] = 0;
			for (k=0; k<4; k++){
				tmp.element[i][j] += a->element[i][k] * b->element[k][j];
				
			}
			/*printf("[%d][%d] = %f",i,j,tmp.element[i][j]);*/
		}
		/*printf("\n");*/
	}
	/*printf("\n");*/
	/* copy the result into b */
	for (i=0; i<4; i++) {
		for (j=0; j<4; j++) {
			b->element[i][j] = tmp.element[i][j];
			/*printf("[%d][%d] = %f",i,j,b->element[i][j]);*/
		}
		/*printf("\n");*/
	}
	/*printf("\n");*/
}	

/*
 *---------------------------------------------------------
 *	pos-multiplies a point by a matrix
 *---------------------------------------------------------
 */
void V3PosMul(Point3D *p, Matrix4 *m)
{
	double w;
	Point3D ptmp;
	
	ptmp.x = (p->x * m->element[0][0]) + (p->y * m->element[1][0]) + 
	(p->z * m->element[2][0]) + m->element[3][0];
	ptmp.y = (p->x * m->element[0][1]) + (p->y * m->element[1][1]) + 
	(p->z * m->element[2][1]) + m->element[3][1];
	ptmp.z = (p->x * m->element[0][2]) + (p->y * m->element[1][2]) + 
	(p->z * m->element[2][2]) + m->element[3][2];
	w = (p->x * m->element[0][3]) + (p->y * m->element[1][3]) + 
	(p->z * m->element[2][3]) + m->element[3][3];
    
	if (w != 0.0) { ptmp.x /= w;  ptmp.y /= w;  ptmp.z /= w; }
    
	*p = ptmp;
}

/*
 *---------------------------------------------------------
 *	pre-multiplies a point by a matrix
 *---------------------------------------------------------
 */
void V3PreMul(Point3D *p, Matrix4 *m)
{
	double w;
	Point3D ptmp;
	
	ptmp.x = (p->x * m->element[0][0]) + (p->y * m->element[0][1]) + (p->z * m->element[0][2]) + m->element[0][3];
	ptmp.y = (p->x * m->element[1][0]) + (p->y * m->element[1][1]) + (p->z * m->element[1][2]) + m->element[1][3];
	ptmp.z = (p->x * m->element[2][0]) + (p->y * m->element[2][1]) + (p->z * m->element[2][2]) + m->element[2][3];
	w = (p->x * m->element[3][0]) + (p->y * m->element[3][1]) + (p->z * m->element[3][2]) + m->element[3][3];
    
	if (w != 0.0) 
	{ 
		ptmp.x /= w;
		ptmp.y /= w;
		ptmp.z /= w;
	}
    
	*p = ptmp;
}

/*
 *---------------------------------------------------------
 * loads the identity matrix into m
 *---------------------------------------------------------
 */
void loadIdentity( Matrix4 *m )
{
	register int i, j;
	
	for (i=0; i<4; i++) {
		for (j=0; j<4; j++){
			m->element[i][j] = 0.0;
		}
	}
	for(i=0; i < 4; i++) m->element[i][i] = 1.0;
}

/*
 *---------------------------------------------------------
 * copy matrix source into destiny
 *---------------------------------------------------------
 */
void MatrixCopy( Matrix4 *source, Matrix4 *destiny)
{
	register int i,j;
	
	for(i = 0; i < 4; i++)
		for(j = 0; j < 4; j++)
			destiny->element[i][j] = source->element[i][j];
}

/*
 *---------------------------------------------------------
 * Compute a rotation matrix around an arbitrary axis
 * specified by 2 points p1 and p2; theta is the angle
 * between the two planes that share the same edge
 * defined by p1 and p2
 *
 * The matrix TM is the matrix that brings the plane of
 * face 'n' into face 'p'; Matrix iTM is the matrix
 * that brings the plane of face 'p' into 'n'
 * 'p' and 'n' are the names of faces according to the
 * winged edge data structure; 'p' is the previous face
 * and 'n' is the next face
 *---------------------------------------------------------
 */
int MxRotateAxis(Point3D p1, Point3D p2,
				 double theta, Matrix4 *TM, Matrix4 *iTM)
{
	Point3D p;
	double dist, cosX, sinX, cosY, sinY, det;
	Matrix4 m1, m2;
	
	p.x = p2.x - p1.x;
	p.y = p2.y - p1.y;
	p.z = p2.z - p1.z;
	
	if( V3Length( &p ) < 0.0 ) return(FALSE);
	
	V3Normalize( &p );
	
	dist = sqrt( p.y * p.y + p.z * p.z );
	
	/* maybe the 2 points are already aligned with the X-axis */
	if(dist < DAMN_SMALL || dist == 0.0){
		cosX = 1.0;
		sinX = 0.0;
	}
	else{
		cosX = p.z / dist;
		sinX = p.y / dist;
	}
	
	/* fprintf(stderr, "p: x = %3.2f y = %3.2f z = %3.2f dist = %3.2f\n",
	 p.x, p.y, p.z, dist); */
	
	cosY = dist;
	sinY = -p.x;
	
	loadIdentity( TM );
	loadIdentity( &m1 );
	
	/* inverse translation matrix */
	m1.element[0][3] = p1.x;
	m1.element[1][3] = p1.y;
	m1.element[2][3] = p1.z;
	/* fprintf(stderr,"Inverse Translation:\n");
	 printMatrix( m1 ); */
	
	/* Inverse Rotation around X */
	loadIdentity( &m2 );
	m2.element[1][1] = cosX;
	m2.element[2][1] = -sinX;
	m2.element[1][2] = sinX;
	m2.element[2][2] = cosX;
	/* fprintf(stderr,"Inverse Rotation around X:\n");
	 printMatrix( m2 ); */
	
	MatMul( &m1, &m2, TM );
	MatrixCopy( TM, &m1 );
	
	/* Inverse Rotation around Y */
	loadIdentity( &m2 );
	m2.element[0][0] = cosY;
	m2.element[2][0] = sinY;
	m2.element[0][2] = -sinY;
	m2.element[2][2] = cosY;
	/* fprintf(stderr,"Inverse Rotation around Y:\n");
	 printMatrix( m2 ); */
	
	MatMul( &m1, &m2, TM );
	MatrixCopy( TM, &m1 );
	
	/* Rotation around Z */
	loadIdentity( &m2 );
	m2.element[0][0]= cos( theta );
	m2.element[1][0]= -sin( theta );
	m2.element[0][1]= sin( theta );
	m2.element[1][1]= cos( theta );
	
	/* fprintf(stderr,"Rotation around Z:\n");
	 printMatrix( m2 ); */
	
	MatMul( &m1, &m2, TM );
	MatrixCopy( TM, &m1 );
	
	/* Rotation around Y */
	loadIdentity( &m2 );
	m2.element[0][0] = cosY;
	m2.element[2][0] = -sinY;
	m2.element[0][2] = sinY;
	m2.element[2][2] = cosY;
	/* fprintf(stderr,"Rotation around Y:\n");
	 printMatrix( m2 ); */
	
	MatMul( &m1, &m2, TM );
	MatrixCopy( TM, &m1 );
	
	/* Rotation around X */
	loadIdentity( &m2 );
	m2.element[1][1] = cosX;
	m2.element[2][1] = sinX;
	m2.element[1][2] = -sinX;
	m2.element[2][2] = cosX;
	/* fprintf(stderr,"Rotation around X:\n");
	 printMatrix( m2 ); */
	
	MatMul( &m1, &m2, TM );
	MatrixCopy( TM, &m1 );
	
	/* Translation matrix */
	loadIdentity( &m2 );
	m2.element[0][3] = -p1.x;
	m2.element[1][3] = -p1.y;
	m2.element[2][3] = -p1.z;
	/* fprintf(stderr,"Translation matrix:\n");
	 printMatrix( m2 ); */
	
	MatMul( &m1, &m2, TM );
	
	det = MxInvert(TM, iTM);
	if(det < L_EPSILON && det > -L_EPSILON)
		return( FALSE );
	/*return( MxInvert(TM, iTM) );*/
	else return ( TRUE );
}

/*
 *------------------------------------------------
 * Returns a rotation around z-axis matrix of
 * theta degrees
 *------------------------------------------------
 */
void MatRotateZ(double theta, Matrix4 *m)
{
	double sine, cosine;
	
	sine   = sin( theta );
	cosine = cos( theta );
	
	loadIdentity( m );
	
	m->element[0][0]= cosine;
	m->element[1][0]= -sine;
	m->element[0][1]= sine;
	m->element[1][1]= cosine;
}

/*
 *------------------------------------------------
 * Inverts matrix a and returns into b
 *------------------------------------------------
 */
double MxInvert( Matrix4 *a, Matrix4 *b)
{
	double det;
	Matrix4 at;
	int i, j;
	
	det = MxDeterminant( a, 4);
	
	if(det < L_EPSILON && det > -L_EPSILON)
		return(det);
	MxTranspose(a, &at);
	for (i=0; i< 4; i++) 
		for(j=0; j<4; j++) 
			b->element[i][j] = MxElementMinor(&at, i, j, 4) / det;
	return(det);
}

/*
 *------------------------------------------------------
 * Compute the determinant of matrix a of order 'order'
 *------------------------------------------------------
 */
double MxDeterminant ( Matrix4 *a, int order)
{
	double det, mux;
	Matrix4 n;
	int i, j, ii, jj, k, l;
	
	if( order == 1) return(a->element[0][0]);
	det = 0.0;
	mux = 1.0;
	i = 0;
	for (j=0; j < order; j++) {
		/* copy Matrix into smaller */
		k = 0;
		for (ii=0; ii < order; ii++) {
			l=0;
			if (ii==i) continue;
			for (jj=0; jj<order; jj++) {
				if (jj==j) continue;
				n.element[k][l] = a->element[ii][jj];
				l++;
			};
			k++;
		};
		det += mux * a->element[i][j] * MxDeterminant( &n, order-1);
		mux = -mux;
	};
	return(det);
}


static double MxElementMinor(Matrix4 *a, 
							 register int i, register int j, int order)
{
	int k, l, ii, jj;
	double sign;
	Matrix4 n;
	
	sign = (((i+j) % 2 ) == 0 ) ? 1.0 : -1.0;
	/* copy Matrix into smaller */
	k = 0;
	for (ii=0; ii<order; ii++) {
		if ( ii == i) continue;
		l=0;
		for (jj=0; jj<order; jj++) {
			if ( jj == j) continue;
			n.element[k][l] = a->element[ii][jj];
			l++;
		};
		k++;
	};
	return( sign * MxDeterminant( &n, 3));
}
/*
 *------------------------------------------------------
 * Transpose a into result r
 *------------------------------------------------------
 */
void MxTranspose( Matrix4 *a, Matrix4 *r)	
{
	register int i,j;
	Matrix4 tmp;
	
	for(i = 0; i < 4; i++)
		for(j = 0; j < 4; j++)
			tmp.element[j][i]=a->element[i][j];
	MatrixCopy( &tmp, r);
}

/*
 *------------------------------------------------------
 * Check if the matrix elements are smaller
 * than some EPSILON and if so make them to 0.0
 *------------------------------------------------------
 */
void MxZero ( Matrix4 *a )
{
	int i, j;
	
	for(i = 0; i < 4; i++)
		for ( j = 0; j < 4; j++)
			if (a->element[i][j] < SMALL) a->element[i][j] = 0.0;
    
}

void checkIdentity( Matrix4 *a )
{
	int i, j;
	
	for(i = 0; i < 4; i++){
		for ( j = 0; j < 4; j++){
			if ( i == j ){
				if ( a->element[i][j] > (1.0 + SMALL) ){
					fprintf(stderr, "element[%d][%d] = %lg\n", i, j, a->element[i][j]);
					errorMsg("Not identity matrix!");
				}
			}
			else{
				if ( a->element[i][j] > SMALL){
					fprintf(stderr, "element[%d][%d] = %lg\n", i, j, a->element[i][j]);
					errorMsg("Not identity matrix!");
				}
			}
		}
	}
}
/*
 *---------------------------------------------------------
 * Compute a rotation matrix around an arbitrary axis
 * specified by 2 points p1 and p2; theta is the angle
 * between the two planes that share the same edge
 * defined by p1 and p2
 *---------------------------------------------------------
 */
int MxRotateAxisAlain(Point3D p1, Point3D p2, double theta, Matrix4 *TM, Matrix4 *iTM)
{
	Point3D p;
	double dist, cosX, sinX, cosY, sinY;
	Matrix4 m1, m2, Identity;
	
	loadIdentity( &Identity );
	
	p.x = p2.x - p1.x;
	p.y = p2.y - p1.y;
	p.z = p2.z - p1.z;
	
	if( V3Length( &p ) < 0.0 ) return(FALSE);
	
	dist = sqrt( p.y * p.y + p.z * p.z );
	
	if(dist < DAMN_SMALL){
		cosX = 1.0;
		sinX = 0.0;
	}
	else{
		cosX =  p.z / dist;
		sinX = -p.y / dist;
	}
	
	cosY = dist;
	sinY = -p.x;
	
	loadIdentity( TM );
	
	TM->element[3][0] = -p1.x;
	TM->element[3][1] = -p1.y;
	TM->element[3][2] = -p1.z;
	
	MatrixCopy( TM, &m1);
	loadIdentity( &m2 );
	
	m2.element[1][1] = cosX;
	m2.element[2][1] = sinX;
	m2.element[1][2] = -sinX;
	m2.element[2][2] = cosX;
	
	MatMul( &m1, &m2, TM );
	MatrixCopy( TM, &m1 );
	loadIdentity( &m2 );
	
	m2.element[0][0] = cosY;
	m2.element[2][0] = sinY;
	m2.element[0][2] = -sinY;
	m2.element[2][2] = cosY;
	
	MatMul( &m1, &m2, TM );
	MatrixCopy( TM, &m1 );
	MatRotateZ( theta, &m2 );
	MatMul( &m1, &m2, TM );
	MatrixCopy( TM, &m1 );
	loadIdentity( &m2 );
	
	m2.element[0][0] = cosY;
	m2.element[2][0] = -sinY;
	m2.element[0][2] = sinY;
	m2.element[2][2] = cosY;
	
	MatMul( &m1, &m2, TM );
	MatrixCopy( TM, &m1 );
	loadIdentity( &m2 );
	
	m2.element[1][1] = cosX;
	m2.element[2][1] = -sinX;
	m2.element[1][2] = sinX;
	m2.element[2][2] = cosX;
	
	MatMul( &m1, &m2, TM );
	MatrixCopy( TM, &m1 );
	loadIdentity( &m2 );
	
	m2.element[3][0] = p1.x;
	m2.element[3][1] = p1.y;
	m2.element[3][2] = p1.z;
	
	MatMul( &m1, &m2, TM );
	
	return( MxInvert(TM, iTM) );
}

/*
 *------------------------------------------------------------
 * copies a matrix into another
 *------------------------------------------------------------
 */
void copyMatrix( Matrix4 *destiny, Matrix4 source )
{
	
	int i, j;
	
	for( i = 0; i < 4; i++ ){
		for( j = 0; j < 4; j++ ){
			destiny->element[i][j] = source.element[i][j];
		}
	}
}
