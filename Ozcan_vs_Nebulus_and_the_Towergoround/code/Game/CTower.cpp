//CTower.cpp, the tower class

//system includes-------
#include "math.h"
#include <iostream>
//----------------------

//header files----------
#include "CTower.h" // header file
#include "Game/defines.h" // useful defines
 // DEBUGGING
//----------------------

// static variables ---------------
CTower* CTower::inst = nullptr;
//---------------------------------

// creates singleton tower instance
CTower * CTower::instance () 
{
    if (!inst) {
        inst = new CTower();
    }
    
//    if (inst->totalTowerObjects > 0 && inst->towerObjects)
//    {
//        for (int i = 0; i < inst->totalTowerObjects; i++){
//            char val = inst->towerObjects[i];
//            std::cout << "\n " + std::to_string(i) + ". " + std::to_string(val);
//        }
//    }
    
    return inst;
    
	//static CTower towerInstance;
    //return &towerInstance;
}

// constructor
CTower::CTower()
{
	towerRows = 0; // initialise tower variables
	towerColumns = 0;
	towerRowHeight = 0;
	towerColumnWidth = 0;
	towerWallPos = 0;
	angleBetweenTowerSegments = 0.0f;
	
	towerObjects = 0; //set dynamic array to 0
}

// destructor
CTower::~CTower()
{
	deleteCurrentTower(); // performs neccessary destruction	
}

char CTower::GetTowerObject(int index)
{
    return towerObjects[index];
}

// initialise tower variables and data structures
bool CTower::initialise(int totalRows, int totalColumns, 
						int rowHeight, int columnWidth)
{
	towerRows = totalRows; 
	towerColumns = totalColumns;
	towerRowHeight = rowHeight;
	towerColumnWidth = columnWidth;
	
	/*  get the percentage increase or decrease in size of the tower as
		compared with the base measurements */
	xPercentFromBaseMeasurement = towerColumnWidth/BASE_MEASUREMENT_X;
	yPercentFromBaseMeasurement = towerRowHeight/BASE_MEASUREMENT_Y;

	// work out the tower wall position
	angleBetweenTowerSegments = (2*PI)/towerColumns;
	towerWallPos = (0.5f*towerColumnWidth) / (float)tan(0.5f*angleBetweenTowerSegments);
	// convert angleBetweenTowerSegments to degrees (rather than radians)
	angleBetweenTowerSegments = radToDeg(angleBetweenTowerSegments);

	// allocate memory for the tower objects
	totalTowerObjects = (towerRows*towerColumns);
	
	/*	if their is something in the towerObjects dynamic array (which will be the case 
		if a level is played then the player goes back to the menu and selects another 
		level) delete the current contents */
	if (towerObjects)
	{
		delete[] towerObjects;
		towerObjects = 0;
	}

	towerObjects = new char[totalTowerObjects];

	if (!towerObjects){
		return(false);} // memory allocation failed
	
	return(true); // memory allocated succesfully
}

// deletes memory associated with the current tower
void CTower::deleteCurrentTower()
{
	if (towerObjects)
	{
		delete[] towerObjects;
		towerObjects = 0;
	}
}
