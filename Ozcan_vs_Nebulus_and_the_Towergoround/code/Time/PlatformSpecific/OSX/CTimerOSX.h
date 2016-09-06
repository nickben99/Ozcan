//CTimer.h, header file for CTimer.cpp

#ifdef OSX

//system includes---
//------------------

#ifndef _CTimer_h_
#define _CTimer_h_

class CTimer
{ 
	private:
//-------private variables ------------------
//-------------------------------------------

	public:
//------public methods --------------------------------
		CTimer(); //constructor
		~CTimer(); // destructor
		unsigned int time(); // return the time in milliseconds
//-----------------------------------------------------
};

#endif // ifndef _CTimer_h_

#endif // #ifdef OSX