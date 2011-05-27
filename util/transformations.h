/*
 *	transformations.h
 */

/* Prototypes */
void buildTransfMatrix(Matrix4 *m, int whichPrim);
void buildTransfMatrixSpecial(Matrix4 *m, int whichPrim);
void buildInvTransfMatrix(Matrix4 *m, int whichPrim);
void buildInvTransfMatrixSpecial(Matrix4 *m, int whichPrim);
void buildScaleMatrix(Matrix4 *m, int whichPrim);
void buildInvScaleMatrix(Matrix4 *m, int whichPrim);
void buildTranslMatrix(Matrix4 *m, int whichPrim);
void buildInvTranslMatrix(Matrix4 *m, int whichPrim);
void buildRotMatrix(Matrix4 *m, int whichPrim);
void buildInvRotMatrix(Matrix4 *m, int whichPrim);
void buildGrowthMatrix(Matrix4 *m, int whichPrim);
void buildInvGrowthMatrix(Matrix4 *m, int whichPrim);
void buildTestRot( Matrix4 *m, int primIndex, int t );
void buildInvTransfMatrixCells(Matrix4 *m, int whichPrim);