/*
 *------------------------------------------------------------
 *	transformations.c
 *
 *	The routines in this module compute
 *	the many different transformation
 *	matrices applied to the primitives
 *
 *	Marcelo Walter
 *------------------------------------------------------------
 */

#include <math.h>

#include "common.h"
#include "transformations.h"

extern float **growthData;
extern float **animData;

flag animFlag = FALSE;

/*
 *----------------------------------------------------------
 *	Given an index primitive 'whichPrim' 
 *	returns a growth matrix correspondent
 *	to that primitive
 *	The growth time is implicit defined since
 *	I assume that the primitives have undergone
 *	the trasnformation already and therefore
 *	the height and radius for the given primitive
 *	will be correct when I call this routine
 *----------------------------------------------------------
 */
void buildGrowthMatrix(Matrix4 *m, int whichPrim)
{
	loadIdentity(m);
	
	m->element[0][0] = Prim[whichPrim].h;
	m->element[1][1] = Prim[whichPrim].r;
	m->element[2][2] = Prim[whichPrim].r;
}
/*
 *----------------------------------------------------------
 *	Given an index primitive 'whichPrim' 
 *	returns an inverse growth matrix correspondent
 *	to that primitive
 *	I assume that the primitives have undergone
 *	the trasnformation already and therefore
 *	the height and radius for the given primitive
 *	will be correct when I call this routine
 *----------------------------------------------------------
 */
void buildInvGrowthMatrix(Matrix4 *m, int whichPrim)
{
    loadIdentity(m);
	
    m->element[0][0] = 1.0 / Prim[whichPrim].h;
    m->element[1][1] = 1.0 / Prim[whichPrim].r;
    m->element[2][2] = 1.0 / Prim[whichPrim].r;
}
/*
 *----------------------------------------------------------
 *	Builds the scaling transformation matrix for the
 * 	primitive received
 *----------------------------------------------------------
 */
void buildScaleMatrix(Matrix4 *m, int whichPrim)
{
	
	loadIdentity(m);
	
	/* set up the scaling matrix */
	if ( whichPrim != WORLD){
		m->element[0][0] = Prim[whichPrim].scale[X];
		m->element[1][1] = Prim[whichPrim].scale[Y];
		m->element[2][2] = Prim[whichPrim].scale[Z];
	}
	
}
/*
 *----------------------------------------------------------
 *	Builds the inverse scaling transformation matrix
 *	for the primitive received
 *----------------------------------------------------------
 */
void buildInvScaleMatrix(Matrix4 *m, int whichPrim)
{
	
	loadIdentity(m);
	
	/* set up the scaling matrix */
	if ( whichPrim != WORLD){
		m->element[0][0] = 1.0 / Prim[whichPrim].scale[X];
		m->element[1][1] = 1.0 / Prim[whichPrim].scale[Y];
		m->element[2][2] = 1.0 / Prim[whichPrim].scale[Z];
	}
	
}
/*
 *----------------------------------------------------------
 *	Builds the translation transformation matrix for the
 * 	primitive received
 *----------------------------------------------------------
 */
void buildTranslMatrix(Matrix4 *m, int whichPrim)
{
	
	loadIdentity(m);
	m->element[0][3] = Prim[whichPrim].trans[X];
	m->element[1][3] = Prim[whichPrim].trans[Y];
	m->element[2][3] = Prim[whichPrim].trans[Z];
}
/*
 *----------------------------------------------------------
 *	Builds the inverse translation transformation matrix
 *	for the primitive received
 *----------------------------------------------------------
 */
void buildInvTranslMatrix(Matrix4 *m, int whichPrim)
{
	
	loadIdentity(m);
	m->element[0][3] = -Prim[whichPrim].trans[X];
	if(m->element[0][3] == -0){
		m->element[0][3] = 0;
	}
	m->element[1][3] = -Prim[whichPrim].trans[Y];
	if(m->element[1][3] == -0){
		m->element[1][3] = 0;
	}
	m->element[2][3] = -Prim[whichPrim].trans[Z];
	if(m->element[2][3] == -0){
		m->element[2][3] = 0;
	}
}
/*
 *----------------------------------------------------------
 *	Builds the rotation transformation matrix for the
 * 	primitive received
 *----------------------------------------------------------
 */
void buildRotMatrix(Matrix4 *m, int whichPrim)
{
	double cosx, sinx, cosy, siny, cosz, sinz;
	
	loadIdentity(m);
	
	//printf("sinz %.100lf\n", Prim[whichPrim].rot[Z]);
	//printf("siny %.100lf\n", Prim[whichPrim].rot[Y]);
	
	sinz = sin(Prim[whichPrim].rot[Z]);
	cosz = cos(Prim[whichPrim].rot[Z]);
	siny = sin(Prim[whichPrim].rot[Y]);
	cosy = cos(Prim[whichPrim].rot[Y]);
	sinx = sin(Prim[whichPrim].rot[X]);
	cosx = cos(Prim[whichPrim].rot[X]);
	
	//printf("sinz1 %.100lf\n", siny);
	// printf("siny1 %.100lG\n", sinx);
	
	/* order is Z Y X. Multiply all rotations into one matrix */
	m->element[0][0] = cosy*cosz;
	if(m->element[0][0] == -0){
		m->element[0][0] = 0;
	}
	// printf("m->element[0][0]: %.100lf\n", m->element[0][0]);
	
	m->element[0][1] = -cosx*sinz + sinx*siny*cosz;
	
	if(m->element[0][1] == -0){
		m->element[0][1] = 0;
	}
	// printf("m->element[0][1]: %.100lf\n", m->element[0][1]);
	
	m->element[0][2] = sinx*sinz + cosx*siny*cosz;
	
	if(m->element[0][2] == -0){
		m->element[0][2] = 0;
	}
	//printf("m->element[0][2]: %.100lf\n", m->element[0][2]);
	m->element[1][0] = cosy*sinz;
    if(m->element[1][0] == -0){
		m->element[1][0] = 0;
	}
	//printf("m->element[1][0]: %.100lf\n", m->element[1][0]);
	m->element[1][1] = cosx*cosz + sinx*siny*sinz;
    if(m->element[1][1] == -0){
		m->element[1][1] = 0;
	}
	//printf("m->element[1][1]: %.100lf\n", m->element[1][1]);
	m->element[1][2] = -sinx*cosz + cosx*siny*sinz;
    if(m->element[1][2] == -0){
		m->element[1][2] = 0;
	}
	//printf("m->element[1][2]: %.100lf\n", m->element[1][2]);
	
	
	m->element[2][0] = -siny;
	if(m->element[2][0] == -0){
		m->element[2][0] = 0;
	}
	// printf("m->element[2][0]: %lg\n", m->element[2][0]);
	m->element[2][1] = sinx*cosy;
	if(m->element[2][1] == -0){
		m->element[2][1] = 0;
	}
	// printf("m->element[2][1]: %lf\n", m->element[2][1]);
	m->element[2][2] = cosx*cosy;
	if(m->element[2][2] == -0){
		m->element[2][2] = 0;
	}
	// printf("m->element[2][2]: %lf\n", m->element[2][2]);
}
/*
 *----------------------------------------------------------
 *	Builds the inverse rotation transformation matrix
 *	for the primitive received
 *----------------------------------------------------------
 */
void buildInvRotMatrix(Matrix4 *m, int whichPrim)
{
	double cosx, sinx, cosy, siny, cosz, sinz;
	
	loadIdentity(m);
	
	sinz = sin(-Prim[whichPrim].rot[Z]);
	cosz = cos(-Prim[whichPrim].rot[Z]);
	siny = sin(-Prim[whichPrim].rot[Y]);
	cosy = cos(-Prim[whichPrim].rot[Y]);
	sinx = sin(-Prim[whichPrim].rot[X]);
	cosx = cos(-Prim[whichPrim].rot[X]);
	
	/* order is X Y Z. Multiply all rotations into one matrix */
	m->element[0][0] = cosy*cosz;
	if(m->element[0][0] == -0){
		m->element[0][0] = 0;
	}
	//printf("m->element[0][0]: %lf\n", m->element[0][0]);
	m->element[0][1] = -cosy*sinz;
	if(m->element[0][1] == -0){
		m->element[0][1] = 0;
	}
	// printf("m->element[0][1]: %lf\n", m->element[0][1]);
	m->element[0][2] = siny;
	if(m->element[0][2] == -0){
		m->element[0][2] = 0;
	}
	// printf("m->element[0][2]: %lf\n", m->element[0][2]);
    
	m->element[1][0] = sinx*siny*cosz + cosx*sinz;
	if(m->element[1][0] == -0){
		m->element[1][0] = 0;
	}
	// printf("m->element[1][0]: %lf\n", m->element[1][0]);
	m->element[1][1] = -sinx*siny*sinz + cosx*cosz;
	if(m->element[1][1] == -0){
		m->element[1][1] = 0;
	}
	// printf("m->element[1][0]: %lf\n", m->element[1][1]);
	m->element[1][2] = -sinx*cosy;
	if(m->element[1][2] == -0){
		m->element[1][2] = 0;
	}
	//printf("m->element[1][2]: %lf\n", m->element[1][2]);
	
	m->element[2][0] = -cosx*siny*cosz + sinx*sinz;
	if(m->element[2][0] == -0){
		m->element[2][0] = 0;
	}
	// printf("m->element[2][0]: %lf\n", m->element[2][0]);
	m->element[2][1] = cosx*siny*sinz + sinx*cosz;
	if(m->element[2][1] == -0){
		m->element[2][1] = 0;
	}
	//printf("m->element[2][1]: %lf\n", m->element[2][1]);
	m->element[2][2] = cosx*cosy;
	if(m->element[2][2] == -0){
		m->element[2][2] = 0;
	}
	// printf("m->element[2][2]: %lf\n", m->element[2][2]);
}

/*
 *----------------------------------------------------------
 *	Builds the inverse transformation matrix for the
 * 	primitive received
 *	Inverse means from WORLD space into primitive
 *	space
 *----------------------------------------------------------
 */
void buildInvTransfMatrix(Matrix4 *m, int whichPrim)
{
	Matrix4 s, r, t, temp;
	double cosx, sinx, cosy, siny, cosz, sinz;
	
	loadIdentity(&s);
	loadIdentity(&r);
	loadIdentity(&t);
	/* set up the scaling matrix */
	if ( whichPrim != WORLD){
		s.element[0][0] = 1.0/Prim[whichPrim].scale[X];
		s.element[1][1] = 1.0/Prim[whichPrim].scale[Y];
		s.element[2][2] = 1.0/Prim[whichPrim].scale[Z];
	}
	
	t.element[0][3] = -Prim[whichPrim].trans[X];
	t.element[1][3] = -Prim[whichPrim].trans[Y];
	t.element[2][3] = -Prim[whichPrim].trans[Z];
	
	/* set up the rotation matrix */
	sinz = sin(-Prim[whichPrim].rot[Z]);
	cosz = cos(-Prim[whichPrim].rot[Z]);
	siny = sin(-Prim[whichPrim].rot[Y]);
	cosy = cos(-Prim[whichPrim].rot[Y]);
	sinx = sin(-Prim[whichPrim].rot[X]);
	cosx = cos(-Prim[whichPrim].rot[X]);
	
	/* order is X Y Z. Multiply all rotations into one matrix */
	r.element[0][0] = cosy*cosz;
	if(m->element[0][0] == -0){
		m->element[0][0] = 0;
	}
	r.element[0][1] = -cosy*sinz;
	if(m->element[0][1] == -0){
		m->element[0][1] = 0;
	}
	r.element[0][2] = siny;
	if(m->element[0][2] == -0){
		m->element[0][2] = 0;
	}  
	r.element[1][0] = sinx*siny*cosz + cosx*sinz;
	if(m->element[1][0] == -0){
		m->element[1][0] = 0;
	}
	r.element[1][1] = -sinx*siny*sinz + cosx*cosz;
	if(m->element[1][1] == -0){
		m->element[1][1] = 0;
	}
	r.element[1][2] = -sinx*cosy;
	if(m->element[1][2] == -0){
		m->element[1][2] = 0;
	}
	
	r.element[2][0] = -cosx*siny*cosz + sinx*sinz;
	if(m->element[2][0] == -0){
		m->element[2][0] = 0;
	}
	r.element[2][1] = cosx*siny*sinz + sinx*cosz;
	if(m->element[2][1] == -0){
		m->element[2][1] = 0;
	}
	r.element[2][2] = cosx*cosy;
	if(m->element[2][2] == -0){
		m->element[2][2] = 0;
	}
    
	MatMul(&s, &r, &temp);
	MatMul(&temp, &t, m );
}
/*
 *----------------------------------------------------------
 *	Builds the inverse transformation matrix for the
 * 	primitive received INCLUDING THE INVERSE OF THE
 *	GROWTH INFORMATION. I AM NOT USING THIS!!!
 *----------------------------------------------------------
 */
void buildInvTransfMatrixCells(Matrix4 *m, int whichPrim)
{
	Matrix4 s, r, t, g, temp, temp1;
	double cosx, sinx, cosy, siny, cosz, sinz;
	
	loadIdentity(&s);
	loadIdentity(&r);
	loadIdentity(&t);
	loadIdentity(&g);
	
	g.element[0][0] = 1.0/Prim[whichPrim].h;
	g.element[1][1] = 1.0/Prim[whichPrim].r;
	g.element[2][2] = 1.0/Prim[whichPrim].r;
	
	/* set up the scaling matrix */
	if ( whichPrim != WORLD){
		s.element[0][0] = 1.0/Prim[whichPrim].scale[X];
		s.element[1][1] = 1.0/Prim[whichPrim].scale[Y];
		s.element[2][2] = 1.0/Prim[whichPrim].scale[Z];
	}
	
	t.element[0][3] = -Prim[whichPrim].trans[X];
	t.element[1][3] = -Prim[whichPrim].trans[Y];
	t.element[2][3] = -Prim[whichPrim].trans[Z];
	
	/* set up the rotation matrix */
	sinz = sin(-Prim[whichPrim].rot[Z]);
	cosz = cos(-Prim[whichPrim].rot[Z]);
	siny = sin(-Prim[whichPrim].rot[Y]);
	cosy = cos(-Prim[whichPrim].rot[Y]);
	sinx = sin(-Prim[whichPrim].rot[X]);
	cosx = cos(-Prim[whichPrim].rot[X]);
	
	/* order is X Y Z. Multiply all rotations into one matrix */
	r.element[0][0] = cosy*cosz;
	r.element[0][1] = -cosy*sinz;
	r.element[0][2] = siny;
    
	r.element[1][0] = sinx*siny*cosz + cosx*sinz;
	r.element[1][1] = -sinx*siny*sinz + cosx*cosz;
	r.element[1][2] = -sinx*cosy;
	
	r.element[2][0] = -cosx*siny*cosz + sinx*sinz;
	r.element[2][1] = cosx*siny*sinz + sinx*cosz;
	r.element[2][2] = cosx*cosy;
    
	MatMul(&s, &g, &temp1);
	MatMul(&temp1, &r, &temp);
	MatMul(&temp, &t, m );
}
/*
 *----------------------------------------------------------
 *	Builds the inverse transformation matrix for the
 * 	primitive received
 *	Inverse means from WORLD space into primitive
 *	space
 *----------------------------------------------------------
 */
void buildInvTransfMatrixSpecial(Matrix4 *m, int whichPrim)
{
	Matrix4 r, t, temp;
	double cosx, sinx, cosy, siny, cosz, sinz;
	
	loadIdentity(&r);
	loadIdentity(&t);
	/* set up the inverse translation matrix */
	t.element[0][3] = -Prim[whichPrim].trans[X];
	t.element[1][3] = -Prim[whichPrim].trans[Y];
	t.element[2][3] = -Prim[whichPrim].trans[Z];
	
	/* set up the inverse rotation matrix */
	sinz = sin(-Prim[whichPrim].rot[Z]);
	cosz = cos(-Prim[whichPrim].rot[Z]);
	siny = sin(-Prim[whichPrim].rot[Y]);
	cosy = cos(-Prim[whichPrim].rot[Y]);
	sinx = sin(-Prim[whichPrim].rot[X]);
	cosx = cos(-Prim[whichPrim].rot[X]);
	
	/* order is X Y Z. Multiply all rotations into one matrix */
	r.element[0][0] = cosy*cosz;
	r.element[0][1] = -cosy*sinz;
	r.element[0][2] = siny;
	
	r.element[1][0] = sinx*siny*cosz + cosx*sinz;
	r.element[1][1] = -sinx*siny*sinz + cosx*cosz;
	r.element[1][2] = -sinx*cosy;
	
	r.element[2][0] = -cosx*siny*cosz + sinx*sinz;
	r.element[2][1] = cosx*siny*sinz + sinx*cosz;
	r.element[2][2] = cosx*cosy;
	
	MatMul(&r, &t, m);
	
}
/*
 *----------------------------------------------------------
 *	Builds a transformation matrix for the primitive
 *	received, that is, from PRIMITIVE space into
 *	WORLD space
 *----------------------------------------------------------
 */
void buildTransfMatrix(Matrix4 *m, int whichPrim)
{
	Matrix4 s, r, t, temp;
	double cosx, sinx, cosy, siny, cosz, sinz;
    
	loadIdentity(&s);
	loadIdentity(&r);
	loadIdentity(&t);
	
	/* set up the scaling matrix */
	if ( whichPrim != WORLD){
		/* scaling only if it is an individual primitive */
		s.element[0][0] = Prim[whichPrim].scale[X];
		s.element[1][1] = Prim[whichPrim].scale[Y];
		s.element[2][2] = Prim[whichPrim].scale[Z];
	}
	
	t.element[0][3] = Prim[whichPrim].trans[X];
	t.element[1][3] = Prim[whichPrim].trans[Y];
	t.element[2][3] = Prim[whichPrim].trans[Z];
   	
	/* set up the rotation matrix */
	sinz = sin(Prim[whichPrim].rot[Z]);
	cosz = cos(Prim[whichPrim].rot[Z]);
	siny = sin(Prim[whichPrim].rot[Y]);
	cosy = cos(Prim[whichPrim].rot[Y]);
	sinx = sin(Prim[whichPrim].rot[X]);
	cosx = cos(Prim[whichPrim].rot[X]);
	
	/* order is Z Y X. Multiply all rotations into one matrix r */
	r.element[0][0] = cosy*cosz;
	if(m->element[0][0] == -0){
		m->element[0][0] = 0;
    }
	r.element[0][1] = -cosx*sinz + sinx*siny*cosz;
	if(m->element[0][1] == -0){
		m->element[0][1] = 0;
    }
	r.element[0][2] = sinx*sinz + cosx*siny*cosz;
    if(m->element[0][2] == -0){
		m->element[0][2] = 0;
    }
	r.element[1][0] = cosy*sinz;
	if(m->element[1][0] == -0){
		m->element[1][0] = 0;
    }
	r.element[1][1] = cosx*cosz + sinx*siny*sinz;
	if(m->element[1][1] == -0){
		m->element[1][1] = 0;
    }
	r.element[1][2] = -sinx*cosz + cosx*siny*sinz;
	if(m->element[1][2] == -0){
		m->element[1][2] = 0;
    }
	r.element[2][0] = -siny;
	if(m->element[2][0] == -0){
		m->element[2][0] = 0;
    }
	r.element[2][1] = sinx*cosy;
	if(m->element[2][1] == -0){
		m->element[2][1] = 0;
    }
	r.element[2][2] = cosx*cosy;
	if(m->element[2][2] == -0){
		m->element[2][2] = 0;
    }
	
	MatMul(&t, &r, &temp);
	MatMul(&temp, &s, m );
}


/*
 *----------------------------------------------------------
 *	Builds a transformation matrix for the primitive
 *	received, that is, from PRIMITIVE space into
 *	WORLD space
 *----------------------------------------------------------
 */
void buildTransfMatrixSpecial(Matrix4 *m, int whichPrim)
{
	Matrix4 r, t, temp;
	double cosx, sinx, cosy, siny, cosz, sinz;
    
	loadIdentity(&r);
	loadIdentity(&t);
	
	/* set up the translation matrix */
	t.element[0][3] = Prim[whichPrim].trans[X];
	t.element[1][3] = Prim[whichPrim].trans[Y];
	t.element[2][3] = Prim[whichPrim].trans[Z];
   	
	/* set up the rotation matrix */
	sinz = sin(Prim[whichPrim].rot[Z]);
	cosz = cos(Prim[whichPrim].rot[Z]);
	siny = sin(Prim[whichPrim].rot[Y]);
	cosy = cos(Prim[whichPrim].rot[Y]);
	sinx = sin(Prim[whichPrim].rot[X]);
	cosx = cos(Prim[whichPrim].rot[X]);
	
	/* order is Z Y X. Multiply all rotations into one matrix r */
	r.element[0][0] = cosy*cosz;
	if(m->element[0][0] == -0){
		m->element[0][0] = 0;
    }
	r.element[0][1] = -cosx*sinz + sinx*siny*cosz;
	if(m->element[0][1] == -0){
		m->element[0][1] = 0;
    }
	r.element[0][2] = sinx*sinz + cosx*siny*cosz;
	if(m->element[0][2] == -0){
		m->element[0][2] = 0;
    }
	
	r.element[1][0] = cosy*sinz;
	if(m->element[1][0] == -0){
		m->element[1][0] = 0;
    }
	r.element[1][1] = cosx*cosz + sinx*siny*sinz;
	if(m->element[1][1] == -0){
		m->element[1][1] = 0;
    }
	r.element[1][2] = -sinx*cosz + cosx*siny*sinz;
	if(m->element[1][2] == -0){
		m->element[1][2] = 0;
    }
	r.element[2][0] = -siny;
	if(m->element[2][0] == -0){
		m->element[2][0] = 0;
    }
	r.element[2][1] = sinx*cosy;
	if(m->element[2][1] == -0){
		m->element[2][1] = 0;
    }
	r.element[2][2] = cosx*cosy;
	if(m->element[2][2] == -0){
		m->element[2][2] = 0;
    }
	
	MatMul(&t, &r, m);
}

/*
 *----------------------------------------------------------
 *	Builds the rotation transformation matrix for the
 * 	primitive received
 *----------------------------------------------------------
 */
void buildTestRot( Matrix4 *m , int primIndex, int t)
{
	double cosx, sinx, cosy, siny, cosz, sinz;
	extern int FINAL_GROWTH_TIME;
	extern int FINAL_ANIM_TIME;
	int row;
	float angle;
	int myTime;
	
	loadIdentity(m);
	
	/* MARCELO, before you can use t, you have to check
     if you are not in the 'pattern+growth' phase */
	if ( computingPatternFlag && Prim[primIndex].animation != -1 ){
		/* I am not too sure yet what to do here! */
		return;
	}
	/* if the primitive has animation information associated
     it will go ahead and compute the rotation matrix.
     Otherwise it will return the Identity matrix */
	
	
	
	if(Prim[primIndex].animation != -1 && t > 0){
		/* compute 'mytime' */
		if (animFlag) myTime = t - (FINAL_GROWTH_TIME - FINAL_ANIM_TIME);
		else myTime = 0;
		
		row = Prim[primIndex].animation;
		
		switch(Prim[primIndex].axis){
			case X:
				/* we just want to rotate about the 'x' axis */
				siny = 0.0;
				cosy = 1.0;
				sinz = 0.0;
				cosz = 1.0;
				angle = animData[row][0] - animData[row][myTime];
				sinx = sin( deg2rad(angle) );
				cosx = cos( deg2rad(angle) );
				break;
			case Y:
				/* we just want to rotate about the 'y' axes */
				sinx = 0.0;
				cosx = 1.0;
				sinz = 0.0;
				cosz = 1.0;
				angle = animData[row][0] - animData[row][myTime];
				siny = sin( deg2rad(angle) );
				cosy = cos( deg2rad(angle) );
				break;
			case Z:
				/* we just want to rotate about the 'z' axes */
				sinx = 0.0;
				cosx = 1.0;
				siny = 0.0;
				cosy = 1.0;
				angle = animData[row][0] - animData[row][myTime];
				sinz = sin( deg2rad(angle) );
				cosz = cos( deg2rad(angle) );
				break;
			default:
				break;
		}
		/* finally compute the matrix */  
		/* order is Z Y X. Multiply all rotations into one matrix */
		m->element[0][0] = cosy*cosz;
		if(m->element[0][0] == -0){
			m->element[0][0] = 0;
		}
		m->element[0][1] = -cosx*sinz + sinx*siny*cosz;
		if(m->element[0][1] == -0){
			m->element[0][1] = 0;
		}
		m->element[0][2] = sinx*sinz + cosx*siny*cosz;
		if(m->element[0][2] == -0){
			m->element[0][2] = 0;
		} 
		m->element[1][0] = cosy*sinz;
		if(m->element[1][0] == -0){
			m->element[1][0] = 0;
		}
		m->element[1][1] = cosx*cosz + sinx*siny*sinz;
		if(m->element[1][1] == -0){
			m->element[1][1] = 0;
		}
		m->element[1][2] = -sinx*cosz + cosx*siny*sinz;
		if(m->element[1][2] == -0){
			m->element[1][2] = 0;
		}
		m->element[2][0] = -siny;
		if(m->element[2][0] == -0){
			m->element[2][0] = 0;
		}
		m->element[2][1] = sinx*cosy;
		if(m->element[2][1] == -0){
			m->element[2][1] = 0;
		}
		m->element[2][2] = cosx*cosy;
		if(m->element[2][2] == -0){
			m->element[2][2] = 0;
		}
	}
}
