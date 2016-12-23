//CTower.h - header for the tower class

//system includes------------
#include "Rendering/OpenGLInclude.h"
//---------------------------

#ifndef _CTower_h_
#define _CTower_h_

//header files---------
//---------------------

//defines--------------
/*	The default horizontal and virtical measurements 
	of the tower.  The difference between these values
	will determine by how much tower objects should be scaled 
	by in the x and y dimensions */
#define BASE_MEASUREMENT_X 3.0f
#define BASE_MEASUREMENT_Y 1.0f
#define BASE_MEASUREMENT_Z 4.0f // specifically for steps, which jut out from the tower
//---------------------

class CTower
{
	private:
//-----private variables-----------
        static CTower* inst;
//---------------------------------

// ----private methods-------------
		CTower(); // constructor (private as singleton class)
// --------------------------------

	public:
//----public variables--------------
		GLint	towerRows,
				towerColumns,
				towerRowHeight,
				towerColumnWidth, 
				totalTowerObjects; // tower rows multiplied by tower columns
		GLfloat towerWallPos,
				angleBetweenTowerSegments,
				xPercentFromBaseMeasurement, // percentage increase/decrease from base measurements
				yPercentFromBaseMeasurement;

		char *towerObjects; // tower objects (dynamic array)
//----------------------------------

//----public methods----------------------------------------
		static CTower * instance(); // return the singleton tower instance
		~CTower(); // destructor
		void deleteCurrentTower(); // deletes memory associated with the current tower
		// initialise tower variables and data structures
		bool initialise(int totalRows, int totalColumns, 
						int rowHeight, int columnWidth);
    
        char GetTowerObject(int index);

		// overloaded operators

//-----------------------------------------------------------
}; // end class CTower

#endif // ifndef _CTower_h_ 