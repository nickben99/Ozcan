/*-----------------------------------------
CLASS BASED ON CQUATERNION.H/CPP
FROM	Bourg, D.M. (2002).  Physics for Game Developers.  1st ed.,  O'Reilly.
-------------------------------------------*/

//CQuaternion.cpp, a quaternion class

// system includes --------
#include <math.h> // maths functions
//-------------------------

// header files -----------
#include "Math/CQuaternion.h" // header file
#include "Game/defines.h" // useful (mainly mathematical) defines
#include "Math/CMatrix.h" // matrix object
//-------------------------

// constructor
CQuaternion::CQuaternion( void)
{ // initialise
	identity();
}

//	Constructor, create quaternion from angle and vector
CQuaternion::CQuaternion(float newN, const CVector& newV)
{
	makeQuaternionFromAngleAndAxis(newN, newV);
}

// copy constructor
CQuaternion::CQuaternion(const CQuaternion& aQuaternion)
{
	*this = aQuaternion;
}

CQuaternion::CQuaternion(float xIn, float yIn, float zIn, float nIn)
	: v(xIn, yIn, zIn)
	, n(nIn)
{
}

// destructor
CQuaternion::~CQuaternion()
{
}

// set to identity
void CQuaternion::identity()
{
	n = 1.0f; // scalar part 
	v.reset(); // vector part
}

// multiply and make equal to
CQuaternion& CQuaternion::operator*=(const CQuaternion& q) 
{
	(*this) = (*this)*q;
	return *this;
}

// quaternion multiplication
CQuaternion CQuaternion::operator*(const CQuaternion& q) const
{
	CQuaternion returnQuat;

	returnQuat.v = ((q.v * n) + (v * q.n)) + (v.crossProduct(q.v));
	returnQuat.n = n*q.n - v.dotProduct(q.v);

	return(returnQuat);
}

bool CQuaternion::operator==(const CQuaternion& m) const
{
	return (m.n == n && m.v == v);
}

bool CQuaternion::operator!=(const CQuaternion& m) const
{
	return !operator==(m);
}

/*	returns the eular angle representation of the quaternion
	returns y and z in the range +/- 180 and x in range +/- 90. 
	BASED ON METHOD FROM http://www.darwin3d.com/gdm1998.htm */
CVector CQuaternion::eularAngleQuaternionRepresentation()
{
	float elements[11];

	// convert	 quaternion to matrix
	elements[0] = 1.0f - 2.0f * v.y * v.y - 2.0f * v.z * v.z;
	elements[1] = 2.0f * v.x * v.y + 2.0f * n * v.z;
	elements[2] = 2.0f * v.x * v.z - 2.0f * n * v.y;

	elements[4] = 2.0f * v.x * v.y - 2.0f * n * v.z;
	elements[5] = 1.0f - 2.0f * v.x * v.x - 2.0f * v.z * v.z;
	elements[6] = 2.0f * v.y * v.z + 2.0f * n * v.x;

	elements[8] = 2.0f * v.x * v.z + 2.0f * n * v.y;
	elements[9] = 2.0f * v.y * v.z - 2.0f * n * v.x;
	elements[10] = 1.0f - 2.0f * v.x * v.x - 2.0f * v.y * v.y;

	float sy, cy, cx, sx, sz, cz;
	CVector euler;

	sy = -elements[2];
	cy = (float)sqrt(1 - (sy * sy));
	euler.y = (float)atan2(sy,cy);

	if (sy != 1.0f && sy != -1.0f)	
	{
		cx = elements[10] / cy;
		sx = elements[6] / cy;
		euler.x = (float)atan2(sx,cx);

		cz = elements[0] / cy;
		sz = elements[1] / cy;
		euler.z = (float)atan2(sz,cz);
	}
	else
	{
		cx = elements[5];
		sx = -elements[9];
		euler.x = (float)atan2(sx,cx);	

		cz = 1.0f;
		sz = 0.0f;
		euler.z = (float)atan2(sz,cz);	
	}
	return(euler);
}

/*	METHOD FROM - http://www.flipcode.com/documents/matrfaq.html#Q56
	makes a quaternion from an angle and axis */
void CQuaternion::makeQuaternionFromAngleAndAxis(float angle, const CVector& axis)
{
	float	radAngle = angle,
			sin_a = (float)sin( radAngle / 2.0f ),
			cos_a = (float)cos( radAngle / 2.0f );

    v = axis * sin_a;
    n   = cos_a;

    normalise();
}

/*	METHOD FROM - http://www.flipcode.com/documents/matrfaq.html#Q56
	normalise quaternion */
void CQuaternion::normalise()
{
	float magnitude = getMagnitude();

	n /= magnitude;
	v /= magnitude;
}

/*	METHOD FROM - http://www.flipcode.com/documents/matrfaq.html#Q56
	conjugate quaternion */
CQuaternion CQuaternion::getConjugate()
{
	CQuaternion returnQuat;
	returnQuat.n = n;
	returnQuat.v = -v;
	return( returnQuat );
}

/*	METHOD FROM - http://www.flipcode.com/documents/matrfaq.html#Q56
	get the magnitude of a quaternion */
float CQuaternion::getMagnitude()
{
	return((float)sqrt(dotProduct(this)));
}

// dot product
float CQuaternion::dotProduct( CQuaternion *q)
{
	return( (n*q->n)+(v.dotProduct(&q->v)));
}

// make quaternion equal to another quaternion
CQuaternion &CQuaternion::operator=(const CQuaternion& q)
{
	n = q.n;
	v = q.v;
	return *this;
}

// convert eular angles to quaternion
void CQuaternion::makeQuaternionFromEularAngles(CVector * vect)
{
	float		angle;
	float		sr, sp, sy, cr, cp, cy;

	angle = vect->getZ() * 0.5f;
	sy = (float)sin(angle);
	cy = (float)cos(angle);
	angle = vect->getY() * 0.5f;
	sp = (float)sin(angle);
	cp = (float)cos(angle);
	angle = vect->getX() * 0.5f;
	sr = (float)sin(angle);
	cr = (float)cos(angle);

	v.setX( sr*cp*cy-cr*sp*sy); // X
	v.setY( cr*sp*cy+sr*cp*sy); // Y
	v.setZ( cr*cp*sy-sr*sp*cy); // Z
	n = cr*cp*cy+sr*sp*sy;		// W
}

void CQuaternion::makeQuaternionFromForwardVector(const CVector& forward)
{
	*this = CMatrix(forward).matrixToQuaternion();
}

void CQuaternion::negateQuaternion()
{
	v = -v;
	n = -n;
}

// get angle of rotation
float CQuaternion::getRotationAngle()
{
	return( safeACos(n) * 2.0f );
}

// get axis of rotation - from 3D Math Primer: Dunn & Parberry
CVector CQuaternion::getRotationAxis()
{
	float	sineAngSquared = 1.0f - (n*n),
			OneOverSinAng;

	if (sineAngSquared < 0.0f){ // angle is zero so return any vector
		return( CVector(1.0f, 0.0f, 0.0f) );}

	/*	compute one over sine of the angle so mulitplication can be used below
		rather than divides */
	OneOverSinAng = 1.0f / (float)sqrt(sineAngSquared);

	return( v*OneOverSinAng );
}

// get axis and angle of rotation
void CQuaternion::getRotationAxisAndAngle(float *angle, CVector * axis)
{
	*angle = getRotationAngle();
	*axis = getRotationAxis();
}

// slerp between quaternions
/*	BASED ON METHOD FROM 3D MATH PRIMER FOR GRAPHICS AND GAME DEVELOPMENT 
	- DUNN AND PARBERRY */
void CQuaternion::quaternionSlerp(CQuaternion * from, CQuaternion * to, float d)
{
	// get cosine of angle between quaternions using dot product
	float cosOmega = from->dotProduct(to);
	
	CQuaternion newTo = *to;

	/*	if dot product is negative, negate a quaternion to
		find the shortest arc */
	if (cosOmega < 0.0f)
	{
		newTo.negateQuaternion();
		cosOmega = -cosOmega;
	} // end if cos is less than 0
	
	float k0, k1;
	/*	if the cos of the angle is very close to one, the angles are very 
		close to one and other.  Therefore use linear interpolation to avoid a 
		divide by 0 */
	if (cosOmega > 0.9999f)
	{
		k0 = 1.0f - d;
		k1 = d;
	}
	else // cos is not very close to one
	{
		// sin^2(ang) + cos^2(ang) = 1.  This is used to find the sine
		float sinOmega = (float)sqrt(1.0f - cosOmega*cosOmega);

		// angle is derived from the sine and the cos
		float omega = (float)atan2(sinOmega, cosOmega);

		// compute denominator to avoid divides
		float oneOverSinOmega = 1.0f / sinOmega;

		//compute interpolation parameters
		k0 = (float)sin((1.0f - d)*omega)*oneOverSinOmega;
		k1 = (float)sin(d*omega)*oneOverSinOmega;
	}// end else cos is not very close to one
	
	// interpolation
	n = from->n*k0 + newTo.n*k1; 
	v.setX(from->v.getX()*k0 + newTo.v.getX()*k1);
	v.setY(from->v.getY()*k0 + newTo.v.getY()*k1);
	v.setZ(from->v.getZ()*k0 + newTo.v.getZ()*k1);
}

// rotate vector
CVector CQuaternion::rotateVector( CVector * vect )
{
	CMatrix rotationMatrix;
	rotationMatrix.quaternionToMatrix(*this); // convert quaternion to matrix

	return( rotationMatrix.multiplyRotateVector(vect) );
}