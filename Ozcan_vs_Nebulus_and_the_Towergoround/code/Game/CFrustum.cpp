//CFrustum.cpp, the frustum class

// system includes --------
#include <math.h>
//-------------------------

// header files -----------
#include "CFrustum.h"
#include "Math/CMatrix.h"
#include "Camera/CCamera.h"
#include "CMenu.h"

//-------------------------

// creates singleton frustum instance
CFrustum* CFrustum::instance() 
{
	static CFrustum frustumInstance; // create static frustum instance once
    return &frustumInstance; // return the address of the frustum instance
}

// constructor
CFrustum::CFrustum()
{
}

// destructor
CFrustum::~CFrustum()
{
}

// call this every frame or every time the camera moves to update the frustum
void CFrustum::calculateFrustum()
{
	CMatrix projection, modelView, clipping;

#ifdef USE_SHADERS
	// get the current projection matrix
	projection = CMenu::GetPerspectiveProjectionMatrix();

	// get the current model-view matrix
	modelView = CCamera::instance()->GetModelViewMatrix();
#else
	// get the current projection matrix
	glGetFloatv(GL_PROJECTION_MATRIX, projection.elements);

	// get the current model-view matrix
	glGetFloatv(GL_MODELVIEW_MATRIX, modelView.elements);
#endif

	// clipping matrix is projection multiplied by modelView
	clipping = projection * &modelView;

	// the sides of the frustum can now be calculated from the clipping matrix

	// get the right side of the frustum
	frustumSides[RIGHT].planeNormal.x = clipping.elements[3] - clipping.elements[0];
	frustumSides[RIGHT].planeNormal.y = clipping.elements[7] - clipping.elements[4];
	frustumSides[RIGHT].planeNormal.z = clipping.elements[11] - clipping.elements[8];
	frustumSides[RIGHT].planeConstant = clipping.elements[15] - clipping.elements[12];

	frustumSides[RIGHT].normalise(); // normalise the plane

	// get the left side of the frustum
	frustumSides[LEFT].planeNormal.x = clipping.elements[3] + clipping.elements[0];
	frustumSides[LEFT].planeNormal.y = clipping.elements[7] + clipping.elements[4];
	frustumSides[LEFT].planeNormal.z = clipping.elements[11] + clipping.elements[8];
	frustumSides[LEFT].planeConstant = clipping.elements[15] + clipping.elements[12];

	frustumSides[LEFT].normalise(); // normalise the plane

	// get the bottom side of the frustum
	frustumSides[BOTTOM].planeNormal.x = clipping.elements[3] + clipping.elements[1];
	frustumSides[BOTTOM].planeNormal.y = clipping.elements[7] + clipping.elements[5];
	frustumSides[BOTTOM].planeNormal.z = clipping.elements[11] + clipping.elements[9];
	frustumSides[BOTTOM].planeConstant = clipping.elements[15] + clipping.elements[13];

	frustumSides[BOTTOM].normalise(); // normalise the plane

	// get the top side of the frustum
	frustumSides[TOP].planeNormal.x = clipping.elements[3] - clipping.elements[1];
	frustumSides[TOP].planeNormal.y = clipping.elements[7] - clipping.elements[5];
	frustumSides[TOP].planeNormal.z = clipping.elements[11] - clipping.elements[9];
	frustumSides[TOP].planeConstant = clipping.elements[15] - clipping.elements[13];

	frustumSides[TOP].normalise(); // normalise the plane

	// get the back side of the frustum
	frustumSides[BACK].planeNormal.x = clipping.elements[3] - clipping.elements[2];
	frustumSides[BACK].planeNormal.y = clipping.elements[7] - clipping.elements[6];
	frustumSides[BACK].planeNormal.z = clipping.elements[11] - clipping.elements[10];
	frustumSides[BACK].planeConstant = clipping.elements[15] - clipping.elements[14];

	frustumSides[BACK].normalise(); // normalise the plane

	// get the front side of the frustum
	frustumSides[FRONT].planeNormal.x = clipping.elements[3] + clipping.elements[2];
	frustumSides[FRONT].planeNormal.y = clipping.elements[7] + clipping.elements[6];
	frustumSides[FRONT].planeNormal.z = clipping.elements[11] + clipping.elements[10];
	frustumSides[FRONT].planeConstant = clipping.elements[15] + clipping.elements[14];

	frustumSides[FRONT].normalise(); // normalise the plane

	// work out the position of the frustum vertices

	int planePoints[8][3] = {{FRONT, LEFT, TOP}, {FRONT, LEFT, BOTTOM}, {FRONT, RIGHT, TOP}, 
							{FRONT, RIGHT, BOTTOM}, {BACK, LEFT, TOP}, {BACK, LEFT, BOTTOM}, 
							{BACK, RIGHT, TOP}, {BACK, RIGHT, BOTTOM}};

	/*	this for loop finds the positions of the vertices through a three plane 
		intersection test which produces the point of intesection 
		(presuming no planes are parallel) */
	for (int i = 0; i < 8; i++)
	{
		int zero	= planePoints[i][0],
			one		= planePoints[i][1],
			two		= planePoints[i][2];

		// the following three plane intersection test produces the point of intersection
		frustumVertices[i] = 
			( ((frustumSides[two].planeNormal.crossProduct(frustumSides[one].planeNormal))*
			frustumSides[zero].planeConstant) + 

			((frustumSides[one].planeNormal.crossProduct(frustumSides[zero].planeNormal))*
			frustumSides[two].planeConstant) +

			((frustumSides[zero].planeNormal.crossProduct(frustumSides[two].planeNormal))*
			frustumSides[one].planeConstant) ) / 

			frustumSides[two].planeNormal.dotProduct(
			(frustumSides[zero].planeNormal.crossProduct(frustumSides[one].planeNormal)));
	}

	// find all the frustum edges

	// front edges
	frustumEdges[FRONT_LEFT] = frustumVertices[FRONT_LEFT_TOP] - &frustumVertices[FRONT_LEFT_BOTTOM];
	frustumEdges[FRONT_TOP] = frustumVertices[FRONT_RIGHT_TOP] - &frustumVertices[FRONT_LEFT_TOP];
	frustumEdges[FRONT_RIGHT] = frustumVertices[FRONT_RIGHT_BOTTOM] - &frustumVertices[FRONT_RIGHT_TOP];
	frustumEdges[FRONT_BOTTOM] = frustumVertices[FRONT_LEFT_BOTTOM] - &frustumVertices[FRONT_RIGHT_BOTTOM];

	//side edges
	frustumEdges[SIDE_LEFT_TOP] = frustumVertices[FRONT_LEFT_TOP] - &frustumVertices[BACK_LEFT_TOP];
	frustumEdges[SIDE_RIGHT_TOP] = frustumVertices[FRONT_RIGHT_TOP] - &frustumVertices[BACK_RIGHT_TOP];
	frustumEdges[SIDE_RIGHT_BOTTOM] = frustumVertices[FRONT_RIGHT_BOTTOM] - &frustumVertices[BACK_RIGHT_BOTTOM];
	frustumEdges[SIDE_LEFT_BOTTOM] = frustumVertices[FRONT_LEFT_BOTTOM] - &frustumVertices[BACK_LEFT_BOTTOM];

	//back edges
	frustumEdges[BACK_LEFT] = frustumVertices[BACK_LEFT_TOP] - &frustumVertices[BACK_LEFT_BOTTOM];
	frustumEdges[BACK_TOP] = frustumVertices[BACK_RIGHT_TOP] - &frustumVertices[BACK_LEFT_TOP];
	frustumEdges[BACK_RIGHT] = frustumVertices[BACK_RIGHT_BOTTOM] - &frustumVertices[BACK_RIGHT_TOP];
	frustumEdges[BACK_BOTTOM] = frustumVertices[BACK_LEFT_BOTTOM] - &frustumVertices[BACK_RIGHT_BOTTOM];
}

// see if a sphere intersects the frustum
bool CFrustum::sphereInFrustum(CVector* centre, float sphereRadius)
{
	// go through all the sides of the frustum
	for(int side = 0; side < 6; side++ )	
	{
		// if the center of the sphere is farther away from the plane than its radius
		if ((frustumSides[side].planeNormal.dotProduct(centre))+frustumSides[side].planeConstant <= -sphereRadius){
			// the distance was greater than the radius so the sphere is outside of the frustum
			return( false );}
	}
	
	// the sphere is inside of the frustum
	return( true );
}

// check if an axially aligned cube is in the frustum
bool CFrustum::axisAlignedCubeInFrustum(CVector* centre, float width)
{
	// get half the cubes width
	float size = width*0.5f;
	// cube vertice identifiers
	int leftBottomBack = 0, 
		rightBottomBack = 1, 
		leftTopBack = 2, 
		//rightTopBack = 3, 
		leftBottomFront = 4; 
		//rightBottomFront = 5, 
		//leftTopFront = 6, 
		//rightTopFront = 7;

	// work out the cube vertice positions
	CVector cubeVertice[] = 
		{CVector((centre->x - size), (centre->y - size), (centre->z - size)),
		CVector((centre->x + size), (centre->y - size), (centre->z - size)),
		CVector((centre->x - size), (centre->y + size), (centre->z - size)),
		CVector((centre->x + size), (centre->y + size), (centre->z - size)),
		CVector((centre->x - size), (centre->y - size), (centre->z + size)),
		CVector((centre->x + size), (centre->y - size), (centre->z + size)),
		CVector((centre->x - size), (centre->y + size), (centre->z + size)),
		CVector((centre->x + size), (centre->y + size), (centre->z + size))};


	// go through the sides of the frustum
	for(int i = 0; i < 6; i++)
	{
		for(int c = 0; c < 9; c++)
		{
			if (8 == c){ return(false);} // no intersection
			
			if((frustumSides[i].planeNormal.dotProduct(cubeVertice[c])+frustumSides[i].planeConstant) > 0.0f){
				break;} /*	if all the cubes vertices are behind a frustum plane, this break will never 
							be called and thus when c==8 the function will return false for no intersection */
		}			
	}

	// set the cube planes
	CPlane	cubeSides[] = {
		CPlane(CVector(1.0f, 0.0f, 0.0f), cubeVertice[leftBottomBack]),
		CPlane(CVector(0.0f, -1.0f, 0.0f), cubeVertice[leftTopBack]),
		CPlane(CVector(-1.0f, 0.0f, 0.0f), cubeVertice[rightBottomBack]),
		CPlane(CVector(0.0f, 1.0f, 0.0f), cubeVertice[leftBottomBack]),
		CPlane(CVector(0.0f, 0.0f, -1.0f), cubeVertice[leftBottomFront]),
		CPlane(CVector(0.0f, 0.0f, 1.0f), cubeVertice[leftBottomBack])};

	// go through the sides of the cube
	for(int i = 0; i < 6; i++)
	{
		for(int c = 0; c < 9; c++)
		{
			if (8 == c){ return(false);} // no intersection

			if((cubeSides[i].planeNormal.dotProduct(frustumVertices[c])+cubeSides[i].planeConstant) > 0.0f){
				break;}/*	if all the frustum vertices are behind a cube side, this break will never 
							be called and thus when c==8 the function will return false for no intersection */
		}
	}

	/*	using seperating axis theorem, make an axis from one edge of the frustum and cube,
		and see if they can be sepested on it (total eighteen axii) */
	
	// six distinct frustum edges
	CVector distinctFrustumEdges[] = {	CVector() = frustumEdges[FRONT_RIGHT],
										CVector() = frustumEdges[FRONT_BOTTOM],
										CVector() = frustumEdges[SIDE_LEFT_TOP], 
										CVector() = frustumEdges[SIDE_RIGHT_TOP],
										CVector() = frustumEdges[SIDE_RIGHT_BOTTOM], 
										CVector() = frustumEdges[SIDE_LEFT_BOTTOM]};

	// three distinct cube edges
	CVector cubeEdges[] = {	CVector() = (cubeVertice[rightBottomBack] - &cubeVertice[leftBottomBack]), 
							CVector() = (cubeVertice[leftTopBack] - &cubeVertice[leftBottomBack]), 
							CVector() = (cubeVertice[leftBottomFront] - &cubeVertice[leftBottomBack])};

	for (int frustumEdge = 0; frustumEdge < 6; frustumEdge++)
	{
		for (int cubeEdge = 0; cubeEdge < 3; cubeEdge++)
		{
			float	frustumMin	= FLT_MAX,	frustumMax	= -FLT_MAX, 
					cubeMin		= FLT_MAX,	cubeMax		= -FLT_MAX;

			CVector cross = 
				distinctFrustumEdges[frustumEdge].crossProduct(&cubeEdges[cubeEdge]);

			// go to next iteration if vectors are parallel so no plane can be derived
			if (cross.fuzzyEquals(CVector())){
				continue;}

			cross.normalise();
			
			// find min and max frustum verts
			for (int frustumVert = 0; frustumVert < TOTAL_FRUSTUM_VERTICES; frustumVert++)
			{
				float dot = frustumVertices[frustumVert].dotProduct(&cross);

				if (dot > frustumMax){ frustumMax = dot;}
				if (dot < frustumMin){ frustumMin = dot;}
			}
			
			// find min and max cube verts
			for (int cubeVert = 0; cubeVert < 8; cubeVert++)
			{
				float dot = cubeVertice[cubeVert].dotProduct(&cross);

				if (dot > cubeMax){ cubeMax = dot;}
				if (dot < cubeMin){ cubeMin = dot;}
			}

			if( (frustumMin > cubeMax) || (cubeMin > frustumMax) ){
				return( false );} // seperable on this axis so no collision
		}
	}

	return true; // the cube is in the frustum
}