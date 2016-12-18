// ReplayManager.h

#ifndef _ReplayManager_H_
#define _ReplayManager_H_

//--- System Includes -----------
#include <vector>
#include <map>
#ifdef DETERMINISTIC_REPLAY_DEBUG
#include <typeinfo.h>
#endif
//-------------------------------

//--- Header files --------------
#include "Math/CVector.h"
#include "Math/CMatrix.h"
#include "Input/CKeyCode.h"
//-------------------------------

//--- defines -------------------
//-------------------------------

class ReplayManager
{
	public:
		enum eReplayStyle
		{
			eRSWholeLevel = 0,
			eRSEnd, 
			eRSMax
		};

	private:
		ReplayManager();

		int CalculateTotalNumberOfRecordedFrames();

		class FrameResults
		{
			public:
				FrameResults()
#ifdef DETERMINISTIC_REPLAY_DEBUG
					: intsReadbackCount(0)
					, unsignedIntsReadbackCount(0)
					, vectorsReadbackCount(0)
					, floatsReadbackCount(0)
					, matrixesReadbackCount(0)
					, quaternionsReadbackCount(0)
#endif
				{
				}

				std::map<eKeyCode, bool> status;
				std::map<eKeyCode, bool> pressed;

#ifdef DETERMINISTIC_REPLAY_DEBUG
				void ZeroReplayTrackingCounts()
				{
					intsReadbackCount = 0;
					unsignedIntsReadbackCount = 0;
					vectorsReadbackCount = 0;
					floatsReadbackCount = 0;
					matrixesReadbackCount = 0;
					quaternionsReadbackCount = 0;
				}

				std::vector<int> intResults;
				unsigned int intsReadbackCount;

				std::vector<unsigned int> unsignedIntResults;
				unsigned int unsignedIntsReadbackCount;

				std::vector<CVector> vectors;
				unsigned int vectorsReadbackCount;

				std::vector<float> floats;
				unsigned int floatsReadbackCount;

				std::vector<CMatrix> matrixes;
				unsigned int matrixesReadbackCount;

				std::vector<CQuaternion> quaternions;
				unsigned int quaternionsReadbackCount;
#endif
		};
		std::vector<FrameResults> frameInput;
#ifdef DETERMINISTIC_REPLAY_DEBUG
		struct DebugTrackingInfo
		{
			const type_info& info;
			int frame;
			int typeIndexForFrame;
			int totalIndexForFrame;

			DebugTrackingInfo(const type_info& InInfo, int InFrame, int InTypeIndexForFrame, int InTotalIndexForFrame)
				: info(InInfo)
				, frame(InFrame)
				, typeIndexForFrame(InTypeIndexForFrame)
				, totalIndexForFrame(InTotalIndexForFrame)
			{
			}

			DebugTrackingInfo(const DebugTrackingInfo& rhs)
				: info(rhs.info)
				, frame(rhs.frame)
				, typeIndexForFrame(rhs.typeIndexForFrame)
				, totalIndexForFrame(rhs.totalIndexForFrame)
			{
			}

			DebugTrackingInfo& operator=(const DebugTrackingInfo& rhs)
			{
				UNUSED(rhs);
				return *this;
			}

			bool operator==(const DebugTrackingInfo& rhs) const
			{
				return info == rhs.info && 
					frame == rhs.frame && 
					typeIndexForFrame == rhs.typeIndexForFrame &&
					totalIndexForFrame == rhs.totalIndexForFrame;
			}

			bool operator!=(const DebugTrackingInfo& rhs) const
			{
				return !(info == rhs.info);
			}
		};
		std::vector<DebugTrackingInfo> debugInfo;
		unsigned int debugTrackingReadbackCount;
#endif

		bool mIsReplaying;
		unsigned mLevelSeed;
		eReplayStyle mReplayStyle;
		int mEndReplayStartFrame;

		int mRecorderStartBufferIndex;
		int mRecorderEndBufferIndex;
		int mOldestEndReplayFrame;
		int mReplayBufferIndex;
		bool mbHaveRecordingFramesBeenSkipped;
		bool mIsRepeating;

	public:		
		~ReplayManager();

		// creates singleton ReplayManager instance
		static ReplayManager* instance() 
		{
			static ReplayManager inst; // create static instance once
			return &inst; // return the address of the instance
		}
		void IncrementFrame();

		void StartRecording();
		void StartReplaying(eReplayStyle style);
		bool IsRecording();
		bool IsReplaying();
		bool IsReplayingWholeLevel();
		bool IsReplayingEndLevel();

		void SetEndReplayStartFrame();
		void ClearEndReplayStartFrame();

#ifdef _DEBUG
		void SetWholeLevelReplayRepeating(bool repeat);
		bool GetWholeLevelReplayRepeating();
		bool IsFinalFrameOfWholeLevelReplay();
#endif

		void AddPressedQuery(eKeyCode DIKey, bool val);
		bool GetPressedQuery(eKeyCode DIKey);

		void AddStatusQuery(eKeyCode DIKey, bool val);
		bool GetStatusQuery(eKeyCode DIKey);		

		int GetNumFramesForEndReplay(); // how many frames the end replay would be

		bool IsRecordingEndFrames();
		bool HasEndReplayFinished();

		void PreSpriteUpdate();
		void PostSpriteUpdate();

		inline int GetRecordingBufferIndex(){ return mRecorderEndBufferIndex;};
		inline int GetEndReplayBufferIndex(){ return mReplayBufferIndex;};
	
#ifdef DETERMINISTIC_REPLAY_DEBUG
		// debugging
		void IntResult(int result);
		void UnsignedIntResult(unsigned int result);
		void VectorResult(const CVector& result);
		void FloatResult(float result);
		void MatrixResult(const CMatrix& result);
		void QuaternionResult(const CQuaternion& res);
		void ZeroReplayTrackingCounts();

		template< class T > void CheckResult(T result, std::vector<T>& theRecordingVector, 
											std::vector<T>& theReplayingVector, unsigned int& readbackCount) 
		{
			if (Globals::Instance().currentLevel.mIsLoading)
			{
				return;
			}

			if (IsRecording())
			{
				theRecordingVector.push_back(result);

				int indexForFrame = 0;
				if (debugInfo.size() > 0)
				{
					const DebugTrackingInfo& previous = debugInfo[debugInfo.size() - 1];
					if (Globals::Instance().currentLevel.GetFrameNumber() == previous.frame)
					{
						indexForFrame = previous.totalIndexForFrame+1;
					}
				}
				debugInfo.push_back(DebugTrackingInfo(typeid(result), Globals::Instance().currentLevel.GetFrameNumber(), 
														theRecordingVector.size()-1, indexForFrame));
			}
			else if (IsReplayingWholeLevel())
			{
				if (debugTrackingReadbackCount >= debugInfo.size())
				{
					DoBreak(true);
				}

				const DebugTrackingInfo& expectedInfo = debugInfo[debugTrackingReadbackCount];
				DebugTrackingInfo info(typeid(result), Globals::Instance().currentLevel.GetFrameNumber(), readbackCount, expectedInfo.totalIndexForFrame);				
				if (expectedInfo != info)
				{
					DoBreak(true);
				}

				unsigned int total = theReplayingVector.size();
				if (readbackCount >= total)
				{
					DoBreak(true);
				}

				const T& compareTo = theReplayingVector[readbackCount];
				if (result != compareTo)
				{
					DoBreak(true);
				}
				++readbackCount;
				++debugTrackingReadbackCount;
			}
		}
#endif
};

#endif
