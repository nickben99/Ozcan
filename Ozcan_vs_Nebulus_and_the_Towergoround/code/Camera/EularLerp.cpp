// EularLerp.cpp

#include <assert.h>

// header files -----------
#include "EularLerp.h"
#include "Math/CMatrix.h"
//-------------------------

// extern ----------------
// -----------------------

EularLerp::EularLerp(const CQuaternion& quat)
{
	CMatrix transform(quat);
	CVector forward = transform.forward();
	CVector forwardGroundPlane = forward;
	if (CVector::SafeNormalizeXZ(forwardGroundPlane))
	{
		yaw = safeACos(forwardGroundPlane.z) * (forwardGroundPlane.x >= 0.0f ? 1.0f : -1.0f);
		pitch = safeACos(forwardGroundPlane.dotProduct(forward)) * (forward.y >= 0.0f ? 1.0f : -1.0f);

		if (yaw < -PI || yaw > PI)
		{
			assert(false);
		}
	}
	else
	{
		assert(false);
	}
}

EularLerp::EularLerp()
	: pitch(0.0f)
	, yaw(0.0f)
{
}

CQuaternion EularLerp::CalculateOrientation()
{
	if (yaw < -PI || yaw > PI)
	{
		assert(false);
	}

	CVector forward((float)sin(yaw), 0.0f, (float)cos(yaw));

	if (forward.IsNaN())
	{
		assert(false);
	}

	forward *= (float)cos(pitch);
	forward.y = (float)sin(pitch);

	CMatrix transform(forward);
	return transform.matrixToQuaternion();
}

CQuaternion EularLerp::Lerp(const CQuaternion& from, const CQuaternion& to, float interp)
{
	EularLerp fromEular(from);
	EularLerp toEular(to);

	EularLerp result = Lerp(fromEular, toEular, interp);
	return result.CalculateOrientation();
}

EularLerp EularLerp::Lerp(const EularLerp& from, const EularLerp& to, float interp)
{
	EularLerp diff = to-from;
	EularLerp diffScaled = diff * interp;
	EularLerp result = from + diffScaled;
	return result;
}

float EularLerp::CalculateShortestYawDistance(float from, float to)
{
	float diff = to-from;
	if (diff > PI)
	{
		diff = -PI + (diff - PI);
	}
	else if (diff < -PI)
	{
		diff = PI - (-PI-diff);
	}

	if (diff < -PI || diff > PI)
	{
		assert(false);
	}

	return diff;
}

float EularLerp::WrapYaw(float inputYaw)
{
	if (inputYaw > PI)
	{
		float diff = -PI + (inputYaw-PI);

		if (diff < -PI || diff > PI)
		{
			assert(false);
		}
		return diff;
	}
	else if (inputYaw < -PI)
	{
		float diff = PI - (-PI - inputYaw);

		if (diff < -PI || diff > PI)
		{
			assert(false);
		}
		return diff;
	}

	if (inputYaw < -PI || inputYaw > PI)
	{
		assert(false);
	}

	return inputYaw;
}

EularLerp EularLerp::operator*(float s) const
{
	EularLerp result = *this;
	result.pitch *= s;
	result.yaw *= s;
	result.yaw = WrapYaw(result.yaw);
	return result;
}

EularLerp EularLerp::operator+(const EularLerp& rhs) const
{
	EularLerp result;
	result.pitch = pitch + rhs.pitch;
	result.yaw = yaw + rhs.yaw;
	result.yaw = WrapYaw(result.yaw);
	return result;
}

EularLerp EularLerp::operator-(const EularLerp& rhs) const
{
	EularLerp result;
	result.pitch = pitch - rhs.pitch;
	result.yaw = CalculateShortestYawDistance(rhs.yaw, yaw);

	if (result.yaw < -PI || result.yaw > PI)
	{
		assert(false);
	}

	return result;
}
