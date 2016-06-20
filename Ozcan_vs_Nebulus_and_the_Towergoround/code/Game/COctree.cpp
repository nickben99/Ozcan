//COctree.cpp, the octree class

// system includes --------
#include <assert.h>
//-------------------------

// header files -----------
#include "COctree.h"
using namespace std; /* needs to be declared after header include as vector is declared
						in header */
#include "Rendering/CModel.h"
#include "intersections.h"
#include "Game/Globals.h"
//-------------------------

// static variable declerations -----------------------
CFrustum* COctree::theFrustum = 0;
int COctree::currentSubDivisionLevel = 0;
vector<aTriangle> COctree::triangles;
#ifdef USE_SHADERS
vector<MeshBuffer*> COctree::meshBuffers;
#endif
int COctree::numberOfTriangles = 0; 
vector<COctree*> COctree::leafNodes;
vector<COctree*> COctree::leafNodesOccupiedWithSprites;
vector<COctree*> COctree::endNodesToDraw; /*	the end nodes which are in view and should be 
												drawn (updated each frame) */
#ifndef USE_SHADERS
int COctree::trianglesDisplayList = 0;
#endif
int COctree::endNodes = 0;
//-----------------------------------------------------

// constructor
COctree::COctree()
{
	init(); // initialisation
}

// destructor
COctree::~COctree()
{
	destroyOctree();
}

// release all the Octree memory
void COctree::destroyOctree() 
{
	leafNodes.clear(); // destroy the static list
	nodeTriangles.clear(); // destroy the list
	spritesCurrentlyInNode.clear(); // destroy the list
	theFrustum = 0; // set to 0 
	triangles.clear(); // destroy the static list

#ifdef USE_SHADERS
	int numMeshBuffers = meshBuffers.size();
	for (int buffer = 0; buffer < numMeshBuffers; ++buffer)
	{
		delete meshBuffers[buffer];
	}
	meshBuffers.clear();
#endif

	leafNodesOccupiedWithSprites.clear();

	currentSubDivisionLevel = 0;
	numberOfTriangles = 0; 
	endNodes = 0;

#ifndef USE_SHADERS
	glDeleteLists(trianglesDisplayList, 1); // delete the static display list
	trianglesDisplayList = 0; // set back to zero
#endif

	// delete all the sub nodes
	for (int i = 0; i < 8; i++)
	{
		if (subNodes[i]) // if not null
		{
			delete subNodes[i]; // delete the sub node
			subNodes[i] = 0;
		}
	}
}

// initialise the octree
void COctree::init()
{
	width = 0.0f; // initialise the width
	triangleCount = 0; // initialise the amount of triangles stored in this Octree node
	subdivided = false; // initialise to false

	for (int i = 0; i < 8; i++){
		subNodes[i] = 0;} //	initialise sub node pointers to null

	// set singleton pointer to frustum
	if (!theFrustum){
		theFrustum = CFrustum::instance();}
}

/*	determine the scene dimensions and get the list of triangles from the 
	sprite engine sprites passed as parameter */
void COctree::getSceneDimensionsAndTriangles(CSprite **sprite, int numberOfSprites)
{
	// temporary variable
	CVector currentMax, lowestXYZ, highestXYZ;

	// go through all the sprites to get all the triangles
	for (int i = 0; i < numberOfSprites; i++)
	{	
		if (!sprite[i]){ // if sprite is null
			continue;} // go to next iteration

		CSprite& aSprite = *sprite[i]; // set referance variable to sprite
		
		// continue if no model or not static environment
		if ((!aSprite.model) || (aSprite.dynamicProperty != STATIC_ENVIRONMENT)){ 
			continue;}

		// go through all the sprite models meshes
		for (int aMesh = 0; aMesh < aSprite.model->meshNo; aMesh++)
		{	
			mesh &thisMesh = aSprite.model->meshes[aMesh]; // referance variable
			
			// go through this mesh's triangles
			for (int triangle = 0; triangle < thisMesh.trianglesNo; triangle++)
			{	
				aTriangle tri;
				triangles.push_back(tri); // push triangle onto back of triangle vector

				aTriangle& thisTriangle = triangles[triangles.size()-1]; // referance variable
				// set the sprite which the triangle originates from
				thisTriangle.assocaitedSprite = &aSprite;
				// set the material for the triangle
				thisTriangle.triMaterial = (thisMesh.matIndex >= 0) ? 
					&aSprite.model->materials[thisMesh.matIndex] : 0;

				// work out the centre of the scene and add triangles to vector
				for (int vertice = 0; vertice < 3; vertice++)
				{
					// set Vert so triangle can get its u, v coordinates
					int vertexRequired	= thisMesh.triangles[triangle].vert[vertice];
					thisTriangle.Vert[vertice] = &thisMesh.vertices[vertexRequired];

					// referance variable for readability
					CVector &vert = thisTriangle.vertices[vertice],
							&norm = thisTriangle.normals[vertice];

					// add triangle vertices to vector of triangles
					vert =          
						(aSprite.finalMatrix.multiplyVector(
						&thisMesh.triangles[triangle].currentVertices[vertice]));
 
					// add triangle normals to vector of triangles
					norm =
						(sprite[i]->rotation.multiplyVector( // just rotate by models rotation (no need to scale or translate)
					&thisMesh.triangles[triangle].normals[vertice]));

					norm.normalise(); // ensure normal is still normalised

					// get lowest and highest vertices
					if( (0 == (triangles.size()-1)) && (0 == vertice) )
					{
						lowestXYZ = vert;
						highestXYZ = vert;
					}
					else
					{
						if (vert.x < lowestXYZ.x){ lowestXYZ.x = vert.x;}
						if (vert.y < lowestXYZ.y){ lowestXYZ.y = vert.y;}
						if (vert.z < lowestXYZ.z){ lowestXYZ.z = vert.z;}

						if (vert.x > highestXYZ.x){ highestXYZ.x = vert.x;}
						if (vert.y > highestXYZ.y){ highestXYZ.y = vert.y;}
						if (vert.z > highestXYZ.z){ highestXYZ.z = vert.z;}
					}// end if first triangle and vertice 
				}// end for vertice 
			}// end for triangles
		}// end for mesh's
	}//end for sprites

	// record number of triangles (much faster than calling triangles.size() each time)
	numberOfTriangles = triangles.size(); 

	centre = lowestXYZ + ((highestXYZ - &lowestXYZ)*0.5); // get scene centre

	// get the whole width
	currentMax = (highestXYZ - &lowestXYZ);
	geometryBoxWidth = currentMax*0.5f;

	// check if the width is the highest value and assign that for the cube dimension
	if(currentMax.x > currentMax.y && currentMax.x > currentMax.z){
		width = currentMax.x;}

	// check if the height is the heighest value and assign that for the cube dimension
	else if(currentMax.y > currentMax.x && currentMax.y > currentMax.z){
		width = currentMax.y;}

	// else it must be the depth or it's the same value as some of the other ones
	else{ width = currentMax.z; }

	// sort all the scene triangles so they are grouped by material
	int j; 
	aTriangle index; // variables for insertion sort below

	/*	go through all the triangles and group them by material */
	int totalTriangles = triangles.size(); 
	/*	sort all the triangles based on the memory address of the materials 
		using the insertion sort */
	for (int i = 1; i < totalTriangles; i++)
	{	
		index = triangles[i]; // get aTriangle
		j = i;

		while ((j > 0) && (triangles[j - 1].triMaterial->sort > index.triMaterial->sort))
		{
			triangles[j] = triangles[j-1];
			j = j - 1;
		}
		triangles[j] = index;
	}

	/*	the following section of code makes sure the transparent triangles are at the end of 
		the triangles vector, and so drawn last */

	// create a vector of transparent triangles 
	vector<aTriangle> transparentTris;
	// find the transparent triangles
	for (int i = 0; i < totalTriangles;)
	{
		// if transparent
		if (triangles[i].triMaterial->transparency < 1.0f)
		{
			transparentTris.push_back(triangles[i]); // add triangle to transparentTris vector
			triangles.erase(triangles.begin()+i); // erase it from triangles vector
			totalTriangles = triangles.size(); // the list size has changed
		}
		else{
			i++;}
	}

	int transparentTriListSize = transparentTris.size();
	for (int i = 0; i < transparentTriListSize; i++){
		// add all the transparent triangles onto the end of the triangles vector
		triangles.push_back(transparentTris[i]);}

	transparentTris.clear(); // clear the temporary transparentTris vector

#ifdef USE_SHADERS
	BuildTrianglesMeshBuffers();
#else
	buildTrianglesDisplayList(); // build a display list for drawing all the triangles
#endif
}

// empty all the sprite node lists
void COctree::clearAllSpriteInNodeLists(COctree *node)
{
	if (!node){ return;} // check to ensure the node is not null

	node->spritesCurrentlyInNode.clear(); // clear the spritesCurrentlyInNode list 

	if(node->subdivided) // if this is not a leaf node
	{
		//	recurse to the bottom of these nodes and clear their spritesCurrentlyInNode list 
		clearAllSpriteInNodeLists(node->subNodes[TOP_LEFT_FRONT]);
		clearAllSpriteInNodeLists(node->subNodes[TOP_LEFT_BACK]);
		clearAllSpriteInNodeLists(node->subNodes[TOP_RIGHT_BACK]);
		clearAllSpriteInNodeLists(node->subNodes[TOP_RIGHT_FRONT]);
		clearAllSpriteInNodeLists(node->subNodes[BOTTOM_LEFT_FRONT]);
		clearAllSpriteInNodeLists(node->subNodes[BOTTOM_LEFT_BACK]);
		clearAllSpriteInNodeLists(node->subNodes[BOTTOM_RIGHT_BACK]);
		clearAllSpriteInNodeLists(node->subNodes[BOTTOM_RIGHT_FRONT]);
	}
}

/*	find all the end nodes the bounding box falls into and populate the endNodesOccupying list */
void COctree::putSpriteIntoEndNode(CVector *halfBoundingBox, CVector *centre, std::vector<COctree*>& endNodesOccupying)
{
	putSpriteIntoEndNode(halfBoundingBox, centre, this, endNodesOccupying);
}

/*	find all the end nodes the sprite falls into and populates the 
	sprite endNodesOccupying list */
void COctree::putSpriteIntoEndNode(	CVector *halfBoundingBox,
									CVector *centre, 
									COctree* node, 
									std::vector<COctree*>& endNodesOccupying)
{
	if (!node){ return;} // check to ensure the node is not null

	// if the sprite is in this node
	if (axisAlignedBoxBoxIntersection(&node->centre, &node->nodeHalfWidths, 
										centre, halfBoundingBox))
	{
		if(node->subdivided) // if this is not a leaf node
		{
			// node->spritesCurrentlyInNode.push_back(aSprite); // this sprite is in this node

			//	recurse to the bottom of these nodes and place the sprite in the end nodes 
			putSpriteIntoEndNode(halfBoundingBox, centre, node->subNodes[TOP_LEFT_FRONT], endNodesOccupying);
			putSpriteIntoEndNode(halfBoundingBox, centre, node->subNodes[TOP_LEFT_BACK], endNodesOccupying);
			putSpriteIntoEndNode(halfBoundingBox, centre, node->subNodes[TOP_RIGHT_BACK], endNodesOccupying);
			putSpriteIntoEndNode(halfBoundingBox, centre, node->subNodes[TOP_RIGHT_FRONT], endNodesOccupying);
			putSpriteIntoEndNode(halfBoundingBox, centre, node->subNodes[BOTTOM_LEFT_FRONT], endNodesOccupying);
			putSpriteIntoEndNode(halfBoundingBox, centre, node->subNodes[BOTTOM_LEFT_BACK], endNodesOccupying);
			putSpriteIntoEndNode(halfBoundingBox, centre, node->subNodes[BOTTOM_RIGHT_BACK], endNodesOccupying);
			putSpriteIntoEndNode(halfBoundingBox, centre, node->subNodes[BOTTOM_RIGHT_FRONT], endNodesOccupying);
		}
		else // this is an end node
		{
			endNodesOccupying.push_back(node);
		} // end if node->subdivided
	} // end if the sprite is in this node
}

// call to create the first octree node
void COctree::createNodeAndSubNodes()
{ 
	endNodes = 0;
	
	// make vector of ints equal in size to list of triangles
	vector<int> trianglesInNode(numberOfTriangles);

	// set all elements in list to triangle indices
	for (int triangle = 0; triangle < numberOfTriangles; triangle++)
	{
		trianglesInNode[triangle] = triangle;
	}

	// create the first node and sub node (this is called recursivly)
	createNodeAndSubNodes(&trianglesInNode, &centre, width);

	trianglesInNode.clear(); // destroy the list of ints
}

// create this octree node and its sub nodes
void COctree::createNodeAndSubNodes(vector<int> *trianglesInNode, CVector *theCentre, 
									float theWidth)
{ 
	// initialize this nodes center point
	centre = theCentre;

	// initialize this nodes cube width
	width = theWidth;
	nodeHalfWidths = CVector(width*0.5f, width*0.5f, width*0.5f);

	/*	compute the distence from the node cubes centre to a vertex,
		so computing the radius a sphere would need to be to compleatly 
		encompass the node */
	sphereRadius = width/( (2.0f*sqrt(3.0f)) / 3.0f );

	subDivisionLevel = currentSubDivisionLevel; // set the nodes sub division level

	/*	if their is too many triangles in this node and the max octree 
		level has not been reached */
	int sizeOfTrianglesInNode = trianglesInNode->size();
	if( (sizeOfTrianglesInNode > MAX_TRIANGLES_PER_NODE) && 
		(currentSubDivisionLevel < MAX_SUBDIVISION_LEVELS) )
	{
		subdivided = true; // this node has been sub-divided

		// create the list of booleans for each triangle index
		vector<int> topLeftFront;
		vector<int> topLeftBack;
		vector<int> topRightBack;
		vector<int> topRightFront;
		vector<int> bottomLeftBack;
		vector<int> bottomLeftFront;
		vector<int> bottomRightBack;
		vector<int> bottomRightFront;

		// get new node centres
		CVector topLeftFrontCentre		= getNewNodeCentre(&centre, width, TOP_LEFT_FRONT),
				topLeftBackCentre		= getNewNodeCentre(&centre, width, TOP_LEFT_BACK),
				topRightBackCentre		= getNewNodeCentre(&centre, width, TOP_RIGHT_BACK),
				topRightFrontCentre		= getNewNodeCentre(&centre, width, TOP_RIGHT_FRONT),
				bottomLeftBackCentre	= getNewNodeCentre(&centre, width, BOTTOM_LEFT_BACK),
				bottomLeftFrontCentre	= getNewNodeCentre(&centre, width, BOTTOM_LEFT_FRONT),
				bottomRightBackCentre	= getNewNodeCentre(&centre, width, BOTTOM_RIGHT_BACK),
				bottomRightFrontCentre	= getNewNodeCentre(&centre, width, BOTTOM_RIGHT_FRONT);

		/*	go through all the triangles to see which sub node the triangle is in,
			could be in more than one node, which is ok */
		for (int triangle = 0; triangle < sizeOfTrianglesInNode; triangle++)
		{	// referance variable for speed and readability
			int triangleNumber = (*trianglesInNode)[triangle];
			aTriangle &thisTriangle = triangles[triangleNumber];

			CVector quarterWidth(width*0.25f, width*0.25f, width*0.25f);
			if (triangleAxisAlignedBoxIntersection( // top left front node
				&topLeftFrontCentre,
				&quarterWidth, 
				&thisTriangle.vertices[0])){
				topLeftFront.push_back(triangleNumber);}

			if (triangleAxisAlignedBoxIntersection( // top left back node
				&topLeftBackCentre,
				&quarterWidth, 
				&thisTriangle.vertices[0])){
				topLeftBack.push_back(triangleNumber);}

			if (triangleAxisAlignedBoxIntersection( // top right back node
				&topRightBackCentre,
				&quarterWidth, 
				&thisTriangle.vertices[0])){
				topRightBack.push_back(triangleNumber);}

			if (triangleAxisAlignedBoxIntersection( // top right front node
				&topRightFrontCentre,
				&quarterWidth, 
				&thisTriangle.vertices[0])){
				topRightFront.push_back(triangleNumber);}

			if (triangleAxisAlignedBoxIntersection( // bottom left back node
				&bottomLeftBackCentre,
				&quarterWidth, 
				&thisTriangle.vertices[0])){
				bottomLeftBack.push_back(triangleNumber);}

			if (triangleAxisAlignedBoxIntersection( // bottom left front node
				&bottomLeftFrontCentre,
				&quarterWidth, 
				&thisTriangle.vertices[0])){
				bottomLeftFront.push_back(triangleNumber);}

			if (triangleAxisAlignedBoxIntersection( // bottom right back node
				&bottomRightBackCentre,
				&quarterWidth, 
				&thisTriangle.vertices[0])){
				bottomRightBack.push_back(triangleNumber);}

			if (triangleAxisAlignedBoxIntersection( // bottom right front node
				&bottomRightFrontCentre,
				&quarterWidth, 
				&thisTriangle.vertices[0])){
				bottomRightFront.push_back(triangleNumber);}
		}// end for triangles

		// create a new node if triangles are in the specified area
		if (topLeftFront.size() > 0) { 
			createNewNode(&topLeftFront, &topLeftFrontCentre, width, TOP_LEFT_FRONT);}
		if (topLeftBack.size() > 0) { 
			createNewNode(&topLeftBack, &topLeftBackCentre, width, TOP_LEFT_BACK);}
		if (topRightBack.size() > 0) { 
			createNewNode(&topRightBack, &topRightBackCentre, width, TOP_RIGHT_BACK);}
		if (topRightFront.size() > 0) { 
			createNewNode(&topRightFront, &topRightFrontCentre, width, TOP_RIGHT_FRONT);}
		if (bottomLeftBack.size() > 0) { 
			createNewNode(&bottomLeftBack, &bottomLeftBackCentre, width, BOTTOM_LEFT_BACK);}
		if (bottomLeftFront.size() > 0) { 
			createNewNode(&bottomLeftFront, &bottomLeftFrontCentre, width, BOTTOM_LEFT_FRONT);}
		if (bottomRightBack.size() > 0) { 
			createNewNode(&bottomRightBack, &bottomRightBackCentre, width, BOTTOM_RIGHT_BACK);}
		if (bottomRightFront.size() > 0) {
			createNewNode(&bottomRightFront, &bottomRightFrontCentre, width, BOTTOM_RIGHT_FRONT);}

		// clear the vector lists
		topLeftFront.clear();
		topLeftBack.clear();
		topRightBack.clear();
		topRightFront.clear();
		bottomLeftBack.clear();
		bottomLeftFront.clear();
		bottomRightBack.clear();
		bottomRightFront.clear();
	}
	else
	{
		// the vertices must be stored in this node as it cannot be subdivided any more
		assignVerticeToNode(trianglesInNode);
	}
}

// assign vertices to the current node
void COctree::assignVerticeToNode(vector<int> *trianglesInNode)
{
	endNodes++;

	leafNodes.push_back(this); // record in the leaf node vector

	subdivided = false; // this node has not been sub-divided

	for (unsigned i = 0; i < trianglesInNode->size(); i++){
		nodeTriangles.push_back(&triangles[(*trianglesInNode)[i]]);} // set pointer into triangle vector

	triangleCount = nodeTriangles.size(); // record this value for speed

	groupNodeTrianglesByMaterial(); // sort the node triangles so their grouped by material
}

// sort the node triangles so their grouped by material
void COctree::groupNodeTrianglesByMaterial( void)
{
	int i, j; 
	aTriangle *index; // variables for insertion sort below

	/*	go through all the triangles in this node and 
		group them by material */
	int totalNodeTriangles = triangleCount; 
	/*	sort all the triangles based on the memory address of the materials 
		using the insertion sort */
	for (i = 1; i < totalNodeTriangles; i++)
	{	
		index = nodeTriangles[i]; // get aTriangle pointer
		j = i;

		while ((j > 0) && ((int)nodeTriangles[j - 1]->triMaterial > (int)index->triMaterial))
		{
			nodeTriangles[j] = nodeTriangles[j-1];
			j = j - 1;
		}
		nodeTriangles[j] = index;
	}
}

#ifdef USE_SHADERS

vector<vector<float>*> COctree::positionsVector;
vector<vector<float>*> COctree::normalsVector;
vector<vector<float>*> COctree::texCoordsVector;
vector<vector<unsigned short>*> COctree::indecisVector;

void COctree::BuildTrianglesMeshBuffers(void)
{
	vector<float>* positions = new vector<float>();
	vector<float>* normals = new vector<float>();
	vector<float>* texCoords = new vector<float>();

	material *previousMaterialMemoryAddress = 0;
	bool trianglesCompleatlyTransparent = false;
	int allTrianglesCount = triangles.size();
	for (int tri = 0; tri < allTrianglesCount; tri++)
	{	// if the triangle has not yet been drawn ths frame
		aTriangle &triangleRequired = triangles[tri];

		if (triangleRequired.triMaterial != previousMaterialMemoryAddress)
		{
			if (positions->size() > 0)
			{
				meshBuffers.push_back(new MeshBuffer());

				vector<float>& arr = *positions;
				meshBuffers[meshBuffers.size()-1]->CreateVertexArray(&arr[0], positions->size(), false);
				vector<float>& arrnormals = *normals;
				meshBuffers[meshBuffers.size()-1]->CreateNormalArray(&arrnormals[0], normals->size(), false);
				vector<float>& arrtexCoords = *texCoords;
				meshBuffers[meshBuffers.size()-1]->CreateTexCoordArray(&arrtexCoords[0], texCoords->size(), false);

				if (0 != previousMaterialMemoryAddress) 
				{
					meshBuffers[meshBuffers.size()-1]->SetMaterial(*previousMaterialMemoryAddress);
				}

				positions = new vector<float>();
				normals = new vector<float>();
				texCoords = new vector<float>();
			}

			// set previous material memory address
			previousMaterialMemoryAddress = triangleRequired.triMaterial;

			// see if the material is compleatly transparent
			trianglesCompleatlyTransparent = (0.0f == previousMaterialMemoryAddress->transparency);
		}// end if not equl to previous material

		if (true == trianglesCompleatlyTransparent){
			continue;} //  dont draw compleatly transparent triangles

		(*normals).push_back(triangleRequired.normals[0].x); (*normals).push_back(triangleRequired.normals[0].y); (*normals).push_back(triangleRequired.normals[0].z);
		(*texCoords).push_back(triangleRequired.Vert[0]->u); (*texCoords).push_back(triangleRequired.Vert[0]->v);
		(*positions).push_back(triangleRequired.vertices[0].x); (*positions).push_back(triangleRequired.vertices[0].y); (*positions).push_back(triangleRequired.vertices[0].z);

		(*normals).push_back(triangleRequired.normals[1].x); (*normals).push_back(triangleRequired.normals[1].y); (*normals).push_back(triangleRequired.normals[1].z);
		(*texCoords).push_back(triangleRequired.Vert[1]->u); (*texCoords).push_back(triangleRequired.Vert[1]->v);
		(*positions).push_back(triangleRequired.vertices[1].x); (*positions).push_back(triangleRequired.vertices[1].y); (*positions).push_back(triangleRequired.vertices[1].z);

		(*normals).push_back(triangleRequired.normals[2].x); (*normals).push_back(triangleRequired.normals[2].y); (*normals).push_back(triangleRequired.normals[2].z);
		(*texCoords).push_back(triangleRequired.Vert[2]->u); (*texCoords).push_back(triangleRequired.Vert[2]->v);
		(*positions).push_back(triangleRequired.vertices[2].x); (*positions).push_back(triangleRequired.vertices[2].y); (*positions).push_back(triangleRequired.vertices[2].z);
	}// end for tri

	if ((*positions).size() > 0)
	{
		meshBuffers.push_back(new MeshBuffer());

		vector<float>& arr = *positions;
		meshBuffers[meshBuffers.size()-1]->CreateVertexArray(&arr[0], positions->size(), false);
		vector<float>& arrnormals = *normals;
		meshBuffers[meshBuffers.size()-1]->CreateNormalArray(&arrnormals[0], normals->size(), false);
		vector<float>& arrtexCoords = *texCoords;
		meshBuffers[meshBuffers.size()-1]->CreateTexCoordArray(&arrtexCoords[0], texCoords->size(), false);

		if (0 != previousMaterialMemoryAddress) 
		{
			meshBuffers[meshBuffers.size()-1]->SetMaterial(*previousMaterialMemoryAddress);
		}
	}
}

void COctree::DrawTrianglesMeshBuffers(void)
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // blend type (for transparency)
	Globals::Instance().gl.GLEnable(GL_BLEND); // enable blending (for transparency)

	int numMeshBuffers = meshBuffers.size();
	for (int buffer = 0; buffer < numMeshBuffers; ++buffer)
	{
		meshBuffers[buffer]->Draw();
	}
}
#endif

#ifndef USE_SHADERS
// builds a display list of all the triangles
void COctree::buildTrianglesDisplayList( void)
{
	/*	clear the screen and the depth buffer and reset the model view matrix 
		so the context for the display list is correct */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Globals::Instance().modelMatrixStack.LoadIdentity();

	//int count = 0;

	// generate the triangle display list					
	trianglesDisplayList = glGenLists(1); 
	glNewList(trianglesDisplayList, GL_COMPILE); // create the display list	
	glPushMatrix();

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // blend type (for transparency)
		Globals::Instance().gl.GLEnable(GL_BLEND); // enable blending (for transparency)
		material *previousMaterialMemoryAddress = 0;
		bool trianglesCompleatlyTransparent = false;
		// go through all the triangles and find the ones in this node
		int totalNodeTriangles = triangles.size(); //triangleCount; 
		for (int tri = 0; tri < totalNodeTriangles; tri++)
		{	// if the triangle has not yet been drawn ths frame
			aTriangle &triangleRequired = triangles[tri]; //*nodeTriangles[tri];

			if (triangleRequired.triMaterial != previousMaterialMemoryAddress)
			{
				//if (1 == count)
				//{
				//	break;
				//}
				//++count;

				// set previous material memory address
				previousMaterialMemoryAddress = triangleRequired.triMaterial;

				// see if the material is compleatly transparent
				if (0.0f == triangleRequired.triMaterial->transparency){
					trianglesCompleatlyTransparent = true;}
				else{
					trianglesCompleatlyTransparent = false;}

				// if their is a material for this mesh
				if (0 != triangleRequired.triMaterial) 
				{	// referance variable for readability
					material &trianglesMaterial = *triangleRequired.triMaterial;
					glMaterialfv( GL_FRONT, GL_AMBIENT, trianglesMaterial.ambient);
					glMaterialfv( GL_FRONT, GL_DIFFUSE, trianglesMaterial.diffuse);
					glMaterialfv( GL_FRONT, GL_SPECULAR, trianglesMaterial.specular);
					glMaterialfv( GL_FRONT, GL_EMISSION, trianglesMaterial.emissive);
					glMaterialf( GL_FRONT, GL_SHININESS, trianglesMaterial.shinnines);

					if(trianglesMaterial.clrTexBndNo > 0) // if this material has a texture
					{	// bind texture
						glBindTexture(GL_TEXTURE_2D, trianglesMaterial.clrTexBndNo);
						Globals::Instance().gl.GLEnable(GL_TEXTURE_2D); // enable texturing
					}
					else{ // material does not have a texture
						Globals::Instance().gl.GLDisable(GL_TEXTURE_2D);}
				}
				else{ // no texture for this mesh
					Globals::Instance().gl.GLDisable(GL_TEXTURE_2D);}
			
			}// end if not equl to previous material

			if (true == trianglesCompleatlyTransparent){
				continue;} //  dont draw compleatly transparent triangles

			glBegin(GL_TRIANGLES); // draw triangles
				glNormal3f(triangleRequired.normals[0].x, triangleRequired.normals[0].y, triangleRequired.normals[0].z); // set normal
				glTexCoord2f(triangleRequired.Vert[0]->u, triangleRequired.Vert[0]->v); // set texcoords
				glVertex3f(triangleRequired.vertices[0].x, triangleRequired.vertices[0].y, triangleRequired.vertices[0].z); // set virtex

				glNormal3f(triangleRequired.normals[1].x, triangleRequired.normals[1].y, triangleRequired.normals[1].z); // set normal
				glTexCoord2f(triangleRequired.Vert[1]->u, triangleRequired.Vert[1]->v); // set texcoords
				glVertex3f(triangleRequired.vertices[1].x, triangleRequired.vertices[1].y, triangleRequired.vertices[1].z); // set virtex

				glNormal3f(triangleRequired.normals[2].x, triangleRequired.normals[2].y, triangleRequired.normals[2].z); // set normal
				glTexCoord2f(triangleRequired.Vert[2]->u, triangleRequired.Vert[2]->v); // set texcoords
				glVertex3f(triangleRequired.vertices[2].x, triangleRequired.vertices[2].y, triangleRequired.vertices[2].z); // set virtex
			glEnd(); // end drawing triangles		
		}// end for tri

	glPopMatrix();
	glEndList(); // end creating display list
}
#endif

// create a new node
void COctree::createNewNode(vector<int> *trianglesInNode, CVector* newCentre, float parentWidth, 
							int nodeID)
{
	subNodes[nodeID] = new COctree(); // create new octree sub node

	// increase the current level of subdivision
	currentSubDivisionLevel++;

	// recurse through this node and subdivide it if necessary
	subNodes[nodeID]->createNodeAndSubNodes(trianglesInNode, newCentre, parentWidth*0.5f);

	// decrease the current level of subdivision
	currentSubDivisionLevel--;
}

// get the centre of the specified node
CVector COctree::getNewNodeCentre(CVector* parentCentre, float parentWidth, int nodeID)
{
	float qtrWidth = parentWidth/4.0f;

	// return the centre of the desired node
	switch(nodeID)
	{
		case TOP_LEFT_FRONT:
			return(CVector(parentCentre->x-qtrWidth, parentCentre->y+qtrWidth, parentCentre->z+qtrWidth));
		case TOP_RIGHT_FRONT:
			return(CVector(parentCentre->x+qtrWidth, parentCentre->y+qtrWidth, parentCentre->z+qtrWidth));
		case TOP_LEFT_BACK:
			return(CVector(parentCentre->x-qtrWidth, parentCentre->y+qtrWidth, parentCentre->z-qtrWidth));
		case TOP_RIGHT_BACK:
			return(CVector(parentCentre->x+qtrWidth, parentCentre->y+qtrWidth, parentCentre->z-qtrWidth));
		case BOTTOM_LEFT_FRONT:
			return(CVector(parentCentre->x-qtrWidth, parentCentre->y-qtrWidth, parentCentre->z+qtrWidth));
		case BOTTOM_RIGHT_FRONT:
			return(CVector(parentCentre->x+qtrWidth, parentCentre->y-qtrWidth, parentCentre->z+qtrWidth));
		case BOTTOM_LEFT_BACK:
			return(CVector(parentCentre->x-qtrWidth, parentCentre->y-qtrWidth, parentCentre->z-qtrWidth));
		case BOTTOM_RIGHT_BACK:
			return(CVector(parentCentre->x+qtrWidth, parentCentre->y-qtrWidth, parentCentre->z-qtrWidth));
		default: break;
	}
	assert(false);
	return CVector();
}

// set triangles to having not been drawn this frame yet
void COctree::setTrianglesToNotDrawn()
{
	// set all triangles to having not been drawn yet this frame
	for (unsigned tri = 0; tri < triangles.size(); tri++){
		triangles[tri].drawnThisFrame = 0x0;} 
}

// draw the octree
void COctree::getEndNodesToDraw(COctree* node)
{
	// if the node passed as parameter is null, return
	if(!node){ return;}

	// check if the node is in the viewing frustum
	if (theFrustum->sphereInFrustum(&node->centre, node->sphereRadius))
	{
		/*	check if this node is subdivided. If so, then we need to recurse 
			and draw it's nodes */
		if(node->subdivided)
		{
			/*	recurse to the bottom of these nodes and draw the end node's vertices
				like creating the octree, we need to recurse through each of the 8 nodes */
			getEndNodesToDraw(node->subNodes[TOP_LEFT_FRONT]);
			getEndNodesToDraw(node->subNodes[TOP_LEFT_BACK]);
			getEndNodesToDraw(node->subNodes[TOP_RIGHT_BACK]);
			getEndNodesToDraw(node->subNodes[TOP_RIGHT_FRONT]);
			getEndNodesToDraw(node->subNodes[BOTTOM_LEFT_FRONT]);
			getEndNodesToDraw(node->subNodes[BOTTOM_LEFT_BACK]);
			getEndNodesToDraw(node->subNodes[BOTTOM_RIGHT_BACK]);
			getEndNodesToDraw(node->subNodes[BOTTOM_RIGHT_FRONT]);
		}
		else // this is an end node so its triangles must be drawn
		{
			// this nodes is in view and should be drawn
			endNodesToDraw.push_back(node);	
		}// end if node->subdivided
	}// end if the nodes in the viewing frustum
}

// draw the end nodes required
void COctree::drawEndNodes( void)
{
#ifdef USE_SHADERS
	DrawTrianglesMeshBuffers();
#else
	/*	drawing all triangles by way of a display list seems to be the quickest 
		drawing method */
	glCallList(trianglesDisplayList); // static display lis of all triangles
#endif
}// end method drawEndNodes
