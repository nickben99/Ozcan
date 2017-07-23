/*-----------------------------------------
CLASS BASED ON VECTOR.H/CPP
FROM 	Deloura, M. (2000). Game Programming Gems. 1st ed.,
Charles River Media -
Article title: Flocking: A Simple Technique for Simulating Group
Behavior by Woodcock, S

CLASS ALSO BASED ON VECTOR.H.CPP
FROM	Bourg, D.M. (2002).  Physics for Game Developers.  1st ed.,  O'Reilly.
-----------------------------------------*/
 
//CVector.h - header file for the 3dvector class

//system includes------------
//---------------------------

#ifndef _CVector_h_
#define _CVector_h_

#include <assert.h>

//header files---------
#include "Game/defines.h"
//---------------------

//defines--------------
#define SMALL_VAL 0.00001f // a very small value 
// floating point equality check, within the bounds of a very small differance
#define FEQUALS(N1, N2) ( fabs(N1 - (N2)) < SMALL_VAL) ? true : false
//---------------------

class CVector4;

class CVector
{
	private:
//-----private variables-----------
//---------------------------------

	public:
		static CVector zero;
		static CVector one;
		static CVector unitX;
		static CVector unitY;
		static CVector unitZ;

//----public variables--------------
		float x, y, z;
//----------------------------------

//----public methods----------------------------------------
		CVector( void); // Constructor
		CVector(float inX, float inY, float inZ); // overloaded constructor
		CVector(const CVector&); // copy constructor
		CVector(const CVector4& in);
		~CVector(); // destructor
		CVector& normalise(); // make unit vector
		void setMagnitude(float mag); // set the length of the vector
		float getMagnitude() const; // get the length of the vector
		float dotProduct(CVector * productWith); // dot product operation
		float dotProduct(const CVector& productWith) const; // dot product operation
		// cross product operation
		void crossProduct(CVector* point1, CVector* point2, CVector* point3); 
		void crossProduct(CVector *vector1, CVector *vector2);
		CVector crossProduct(CVector *vector1); // this vector crossed with another
		// get the angle between this and another vector
		float getAngleBetween(CVector* otherVector) const;
		void reset(); // reset elements to 0,0,0
		// distance between two points
		float distanceBetweenTwoPoints(CVector * point);
		// return the length of the magnitude squared
		float getMagnitudeSquared() const;
		// roughly equals (within 0.00001f difference) 
		bool fuzzyEquals(CVector * v);
		bool fuzzyEquals(const CVector& v);
		// make all the vectors elements positive
		void makeVectorPositive();

		CVector Get2D(){ return CVector(x, 0.0f, z);}
		bool IsNaN();

		//overloaded operators
		CVector& operator=(const CVector *v); // equals
		CVector& operator+=(const CVector * v); // plus equals
		CVector& operator+=(const CVector& v); // plus equals
		CVector& operator*=(const CVector * v); // multiply equals
		CVector& operator*=(float s); // multiply this vector by a scalar
		CVector& operator/=(float s); // divide by and make equal
		CVector& operator-=(const CVector * v); // minus equals
		CVector& operator/=(const CVector *v); // divide by vector and make equal
		CVector operator*(float s) const; // multiply by scaler
		CVector operator/(float s);	// divide by a scaler 
		CVector operator/(const CVector *v); // divide by vector
		CVector operator*(const CVector *v); // multiply by vector
		CVector operator*(const CVector& v); // multiply by vector
		CVector operator-(const CVector *v); // this vector minus another vector
		CVector operator-(const CVector& v) const; // this vector minus another vector
		CVector operator-() const; // return the negation of this vector
		CVector operator+(const CVector *v); // addition operator
		CVector operator+(const CVector& v) const; // addition operator
		bool operator==(const CVector &v) const; // is equal to (within EPSILON difference) 
		bool operator!=(const CVector &v) const; // is not equal to (within EPSILON difference)

// ----------------- new new new new new new --------------------------------------------------------------
		// cross product operation
		void crossProduct(const CVector& point1, const CVector& point2, const CVector& point3); 
		void crossProduct(const CVector& vector1, const CVector& vector2);
		CVector crossProduct(const CVector& vector1) const; // this vector crossed with another
		// return true/false based on whether the vectors were disimilar enough to be crossed
		static bool CrossProduct(const CVector& vector1, const CVector& vector2, CVector& result);
		// get the angle between this and another vector
		float getAngleBetween(const CVector& otherVector);
		// distance between two points
		float distanceBetweenTwoPoints(const CVector& point);

		CVector& SafeNormalizeXZ();
		static bool SafeNormalizeXZ(CVector& outVector);
		static bool SafeNormalize(CVector& outVector);

		//overloaded operators
		CVector& operator=(const CVector& v); // equals
		CVector& operator*=(const CVector& v); // multiply equals
		CVector& operator-=(const CVector& v); // minus equals
		CVector& operator/=(const CVector& v); // divide by vector and make equal
		CVector operator/(const CVector& v); // divide by vector
// ----------------- new new new new new new --------------------------------------------------------------
		
		// getters and setters
		inline float getX()const {return(x);}
		inline float getY()const {return(y);}
		inline float getZ()const {return(z);}
		inline void setX(float newX) { x = newX;}
		inline void setY(float newY) { y = newY;}
		inline void setZ(float newZ) { z = newZ;}
//-----------------------------------------------------------
}; // end class CVector

class CVector4
{
	public:
		static CVector4 red;
		static CVector4 white;
		static CVector4 blue;
		static CVector4 green;
		static CVector4 black;

		CVector4(float xIn, float yIn, float zIn, float wIn)
			: v3(xIn, yIn, zIn)
			, w(wIn)
		{

		}

		CVector4()
			: w(1.0f)
		{

		}

		CVector4(const CVector& in, float wIn)
			: v3(in) 
			, w(wIn)
		{

		}

		float operator [](int element) const
        {
            switch (element)
			{
			case 0:
				return v3.x;
			case 1:
				return v3.y;
			case 2:
				return v3.z;
			case 3:
				return w;
			default:
				assert(false);
				return 0;
			}
        }

		CVector4 operator-() const // return the negation of this vector
		{
			return CVector4(-v3, -w); 
		}

		CVector v3;
		float w;
};

#endif // ifndef _CVector_h_ 
