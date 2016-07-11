/*-----------------------------------------
CLASS BASED ON MATRIX3*3.H/CPP
FROM	Bourg, D.M. (2002).  Physics for Game Developers.  1st ed.,  O'Reilly.
-------------------------------------------*/

//CMatrix.cpp, a 4*4 matrix class

// system includes --------
#include <math.h>
#include <algorithm>
//-------------------------

// header files -----------
#include "Math/CMatrix.h"

#include "stdio.h"
//-------------------------

// constructor
CMatrix::CMatrix( void)
{ // initialise to the identity matrix
	identity();
}

// Constructor, with initialisation values
CMatrix::CMatrix(float * in)
{
	elements[0]=in[0];	 elements[1]=in[1];	  elements[2]=in[2];   elements[3]=in[3];
	elements[4]=in[4];	 elements[5]=in[5];	  elements[6]=in[6];   elements[7]=in[7];
	elements[8]=in[8];	 elements[9]=in[9];	  elements[10]=in[10]; elements[11]=in[11];
	elements[12]=in[12]; elements[13]=in[13]; elements[14]=in[14];  elements[15]=in[15];
}

CMatrix::CMatrix(const CQuaternion& quat)
{
	quaternionToMatrix(quat);
}

CMatrix::CMatrix(const CVector& forward)
{
	CreateMatrix(forward);
}

CMatrix::CMatrix(const CVector4& right, const CVector4& up, const CVector4& forward, const CVector4& translation)
{
	SetRight(right);
	SetUp(up);
	SetForward(forward);
	SetMatrixTranslation(translation); // set the matrix translation
}

// destructor
CMatrix::~CMatrix()
{
}

// multiply this matrix by another matrix
CMatrix &CMatrix::operator*=(const CMatrix *m)
{
	*this = (*this)*m;
	return *this;
}

// multiply this matrix by another matrix
CMatrix &CMatrix::operator*=(const CMatrix& m)
{
	*this = (*this)*m;
	return *this;

}
// multiply
CMatrix CMatrix::operator*(const CMatrix& m)
{
	return operator*(&m);
}

// multiply
CMatrix CMatrix::operator*(const CMatrix *m)
{
	float newElements[16];

	newElements[0] =	elements[0]*m->elements[0] + elements[4]*m->elements[1] +
						elements[8]*m->elements[2] + elements[12]*m->elements[3];

	newElements[4] =	elements[0]*m->elements[4] + elements[4]*m->elements[5] +
						elements[8]*m->elements[6] + elements[12]*m->elements[7];
	newElements[8] =	elements[0]*m->elements[8] + elements[4]*m->elements[9] +
						elements[8]*m->elements[10] + elements[12]*m->elements[11];
	newElements[12] =	elements[0]*m->elements[12] + elements[4]*m->elements[13] +
						elements[8]*m->elements[14] + elements[12]*m->elements[15];
	
	newElements[1] =	elements[1]*m->elements[0] + elements[5]*m->elements[1] +
						elements[9]*m->elements[2] + elements[13]*m->elements[3];
	newElements[5] =	elements[1]*m->elements[4] + elements[5]*m->elements[5] +
						elements[9]*m->elements[6] + elements[13]*m->elements[7];
	newElements[9] =	elements[1]*m->elements[8] + elements[5]*m->elements[9] +
						elements[9]*m->elements[10] + elements[13]*m->elements[11];
	newElements[13] =	elements[1]*m->elements[12] + elements[5]*m->elements[13] +
						elements[9]*m->elements[14] + elements[13]*m->elements[15];

	newElements[2] =	elements[2]*m->elements[0] + elements[6]*m->elements[1] +
						elements[10]*m->elements[2] + elements[14]*m->elements[3];
	newElements[6] =	elements[2]*m->elements[4] + elements[6]*m->elements[5] +
						elements[10]*m->elements[6] + elements[14]*m->elements[7];
	newElements[10] =	elements[2]*m->elements[8] + elements[6]*m->elements[9] +
						elements[10]*m->elements[10] + elements[14]*m->elements[11];
	newElements[14] =	elements[2]*m->elements[12] + elements[6]*m->elements[13] +
						elements[10]*m->elements[14] + elements[14]*m->elements[15];

	newElements[3] =	elements[3]*m->elements[0] + elements[7]*m->elements[1] +
						elements[11]*m->elements[2] + elements[15]*m->elements[3];
	newElements[7] =	elements[3]*m->elements[4] + elements[7]*m->elements[5] +
						elements[11]*m->elements[6] + elements[15]*m->elements[7];
	newElements[11] =	elements[3]*m->elements[8] + elements[7]*m->elements[9] +
						elements[11]*m->elements[10] + elements[15]*m->elements[11];
	newElements[15] =	elements[3]*m->elements[12] + elements[7]*m->elements[13] +
						elements[11]*m->elements[14] + elements[15]*m->elements[15];
	
	return( CMatrix(&newElements[0]) );
}

// multiply
CVector4 CMatrix::operator*(const CVector4& vect)
{
	CVector4 v;
	v.v3.x =	vect.v3.x*elements[0] + vect.v3.y*elements[4] + vect.v3.z*elements[8] + vect.w*elements[12];
	v.v3.y =	vect.v3.x*elements[1] + vect.v3.y*elements[5] + vect.v3.z*elements[9] + vect.w*elements[13];
	v.v3.z =	vect.v3.x*elements[2] + vect.v3.y*elements[6] + vect.v3.z*elements[10] + vect.w*elements[14]; 
	v.w =		vect.v3.x*elements[3] + vect.v3.y*elements[7] + vect.v3.z*elements[11] + vect.w*elements[15];
	return v;
}

CMatrix CMatrix::Transposed() const
{
	CVector4 x = right();
	CVector4 y = up();
	CVector4 z = forward();
	CVector4 w = getMatrixTranslation();

	CMatrix m;
	m.SetRight(CVector4(x.v3.x, y.v3.x, z.v3.x, w.v3.x));
	m.SetUp(CVector4(x.v3.y, y.v3.y, z.v3.y, w.v3.y));
	m.SetForward(CVector4(x.v3.z, y.v3.z, z.v3.z, w.v3.z));
	m.SetMatrixTranslation(CVector4(x.w, y.w, z.w, w.w));
    return m;
}

// set to the identity matrix
void CMatrix::identity()
{
	elements[0]=1.0f;	elements[1]=0.0f;	elements[2]=0.0f;	elements[3]=0.0f;
	elements[4]=0.0f;	elements[5]=1.0f;	elements[6]=0.0f;	elements[7]=0.0f;
	elements[8]=0.0f;	elements[9]=0.0f;	elements[10]=1.0f;	elements[11]=0.0f;
	elements[12]=0.0f;	elements[13]=0.0f;	elements[14]=0.0f;	elements[15]=1.0f;
}

// make matrix equal to another matrix
CMatrix &CMatrix::operator=(const CMatrix *m)
{
	for (int i = 0; i < 16; i++){
		elements[i] = m->elements[i];}
	return *this;
}

bool CMatrix::operator==(const CMatrix& m) const
{
	for (int i = 0; i < 16; i++)
	{
		if (!FLOAT_EQUALS(elements[i], m.elements[i]))
		{
			return false;
		}
	}
	return true;
}

bool CMatrix::operator!=(const CMatrix& m) const
{
	return !operator==(m);
}

void CMatrix::createXYZRotationMatrix(CVector *v)
{
	CMatrix m;

	double cX = cos( v->getX() );
	double sX = sin( v->getX() );
	double cY = cos( v->getY() );
	double sY = sin( v->getY() );
	double cZ = cos( v->getZ() );
	double sZ = sin( v->getZ() );

	m.elements[0] = ( float )( cY*cZ );
	m.elements[1] = ( float )( cY*sZ );
	m.elements[2] = ( float )( -sY );

	double sXsY = sX*sY;
	double cXsY = cX*sY;

	m.elements[4] = ( float )( sXsY*cZ-cX*sZ );
	m.elements[5] = ( float )( sXsY*sZ+cX*cZ );
	m.elements[6] = ( float )( sX*cY );

	m.elements[8] = ( float )( cXsY*cZ+sX*sZ );
	m.elements[9] = ( float )( cXsY*sZ-sX*cZ );
	m.elements[10] = ( float )( cX*cY );

	*this = m;
}

//create a matrix which will rotate around the X axis
void CMatrix::createXRotationMatrix(float angle)
{
	float Sin = (float)(float)sin(angle);
	float Cos = (float)(float)cos(angle);
	identity(); // set to identity
	// create matrix
	elements[5] = Cos; 
	elements[6] = Sin;
	elements[9] = -Sin;
	elements[10] = Cos;
}

//create a matrix which will rotate around the Y axis
void CMatrix::createYRotationMatrix(float angle)
{
	float Sin = (float)(float)sin(angle);
	float Cos = (float)(float)cos(angle);
	identity(); // set to identity
	// create matrix
	elements[0] = Cos; 
	elements[2] = -Sin;
	elements[8] = Sin;
	elements[10] = Cos;
}

//create a matrix which will rotate around the Z axis
void CMatrix::createZRotationMatrix(float angle)
{
	float Sin = (float)(float)sin(angle);
	float Cos = (float)(float)cos(angle);
	identity(); // set to identity
	// create matrix
	elements[0] = Cos; 
	elements[1] = Sin;
	elements[4] = -Sin;
	elements[5] = Cos;	
}

/*	multiplies the rotational portion of this matrix by the row vector sent as a parameter
	and returns the result */
CVector CMatrix::multiplyRotateVector(CVector *vect)
{
	CVector newVect(vect->getX()*elements[0]+vect->getY()*elements[4]+vect->getZ()*elements[8],
					vect->getX()*elements[1]+vect->getY()*elements[5]+vect->getZ()*elements[9],
					vect->getX()*elements[2]+vect->getY()*elements[6]+vect->getZ()*elements[10]);

	return(newVect);
}

/*	multiplies the rotational portion of this matrix by the row vector sent as a parameter
	and returns the result */
CVector CMatrix::multiplyRotateVector(const CVector& vect)
{
	CVector newVect(vect.x*elements[0]+vect.y*elements[4]+vect.z*elements[8],
					vect.x*elements[1]+vect.y*elements[5]+vect.z*elements[9],
					vect.x*elements[2]+vect.y*elements[6]+vect.z*elements[10]);

	return(newVect);
}

/*	produces a new row vector by multiplying this matrix by the row vector paramter as 
	if the paramter was a column vector.  Therefore producing the inverse vector.  (Only
	the rotational portion of the vector is considered) */
CVector CMatrix::multiplyRotateVectorInverse(CVector *vect)
{
	CMatrix inverseMatrix = this->getInverseRotationMatrix();

	return(inverseMatrix.multiplyRotateVector(vect));
}

// get inverse of rotational part of matrix
CMatrix CMatrix::getInverseRotationMatrix()
{
	CMatrix inverseMatrix;

	inverseMatrix.elements[0] = elements[0];
	inverseMatrix.elements[1] = elements[4];
	inverseMatrix.elements[2] = elements[8];

	inverseMatrix.elements[4] = elements[1];
	inverseMatrix.elements[5] = elements[5];
	inverseMatrix.elements[6] = elements[9];
	
	inverseMatrix.elements[8] = elements[2];
	inverseMatrix.elements[9] = elements[6];
	inverseMatrix.elements[10] = elements[10];

	return(inverseMatrix);
}

// multiply the matrix by the vector parameter to form a new rotated and translated vector
CVector CMatrix::multiplyVector(CVector *vect)
{
	CVector newVect(vect->getX()*elements[0]+vect->getY()*elements[4]+vect->getZ()*elements[8]+1.0f*elements[12],
					vect->getX()*elements[1]+vect->getY()*elements[5]+vect->getZ()*elements[9]+1.0f*elements[13],
					vect->getX()*elements[2]+vect->getY()*elements[6]+vect->getZ()*elements[10]+1.0f*elements[14]);
	return(newVect);
}

// translates by the inverse of this matrix
CVector CMatrix::matrixInverseTranslation(CVector *vect)
{
	CVector newVect(vect->getX()-elements[12],
					vect->getY()-elements[13],
					vect->getZ()-elements[14]);
	return(newVect);
}

// make *this into a matrix representation of the quaternion parameter 
void CMatrix::quaternionToMatrix(const CQuaternion& quat)
{
	elements[0] = 1.0f - 2.0f * quat.v.y * quat.v.y - 2.0f * quat.v.z * quat.v.z;
	elements[1] = 2.0f * quat.v.x * quat.v.y + 2.0f * quat.n * quat.v.z;
	elements[2] = 2.0f * quat.v.x * quat.v.z - 2.0f * quat.n * quat.v.y;

	elements[4] = 2.0f * quat.v.x * quat.v.y - 2.0f * quat.n * quat.v.z;
	elements[5] = 1.0f - 2.0f * quat.v.x * quat.v.x - 2.0f * quat.v.z * quat.v.z;
	elements[6] = 2.0f * quat.v.y * quat.v.z + 2.0f * quat.n * quat.v.x;

	elements[8] = 2.0f * quat.v.x * quat.v.z + 2.0f * quat.n * quat.v.y;
	elements[9] = 2.0f * quat.v.y * quat.v.z - 2.0f * quat.n * quat.v.x;
	elements[10] = 1.0f - 2.0f * quat.v.x * quat.v.x - 2.0f * quat.v.y * quat.v.y;
}

// compute a quaternion representation of this rotation matrix
/*	BASED ON http://www.cs.princeton.edu/~gewang/projects/darth/stuff/quat_faq.html */
CQuaternion CMatrix::matrixToQuaternion()
{
	float X = 0.0f, Y = 0.0f, Z = 0.0f, W = 0.0f;            
	float T = 1.0f + elements[0] + elements[5] + elements[10];

    if ( T > 0.00000001f )
	{
      float S = (float)sqrt(T) * 2.0f;
      X = ( elements[6] - elements[9] ) / S;
      Y = ( elements[8] - elements[2] ) / S;
      Z = ( elements[1] - elements[4] ) / S;
      W = 0.25f * S;
	}
    else if ( elements[0] > elements[5] && elements[0] > elements[10] )  {	// Column 0: 
        float S = (float)sqrt( 1.0f + elements[0] - elements[5] - elements[10] ) * 2.0f;
        X = 0.25f * S;
        Y = (elements[1] + elements[4] ) / S;
        Z = (elements[8] + elements[2] ) / S;
        W = (elements[6] - elements[9] ) / S;
	
    } else if ( elements[5] > elements[10] ) {			// Column 1: 
        float S  = (float)sqrt( 1.0f + elements[5] - elements[0] - elements[10] ) * 2.0f;
        X = (elements[1] + elements[4] ) / S;
        Y = 0.25f * S;
        Z = (elements[6] + elements[9] ) / S;
        W = (elements[8] - elements[2] ) / S;

    } else {						// Column 2:
        float S  = (float)sqrt( 1.0f + elements[10] - elements[0] - elements[5] ) * 2.0f;
        X = (elements[8] + elements[2] ) / S;
        Y = (elements[6] + elements[9] ) / S;
        Z = 0.25f * S;
        W = (elements[1] - elements[4] ) / S;
    }

	CQuaternion newQuat(X, Y, Z, W);
	newQuat.normalise();

	//CMatrix temp(newQuat);
	//if (temp != *this)
	//{
	//	temp = *this;
	//}


	return newQuat;
}// end matrixToQuaternion

// multiply vector by the inverse of this matrix
CVector CMatrix::multiplyVectorInverseMatrix(CVector *vect)
{
	float init[16];

	init[0] = elements[0];
	init[1] = elements[4];
	init[2] = elements[8];
	init[3] = 0.0f;

	init[4] = elements[1];
	init[5] = elements[5];
	init[6] = elements[9];
	init[7] = 0.0f;
	
	init[8] = elements[2];
	init[9] = elements[6];
	init[10] = elements[10];
	init[11] = 0.0f;

	init[12] =	(-elements[12]*init[0]) +
				(-elements[13]*init[4]) +
				(-elements[14]*init[8]);
	init[13] =	(-elements[12]*init[1]) +
				(-elements[13]*init[5]) +
				(-elements[14]*init[9]);
	init[14] =	(-elements[12]*init[2]) +
				(-elements[13]*init[6]) +
				(-elements[14]*init[10]);
	init[15] = 1.0f;

	CMatrix inverseMatrix(&init[0]);

	return(inverseMatrix.multiplyVector(vect));
}

// create scaling matrix
void CMatrix::creatScalingMatrix(float x, float y, float z)
{
	identity();

	elements[0] = x;
	elements[5] = y;
	elements[10] = z;
}

// get inverse of scaling matrix
CMatrix CMatrix::getInverseScalingMatrix()
{
	CMatrix inverseMatrix;

	inverseMatrix.elements[0] = 1.0f/elements[0];
	inverseMatrix.elements[5] = 1.0f/elements[5];
	inverseMatrix.elements[10] = 1.0f/elements[10];

	return(inverseMatrix);
}

// create a rotation matrix around an arbitrary axis
void CMatrix::createArbitraryAxisRotation(float angleRad, const CVector& rotationVect)
{
	// create quaternion and convert to matrix
	quaternionToMatrix( CQuaternion(angleRad, rotationVect) );
}

/*	create billboard matrix (creates the matrix required to 
	position and rotate a billboard so its facing the camera */
void CMatrix::createBillboardMatrix(CVector *right, CVector *up, 
									CVector *look, CVector *position)
{
	identity(); // load identity matrix
	
	// set the elements from the right vector
	elements[0] = right->x;
	elements[1] = right->y;
	elements[2] = right->z;

	// set the elements from the up vector
	elements[4] = up->x;
	elements[5] = up->y;
	elements[6] = up->z;

	// set the elements from the look vector
	elements[8] = look->x;
	elements[9] = look->y;
	elements[10] = look->z;

	SetMatrixTranslation(CVector4(*position, 1.0f));
}

bool CMatrix::CreateMatrix(const CVector& forward, const CVector& up)
{
	//float forwardLength = forward.getMagnitude();
	//UNUSED(forwardLength);
	CVector normalizedForward = forward;
	normalizedForward.normalise(); // double noramlize
	if (CVector::SafeNormalize(normalizedForward)) // double noramlize - shouldn't be required, but is
	{
		//float normalizedForwardLength = normalizedForward.getMagnitude();
		//UNUSED(normalizedForwardLength);

		CVector rightVector;
		if (CVector::CrossProduct(up, normalizedForward, rightVector))
		{
			rightVector.normalise();
			CVector upVector;
			upVector.crossProduct(normalizedForward, rightVector);
			upVector.normalise();

			SetForward(CVector4(normalizedForward, 0.0f));
			SetRight(CVector4(rightVector, 0.0f));
			SetUp(CVector4(upVector, 0.0f));
			return true;
		}
	}
	return false;
}

CVector4 CMatrix::forward() const
{
	return CVector4(elements[8], elements[9], elements[10], elements[11]);
}

CVector4 CMatrix::right() const
{
	return CVector4(elements[0], elements[1], elements[2], elements[3]);
}

CVector4 CMatrix::up() const
{
	return CVector4(elements[4], elements[5], elements[6], elements[7]);
}

CVector4 CMatrix::getMatrixTranslation() const
{
	return CVector4(elements[12], elements[13], elements[14], elements[15]);
}

// set the translation part of the matrix
void CMatrix::SetMatrixTranslation(const CVector4& v)
{
	elements[12] = v.v3.x;
	elements[13] = v.v3.y;
	elements[14] = v.v3.z;
	elements[15] = v.w;
}

void CMatrix::SetForward(const CVector4& newForward)
{
	elements[8] = newForward.v3.x; 
	elements[9] = newForward.v3.y; 
	elements[10] = newForward.v3.z;
	elements[11] = newForward.w;
}

void CMatrix::SetRight(const CVector4& newRight)
{
	elements[0] = newRight.v3.x;
	elements[1] = newRight.v3.y; 
	elements[2] = newRight.v3.z;
	elements[3] = newRight.w;
}

void CMatrix::SetUp(const CVector4& newUp)
{
	elements[4] = newUp.v3.x; 
	elements[5] = newUp.v3.y;
	elements[6] = newUp.v3.z;
	elements[7] = newUp.w;
}

CMatrix CMatrix::CreateOrthographicProjection(float left, float right, float bottom, float top, float near, float far)
{
    float a = 2.0f / (right - left);
    float b = 2.0f / (top - bottom);
    float c = -2.0f / (far - near);
    float tx = (right + left) / (right - left);
    float ty = (top + bottom) / (top - bottom);
    float tz = (far + near) / (far - near);

	CMatrix m;
	m.elements[0]=a;		m.elements[1]=0.0f;		m.elements[2]=0.0f;		m.elements[3]=tx;
	m.elements[4]=0.0f;		m.elements[5]=b;		m.elements[6]=0.0f;		m.elements[7]=ty;
	m.elements[8]=0.0f;		m.elements[9]=0.0f;		m.elements[10]=c;		m.elements[11]=tz;
	m.elements[12]=0.0f;	m.elements[13]=0.0f;	m.elements[14]=0.0f;	m.elements[15]=1.0f;
	return m;
}

CMatrix CMatrix::CreatePerspectiveProjection(float fov, float aspectRatio, float nearClipPlane, float farClipPlane)
{
	fov=nearClipPlane*tan(fov*PI/360.0f);
	aspectRatio=fov*aspectRatio;
	return CreatePerspectiveProjection(-aspectRatio, aspectRatio, -fov, fov, nearClipPlane, farClipPlane);
}

CMatrix CMatrix::CreatePerspectiveProjection(float left, float right, float bottom, float top, float near, float far)
{
    float a = 2 * near / (right - left);
    float b = 2 * near / (top - bottom);
    float c = (right + left) / (right - left);
    float d = (top + bottom) / (top - bottom);
    float e = - (far + near) / (far - near);
    float f = -2 * far * near / (far - near);

	CMatrix m;
	m.elements[0]=a;		m.elements[1]=0.0f;		m.elements[2]=0.0f;		m.elements[3]=0.0f;
	m.elements[4]=0.0f;		m.elements[5]=b;		m.elements[6]=0.0f;		m.elements[7]=0.0f;
	m.elements[8]=c;		m.elements[9]=d;		m.elements[10]=e;		m.elements[11]=-1;
	m.elements[12]=0.0f;	m.elements[13]=0.0f;	m.elements[14]=f;		m.elements[15]=1.0f;
	return m;
}

CMatrix CMatrix::LookAt(const CVector& eye, const CVector& target, const CVector& up)
{
	CMatrix rotationMat;
	rotationMat.CreateMatrix(eye-target, up);
	rotationMat = rotationMat.getInverseRotationMatrix();

	CMatrix translationMat;
	translationMat.SetMatrixTranslation(CVector4(-eye, 1.0f));

	return rotationMat * translationMat;
}