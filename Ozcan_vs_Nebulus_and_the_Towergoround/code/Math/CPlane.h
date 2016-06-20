//CPlane.h, header file for CPlane class

//system includes---
//------------------

#ifndef _CPlane_h_
#define _CPlane_h_

// header files---------
#include "Math/CVector.h"
#include "Game/defines.h"
//----------------------

//defines---------------
#define SMALL_VAL 0.00001f // a very small value 
// floating point equality check, within the bounds of a very small differance
#define FEQUALS(N1, N2) ( fabs(N1 - (N2)) < SMALL_VAL) ? true : false
//----------------------

// plane class
class CPlane              
{
	public:
		CPlane(){ planeConstant = 0.0f;} // constructor

		// constructor (presumes the normal is normalised)
		CPlane(const CVector& normal, const CVector& pointOnSurface)
		{
			planeNormal = normal;
			pointOnPlane = pointOnSurface;
			// set plane constant
			planeConstant = -(planeNormal.dotProduct(pointOnPlane));
		}

		// constructor
		CPlane(CVector * triangleVertices)
		{
			planeNormal.crossProduct(&triangleVertices[0], &triangleVertices[1], 
									&triangleVertices[2]);
			planeNormal.normalise();
			pointOnPlane = triangleVertices[0];
			// set plane constant
			planeConstant = -(planeNormal.dotProduct(&pointOnPlane));
		}

		// normalise the plane
		void normalise()
		{
			float magnitude = planeNormal.getMagnitude(); // get normals magnitude

			planeNormal /= magnitude; // normalise plane normal
			planeConstant /= magnitude; // normalise plane constant
		}

		// is this plane the same as another plane
		bool operator==(const CPlane &p)
		{
			double	x = planeNormal.x, px = p.planeNormal.x,
					y = planeNormal.y, py = p.planeNormal.y,
					z = planeNormal.z, pz = p.planeNormal.z;

			return( ( FEQUALS(x, px) ) &&
					( FEQUALS(y, py) ) &&
					( FEQUALS(z, pz) ) );
		}

		// is this a different plane to another plane
		bool operator!=(const CPlane &p)
		{
			return((!( (*this) == p )));
		}	
	
		// copy constructor
		CPlane(const CPlane& aPlane)
		{
			*this = aPlane;
		}
	
		// make plane the same as another plane
		CPlane& CPlane::operator=(const CPlane *p)
		{
			planeNormal = p->planeNormal;
			pointOnPlane = p->pointOnPlane;
			planeConstant = p->planeConstant;
			return(*this);
		}

		CVector planeNormal, 
				pointOnPlane;
		float planeConstant;
};

#endif