// EularLerp.h

//system includes------------
//---------------------------

#ifndef _EularLerp_h_
#define _EularLerp_h_

//header files---------
#include "Math/CQuaternion.h"
//---------------------

//forward declerations----
//------------------------

class EularLerp
{
public:
	EularLerp();
	EularLerp(const CQuaternion& quat);

	static CQuaternion Lerp(const CQuaternion& from, const CQuaternion& to, float interp);
	static EularLerp Lerp(const EularLerp& from, const EularLerp& to, float interp);	
	CQuaternion CalculateOrientation();
	EularLerp operator-(const EularLerp& rhs) const; // this EularLerp minus another EularLerp

	void SetYaw(float inputYaw)
	{
		this->yaw = inputYaw;
	}

	void SetPitch(float inputPitch)
	{
		this->pitch = inputPitch;
	}

	float GetYaw()
	{
		return yaw;
	}

	float GetPitch()
	{
		return pitch;
	}

private:	
	static float CalculateShortestYawDistance(float from, float to);
	static float WrapYaw(float inpuYaw);

	EularLerp operator*(float s) const; // multiply by scaler
	EularLerp operator+(const EularLerp& rhs) const; // addition operator

private:
	float pitch;
	float yaw;
}; // end class EularLerp

#endif // ifndef _EularLerp_h_ 