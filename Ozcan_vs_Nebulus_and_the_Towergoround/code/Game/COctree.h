//COctree.h - header for the octree class

//system includes------------
#include <windows.h>	// header file for windows
#include <gl\gl.h>		// header file for the OpenGL32 library
#include <gl\glu.h>		// header file for the GLu32 library
#include <vector>		// include this to use the vector class
//---------------------------

#ifndef _COctree_h_
#define _COctree_h_

//header files---------
#include "Math/CVector.h"
#include "CSprite.h"
#include "CFrustum.h"
#ifdef USE_SHADERS
#include "Rendering\MeshBuffer.h"
#endif
//---------------------

// forward declarations ---
// ------------------------

//defines--------------
class aTriangle            
{
	public:
		CVector	vertices[3],
				normals[3]; // normal for each vertice

		vertex *Vert[3]; /*	vertex information for each vertice 
							(required in order to get tex cords) */

		int	drawnThisFrame; // whether the triangle has been drawn yet this frame
		bool collisionTested; // to stop a sprite testing itself twice on a triangle

		CSprite *assocaitedSprite; // pointer to the sprite the triangle is from
		material *triMaterial; // pointer to this triangles material

		// constructor
		aTriangle(){}

		// copy constructor
		aTriangle(const aTriangle& triangle)
		{
			*this = triangle;
		}

		// make aTriangle equal to another aTriangle
		aTriangle& operator=(const aTriangle *tri)
		{
			for (int i = 0; i < 3; i++)
			{
				vertices[i] = tri->vertices[i];
				normals[i] = tri->normals[i];
				Vert[i] = tri->Vert[i];
			}

			drawnThisFrame = tri->drawnThisFrame;
			collisionTested = tri->collisionTested;
			assocaitedSprite = tri->assocaitedSprite;
			triMaterial = tri->triMaterial;

			return *this;
		}
};

enum nodeIdentifiers
{
	TOP_LEFT_FRONT,
	TOP_RIGHT_FRONT,
	TOP_LEFT_BACK,
	TOP_RIGHT_BACK,
	BOTTOM_LEFT_FRONT,
	BOTTOM_RIGHT_FRONT,
	BOTTOM_LEFT_BACK,
	BOTTOM_RIGHT_BACK
};

#define MAX_SUBDIVISION_LEVELS 8 // max levels to the octree
#define MAX_TRIANGLES_PER_NODE 10 // max triangles in a node
//---------------------

//forward declerations----
//------------------------

class COctree
{
	public: 
//-----public variables-----------
		float	width, // the width of this octree cube node
				sphereRadius; /*	the distance from the cubes centre to a vertex i.e
									the radius a sphere would need to be to compleatly 
									encompass the node */

		int triangleCount, // the amount of triangles stored in this node
			subDivisionLevel, // the nodes level of sub division
			spritesInNode; // whether any sprites are currently in the node

		CVector centre; // the center of this node
		CVector nodeHalfWidths; // the half widths of the node on the x, y, z axis
		CVector geometryBoxWidth;
				
		COctree *subNodes[8]; /*	pointers to the eight sub nodes branching down from 
									this node */

		bool subdivided; // whether this node has been subdivided into smaller nodes

		std::vector<aTriangle*> nodeTriangles; /*	contains pointers into the static triangle vector 
													to the triangles in this octree node */
		std::vector<CSprite*> spritesCurrentlyInNode; /*	the dynamic sprites currently in the 
															node (updated each frame) */
//------static variables -----------
		static int	endNodes;
		static CFrustum *theFrustum; // pointer to the singleton frustum
		static std::vector<aTriangle> triangles; // dynamic array of the octree's triangles
#ifdef USE_SHADERS
		static std::vector<MeshBuffer*> meshBuffers;

		static std::vector<std::vector<float>*> positionsVector;
		static std::vector<std::vector<float>*> normalsVector;
		static std::vector<std::vector<float>*> texCoordsVector;
		static std::vector<std::vector<unsigned short>*> indecisVector;
#endif
		static int numberOfTriangles; // the number of triangles in the above dynamic array
		static int currentSubDivisionLevel; // the current sub division level
		static std::vector<COctree*> leafNodes;
		static std::vector<COctree*> leafNodesOccupiedWithSprites;
		static std::vector<COctree*> COctree::endNodesToDraw; /*	the end nodes which are in view and should be 
																	drawn (updated each frame) */
#ifndef USE_SHADERS
		static int trianglesDisplayList;
#endif
//---------------------------------
		
//-----public methods-------------
		// create this octree node and its sub nodes
		void createNodeAndSubNodes(	std::vector<int> *trianglesInNode, CVector *theCentre, 
									float theWidth);
		// assign vertices to the current node
		void assignVerticeToNode(std::vector<int> *trianglesInNode);
		// get the centre of the specified node
		CVector getNewNodeCentre(CVector* parentCentre, float parentWidth, int nodeID);
		// create a new node
		void createNewNode(	std::vector<int> *trianglesInNode, CVector* newCentre, float parentWidth, 
							int nodeID);
		// builds an end nodes display list of triangles
#ifndef USE_SHADERS
		static void buildTrianglesDisplayList( void);
#endif
		// sort the node triangles so their grouped by material
		void groupNodeTrianglesByMaterial( void); 


		COctree(); // constructor
		~COctree(); // destructor
		// release all the Octree memory
		void destroyOctree();
		// initialise the octree
		void init();
		// determine the scene dimensions and get the list of triangles
		void getSceneDimensionsAndTriangles(CSprite **sprite, int numberOfSprites);
		// empty all the nodes sprite lists
		void clearAllSpriteInNodeLists(COctree *node);
		// sort level sprites into sections of the Octree
		void sortSpritesIntoSections(std::vector<CSprite*> *movingSprites); 
		// call to create the first octree node
		void createNodeAndSubNodes();
		/*	find all the end nodes the bounding box falls into and populate the endNodesOccupying list */
		void putSpriteIntoEndNode(CVector *halfBoundingBox, CVector *centre, std::vector<COctree*>& endNodesOccupying);
		// set triangles to having not been drawn this frame yet
		void setTrianglesToNotDrawn();
		// draw the octree
		void getEndNodesToDraw(COctree* node);
		// draw the octree
		void drawEndNodes( void);


		/*	sorts a sprite into the smallest node in which the sprite 
			can be compleatly contained in */
		void sortSpriteIntoLowestNodeLevel(	COctree *node, CSprite *aSprite, 
											CVector *halfBoundingBox, CVector *centre, 
											int levelToSearchTo = MAX_SUBDIVISION_LEVELS);

		// virtual methods

		// overloaded operators

private:
	/*	find all the end nodes the bounding box falls into and populate the endNodesOccupying list */
		void putSpriteIntoEndNode(CVector *halfBoundingBox, CVector *centre, COctree* node, std::vector<COctree*>& endNodesOccupying);
#ifdef USE_SHADERS
		static void BuildTrianglesMeshBuffers(void);
		static void DrawTrianglesMeshBuffers(void);
#endif
//-----------------------------------------------------------
}; // end class COctree

#endif // ifndef _COctree_h_ 


