//CBox.h, header file for CPlane class

//system includes---
//------------------

#ifndef _CBox_h_
#define _CBox_h_

// header files---------
#include "Math/CVector.h"
#include "Game/defines.h"
//----------------------

//defines---------------
//----------------------

class CBox              
{
public:
	CBox(CVector positionIn, CVector halfWidthsIn)
		: position(positionIn)
		, halfWidths(halfWidthsIn)
	{
	}

	float CalculateMaxProjectionOnToLine(CVector linePos, CVector lineDir)
	{
		CVector verts[8];
		CalculateBoxVerts(verts);

		float maxProjection = 0.0f;
		for (int vert = 0; vert < 8; ++vert)
		{
			CVector toVert = verts[vert] - linePos;
			maxProjection = defines::Max(maxProjection, fabs(toVert.dotProduct(lineDir)));
		}
		return maxProjection;
	}

private:
	void CalculateBoxVerts(CVector* verts)
	{
		verts[0] = position + CVector(halfWidths.x, halfWidths.y, halfWidths.z);
		verts[1] = position + CVector(halfWidths.x, -halfWidths.y, halfWidths.z);
		verts[2] = position + CVector(-halfWidths.x, halfWidths.y, halfWidths.z);
		verts[3] = position + CVector(-halfWidths.x, -halfWidths.y, halfWidths.z);

		verts[4] = position + CVector(halfWidths.x, halfWidths.y, -halfWidths.z);
		verts[5] = position + CVector(halfWidths.x, -halfWidths.y, -halfWidths.z);
		verts[6] = position + CVector(-halfWidths.x, halfWidths.y, -halfWidths.z);
		verts[7] = position + CVector(-halfWidths.x, -halfWidths.y, -halfWidths.z);
	}
	
private:
	CVector position; 
	CVector halfWidths;
};

#endif