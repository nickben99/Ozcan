/*-----------------------------------------
CLASS BASED ON VECTOR.H/CPP
FROM 	Deloura, M. (2000). Game Programming Gems. 1st ed., Charles River Media -
Article title: Flocking: A Simple Technique for Simulating Group Behavior by Woodcock, S

CLASS ALSO BASED ON VECTOR.H.CPP
FROM	Bourg, D.M. (2002).  Physics for Game Developers.  1st ed.,  O'Reilly.
-------------------------------------------*/

//CVector.cpp, a 3d vector class

// system includes --------
#include <math.h>
#include <float.h>
//-------------------------

// header files -----------
#include "Math/CVector.h"

//-------------------------

CVector CVector::zero(0.0f, 0.0f, 0.0f);
CVector CVector::one(1.0f, 1.0f, 1.0f);
CVector CVector::unitX(1.0f, 0.0f, 0.0f);
CVector CVector::unitY(0.0f, 1.0f, 0.0f);
CVector CVector::unitZ(0.0f, 0.0f, 1.0f);

CVector4 CVector4::red(1.0f, 0.0f, 0.0f, 1.0f);
CVector4 CVector4::white(1.0f, 1.0f, 1.0f, 1.0f);
CVector4 CVector4::blue(0.0f, 0.0f, 1.0f, 1.0f);
CVector4 CVector4::green(0.0f, 1.0f, 0.0f, 1.0f);
CVector4 CVector4::black(0.0f, 0.0f, 0.0f, 1.0f);

// constructor
CVector::CVector(void)
{
	reset();
}

// Constructor, with initialisation values
CVector::CVector(float inX, float inY, float inZ)
{
	x = (float)inX;
	y = (float)inY;
	z = (float)inZ;
}

// copy constructor
CVector::CVector(const CVector& aVector)
{
	*this = aVector;
}

CVector::CVector(const CVector4& in)
{
	*this = in.v3;
}

// destructor
CVector::~CVector()
{
}

// make vector equal to another vector
CVector &CVector::operator=(const CVector *m)
{
	x = m->getX();
	y = m->getY();
	z = m->getZ();
	return *this;
}

// make vector equal to another vector
CVector &CVector::operator=(const CVector& m)
{
	x = m.x;
	y = m.y;
	z = m.z;
	return *this;
}

const float kNormalizeTolerance = 0.0001f;
// make vector unit length
CVector& CVector::normalise()
{
	float length = (float)sqrt(x*x + y*y + z*z);
	if (length <= kNormalizeTolerance)
	{ 
		length = 1;
	}
	// divide vector by length
	x /= length;
	y /= length;
	z /= length;
	
	// if practically less than zero, set to zero
	if (fabs(x) < kNormalizeTolerance)
	{ 
		x = 0.0f;
	}
	if (fabs(y) < kNormalizeTolerance)
	{ 
		y = 0.0f;
	}
	if (fabs(z) < kNormalizeTolerance)
	{ 
		z = 0.0f;
	}
	return *this;
}

bool CVector::SafeNormalize(CVector& outVector)
{
	bool allGood = true;
	float length = (float)sqrt(outVector.x*outVector.x + outVector.y*outVector.y + outVector.z*outVector.z);
	if (length <= kNormalizeTolerance)
	{ 
		allGood = false;
		length = 1;
	}
	// divide vector by length
	outVector.x /= length;
	outVector.y /= length;
	outVector.z /= length;
	
	// if practically less than zero, set to zero
	if (fabs(outVector.x) < kNormalizeTolerance)
	{ 
		outVector.x = 0.0f;
	}
	if (fabs(outVector.y) < kNormalizeTolerance)
	{ 
		outVector.y = 0.0f;
	}
	if (fabs(outVector.z) < kNormalizeTolerance)
	{ 
		outVector.z = 0.0f;
	}
	return allGood;

	//float length = outVector.getMagnitude();
	//// if already practically normalised
	//if (length > kNormalizeTolerance)
	//{ // divide vector by length
	//	outVector.x /= length;
	//	outVector.y /= length;
	//	outVector.z /= length;
	//	return true;
	//}
	//else
	//{
	//	outVector.x = outVector.y = outVector.z = 0.0f;
	//}
	//return false;
}

CVector& CVector::SafeNormalizeXZ()
{
	SafeNormalizeXZ(*this);
	return *this;
}

bool CVector::SafeNormalizeXZ(CVector& outVector)
{
	bool allGood = true;
	float length = (float)sqrt(outVector.x*outVector.x + outVector.z*outVector.z);
	if (length <= kNormalizeTolerance)
	{ 
		allGood = false;
		length = 1;
	}
	// divide vector by length
	outVector.x /= length;
	outVector.y = 0.0f;
	outVector.z /= length;
	
	// if practically less than zero, set to zero
	if (fabs(outVector.x) < kNormalizeTolerance)
	{ 
		outVector.x = 0.0f;
	}
	if (fabs(outVector.z) < kNormalizeTolerance)
	{ 
		outVector.z = 0.0f;
	}
	return allGood;

	//float length = (float)sqrt(outVector.x*outVector.x + outVector.z*outVector.z);
	//if (length > kNormalizeTolerance)
	//{ // divide vector by length
	//	outVector.x /= length;
	//	outVector.y = 0.0f;
	//	outVector.z /= length;
	//	return true;
	//}
	//else
	//{
	//	outVector.x = outVector.y = outVector.z = 0.0f;
	//}
	//return false;
}

// add another vector to this vector
CVector &CVector::operator+=(const CVector * v)
{
	(*this) = (*this)+v;
	return *this;
}

// add another vector to this vector
CVector &CVector::operator+=(const CVector& v)
{
	(*this) = (*this)+v;
	return *this;
}

// subtract another vector from this vector
CVector &CVector::operator-=(const CVector * v)
{
	(*this) = (*this)-v;
	return *this;
}

// subtract another vector from this vector
CVector &CVector::operator-=(const CVector& v)
{
	(*this) = (*this)-v;
	return *this;
}

// divide by vector and make equal
CVector &CVector::operator/=(const CVector * v)
{
	(*this) = (*this)/v;
	return *this;
}

// divide by vector and make equal
CVector &CVector::operator/=(const CVector& v)
{
	(*this) = (*this)/v;
	return *this;
}

// multiply by vector and make equal
CVector &CVector::operator*=(const CVector * v)
{
	(*this) = (*this)*v;
	return *this;
}

// multiply by vector and make equal
CVector &CVector::operator*=(const CVector& v)
{
	(*this) = (*this)*v;
	return *this;
}

// divide by and make equal
CVector &CVector::operator/=(float s)
{
	(*this) = (*this)/s;
	return *this;
}

// return the result of this vector multiplied by a scaler
CVector CVector::operator*(float s) const
{
	return( CVector(x*s, y*s, z*s) );
}

// return the result of this vector minus another vector
CVector CVector::operator-(const CVector *v)
{
	return( CVector(x-v->x, y-v->y, z-v->z) );
}

// return the result of this vector minus another vector
CVector CVector::operator-(const CVector& v) const
{
	return( CVector(x-v.x, y-v.y, z-v.z) );
}


// addition operator
CVector CVector::operator+(const CVector *v)
{
	return( CVector(x+v->x, y+v->y, z+v->z) );
}

// addition operator
CVector CVector::operator+(const CVector& v) const
{
	return( CVector(x+v.x, y+v.y, z+v.z) );
}

// divide by vector operator
CVector CVector::operator/(const CVector *v)
{
	float	newX = 	x/v->x, 
			newY = 	y/v->y, 
			newZ = 	z/v->z;
	// check the results are numbers (not +/- infinity etc)
    if (defines::IsNaN(newX))
	{ 
		newX = 0.0f;
	}
	if (defines::IsNaN(newY))
	{ 
		newY = 0.0f;
	}
	if (defines::IsNaN(newZ))
	{ 
		newZ = 0.0f;
	}
	return( CVector(newX, newY, newZ) );
}

// divide by vector operator
CVector CVector::operator/(const CVector& v)
{
	float	newX = 	x/v.x, 
			newY = 	y/v.y, 
			newZ = 	z/v.z;
	// check the results are numbers (not +/- infinity etc)
	if (defines::IsNaN(newX))
	{ 
		newX = 0.0f;
	}
	if (defines::IsNaN(newY))
	{ 
		newY = 0.0f;
	}
	if (defines::IsNaN(newZ))
	{ 
		newZ = 0.0f;
	}
	return( CVector(newX, newY, newZ) );
}

// multiply by vector operator
CVector CVector::operator*(const CVector* v)
{
	return( CVector(x*v->x, y*v->y, z*v->z) );
}

// multiply by vector operator
CVector CVector::operator*(const CVector& v)
{
	return( CVector(x*v.x, y*v.y, z*v.z) );
}

// return the negation of this vector
CVector CVector::operator-() const
{
	return( CVector(-x, -y, -z) );
}

// divide by a scaler
CVector CVector::operator/(float s)
{
	float multiplier = 1.0f/s; // compute one over s, for multiplies below
	// a divide by zero above may make multiplier a NaN (not a number)
#ifdef _WINDOWS
	int floatClass = _fpclass(multiplier);
	if (_FPCLASS_SNAN == floatClass ||
		_FPCLASS_QNAN == floatClass ||
		_FPCLASS_NINF == floatClass ||
		_FPCLASS_PINF == floatClass ){
#elif OSX
    if (defines::IsNaN(multiplier)) {
#endif
		multiplier = 0.0f;} // not a number, set to zero
	return (*this*multiplier); 
}

// set the length of the vector
void CVector::setMagnitude(float mag)
{
	normalise();
	*this *= mag;
}

// reset elements to 0
void CVector::reset()
{
	x = y = z = (float)0.0f;
}

// get the length of the vector
float CVector::getMagnitude() const
{
	return((float)sqrt(getMagnitudeSquared()));
}

// multiply this vector by a scaler
CVector &CVector::operator*=(float s)
{
	(*this) = (*this)*s;
	return *this;
}

// dot product operation 
float CVector::dotProduct(CVector * productWith)
{
	float dotProduct = (	x*productWith->x +
							y*productWith->y +
							z*productWith->z	);

	if( defines::IsNaN(dotProduct) ){ // avoids -1.#IND0000000 NaN problems
		return( 0.0f );}

	return( dotProduct );
}

// dot product operation 
float CVector::dotProduct(const CVector& productWith) const
{
	float dotProduct = (	x*productWith.x +
							y*productWith.y +
							z*productWith.z	);

	if( defines::IsNaN(dotProduct) ){ // avoids -1.#IND0000000 NaN problems
		return( 0.0f );}

	return( dotProduct );
}

// get the angle between this and another vector
float CVector::getAngleBetween(CVector* otherVector) const
{
	CVector thisVector	=	*this,
			otherVect	=	*otherVector;

	thisVector.normalise();
	otherVect.normalise();
	
	float dotProduct = thisVector.dotProduct(&otherVect);

	float angle = ((float)acos( dotProduct ) );

	if( defines::IsNaN(angle) ){ // avoids -1.#IND0000000 NaN problems
	return( 0.0f );} // dotProduct will be practically 1 so return an acos of 0 angle

	return( angle );
}

// get the angle between this and another vector
float CVector::getAngleBetween(const CVector& otherVector)
{
	CVector thisVector	=	*this,
			otherVect	=	otherVector;

	thisVector.normalise();
	otherVect.normalise();
	
	float dotProduct = thisVector.dotProduct(otherVect);

	float angle = ((float)acos( dotProduct ) );

	if( defines::IsNaN(angle) ){ // avoids -1.#IND0000000 NaN problems
	return( 0.0f );} // dotProduct will be practically 1 so return an acos of 0 angle

	return( angle );
}

/*	cross product operation
	METHOD FROM THE OPENGL SUPERBIBLE */
void CVector::crossProduct(CVector* point1, CVector* point2, CVector* point3)
{
	CVector vector1, vector2;
	
	//calculate two vectors from the three points
	vector1 = *point2 - point1;
	vector2 = *point3 - point1;

	crossProduct(&vector1, &vector2); // call cross product which takes vector parameters
}

/*	cross product operation
	METHOD FROM THE OPENGL SUPERBIBLE */
void CVector::crossProduct(CVector *vector1, CVector *vector2)
{
	// get the cross product of the two vector
	x = ( vector1->getY()*vector2->getZ() ) - ( vector1->getZ()*vector2->getY() );
	y = ( vector1->getZ()*vector2->getX() ) - ( vector1->getX()*vector2->getZ() );
	z = ( vector1->getX()*vector2->getY() ) - ( vector1->getY()*vector2->getX() );
}

// this vector crossed with another
CVector CVector::crossProduct(CVector *vector1)
{
	CVector cross;
	cross.crossProduct(this, vector1);

	return(cross);
}

/*	cross product operation
	METHOD FROM THE OPENGL SUPERBIBLE */
void CVector::crossProduct(const CVector& point1, const CVector& point2, const CVector& point3)
{
	CVector vector1, vector2;
	
	//calculate two vectors from the three points
	vector1 = point2 - point1;
	vector2 = point3 - point1;

	crossProduct(vector1, vector2); // call cross product which takes vector parameters
}

/*	cross product operation
	METHOD FROM THE OPENGL SUPERBIBLE */
void CVector::crossProduct(const CVector& vector1, const CVector& vector2)
{
	// get the cross product of the two vector
	x = ( vector1.y*vector2.z ) - ( vector1.z*vector2.y );
	y = ( vector1.z*vector2.x ) - ( vector1.x*vector2.z );
	z = ( vector1.x*vector2.y ) - ( vector1.y*vector2.x );
}

bool CVector::CrossProduct(const CVector& vector1, const CVector& vector2, CVector& result)
{
	result.crossProduct(vector1, vector2);
	const float kCrossProductTolSqr = 0.0001f*0.0001f;
	if (result.getMagnitudeSquared() > kCrossProductTolSqr)
	{
		return true;
	}
	return false;
}

// this vector crossed with another
CVector CVector::crossProduct(const CVector& vector1) const
{
	CVector cross;
	cross.crossProduct(*this, vector1);

	return(cross);
}

// distance between two points
float CVector::distanceBetweenTwoPoints(CVector * point)
{
	return( (*this - point).getMagnitude() );
}

// distance between two points
float CVector::distanceBetweenTwoPoints(const CVector& point)
{
	return( (*this - point).getMagnitude() );
}

// return the length of the magnitude squared
float CVector::getMagnitudeSquared() const
{
	return( x*x + y*y + z*z );
}

// is this vector equal to another vector (within EPSILON difference)
bool CVector::operator==(const CVector &v) const
{
	//return((FLOAT_EQUALS(x, v.x)) && (FLOAT_EQUALS(y, v.y)) && 
	//		(FLOAT_EQUALS(z, v.z)));
	return(x == v.x && y == v.y && z == v.z);
}

// is this vector not equal to another vector
bool CVector::operator!=(const CVector &v) const
{
	return((!( (*this) == v )));
}

// is this vector equal to another vector (within 0.00001f difference) 
bool CVector::fuzzyEquals(CVector * v)
{
	return( ( FEQUALS(x, v->x) ) &&
			( FEQUALS(y, v->y) ) &&
			( FEQUALS(z, v->z) ) );
}

// is this vector equal to another vector (within 0.00001f difference) 
bool CVector::fuzzyEquals(const CVector& v)
{
	return( ( FEQUALS(x, v.x) ) &&
			( FEQUALS(y, v.y) ) &&
			( FEQUALS(z, v.z) ) );
}

// make all the vectors elements positive
void CVector::makeVectorPositive()
{
	x = (float)fabs(x);
	y = (float)fabs(y);
	z = (float)fabs(z);
}

bool CVector::IsNaN()
{
	return x != x || y != y || z != z;
}