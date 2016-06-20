//CameraLinks.cpp, the camera links class

// system includes --------
#include <assert.h>
//-------------------------

// header files -----------
#include "CameraLinks.h"
#include "CamType.h"
//-------------------------

CameraLinks::CameraLinks()
	: mNumLinks(0)
{
}

CameraLinks::~CameraLinks()
{
	Reset();
}

void CameraLinks::Reset()
{
	for (int link = 0; link < mNumLinks; ++link)
	{
		if (mLinks[link].mpLerpTemplate)
		{
			delete mLinks[link].mpLerpTemplate;
		}
	}
	mNumLinks = 0;
}

void CameraLinks::AddLink(CamType::eCamIdentifier from, CamType::eCamIdentifier to, CameraLerp* pLerp)
{
	assert(mNumLinks < kMaxLinks);
	mLinks[mNumLinks] = CameraLink(from, to, pLerp);
	++mNumLinks;
}

CameraLerp* CameraLinks::CreateNewLerpFromTemplate(const CamType::eCamIdentifier from, const CamType::eCamIdentifier to)
{
	for (int link = 0; link < mNumLinks; ++link)
	{
		if (from == mLinks[link].mFrom && to == mLinks[link].mTo)
		{
			return mLinks[link].mpLerpTemplate->CreateCopy();
		}
		// anything into this cam
		if (CamType::eCIMax == mLinks[link].mFrom && to == mLinks[link].mTo)
		{
			return mLinks[link].mpLerpTemplate->CreateCopy();
		}

		// anything from this cam
		if (from == mLinks[link].mFrom && CamType::eCIMax == mLinks[link].mTo)
		{
			return mLinks[link].mpLerpTemplate->CreateCopy();
		}
	}
	return new CameraLerp();
}
