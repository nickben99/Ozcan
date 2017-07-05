/*-----------------------------------------
CLASS BASED ON MATRIX3*3.H/CPP
FROM	Bourg, D.M. (2002).  Physics for Game Developers.  1st ed.,  O'Reilly.
-----------------------------------------*/
 
//CMatrix.h - header for the 4*4 matrix class

#ifndef _CMatrix_h_
#define _CMatrix_h_

//header files-------------------
#include "Math/CVector.h"
#include "Math/CQuaternion.h"
//--------------------------------

struct Rows
{
	float x[4];
	float y[4];
	float z[4];
	float position[4];
};

class CMatrix
{
	private:
//-----private variables-----------
//---------------------------------

	public:
//----public variables--------------
		union
		{
			float elements[16];
			Rows rows;
		};
//----------------------------------

//----public methods----------------------------------------
		CMatrix( void); // Constructor
		CMatrix(float * in); // overloaded constructor
    
        CMatrix(float in0, float in1, float in2, float in3,
                float in4, float in5, float in6, float in7,
                float in8, float in9, float in10, float in11,
                float in12, float in13, float in14, float in15);
    
		CMatrix(const CQuaternion& quat); // matrix from quat
		CMatrix(const CVector& forward);
		CMatrix(const CVector4& right, const CVector4& up, const CVector4& forward, const CVector4& translation);
		~CMatrix(); // destructor
		void identity(); // set to identity matrix
		void createXYZRotationMatrix(CVector *v); //create x, y, z rotation matrix
		void createXRotationMatrix(float angle); // create x rotation matrix
		void createYRotationMatrix(float angle); // create y rotation matrix
		void createZRotationMatrix(float angle); // create z rotation matrix						
		CVector multiplyRotateVector(CVector *vect); /*	produces a new row vector by 
														multiplying this matrix by the 
														row vector paramter. (only the 
														matrix rotational portion is 
														considered)*/
		CVector multiplyRotateVector(const CVector& vect);
		/*	produces a new row vector by multiplying this matrix by 
			the row vector paramter as if the paramter was a column
			vector.  Therefore producing the inverse vector. (only the matrix
			rotational portion is considered) */
		CVector multiplyRotateVectorInverse(CVector *vect); 
		/*	multiplies the matrix by the vector parameter to form a new rotated and 
			translated vector */
		CVector multiplyVector(CVector *vect);
		CVector matrixInverseTranslation(CVector *vect); /*	translates by the inverse of this
															matrix */
		// turns the quaternion paramater in to its matrix representation 
		void quaternionToMatrix(const CQuaternion& quat);
		CQuaternion matrixToQuaternion(); // create quaternion from matrix

		// multiply vector by the inverse of this matrix
		CVector multiplyVectorInverseMatrix(CVector *vect);
		// create scaling matrix
		void creatScalingMatrix(float x, float y, float z);
		// get inverse of rotational part of matrix
		CMatrix getInverseRotationMatrix();
		// get inverse of scaling matrix
		CMatrix getInverseScalingMatrix();
		// create a rotation matrix around an arbitrary axis
		void createArbitraryAxisRotation(float angleRad, const CVector& rotationVect);
		/*	create billboard matrix (creates the matrix required to position and 
			rotate a billboard so its facing the camera */
		void createBillboardMatrix(CVector *right, CVector *up, 
									CVector *look, CVector *position);
		bool CreateMatrix(const CVector& forward, const CVector& up = CVector::unitY);
		CMatrix Transposed() const;
		
		CVector4 right() const;
		CVector4 up() const;
		CVector4 forward() const;
		CVector4 getMatrixTranslation() const; // get the translation part of the matrix
		
		void SetRight(const CVector4& newRight);
		void SetUp(const CVector4& newUp);
		void SetForward(const CVector4& newForward);
		void SetMatrixTranslation(const CVector4& v); // set the matrix translation

		static CMatrix CreateOrthographicProjection(float left, float right, float bottom, float top, float near, float far);
		static CMatrix CreatePerspectiveProjection(float fov, float aspectRatio, float nearClipPlane, float farClipPlane);
		static CMatrix CreatePerspectiveProjection(float left, float right, float bottom, float top, float near, float far);
		static CMatrix LookAt(const CVector& eye, const CVector& target, const CVector& up);

		// overloaded operators
		CMatrix& operator*=(const CMatrix *m); // multiply equals
		CMatrix& operator*=(const CMatrix& m); // multiply equals
		CMatrix& operator=(const CMatrix *m); // equals
		CMatrix operator*(const CMatrix *m); // multiply
		CMatrix operator*(const CMatrix& m); // multiply
		CVector4 operator*(const CVector4& vect); // multiply
		bool operator==(const CMatrix& m) const;
		bool operator!=(const CMatrix& m) const;
        float& operator[] (unsigned int n);
//-----------------------------------------------------------
}; // end class CMatrix

#endif // ifndef _CMatrix_h_ 