//------------------------------------------------------
// BASED ON CODE FROM GAMETUTORIALS.COM
//------------------------------------------------------

//intersections.h, geometric intersection functions

#ifndef _intersections_h_
#define _intersections_h_

//---system includes---------
#include <math.h>		// maths functions
#include "Rendering/OpenGLInclude.h"
//---------------------------

//header files---------
#include "Math/CVector.h"
#include "Math/CPlane.h"
#include "Game/defines.h"
//---------------------

//defines--------------	
enum sphereTriangleIntersectionClassifications
{
	SPHERE_BEHIND_PLANE,			
	SPHERE_INTERSECTS_PLANE,		
	SPHERE_IN_FRONT_OF_PLANE		 
};             

//---------------------

//external variables --
//---------------------

//global variables-----
//---------------------

//----functions----------------------

// intersection of two axis aligned boxes
int axisAlignedBoxBoxIntersection(	CVector *box1Centre, CVector *box1HalfWidths, 
									CVector *box2Centre, CVector *box2HalfWidths);

/*	main functionality of sphereBox method from 
	Dunn & Parberry - 3D Math Primer for Graphics and Game Development */
int sphereAxisAlignedBoxIntersection(CVector *boxCentre, CVector *boxHalfWidths, 
									 CVector *sphereCentre, float sphereRadius);

//see if a triangle and an axis aligned box have intersected
bool triangleAxisAlignedBoxIntersection(CVector *boxCentre, CVector *halfBoxWidths, 
										CVector * triangleVertex);

// see if a plane and an axis aligned origin centered box have intersected
bool planeAxisAlignedOriginCenteredBoxIntersection(CPlane *plane, CVector *halfBoxWidths);

// see if two spheres will hit within their velocity range
bool sphereSphereIntersectionWithinVelocity(const CVector& sphere1Pos, float sphere1Rad, 
											const CVector& sphere1Veloc,
											const CVector& sphere2Pos, float sphere2Rad, 
											const CVector& sphere2Veloc, float & time);

// see if two static spheres are colliding
bool sphereSphereIntersection(	CVector * sphere1Pos, float sphere1Rad, 
								CVector * sphere2Pos, float sphere2Rad);

// determines whether a sphere and a capsule have intersected
bool sphereAxisAlignedCapsuleIntersection(CVector * capsuleCentre, 
												GLfloat capsuleRadius,
												GLfloat capsuleHeight, 
												CVector * spherePos, float sphereRad);

// determine if an infinite ray intersects a sphere
bool raySphereIntersection(CVector * rayOrigin, CVector * rayVectorNorm, 
							CVector * sphereCentre, float sphereRadius, 
							CVector & closestSphereIntersectionPoint,
							float & distenceToClosestSphereIntersectionPoint);

// determines whether a sphere and triangle have collided
bool sphereTriangleCollision(CVector * triangleVertex, CVector * sphereCentre, 
							 float sphereRadius, CVector &returnNormal, 
							 int &classification, float &distanceFromPlane);

// determines whether a point on a plane is inside a triangle
bool insideTriangle(CVector * posOnPlane, CVector * triangleVertex);

// determines whether a sphere is in front of, behind or intersecting a plane
int classifySphere(CVector * sphereCentre, CVector * planeNormal, CVector * triangleVertex,
					float sphereRadius, float &distance);

// returns distance between plane and origin
float planeDistance(CVector * normal, CVector * point);

// find the closest point on a line to a point
CVector closestPointOnLine(CVector * startLinePoint, CVector * endLinePoint, 
						   CVector * sphereCentre);

// determines whether any part of a sphere has hit the perimeter of a triangle
bool edgeSphereCollision(CVector * sphereCentre, CVector * triangleVertex, float sphereRadius);

// get the closest point on the perimeter of a triangle to a point
CVector closestPointOnTrianglePerimeter(CVector * point, CVector * triangleVertex);

// see if a line section and a plane have collided
bool lineSectionPlaneCollision(	CVector * triangleVertex, const CVector& lineStart, 
								const CVector& endLine, CVector &normal, float &originDistance);

// this returns the point on a plane which a line segment intersects
CVector planeIntersectionPoint(CVector *planeNormal, const CVector& startLine, const CVector& endLine, 
							  float distance);

// checks if a line segment intersects a triangle
bool lineSectionTriangleCollision(CVector * triangleVertex, const CVector& startLinePoint, 
								  const CVector& endLinePoint, CVector& theHitPoint, CVector &theNormal);

// checks if a line segment intersects a triangle
bool SweptSphereTriangleCollision(CVector * triangleVertex, const CVector& sphereStartPoint, const CVector& sphereEndPoint, 
								  float sphereRadius, CVector& theHitPoint, CVector& thePolygonIntersectionPoint, CVector &theNormal);
//-----------------------------------

#endif // _intersections_h_