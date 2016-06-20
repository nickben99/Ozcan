//CameraPriorityTypes.cpp, the camera class

// system includes --------
#include <assert.h>
//-------------------------

// header files -----------
#include "CameraPriorityTypes.h"
//-------------------------

CameraPriorityTypes::CameraPriorityTypes()
	: mNumPriorityTypes(0)
{
	for (int index = 0; index < kMaxPriorities; ++index)
	{
		mPriorities[index].mIdentifier = CamType::eCIMax;
	}
}

CameraPriorityTypes::~CameraPriorityTypes()
{
}

void CameraPriorityTypes::Initialize()
{
	mNumPriorityTypes = 0;
}

bool CameraPriorityTypes::GetHighestPriority(CamTypeAndData& outTypeAndData)
{
	if (GetHighestPriorityIndex() >= 0)
	{
		outTypeAndData = mPriorities[GetHighestPriorityIndex()];
		return true;
	}
	return false;
}

void CameraPriorityTypes::Add(const CamTypeAndData& typeAndData)
{
	assert(mNumPriorityTypes < kMaxPriorities);
	int insertionIndex = 0;
	for(int index = GetHighestPriorityIndex(); index >= GetLowestPriorityIndex(); --index)
	{
		if (typeAndData.mPriority >= mPriorities[index].mPriority)
		{
			insertionIndex = index + 1; // insert in front of lower priority cam type
			break;
		}
	}
	Insert(insertionIndex, typeAndData);
}

void CameraPriorityTypes::Remove(const CamTypeAndData& typeAndData)
{
	for(int index = 0; index < mNumPriorityTypes; ++index)
	{
		if (typeAndData == mPriorities[index])
		{
			Remove(index);
			break;
		}
	}
}

int CameraPriorityTypes::GetHighestPriorityIndex()
{
	return mNumPriorityTypes-1; 
}

int CameraPriorityTypes::GetLowestPriorityIndex()
{
	return 0;
}

void CameraPriorityTypes::Insert(const int insertionIndex, const CamTypeAndData& typeAndData)
{
	assert(mNumPriorityTypes < kMaxPriorities);	
	assert(insertionIndex >= GetLowestPriorityIndex() && insertionIndex <= GetHighestPriorityIndex() + 1);
	for (int index = GetHighestPriorityIndex() + 1; index > insertionIndex; --index)
	{
		mPriorities[index] = mPriorities[index - 1];
	}			
	mPriorities[insertionIndex] = typeAndData;
	++mNumPriorityTypes;
}

void CameraPriorityTypes::Remove(const int indexToRemove)
{
	if (indexToRemove >= GetLowestPriorityIndex() && indexToRemove <= GetHighestPriorityIndex())
	{
		for (int index = indexToRemove; index < GetHighestPriorityIndex(); ++index)
		{
			mPriorities[index] = mPriorities[index + 1];
		}
		--mNumPriorityTypes;
	}
}