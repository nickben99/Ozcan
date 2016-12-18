// ReplayManager.cpp

//--- System includes --------------
#include <algorithm>
#include <assert.h>
//----------------------------------

//--- Header files ---------
#include "ReplayManager.h"
#include "Random.h"
#include "CLevel.h"
#include "Game/Globals.h"
#include "Game/defines.h"
//--------------------------

//--- external variables --------
//-------------------------------

const int kEndReplayStartFrameNotSet = -1;
const int kEndRecordingTime = 10; // seconds
const float kBufferTimeBeforeStartFrame = 0.75f; // seconds
const float kOptimalEndReplayLength = 3.0f;

ReplayManager::ReplayManager()
#ifdef DETERMINISTIC_REPLAY_DEBUG
	: debugTrackingReadbackCount(0)
	, mIsReplaying(true)
#else
	: mIsReplaying(true)
#endif
	, mLevelSeed(0)
	, mReplayStyle(eRSEnd)
	, mEndReplayStartFrame(kEndReplayStartFrameNotSet)
	, mRecorderStartBufferIndex(-1)
	, mRecorderEndBufferIndex(-1)
	, mOldestEndReplayFrame(0)
	, mReplayBufferIndex(-1)
	, mbHaveRecordingFramesBeenSkipped(false)
	, mIsRepeating(false)
{
	assert(kEndRecordingTime > kOptimalEndReplayLength && kOptimalEndReplayLength > kBufferTimeBeforeStartFrame);
}

ReplayManager::~ReplayManager() //destructor
{
}

int ReplayManager::GetNumFramesForEndReplay()
{
	float fps = 1.0f/GameTime::Instance().GetFixedTimeDeltaSeconds();
	return (int)(kEndRecordingTime * fps);
}

bool DoBreak(bool val)
{
	return !val;
}

void ReplayManager::IncrementFrame()
{
	if (IsRecording())
	{
		frameInput.push_back(FrameResults());
	}
	else if (IsReplayingWholeLevel()) // if we're not recording we're replaying
	{
#ifdef DETERMINISTIC_REPLAY_DEBUG
		int replayCounter = Globals::Instance().currentLevel.GetFrameNumber() - 1;
		unsigned int intsReadbackCount = (replayCounter > -1) ? frameInput[replayCounter].intsReadbackCount : 0;
		unsigned int intsTotal = (replayCounter > -1) ? frameInput[replayCounter].intResults.size() : 0;
		if ( replayCounter > -1 && intsReadbackCount != intsTotal)
		{
			DoBreak(true);
		}

		unsigned int unsignedIntsReadbackCount = (replayCounter > -1) ? frameInput[replayCounter].unsignedIntsReadbackCount : 0;
		unsigned int unsignedIntsTotal = (replayCounter > -1) ? frameInput[replayCounter].unsignedIntResults.size() : 0;
		if ( replayCounter > -1 && unsignedIntsReadbackCount != unsignedIntsTotal)
		{
			DoBreak(true);
		}

		unsigned int vectorsReadbackCount = (replayCounter > -1) ? frameInput[replayCounter].vectorsReadbackCount : 0;
		unsigned int vectorsTotal = (replayCounter > -1) ? frameInput[replayCounter].vectors.size() : 0;
		if ( replayCounter > -1 && vectorsReadbackCount != vectorsTotal)
		{
			DoBreak(true);
		}

		unsigned int floatsReadbackCount = (replayCounter > -1) ? frameInput[replayCounter].floatsReadbackCount : 0;
		unsigned int floatsTotal = (replayCounter > -1) ? frameInput[replayCounter].floats.size() : 0;
		if ( replayCounter > -1 && floatsReadbackCount != floatsTotal)
		{
			DoBreak(true);
		}

		unsigned int matrixesReadbackCount = (replayCounter > -1) ? frameInput[replayCounter].matrixesReadbackCount : 0;
		unsigned int matrixesTotal = (replayCounter > -1) ? frameInput[replayCounter].matrixes.size() : 0;
		if ( replayCounter > -1 && matrixesReadbackCount != matrixesTotal)
		{
			DoBreak(true);
		}
#endif		
	}
}

int ReplayManager::CalculateTotalNumberOfRecordedFrames()
{
	if (mRecorderEndBufferIndex >= mRecorderStartBufferIndex)
	{
		return (mRecorderEndBufferIndex - mRecorderStartBufferIndex) + 1; 
	}
	return (GetNumFramesForEndReplay() - mRecorderStartBufferIndex) + mRecorderEndBufferIndex + 1;
}

void ReplayManager::StartRecording()
{
	mLevelSeed = (unsigned)Globals::Instance().timer.time();
	frameInput.clear(); // prepare for recording

#ifdef DETERMINISTIC_REPLAY_DEBUG
	debugInfo.clear();
#endif

	mEndReplayStartFrame = kEndReplayStartFrameNotSet;

	mRecorderStartBufferIndex = -1;
	mRecorderEndBufferIndex = -1;
	mReplayBufferIndex = -1;
	mbHaveRecordingFramesBeenSkipped = false;

	mOldestEndReplayFrame = 0;

	Random::GetInst().Restart(mLevelSeed);
	mIsReplaying = false;
}

void ReplayManager::StartReplaying(eReplayStyle style)
{
	float fps = 1.0f/GameTime::Instance().GetFixedTimeDeltaSeconds();
		
	int startFrame = mEndReplayStartFrame - (int)(fps * kBufferTimeBeforeStartFrame);
    startFrame = defines::Max(startFrame, mOldestEndReplayFrame);
		
	int endFrame = startFrame + (int)(kOptimalEndReplayLength * fps);
	endFrame = defines::Min(endFrame, mOldestEndReplayFrame + CalculateTotalNumberOfRecordedFrames() - 1);
		
	mReplayBufferIndex = (mRecorderStartBufferIndex+(startFrame - mOldestEndReplayFrame))%GetNumFramesForEndReplay();
	mRecorderEndBufferIndex = (mRecorderStartBufferIndex+(endFrame - mOldestEndReplayFrame))%GetNumFramesForEndReplay(); 

	Random::GetInst().Restart(mLevelSeed);
	mIsReplaying = true;
	mReplayStyle = style;

#ifdef DETERMINISTIC_REPLAY_DEBUG
	ZeroReplayTrackingCounts();
	debugTrackingReadbackCount = 0;
#endif	
}

#ifdef DETERMINISTIC_REPLAY_DEBUG
void ReplayManager::ZeroReplayTrackingCounts()
{
	for (unsigned int frame = 0; frame < frameInput.size(); ++frame)
	{
		frameInput[frame].ZeroReplayTrackingCounts();
	}
}
#endif	

bool ReplayManager::IsRecording()
{
	return !mIsReplaying;
}

bool ReplayManager::IsReplaying()
{
	return mIsReplaying;
}

bool ReplayManager::IsReplayingWholeLevel()
{
	return IsReplaying() && eRSWholeLevel == mReplayStyle;
}

bool ReplayManager::IsReplayingEndLevel()
{
	return IsReplaying() && eRSEnd == mReplayStyle;
}

void ReplayManager::SetEndReplayStartFrame()
{
	if (kEndReplayStartFrameNotSet == mEndReplayStartFrame)
	{
		mEndReplayStartFrame = Globals::Instance().currentLevel.GetFrameNumber(); // this is the frame we start the end replay on
	}
}

void ReplayManager::ClearEndReplayStartFrame()
{
	mEndReplayStartFrame = kEndReplayStartFrameNotSet;
}

bool ReplayManager::IsRecordingEndFrames()
{
	if (kEndReplayStartFrameNotSet == mEndReplayStartFrame) // we don't have a start frame, so we're continuously recording
	{
		return true;
	}
	else
	{
		float currentReplayLength = (Globals::Instance().currentLevel.GetFrameNumber() - mEndReplayStartFrame) * GameTime::Instance().GetFixedTimeDeltaSeconds();

		if ((kEndRecordingTime-currentReplayLength) <= kBufferTimeBeforeStartFrame)
		{
			return false; // stop recording otherwise we'll overwrite our replay frames
		}
		// we still have nough of a buffer, we can keep recording		
		return true;		
	}
}

bool ReplayManager::HasEndReplayFinished()
{
	if (IsReplayingEndLevel() && mReplayBufferIndex == mRecorderEndBufferIndex)
	{
		return true;
	}
	return false;
}

#ifdef _DEBUG
bool ReplayManager::IsFinalFrameOfWholeLevelReplay()
{
	if (IsReplayingWholeLevel() && (Globals::Instance().currentLevel.GetFrameNumber() + 1) == (int)frameInput.size())
	{
		return true;
	}
	return false;
}
#endif

void ReplayManager::PreSpriteUpdate()
{
	if (ReplayManager::instance()->IsRecording())
	{
		bool doIncrement = true;
		if (0 == Globals::Instance().currentLevel.GetFrameNumber())
		{
			mRecorderStartBufferIndex = mRecorderEndBufferIndex = 0;
			mOldestEndReplayFrame = 0;
			doIncrement = false;
		}
			
		if (!IsRecordingEndFrames())
		{
			mbHaveRecordingFramesBeenSkipped = true;
			return; // we already have our end sequence recorded
		}

		if (mbHaveRecordingFramesBeenSkipped) // at least one recording frame has been skipped, so we must restart
		{
			mbHaveRecordingFramesBeenSkipped = false;
			mRecorderStartBufferIndex = mRecorderEndBufferIndex = 0;
			mOldestEndReplayFrame = Globals::Instance().currentLevel.GetFrameNumber();
			doIncrement = false;
		}
			
		if (doIncrement)
		{
			mRecorderEndBufferIndex = (++mRecorderEndBufferIndex)%GetNumFramesForEndReplay();
			if (mRecorderStartBufferIndex == mRecorderEndBufferIndex)
			{
				mRecorderStartBufferIndex = (++mRecorderStartBufferIndex)%GetNumFramesForEndReplay();
				++mOldestEndReplayFrame;
			}
		}
	}
}

#ifdef _DEBUG
void ReplayManager::SetWholeLevelReplayRepeating(bool repeat)
{
	mIsRepeating = repeat;
}

bool ReplayManager::GetWholeLevelReplayRepeating()
{
	return mIsRepeating;
}
#endif

void ReplayManager::PostSpriteUpdate()
{
	if (ReplayManager::instance()->IsReplayingEndLevel()) // replaying end level, we don't need to do anything for replaing the whole level
	{
		mReplayBufferIndex = (++mReplayBufferIndex)%GetNumFramesForEndReplay();
	}
}

void ReplayManager::AddPressedQuery(eKeyCode key, bool val)
{
	frameInput[frameInput.size()-1].pressed[key] = val;
}

bool ReplayManager::GetPressedQuery(eKeyCode key)
{
	if (IsRecording() || IsReplayingWholeLevel())
	{
		return frameInput[Globals::Instance().currentLevel.GetFrameNumber()].pressed[key];
	}
	return false;
}

void ReplayManager::AddStatusQuery(eKeyCode key, bool val)
{
	frameInput[Globals::Instance().currentLevel.GetFrameNumber()].status[key] = val;
}

bool ReplayManager::GetStatusQuery(eKeyCode key)
{
	if (IsRecording() || IsReplayingWholeLevel())
	{
		return frameInput[Globals::Instance().currentLevel.GetFrameNumber()].status[key];
	}
	return false; // we're replaying the end of the level, positions are stored, no need for saved key input
}

#ifdef DETERMINISTIC_REPLAY_DEBUG

void ReplayManager::IntResult(int randResult)
{
	int frameNumber = Globals::Instance().currentLevel.GetFrameNumber();
	CheckResult(randResult, frameInput[frameInput.size()-1].intResults, frameInput[frameNumber].intResults,
		frameInput[frameNumber].intsReadbackCount); 
}

void ReplayManager::UnsignedIntResult(unsigned int randResult)
{
	int frameNumber = Globals::Instance().currentLevel.GetFrameNumber();
	CheckResult(randResult, frameInput[frameInput.size()-1].unsignedIntResults, frameInput[frameNumber].unsignedIntResults, 
				frameInput[frameNumber].unsignedIntsReadbackCount); 
}

void ReplayManager::VectorResult(const CVector& pos)
{
	int frameNumber = Globals::Instance().currentLevel.GetFrameNumber();
	CheckResult(pos, frameInput[frameInput.size()-1].vectors, frameInput[frameNumber].vectors, 
				frameInput[frameNumber].vectorsReadbackCount); 
}

void ReplayManager::FloatResult(float res)
{
	int frameNumber = Globals::Instance().currentLevel.GetFrameNumber();
	CheckResult(res, frameInput[frameInput.size()-1].floats, frameInput[frameNumber].floats, 
				frameInput[frameNumber].floatsReadbackCount);
}

void ReplayManager::MatrixResult(const CMatrix& res)
{
	int frameNumber = Globals::Instance().currentLevel.GetFrameNumber();
	CheckResult(res, frameInput[frameInput.size()-1].matrixes, frameInput[frameNumber].matrixes, 
				frameInput[frameNumber].matrixesReadbackCount); 
}

void ReplayManager::QuaternionResult(const CQuaternion& res)
{
	int frameNumber = Globals::Instance().currentLevel.GetFrameNumber();
	CheckResult(res, frameInput[frameInput.size()-1].quaternions, frameInput[frameNumber].quaternions, 
				frameInput[frameNumber].quaternionsReadbackCount); 
}

#endif