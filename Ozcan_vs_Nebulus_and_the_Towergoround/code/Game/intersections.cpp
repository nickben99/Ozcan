//------------------------------------------------------
// BASED ON CODE FROM GAMETUTORIALS.COM
//------------------------------------------------------

// intersections.cpp - geometric intersection functions

//--- System includes -----------------
#include <assert.h>
#include <float.h>
//-------------------------------------

//--- Header files --------------------
#include "intersections.h" // the header file
#include "Math/CMatrix.h"
#include "Game/defines.h"
//-------------------------------------

//global variables---------------------
//-------------------------------------

// intersection of two axis aligned boxes
int axisAlignedBoxBoxIntersection(	CVector *box1Centre, CVector *box1HalfWidths, 
									CVector *box2Centre, CVector *box2HalfWidths)
{
	// build up the dimensions of the two boxes
	CVector box1Positives = *box1Centre + box1HalfWidths, 
			box1Negatives = *box1Centre - box1HalfWidths,
			box2Positives = *box2Centre + box2HalfWidths, 
			box2Negatives = *box2Centre - box2HalfWidths;

	// if they overlap in all dimensions
	if( ( (box1Positives.x >= box2Negatives.x) && (box1Negatives.x <= box2Positives.x) ) &&
		( (box1Positives.z >= box2Negatives.z) && (box1Negatives.z <= box2Positives.z) ) &&
		( (box1Positives.y >= box2Negatives.y) && (box1Negatives.y <= box2Positives.y) ) ){
		return(1);} // intersection
	
	return(0); // no intersection
}

/*	main functionality of sphereBox method from 
	Dunn & Parberry - 3D Math Primer for Graphics and Game Development */
int sphereAxisAlignedBoxIntersection(CVector *boxCentre, CVector *boxHalfWidths, 
									 CVector *sphereCentre, float sphereRadius)
{
	// build up dimensions of the box to be collided with
	CVector boxPositives = *boxCentre + boxHalfWidths, 
			boxNegatives = *boxCentre - boxHalfWidths,
	//	see if sphere has collided with cube.  Find closest point on cube
			closestPoint = *sphereCentre;

	// closeX/Y/Z values wont be changed if already a collision on relevant axis
	if (sphereCentre->x < boxNegatives.x){
		closestPoint.x = boxNegatives.x;}
	else if (sphereCentre->x > boxPositives.x){
		closestPoint.x = boxPositives.x;}
			
	if (sphereCentre->y < boxNegatives.y){
		closestPoint.y = boxNegatives.y;}
	else if (sphereCentre->y > boxPositives.y){
		closestPoint.y = boxPositives.y;}

	if (sphereCentre->z < boxNegatives.z){
		closestPoint.z = boxNegatives.z;}
	else if (sphereCentre->z > boxPositives.z){
		closestPoint.z = boxPositives.z;}

	float distenceSquared = 
		(closestPoint - sphereCentre).dotProduct(closestPoint - sphereCentre);

	/*	if closest point on the box is less than sphere radius away from the sphere
		centre then the box and sphere have collided (using distances squared to avoid 
		square root operations) */
	if ( distenceSquared <= (sphereRadius*sphereRadius) ){
		return(1);} // intersection
	
	return(0); // no intersection
}

// macro specific for following method
#define FINDMINMAX(x0, x1, x2, min, max) \
			min = max = x0;   \
			if (x1 < min){ min=x1;}\
			if (x1 > max){ max=x1;}\
			if (x2 < min){ min=x2;}\
			if (x2 > max){ max=x2;}

//see if a triangle and an axis aligned box have intersected
bool triangleAxisAlignedBoxIntersection(CVector *boxCentre, CVector *halfBoxWidths, 
										CVector * triangleVertex)
{
	float min, max; // temp variables

	// translate triangle to the origin
	CVector verts[] = {	CVector() = (triangleVertex[0] - boxCentre), 
						CVector() = (triangleVertex[1] - boxCentre), 
						CVector() = (triangleVertex[2] - boxCentre)}; 

	// thirteen axii in which box and triangle can be split

	// see if they can be split on cardinal axii (three tests)
	
	// find min and max triangle x axis extents
	FINDMINMAX(verts[0].x, verts[1].x, verts[2].x, min, max);
	// if not intersecting on the x axis
	if(min > halfBoxWidths->x || max < -halfBoxWidths->x){ return( false );}

	// find min and max triangle y axis extents
	FINDMINMAX(verts[0].y, verts[1].y, verts[2].y, min, max);
	// if not intersecting on the y axis
	if(min > halfBoxWidths->y || max < -halfBoxWidths->y){ return( false );}

	// find min and max triangle z axis extents
	FINDMINMAX(verts[0].z, verts[1].z, verts[2].z, min, max);
	// if not intersecting on the z axis
	if(min > halfBoxWidths->z || max < -halfBoxWidths->z){ return( false );}

	// a test to see if the triangle plane intersects the box
	CPlane trianglePlane(&verts[0]);

	if (!planeAxisAlignedOriginCenteredBoxIntersection(&trianglePlane, halfBoxWidths)){ 
		return( false );} // no collision

	// 9 axis tests to see if the triangle and box can be split
	CVector boxEdges[] = {	CVector(1.0f, 0.0f, 0.0f), CVector(0.0f, 1.0f, 0.0f), 
							CVector(0.0f, 0.0f, 1.0f)},
			triEdges[] = {	CVector() = (verts[1] - &verts[0]), 
							CVector() = (verts[2] - &verts[1]), 
							CVector() = (verts[0] - &verts[2])};

	// work out the cube vertice positions
	CVector boxVertice[] = 
		{CVector((-halfBoxWidths->x), (-halfBoxWidths->y), (-halfBoxWidths->z)),
		CVector((halfBoxWidths->x), (-halfBoxWidths->y), (-halfBoxWidths->z)),
		CVector((-halfBoxWidths->x), (halfBoxWidths->y), (-halfBoxWidths->z)),
		CVector((halfBoxWidths->x), (halfBoxWidths->y), (-halfBoxWidths->z)),
		CVector((-halfBoxWidths->x), (-halfBoxWidths->y), (halfBoxWidths->z)),
		CVector((halfBoxWidths->x), (-halfBoxWidths->y), (halfBoxWidths->z)),
		CVector((-halfBoxWidths->x), (halfBoxWidths->y), (halfBoxWidths->z)),
		CVector((halfBoxWidths->x), (halfBoxWidths->y), (halfBoxWidths->z))};

	for (int boxEdge = 0; boxEdge < 6; boxEdge++)
	{
		for (int triEdge = 0; triEdge < 3; triEdge++)
		{
			float	triMin		= FLT_MAX,	triMax		= -FLT_MAX, 
					boxMin		= FLT_MAX,	boxMax		= -FLT_MAX;

			CVector cross = 
				triEdges[triEdge].crossProduct(&boxEdges[boxEdge]);

			// go to next iteration if vectors are parallel so no plane can be derived
			if (cross.fuzzyEquals(CVector())){
				continue;}

			cross.normalise();
			
			// find min and max triangle verts
			for (int triVert = 0; triVert < 3; triVert++)
			{
				float dot = verts[triVert].dotProduct(&cross);

				if (dot > triMax){ triMax = dot;}
				if (dot < triMin){ triMin = dot;}
			}
			
			// find min and max box verts
			for (int boxVert = 0; boxVert < 8; boxVert++)
			{
				float dot = boxVertice[boxVert].dotProduct(&cross);

				if (dot > boxMax){ boxMax = dot;}
				if (dot < boxMin){ boxMin = dot;}
			}

			if( (triMin > boxMax) || (boxMin > triMax) ){
				return( false );} // seperable on this axis so no collision
		}
	}

	return( true ); // intersection
}

// see if a plane and an axis aligned origin centered box have intersected
bool planeAxisAlignedOriginCenteredBoxIntersection(CPlane *plane, CVector *halfBoxWidths)
{
	CVector min, max;

	// set min and max x
	if (plane->planeNormal.x > 0.0f)
	{
		min.x = -halfBoxWidths->x - plane->pointOnPlane.x;
		max.x = halfBoxWidths->x - plane->pointOnPlane.x;
	}
	else
	{
		min.x = halfBoxWidths->x - plane->pointOnPlane.x;
		max.x = -halfBoxWidths->x - plane->pointOnPlane.x;
	}

	// set min and max y
	if (plane->planeNormal.y > 0.0f)
	{
		min.y = -halfBoxWidths->y - plane->pointOnPlane.y;
		max.y = halfBoxWidths->y - plane->pointOnPlane.y;
	}
	else
	{
		min.y = halfBoxWidths->y - plane->pointOnPlane.y;
		max.y = -halfBoxWidths->y - plane->pointOnPlane.y;
	}

	// set min and max z
	if (plane->planeNormal.z > 0.0f)
	{
		min.z = -halfBoxWidths->z - plane->pointOnPlane.z;
		max.z = halfBoxWidths->z - plane->pointOnPlane.z;
	}
	else
	{
		min.z = halfBoxWidths->z - plane->pointOnPlane.z;
		max.z = -halfBoxWidths->z - plane->pointOnPlane.z;
	}

	// if the minimum is in front of the plane their is no intersection
	if (plane->planeNormal.dotProduct(&min) > 0.0f){ return( false );}
	// if the maximum is in front of the plane an intersection has occured
	if (plane->planeNormal.dotProduct(&max) >= 0.0f){ return( true );}

	return( false ); // no intersection
}

// see if two spheres will hit within their velocity range
bool sphereSphereIntersectionWithinVelocity(const CVector& sphere1Pos, float sphere1Rad, 
											const CVector& sphere1Veloc,
											const CVector& sphere2Pos, float sphere2Rad, 
											const CVector& sphere2Veloc, float & time)
{
	time = 0.0f; // initialise referance variable

	// work out variables for quadratic equation
	CVector A = sphere1Pos - sphere2Pos, 
			B = sphere1Veloc - sphere2Veloc;

	float	BSquared = B.getMagnitudeSquared();

	/*	if both spheres are travelling in the same direction or they are both
		stationary return no collision */
	if (BSquared < EPSILON){
		return( false );}
	
	float	dSquared = (sphere1Rad + sphere2Rad) * (sphere1Rad + sphere2Rad),
			ADotB = A.dotProduct(&B), 
			ADotBSquared = ADotB * ADotB,
			ASquared = A.getMagnitudeSquared(),
			/*	work out the closest the spheres will ever come to each other
				based on the current velocity vector */
			closeDistSquared = ASquared - ((ADotBSquared) / BSquared);

	// if the spheres never intersect each other
	if (closeDistSquared > dSquared){
		return( false );}

	float radicand = ( ADotBSquared - (BSquared*(ASquared-dSquared)) );

	// if the radicand is negative, no collision can occur
	if (radicand < 0.0f){
		return( false );}

	// get the time the two spheres first make contact approaching each other
	float t1 = ((-ADotB)-(float)sqrt(radicand)) / BSquared;

	// if the collision would occur out of the velocity range return no collision
	if (t1 < 0.0f || t1 > 1.0f){
		return( false );}
	
	// set time when collision occurs
	time = t1;

	// collision will occur within the velocity range
	return( true );
}

// see if two static spheres are colliding
bool sphereSphereIntersection(	CVector * sphere1Pos, float sphere1Rad, 
								CVector * sphere2Pos, float sphere2Rad)
{
	float distanceBetweenSquared = /*	get distence between points squared avoid 
										sqrt operation */
		(*sphere1Pos - sphere2Pos).dotProduct((*sphere1Pos - sphere2Pos));

	// see if distance between squared is less than combined radius squared
	if( distanceBetweenSquared <= ((sphere1Rad + sphere2Rad)*(sphere1Rad + sphere2Rad)) ){
		return(true);} // return true if collision
	
	return(false); // no collision
}

// determine if an infinite ray intersects a sphere
bool raySphereIntersection(CVector * rayOrigin, CVector * rayVectorNorm, 
							CVector * sphereCentre, float sphereRadius, 
							CVector & closestSphereIntersectionPoint,
							float & distenceToClosestSphereIntersectionPoint)
{
	// create triangle - hypotenuse from ray origin to sphere origin
	CVector q = *sphereCentre - rayOrigin;
	float c = q.getMagnitude(); // length of hypotenuse
	// project hypotenuse onto ray vector
	float v = q.dotProduct(rayVectorNorm);
	/*	(c^2 - v^2) gives squared distence from sphere centre to closest point on ray.
		if this is less than sphere radius squared, d will be positive and sphere will
		intersect ray */
	float d = (sphereRadius * sphereRadius) - (c*c - v*v);

	// if d is less than zero their is no collision
	if (d < 0.0f){
		return (false);}

	// if their was a collision, set the referance variables
	distenceToClosestSphereIntersectionPoint = (v - (float)sqrt(d));
	closestSphereIntersectionPoint = *rayOrigin + (*rayVectorNorm * distenceToClosestSphereIntersectionPoint);
	// collision, return true
	return(true);
}

// determines whether a sphere and a capsule have intersected
bool sphereAxisAlignedCapsuleIntersection(CVector * capsuleCentre, 
												GLfloat capsuleRadius,
												GLfloat capsuleHeight, 
												CVector * spherePos, float sphereRad)
{
	CVector capsuleTop = *capsuleCentre + CVector(0.0f, capsuleHeight*0.5f, 0.0f),
			capsuleBottom = *capsuleCentre - CVector(0.0f, capsuleHeight*0.5f, 0.0f),
	
	/*	find the closest point on the imaginary line running up the centre of the capsule
		to the sphere */
			closestPoint = 
				closestPointOnLine(&capsuleBottom, &capsuleTop, spherePos);
	
	// the problem is now reduced to the intersection of two spheres
	return( sphereSphereIntersection(&closestPoint, capsuleRadius, 
							spherePos, sphereRad));
}

// determine if a sphere has collided with a triangle
bool sphereTriangleCollision(CVector * triangleVertex, CVector * sphereCentre, 
							 float sphereRadius, CVector &returnNormal, int &inRelationToPlane,
							 float &distanceFromPlane)
{
	// get the triangles normal
	CVector triangleNormal;
	triangleNormal.crossProduct(&triangleVertex[0], &triangleVertex[1], &triangleVertex[2]);
	triangleNormal.normalise();
	returnNormal = triangleNormal; /*	should their be a hit the normal of the surface
										struck will need to be known */

	float distance = 0.0f;

	// determines whever the sphere is in front of, behind or intersecting the triangles plane  
	int classification = classifySphere(sphereCentre, &triangleNormal, &triangleVertex[0], 
										sphereRadius, distance); /* (distence is a referance 
																	variable) */
	distanceFromPlane = distance;
	inRelationToPlane = classification; // set referance variable

	/*	if the sphere intersects the polygons plane, further checks are required 
		to determine if the sphere is intersecting the actual triangle */
	if(SPHERE_INTERSECTS_PLANE == classification) 
	{
		// determine the plane intersection point
		CVector posOnPlane = *sphereCentre - (triangleNormal * distance);

		// determines whever the position on the plane is inside the triangle
		if(insideTriangle(&posOnPlane, triangleVertex)){
			return(true);}	// collision

		// determines if any part of the sphere has hit the edges of the triangle
		if(edgeSphereCollision(sphereCentre, triangleVertex, sphereRadius)){
			return(true);} // collision
	}

	return(false); // no collision
}

// determines whether a sphere is in front of, behind or intersecting a plane
int classifySphere(CVector * sphereCentre, CVector * planeNormal, CVector * triangleVertex,
					float sphereRadius, float &distance)
{
	// work out distance plane is from origin
	float d = planeDistance(planeNormal, triangleVertex);

	// use distance formula to find the distance the sphere center to the polygons plane
	distance = (planeNormal->dotProduct(sphereCentre) + d);

	// if spheres distance from the plane is less than or equal to its radius
	if(fabs(distance) <= sphereRadius){
		return( SPHERE_INTERSECTS_PLANE );} // intersects
	
	// if distance is greater than radius the sphere is in front of the plane
	if(distance > sphereRadius){
		return( SPHERE_IN_FRONT_OF_PLANE );}

	// sphere is behind plane as it is not in front and it does not intersect it
	return( SPHERE_BEHIND_PLANE );
}

// returns distance between plane and origin
float planeDistance(CVector * normal, CVector * point)
{	
	float distance = 0.0f;	

	// use the plane equation to find the distance (Ax + By + Cz + D = 0)
	// D = -(Ax + By + Cz)
	distance = -( normal->dotProduct(point) ); 

	return( distance );	// return the distance
}

// determines whether a point on a plane is inside a triangle
bool insideTriangle(CVector * posOnPlane, CVector * triangleVertex)
{
	const double MATCH_FACTOR = 0.999f;		// used to accomodate floating point error
	double angle = 0.0f;					// initialize the angle
	CVector vA, vB;							// create temp vectors
	
	for (int i = 0; i < 3; i++)		// go in a circle to each vertex and get the angle between
	{	
		vA = triangleVertex[i] - posOnPlane; // subtract the intersection point from the current vertex
		// subtract the point from the next vertex 
		vB = triangleVertex[(i + 1) % 3] - posOnPlane;
												
		angle += vA.getAngleBetween(&vB);	// find the angle between the 2 vectors and add them all up as we go along
	}
	
	if(angle >= (MATCH_FACTOR * (2.0 * PI)) ){	// if the angle is greater than 2 PI, (360 degrees)
		return( true );}							// the point is inside the triangle
	
	// the point is not inside the triangle
	return( false );
}

// find the closest point on a line to a point
CVector closestPointOnLine(CVector * startLinePoint, CVector * endLinePoint, CVector * sphereCentre)
{
	// create vector from startLinePoint to sphereCentre
	CVector vector1 = *sphereCentre - startLinePoint;

	// create a normalized direction vector from startLinePoint to endLinePoint
    CVector lineNormal = (*endLinePoint - startLinePoint);
	lineNormal.normalise();

	// use the distance formula to find the distance of the line segment (or magnitude)
    float distance = startLinePoint->distanceBetweenTwoPoints(endLinePoint);

	/*	using the dot product, vector1 is projected onto lineNormal
		this gives the distance vector1 projects onto the line from startLinePoint */
    float projectedDistance = vector1.dotProduct(&lineNormal);

	/*	if the projected distance from startLinePoint "projectedDistance", is less than or equal 
		to 0, it must be closest to the end point startLinePoint */
    if (projectedDistance <= 0.0f){ 
		return( *startLinePoint );}

	/*	if the projected distance from startLinePoint "projectedDistance", is greater than or 
		equal to the magnitude of the line segment, it must be closest to endLinePoint */
    if (projectedDistance >= distance){ 
		return( *endLinePoint );}
 
	/*	add lineNormal multiplied by projected distance to startLinePoint to find 
		the closest point on the line */
    CVector closestPoint = *startLinePoint + (lineNormal * projectedDistance);

	// return the closest point on the line segment
	return( closestPoint );
}

// determines whether any part of a sphere has hit the perimeter of a triangle
bool edgeSphereCollision(CVector * sphereCentre, CVector * triangleVertex, 
						 float sphereRadius)
{
	CVector closestPoint;

	// go through all of the vertices in the triangle
	for(int i = 0; i < 3; i++)
	{
		// find the closest point on an edge to the centre of the sphere 
		closestPoint = closestPointOnLine(&triangleVertex[i], &triangleVertex[(i + 1) % 3], 
											sphereCentre);
		
		// calculate the distance between the closest point and the centre
		float distance = closestPoint.distanceBetweenTwoPoints(sphereCentre);
	
		// if the distance is less than the radius
		if(distance <= sphereRadius){
			return( true );} // collision
	}

	// no intersection of the sphere and the edges of the triangle
	return( false );
}

// get the closest point on the perimeter of a triangle to a point
CVector closestPointOnTrianglePerimeter(CVector * point, CVector * triangleVertex)
{
	CVector closestPoint;
	float closestDistance = 0.0f;

	// go through all of the vertices in the triangle
	for(int i = 0; i < 3; i++)
	{
		// find the closest point on an edge to the point 
		CVector thisPoint = closestPointOnLine(&triangleVertex[i], 
							&triangleVertex[(i + 1) % 3], point);
		
		// calculate the distance between the points
		float distance = thisPoint.distanceBetweenTwoPoints(point);
	
		if (0 == i) // first edge, set closest variables
		{ 
			closestPoint = thisPoint;
			closestDistance = distance; 
		}

		// if the distance is less than the current closest distence
		if(distance <= closestDistance)
		{
			closestPoint = thisPoint;
			closestDistance = distance;
		} // current closest
	}

	return(closestPoint); // return te closest point
}

// see if a line section and a plane have collided
bool lineSectionPlaneCollision(	CVector * triangleVertex, const CVector& lineStart, 
								const CVector& endLine, CVector &normal, float &originDistance)
{
	float distance1 = 0.0f, distance2 = 0.0f;			
			
	// get the plane's normal
	CVector planeNormal;
	planeNormal.crossProduct(&triangleVertex[0], &triangleVertex[1], &triangleVertex[2]);
	planeNormal.normalise();
	normal = planeNormal; // set referance variable

	// work out distance plane is from origin
	originDistance = planeDistance(&planeNormal, triangleVertex);

	/*	using the plane equation: Ax + By + Cz + D, get the distance the start of the line is from
		the plane */
	distance1 = ( planeNormal.dotProduct(lineStart) ) + originDistance;
	
	/*	using the plane equation: Ax + By + Cz + D, get the distance the end of the line is from
		the plane */
	distance2 = ( planeNormal.dotProduct(endLine) ) + originDistance;

	
	/*	if the start of the line is on the plane and the end of the line is behind the plane,
		return true, theirfore if heading away from the plane it will not be a collision.  This is 
		very specific to the needs of this implementation */
	if ( (0.0f == distance1) && (distance2 < 0.0f) ){
		return( true );} // collision

	/*	if the two points are on opposite sides of the plane return true, 
		multiplying the distances together gives a negative number if one distance is positive and the
		other negative i.e on opposite sides of the plane (both positive or both negative gives positive) */

	if( (distance1 * distance2) >= 0){	/*	ZERO INDICATES BOTH POINTS ARE ON PLANE WHICH GIVES NO COLLISION
											HERE, ALTHOUGH THIS IS SOMETIMES VIEWED AS COLLIDING */
		return( false );} // no collision					
	
	return( true ); // collision			
}

// this returns the point on a plane which a line segment intersects
CVector planeIntersectionPoint(CVector *planeNormal, const CVector& startLine, const CVector& endLine, 
							  float distance)
{
	CVector intersectionPoint, unitLine;	
	float numeratorDistance = 0.0f, denominatorAngle = 0.0f, dist = 0.0f;

	// get the normalalised line vector
	unitLine = ( endLine - startLine );
	unitLine.normalise(); // normalise

	/*	using the plane equation: Ax + By + Cz + D) get the distance the start 
		of the line is from the plane */
	numeratorDistance = - ( planeNormal->dotProduct(startLine) + distance );

	// get the dot product of the normal line vector and the plane normal
	denominatorAngle = planeNormal->dotProduct(&unitLine);
				  
	/*	if denominator is zero the line is on the plane, therefore their are 
		infinite intersecting points, in this instance just return an arbitrary point */
	if( denominatorAngle == 0.0f){			
		return( startLine );}					

	dist = numeratorDistance / denominatorAngle; /* divide distance by angle to get the actual distance from
													the plane along the line segment */
	
	// add unit vector multiplied by dist to get intersection point
	intersectionPoint = (startLine + (unitLine * dist));

	return( intersectionPoint ); // return the intersection point
}

// checks if a line segment intersects a triangle
bool lineSectionTriangleCollision(CVector * triangleVertex, const CVector& startLinePoint, const CVector& endLinePoint, 
								  CVector& theHitPoint, CVector &theNormal)
{
	CVector normal;
	float originDistance = 0.0f;

	// check to see if the line segment intersects the plane of the triangle
	if(!lineSectionPlaneCollision(	triangleVertex, startLinePoint, endLinePoint, normal,   
									originDistance)){ /*	normal and originDistance are referance 
															variables */
		return( false );} // no collision
	theNormal = normal; // set referance variable

	// get the point on the line which is actually on the plane
	theHitPoint = planeIntersectionPoint(&normal, startLinePoint, endLinePoint, originDistance);

	// see if the point on the plane is inside the triangle
	if(insideTriangle(&theHitPoint, triangleVertex)){
		return( true );}	// collision

	return( false ); // no collision
}

// checks if a line segment intersects a triangle
bool SweptSphereTriangleCollision(CVector * vertices, const CVector& sphereStartPoint, const CVector& sphereEndPoint, 
								  float sphereRadius, CVector& theHitPoint, CVector& thePolygonIntersectionPoint, CVector &theNormal)
{
	static CMatrix scale;
	static CVector velocity, spriteOldPosition,	spritePosition;

	// create a scaling matrix to convert into ellipsoid space
	CVector scaledElipsoidRadiusVector(sphereRadius, sphereRadius, sphereRadius);
	scale.creatScalingMatrix(sphereRadius, sphereRadius, sphereRadius);

	// get previous and new sprite position
	spriteOldPosition = sphereStartPoint;
	spritePosition = sphereEndPoint;
	// get vector from previous to current position
	velocity = (sphereEndPoint - sphereStartPoint);
	
	// convert position and velocity to elipsoid space
	spriteOldPosition = 
		(scale.getInverseScalingMatrix()).multiplyVector(&spriteOldPosition);
		
	velocity  = 
		(scale.getInverseScalingMatrix()).multiplyVector(&velocity);

	float velocDotVeloc = velocity.dotProduct(velocity),
			velocMagnitude = velocity.getMagnitude();

	CVector verts[] = {	vertices[0], vertices[1], vertices[2]};

	theNormal.crossProduct(&verts[0], &verts[1], &verts[2]); 
	theNormal.normalise();

	// convert vertices to elipsoid space
	verts[0] /= &scaledElipsoidRadiusVector;
	verts[1] /= &scaledElipsoidRadiusVector;
	verts[2] /= &scaledElipsoidRadiusVector;
	// referance variable for speed and readability
	CVector &FirstVert = verts[0];

	/*	get the normal of the triangles plane 
		(can't divide triangle pane normal by elipsoid radius vector, won't work) */
	CVector planeNormal;
	planeNormal.crossProduct(&verts[0], &verts[1], &verts[2]); 
	planeNormal.normalise();
						
	CPlane thisTriPlane(planeNormal, FirstVert); // create plane

	// get plane constant
	float	originDistance = thisTriPlane.planeConstant,
			// get spheres distance from plane
			distanceFromPlane = 
				( planeNormal.dotProduct(spriteOldPosition) ) + 
													originDistance,
			velocDotNorm = (planeNormal.dotProduct(velocity)),
			/*	variables for recording when the sphere makes contact with
				the triangle plane */
			t0, t1;

	// if the starting point is behind the plane 
	if (distanceFromPlane < 0.0f){
		return false;} // disregard starting point behind the plane

	// if sphere is running parallel with the triangle plane
	if (0.000001f > fabs(velocDotNorm))
	{
		// if sphere is not intersecting the triangle plane
		if (distanceFromPlane >= 0.9999f){
			return false;} // go to next triangle
		// sphere is always in contact with the triangle plane
		t0 = 0.0f;
		//t1 = 1.0f;
	}
	else // sphere is not running parallel with triangle plane
	{
		// get time sphere makes contact with front of triangle plane
		t0 = (1.0f - distanceFromPlane) / velocDotNorm;

		// get time sphere makes contact with back of triangle plane
		t1 = (-1.0f - distanceFromPlane) / velocDotNorm;
		// if heading away from the plane
		if (t1 < t0){
			return false;} // no collision
	}
						
	// if the sphere hits the front of the triangle outside the velocity vector
	if (t0 > 1.0f){
		return false;} // no collision

	/*	if the sphere did not begin in a position intersecting the 
		triangle plane, see if it intersected with the centre of the 
		triangle before checking the virtices and edges */
	if (distanceFromPlane >= 1.0f)
	{	// get the intersection point
		CVector intersectionPoint = (spriteOldPosition - planeNormal) + 
									(velocity * t0);

		// if the point is inside the triangle
		if (insideTriangle(&intersectionPoint, &FirstVert))
		{					
			thePolygonIntersectionPoint = scale.multiplyVector(&intersectionPoint);
			theHitPoint = thePolygonIntersectionPoint + theNormal * sphereRadius;
			return true; // collision found, go to next triangle
		}// end if points inside triangle
	}// end if originally not intersecting plane

	float perCentOfMoveComplete = 2.0f;
	bool hit = false;

	// check if sphere intersects the virtices of the triangle
	for (int virtex = 0; virtex < 3; virtex++)
	{
		// work out elements for quadratic equation
		float	a = velocDotVeloc, 
				b = 2.0f*(velocity.dotProduct((spriteOldPosition - 
											verts[virtex]))),
				vertMinusOldPosMagnitude = 
					(verts[virtex] - spriteOldPosition).getMagnitude(),
				c = ( vertMinusOldPosMagnitude * vertMinusOldPosMagnitude )
					- 1.0f,
				lowestRoot;
		// see if sphere hits the virtex within velocity range
		if (solveQuadraticEquation(a, b, c, 0.0f, 1.0f, &lowestRoot))
		{
			// if it is closer than the current closest triangle hit
			if (lowestRoot <= perCentOfMoveComplete)
			{	
				theHitPoint = spriteOldPosition + (velocity * lowestRoot);
				theHitPoint = scale.multiplyVector(&theHitPoint);

				// set closest triangle distance
				perCentOfMoveComplete = lowestRoot;
				// record triangle intersection point
				thePolygonIntersectionPoint = scale.multiplyVector(&verts[virtex]);
				hit = true;
			}// end if closest triangle
		}
	}// end for virtices

	// check if the sphere intersects the edges of the triangle
	for (int perimeter = 0; perimeter < 3; perimeter++)
	{
		// work out elements for quadratic equation
		CVector	edge = verts[(perimeter + 1) % 3] - &verts[perimeter], 
				baseToVirtex = verts[perimeter] - spriteOldPosition; 

		float	edgeMagnitude			= edge.getMagnitude(),
				edgeDotVeloc			= edge.dotProduct(velocity), 
				baseToVertexMagnitude	= baseToVirtex.getMagnitude(),
				edgeDotBaseToVirtex		= edge.dotProduct(&baseToVirtex),
						
				a = (edgeMagnitude*edgeMagnitude) * 
					-(velocMagnitude * velocMagnitude) +
					(edgeDotVeloc * edgeDotVeloc),

				b = (edgeMagnitude * edgeMagnitude) * 
					(2.0f*(velocity.dotProduct(&baseToVirtex))) - 
					(2.0f*((edgeDotVeloc)*(edgeDotBaseToVirtex))),

				c = (edgeMagnitude * edgeMagnitude) * 
					(1.0f - (baseToVertexMagnitude * baseToVertexMagnitude)) +
					((edgeDotBaseToVirtex)*(edgeDotBaseToVirtex)),
				lowestRoot;
		/*	see if sphere hits the infinte line segment within velocity 
			range */
		if (solveQuadraticEquation(a, b, c, 0.0f, 1.0f, &lowestRoot))
		{
			/*	see if the intersection takes place on the section of the 
				infinite line which is this triangle perimeter edge */
			float edgeIntersect = 
				( (edgeDotVeloc)*lowestRoot - 
				(edgeDotBaseToVirtex) ) /
				(edgeMagnitude*edgeMagnitude);
			// see if hit within edge section
			if (edgeIntersect >= 0.0f && edgeIntersect <= 1.0f)
			{
				// if it is closer than the current closest triangle hit
				if (lowestRoot <= perCentOfMoveComplete)
				{	
					theHitPoint = spriteOldPosition + (velocity * lowestRoot);
					theHitPoint = scale.multiplyVector(&theHitPoint);
					CVector edgeMultipliedByEdgeIntersect = (edge * edgeIntersect);

					// set closest triangle distance
					perCentOfMoveComplete = lowestRoot;
					// record triangle intersection point
					thePolygonIntersectionPoint = verts[perimeter] + &(edgeMultipliedByEdgeIntersect);
					thePolygonIntersectionPoint = scale.multiplyVector(&thePolygonIntersectionPoint);		
					hit = true;
				}// end if closest triangle
			}// end if edgeIntersect
		}// end if solveQuadraticEquation
	}// end for edges	

	return hit; // no collision
}


