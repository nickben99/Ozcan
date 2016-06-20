//CameraPriorityTypes.h - header for the camera class

//system includes------------
//---------------------------

#ifndef _CameraPriorityTypes_h_
#define _CameraPriorityTypes_h_

//header files---------
#include "CamType.h"
//---------------------

//defines--------------
//---------------------

//forward declerations----
//------------------------

class CameraPriorityTypes
{
public:
	CameraPriorityTypes();
	~CameraPriorityTypes();

	void Initialize();

	struct CamTypeAndData
	{
		CamTypeAndData(CamType::eCamIdentifier identifier, int priority)
			: mIdentifier(identifier)
			, mPriority(priority)
		{
		}

		CamTypeAndData()
			: mIdentifier(CamType::eCICircleFollow)
			, mPriority(0)
		{
		}

		bool operator==(const CamTypeAndData& rhs) const
		{
			return mIdentifier == rhs.mIdentifier &&
					mPriority == rhs.mPriority;
		}

		CamType::eCamIdentifier mIdentifier;
		int mPriority;
	};

	bool GetHighestPriority(CamTypeAndData& outTypeAndData);
	void Add(const CamTypeAndData& typeAndData);
	void Remove(const CamTypeAndData& typeAndData);

private:
	void Insert(const int insertionIndex, const CamTypeAndData& typeAndData);
	void Remove(const int indexToRemove);
	int GetHighestPriorityIndex();
	int GetLowestPriorityIndex();

private:
	static const int kMaxPriorities = 10;
	int mNumPriorityTypes;
	CamTypeAndData mPriorities[kMaxPriorities];
}; // end class CameraPriorityTypes

#endif // ifndef _CameraPriorityTypes_h_ 