/*-----------------------------------------
CLASS BASED ON QUATERNION.H/CPP
FROM	Bourg, D.M. (2002).  Physics for Game Developers.  1st ed.,  O'Reilly.
-----------------------------------------*/
 
//CQuaternion.h - header for the quaternion class

#ifndef _CQuaternion_h_
#define _CQuaternion_h_

//header files---------
#include "Math/CVector.h" // vector object
//---------------------

//defines--------------
//---------------------

class CQuaternion
{
	private:
//-----private variables-----------
//---------------------------------

	public:
//----public variables--------------
		float n; // quaternions scalar part (angle)
		CVector v; // quaternion vector part
//----------------------------------

//----public methods----------------------------------------
		CQuaternion( void); // Constructor
		// Constructor, create quaternion from angle and vector
		CQuaternion(float newN, const CVector& newV);
		// constructor
		CQuaternion(float x, float y, float z, float n);
		// copy constructor
		CQuaternion(const CQuaternion& aQuaternion);
		~CQuaternion(); // destructor
		
		void identity(); // set to identity
		// get angle of rotation
		float getRotationAngle();
		// get axis of rotation
		CVector getRotationAxis();
		// get axis and angle of rotation
		void getRotationAxisAndAngle(float *angle, CVector * axis);
		// create quaternion from euler angles
		void makeQuaternionFromEularAngles(CVector * vect);
		// make quaternion from forward vector
		void makeQuaternionFromForwardVector(const CVector& forward);		
		// converts quaternion to euler angles
		CVector eularAngleQuaternionRepresentation();

		//	normalise quaternion 
		void normalise();
		//	conjugate quaternion 
		CQuaternion getConjugate();
		//	get the magnitude of a quaternion 
		float getMagnitude();
		// dot product
		float dotProduct( CQuaternion *q);
		void negateQuaternion(); /*	make all quaternion elements the negative of their 
									current value */

		/*	spherically interpolate from a quaternion representing a rotation
			to another quaternion representing another rotation */
		void quaternionSlerp(CQuaternion * from, CQuaternion * to, float d);

		// rotate vector
		CVector rotateVector( CVector * vect );

		// overloaded operators
		CQuaternion& operator=(const CQuaternion& q); // make quaternion equal to another quaternion
		CQuaternion& operator*=(const CQuaternion& q); // multiply and make equal to
		CQuaternion operator*(const CQuaternion& q) const; // quaternion multiplication
		bool operator==(const CQuaternion& m) const;
		bool operator!=(const CQuaternion& m) const;

private:
	//	makes a quaternion from an angle and axis 
	void makeQuaternionFromAngleAndAxis(float angle, const CVector& axis);
//-----------------------------------------------------------
}; // end class CQuaternion

#endif // ifndef _CQuaternion_h_ 


