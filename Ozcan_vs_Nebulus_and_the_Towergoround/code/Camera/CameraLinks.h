//CameraLinks.h - header for the camera links class

//system includes------------
//---------------------------

#ifndef _CameraLinks_h_
#define _CameraLinks_h_

//header files---------
#include "CamType.h"
#include "CameraLerp.h"
//---------------------

//defines--------------
//---------------------

//forward declerations----
//------------------------

class CameraLinks
{
public:
	CameraLinks();
	~CameraLinks();

	void Reset();
	void AddLink(CamType::eCamIdentifier from, CamType::eCamIdentifier to, CameraLerp* pLerp);
	CameraLerp* CreateNewLerpFromTemplate(const CamType::eCamIdentifier from, const CamType::eCamIdentifier to);

private:
	struct CameraLink
	{
		CameraLink(CamType::eCamIdentifier from, CamType::eCamIdentifier to, CameraLerp* pLerp)
			: mFrom(from)
			, mTo(to)
			, mpLerpTemplate(pLerp)
		{
		}

		CameraLink()
			: mFrom(CamType::eCICircleFollow)
			, mTo(CamType::eCICircleFollow)			
		{
		}

		CamType::eCamIdentifier mFrom;
		CamType::eCamIdentifier mTo;
		CameraLerp* mpLerpTemplate;
	};

private:
	static const int kMaxLinks = 10;
	int mNumLinks;
	CameraLink mLinks[kMaxLinks];
}; // end class CameraLinks

#endif // ifndef _CameraLinks_h_ 