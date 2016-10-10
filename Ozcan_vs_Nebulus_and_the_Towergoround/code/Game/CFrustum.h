//CFrustum.h - header for the frustum class

//system includes------------
#include "Rendering/OpenGLInclude.h"
#include <float.h>      // for FLT_MAX
//---------------------------

#ifndef _CFrustum_h_
#define _CFrustum_h_

//header files---------
#include "Math/CPlane.h" // describes a 3d plane
//---------------------

//defines--------------
enum frustumSides // enumeration of frustum sides for readability
{
	RIGHT, LEFT, BOTTOM, TOP, BACK, FRONT, TOTAL_FRUSTUM_SIDES
};
	
enum frustumVerticeIdentifiers // enumeration of frustum vertice for readability
{
	FRONT_LEFT_TOP, FRONT_LEFT_BOTTOM, FRONT_RIGHT_TOP, FRONT_RIGHT_BOTTOM, 
	BACK_LEFT_TOP, BACK_LEFT_BOTTOM, BACK_RIGHT_TOP, BACK_RIGHT_BOTTOM, TOTAL_FRUSTUM_VERTICES
};

enum frustumEdgeIdentifiers // enumeration of frustum edges for readability
{
	FRONT_LEFT, FRONT_TOP, FRONT_RIGHT, FRONT_BOTTOM,
	SIDE_LEFT_TOP, SIDE_RIGHT_TOP, SIDE_RIGHT_BOTTOM, SIDE_LEFT_BOTTOM,
	BACK_LEFT, BACK_TOP, BACK_RIGHT, BACK_BOTTOM, TOTAL_FRUSTUM_EDGES
};
//---------------------

//forward declerations----
//------------------------

class CFrustum
{
	private:
//-----private variables-----------
		CPlane frustumSides[6]; // the frustum has six sides
		CVector frustumVertices[8]; // the frustum has eight vertices
		CVector frustumEdges[12]; // the frustum has 12 edges
//---------------------------------

//-----private methods-------------
		CFrustum(); // constructor
//---------------------------------

	public:
//----public variables--------------
//----------------------------------

//----public methods----------------------------------------
		static CFrustum* instance(); // get the singleton frustum instance
		~CFrustum(); // destructor
		// call this every frame or every time the camera moves to update the frustum
		void calculateFrustum();
		// check if an axially aligned cube is in the frustum
		bool axisAlignedCubeInFrustum(CVector* centre, float width);
		// see if a sphere intersects the frustum
		bool sphereInFrustum(CVector* centre, float sphereRadius);

		// virtual methods

		// overloaded operators

//-----------------------------------------------------------
}; // end class CFrustum

#endif // ifndef _CFrustum_h_ 