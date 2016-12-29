//CScene.cpp, the scene class

//system includes-------
//----------------------

//header files----------
#include "CScene.h" // header file
#include "FileReading/texture.h" // REUSED CLASS FROM OPENGL SUPERBIBLE - sets up textures
#include "Rendering/CModelLoader.h"
#include "ReplayManager.h"
#include "game/Globals.h"
#if (_DEBUG && USE_SHADERS)
	#include "Debug/DebugMenu.h"
#endif
//----------------------

// constructor
CScene::CScene()
#ifdef USE_SHADERS
	: ambientColorUniform(-1)
	, pointLightingLocationUniform(-1)
	, pointLightingSpecularColorUniform(-1)
	, pointLightingDiffuseColorUniform(-1)
#endif
{
	tower = CTower::instance(); // get tower and camera singleton instances
	theCamera = CCamera::instance();
	theSprites = 0;
}

// destructor
CScene::~CScene()
{
	deleteCurrentScene();
}
CVector4 lightPosition(300.0f, 1000.0f, 0.0f, 1.0f);
CVector4 lightDirection(-2.75f, -2.5f, 0.0f, 0.0f);

#if  (_DEBUG && USE_SHADERS)
bool CSceneVariablesAddedToDebugMenu = false;
#endif

// do any initialisation
bool CScene::initialise(char * backgroundTexture)
{
#if  (_DEBUG && USE_SHADERS)
	if (!CSceneVariablesAddedToDebugMenu)
	{
		CSceneVariablesAddedToDebugMenu = true;
		CVector4 increment(0.25f, 0.25f, 0.25f, 0.25f);
		DebugMenu::DebugMenuItemContainer* container = new DebugMenu::DebugMenuItemContainer("Lights");
			container->AddVariable(new DebugMenu::CVector4DebugMenuItem("Light pos", &lightPosition, increment, -increment));
			container->AddVariable(new DebugMenu::CVector4DebugMenuItem("Light dir", &lightDirection, increment, -increment));

			Globals::Instance().debugMenu.AddVariable(container);
	}
#endif

	if(!skyBox.initialise(backgroundTexture)){
		return( false);} // return false if problems

	theFrustum = CFrustum::instance(); // get frustum singleton pointer

#ifdef USE_SHADERS
	ambientColorUniform = Globals::Instance().gl.GetUniformLocation("uAmbientColor");
    pointLightingLocationUniform = Globals::Instance().gl.GetUniformLocation("uPointLightingLocation");
    pointLightingSpecularColorUniform = Globals::Instance().gl.GetUniformLocation("uPointLightingSpecularColor");
	pointLightingDiffuseColorUniform = Globals::Instance().gl.GetUniformLocation("uPointLightingDiffuseColor");
#endif

	return( true ); // everything initialised succesfully
}

// deletes any memory associated with the current scene
void CScene::deleteCurrentScene()
{
	tower->deleteCurrentTower();
	skyBox.deleteSkyBoard();
}

// position the scene
void CScene::positionCamera(GLfloat timeChangeInSeconds)
{
	// clear the screen and the depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Globals::Instance().viewMatrixStack.LoadIdentity();
	Globals::Instance().modelMatrixStack.LoadIdentity();
	
	theCamera->Update(); // move the camera into position
	theCamera->SetCameraOutputVariables(true);

	theCamera->SetCamera();
	theFrustum->calculateFrustum(); // calculate frustum dimensions

#ifdef USE_SHADERS	
	Globals::Instance().gl.SetUniformVector3(ambientColorUniform, CVector(0.3f, 0.3f, 0.3f));	
	Globals::Instance().gl.SetUniformVector3(pointLightingLocationUniform, theCamera->GetModelViewMatrix()*GetLightPosition());

#ifdef _DEBUG	

	//Globals::Instance().debugDraw.DrawBox(GetLightPosition().v3, 3.0f, CVector4::white);
	//Globals::Instance().debugDraw.DrawBox(GetLightPosition().v3 + (GetLightDirection().v3.normalise()*15.0f), 2.0f, CVector4::red);

	//CMatrix worldBox;
	//CVector radius = Globals::Instance().currentLevel.levelOctree.geometryBoxWidth;
	//worldBox.creatScalingMatrix(radius.x, radius.y, radius.z);
	//worldBox.SetMatrixTranslation(CVector4(Globals::Instance().currentLevel.levelOctree.centre, 1.0f));
	//Globals::Instance().debugDraw.DrawBox(worldBox, CVector4::blue);
#endif

    Globals::Instance().gl.SetUniformVector3(pointLightingSpecularColorUniform, CVector(1.0f, 1.0f, 1.0f));
	Globals::Instance().gl.SetUniformVector3(pointLightingDiffuseColorUniform, CVector(0.3f, 0.3f, 0.3f));
#else
	// position light after moving camera
	GLfloat	 lightPos[] = {30.0f, 100.0f, 0.0f, 0.0f};
	glLightfv(GL_LIGHT0,GL_POSITION,lightPos);
#endif

	skyBox.move(timeChangeInSeconds);
	skyBox.draw();
}

// do a ray/sphere cast against the scene triangles
bool CScene::ShapeCast(const CVector& from, const CVector& to, float radius, unsigned int dynamicTypes, unsigned int spriteTypes, 
					 CVector& outHitPoint, CVector& outHitNormal)
{
	if (dynamicTypes & (1 << MOVING))
	{
		return false; // MOVING Shape casting against MOVING not supported yet
	}

	// get bounding box around from and to positions
	CVector boundingBoxHalfWidths		= (to-from) * 0.5f, 
			boundingBoxCentre	= (from + boundingBoxHalfWidths);

	boundingBoxHalfWidths.makeVectorPositive();
	boundingBoxHalfWidths += CVector(radius, radius, radius); // + sphere cast radius, if doing sphere cast

	bool hasTriangleBeenHit = false;
	float closestHitPointSquared = FLT_MAX;

	const float kLineCastTol = 0.001f;
	bool doLineCast = radius < kLineCastTol;

	if (dynamicTypes & (1 << STATIC_ENVIRONMENT))
	{
		// see which octree end nodes the cast is in
		std::vector<COctree*> endNodesOccupying;
		theSprites->levelOctree->putSpriteIntoEndNode(&boundingBoxHalfWidths, &boundingBoxCentre, endNodesOccupying);

		// go through the octree end nodes which the cast occupys
		int totalEndNodesOccupying = (int)endNodesOccupying.size();

		// set all triangles in all end nodes occupying to not evaluated yet
		for (int endNode = 0; endNode < totalEndNodesOccupying; endNode++)
		{	// referance for speed and readability
			COctree &aNode = *endNodesOccupying[endNode]; 
			int totalNodeTriangles = (int)aNode.nodeTriangles.size();

			for (int nodeTriangle = 0; nodeTriangle < totalNodeTriangles; nodeTriangle++){
				aNode.nodeTriangles[nodeTriangle]->collisionTested = false;}
		}

		for (int endNode = 0; endNode < totalEndNodesOccupying; endNode++)
		{	// referance for speed and readability
			COctree &aNode = *endNodesOccupying[endNode]; 
			int totalNodeTriangles = (int)aNode.nodeTriangles.size();
				
			for (int nodeTriangle = 0; nodeTriangle < totalNodeTriangles; nodeTriangle++)
			{	// referance for speed and readability
				aTriangle &tri = *aNode.nodeTriangles[nodeTriangle]; 

				if (tri.collisionTested) // has the triangle been tested
				{
					continue;
				}
				tri.collisionTested = true; // dont test this triangle again

				if (0 == (spriteTypes & (1 << tri.assocaitedSprite->spriteType)))
				{
					continue; // continue if it's not the type of sprite we're interested in
				}

				CVector verts[] = {	tri.vertices[0], tri.vertices[1], tri.vertices[2]};

				CVector hitPoint;
				CVector hitNormal;
				CVector notUsed;
				bool hit = false;
				if (doLineCast)
				{
					hit = lineSectionTriangleCollision(verts, from, to, hitPoint, hitNormal);
				}
				else // sphere cast
				{
					hit = SweptSphereTriangleCollision(verts, from, to, radius, hitPoint, notUsed, hitNormal);
				}

				if (hit)
				{
					float distanceToHitSquared = (hitPoint-from).getMagnitudeSquared();
					if (!hasTriangleBeenHit || distanceToHitSquared < closestHitPointSquared)
					{
						closestHitPointSquared = distanceToHitSquared;
						hasTriangleBeenHit = true;
						outHitPoint = hitPoint;
						outHitNormal = hitNormal;
					}
				}
			}
		}
	}

	if (dynamicTypes & (1 << MOVING_ENVIRONMENT))
	{
		int totalMovingSprites = (int)theSprites->movingSprites.size(); // get amt of moving sprites
		// go through all the sprites to get all the triangles
		for (int i = 0; i < totalMovingSprites; i++)
		{	
			CSprite& aSprite = *theSprites->movingSprites[i]; // set referance variable to sprite
		
			// continue if not moving environment
			if (aSprite.dynamicProperty != MOVING_ENVIRONMENT){ 
				continue;}

			if (0 == (spriteTypes & (1 << aSprite.spriteType)))
			{
				continue; // continue if it's not the type of sprite we're interested in
			}

			// continue if sprite and moving_environment don't collide
			if (!sphereAxisAlignedBoxIntersection(&boundingBoxCentre, &boundingBoxHalfWidths, &aSprite.position, aSprite.boundingBoxRadius)){
				continue;}

			vector<CVector> meshVertices; /*	vector to store the sprites models 
												vertices once transformed by the sprite
												final matrix */

			// go through all the sprite models meshes
			for (int aMesh = 0; aMesh < aSprite.model->meshNo; aMesh++)
			{	
				mesh &thisMesh = aSprite.model->meshes[aMesh]; // referance variable

				//	go through all the mesh's vertices and transform them by the sprites final matrix, then store in the 'meshVertices' vector
				for (int vertex = 0; vertex < thisMesh.virticesNo; vertex++)
				{
					CVector newVertex; // create new vertex
					meshVertices.push_back(newVertex); // store vertex in vector

					// transform the mesh vertex and store in the vector
					meshVertices[vertex] =	aSprite.finalMatrix.multiplyVector(&
											thisMesh.vertices[vertex].originalXYZ);
				}
			
				// go through this mesh's triangles
				for (int triangle = 0; triangle < thisMesh.trianglesNo; triangle++)
				{	
					// store the triangle vertices required in an array
					CVector verts[] = {	meshVertices[thisMesh.triangles[triangle].vert[0]], 
										meshVertices[thisMesh.triangles[triangle].vert[1]],
										meshVertices[thisMesh.triangles[triangle].vert[2]]};

					CVector hitPoint;
					CVector hitNormal;
					CVector notUsed;
					bool hit = false;
					if (doLineCast)
					{
						hit = lineSectionTriangleCollision(verts, from, to, hitPoint, hitNormal);
					}
					else // sphere cast
					{
						hit = SweptSphereTriangleCollision(verts, from, to, radius, hitPoint, notUsed, hitNormal);
					}

					if (hit)
					{
						float distanceToHitSquared = (hitPoint-from).getMagnitudeSquared();
						if (!hasTriangleBeenHit || distanceToHitSquared < closestHitPointSquared)
						{
							closestHitPointSquared = distanceToHitSquared;
							hasTriangleBeenHit = true;
							outHitPoint = hitPoint;
							outHitNormal = hitNormal;
						}
					}
				}// end for triangle
				meshVertices.clear(); // clear the vector
			}// end for aMesh
		}// end for i
	}
	return hasTriangleBeenHit;
}

// do a ray cast against the scene triangles
bool CScene::RayCast(const CVector& from, const CVector& to, unsigned int dynamicTypes, unsigned int spriteTypes, CVector& outHitPoint, CVector& outHitNormal)
{
	return ShapeCast(from, to, 0.0f, dynamicTypes, spriteTypes, outHitPoint, outHitNormal);
}

// do a sphere cast against the scene triangles
bool CScene::SphereCast(const CVector& from, const CVector& to, float radius, unsigned int dynamicTypes, unsigned int spriteTypes, CVector& outHitPoint, CVector& outHitNormal)
{
	return ShapeCast(from, to, radius, dynamicTypes, spriteTypes, outHitPoint, outHitNormal);
}

CVector4 CScene::GetLightPosition()
{
	return lightPosition;
}

CVector4 CScene::GetLightDirection()
{
	return lightDirection;
}
