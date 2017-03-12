/*--------------------------------------------
CODE BASED ON TUTORIAL AT http://rsn.gamedev.net/tutorials/ms3danim.asp
--------------------------------------------*/

//CModel.cpp, the Model class

//system includes-------
#include <stdio.h> //for 0
//----------------------

//header files----------
#include "CModel.h"
#include "FileReading/texture.h"
#include "Game/Globals.h"
//----------------------

int material::latestSortNumber = 0;

CModel::CModel()
{
	boundingSphereRadUnAnimated = 0.0f;

	// initialise to 0
	totalFrames = currentFrame = meshNo = 
	materialNo = bonesNo = 0;

	meshes = 0;  //mesh dynamic array set to 0
	materials = 0; // material dynamic array to 0
	bones = 0;

	IsTransparent = false; // will be changed to true if any of the model is transparent
	modelDisplayList = 0; /*	will get a value if the model has no bones and so 
								does not move and can thus be drawn with a display 
								list */
}

// destructor
CModel::~CModel()
{
	if (bones) // delete bones (if not 0)
	{
		for (int bone = 0; bone < bonesNo; bone++) // delete each bones component parts 
		{
			if (bones[bone].rotKeyFrames) // delete this bones rotation key frames (if not 0)
			{
				delete[] bones[bone].rotKeyFrames;
				bones[bone].rotKeyFrames = 0;
			}
			if (bones[bone].posKeyFrames) // delete this bones position key frames (if not 0)
			{
				delete[] bones[bone].posKeyFrames;
				bones[bone].posKeyFrames = 0;
			}
		} // end for each bone
	
		// delete the bones dynamic array
		delete[] bones;
		bones = 0;
	}
	if (materials) // delete materials (if not 0)
	{
		for (int material = 0; material < materialNo; material++)
		{
			if (materials[material].clrTexBndNo > 0){ // if their is a texture
				deleteTexture(materials[material].clrTexBndNo);}// release openGL memory
		}// end for materisl

		// delete the materials dynamic array
		delete[] materials;
		materials = 0;
	}

	if (meshes) // (if not 0)
	{
		for (int mesh = 0; mesh < meshNo; mesh++) // delete each mesh's component parts
		{
			if (meshes[mesh].triangles) // delete the meshes triangles (if not 0)
			{
				delete[] meshes[mesh].triangles;
				meshes[mesh].triangles = 0;
			}
			if (meshes[mesh].normals) // delete the meshes normals (if not 0)
			{
				delete[] meshes[mesh].normals;
				meshes[mesh].normals = 0;
			}
			if (meshes[mesh].vertices) // delete the mesh's vertices (if not 0)
			{
				delete[] meshes[mesh].vertices;
				meshes[mesh].vertices = 0;
			}
			if (meshes[mesh].currentVirtices) // delete the mesh's current vertices (if not 0)
			{
				delete[] meshes[mesh].currentVirtices;
				meshes[mesh].currentVirtices = 0;
			}
		}

		// delete the meshes dynamic array 
		delete[] meshes;
		meshes = 0;
	}

#ifndef USE_SHADERS
	glDeleteLists(modelDisplayList, 1); // delete the models display list
#endif
}

// create the models display list
void CModel::createDisplayList( void)
{
#ifdef USE_SHADERS
	CreateMeshBuffers();
#else
	/*	if this model has no bones a display list can be used 
		to render the geometry as it will never move */
	if (0 == bonesNo)
	{
		// generate the models display list					
		modelDisplayList = glGenLists(1); 
		glNewList(modelDisplayList, GL_COMPILE); // create the display list	
			drawArticulatedModel();
		glEndList(); // end creating display list
	}
#endif
}

bool CModel::AreDynamicBuffersInUse()
{
	return bonesNo > 0;
}

#ifdef USE_SHADERS
void CModel::CreateMeshBuffers()
{
	for ( int aMesh = 0; aMesh < meshNo; aMesh++ )
	{ 
		mesh &thisMesh = meshes[aMesh];
		int matIndex = thisMesh.matIndex;
		if (matIndex >= 0 ) // if their is a texture for this mesh
		{
			thisMesh.meshBuffer.SetMaterial(materials[matIndex]);
		}

		const int NumFloatsPerVert = 8;
		const int NumVertsPerTriangle = 3;
		thisMesh.meshBuffer.CreateVertexArray((float*)thisMesh.currentVirtices, thisMesh.trianglesNo * NumFloatsPerVert * NumVertsPerTriangle, true);
	}// end for meshes
}

void CModel::DrawUsingMeshBuffers()
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // blend type (for transparency)
	Globals::Instance().gl.GLEnable(GL_BLEND); // enable blending (for transparency)

	// draw all the meshes
	for ( int aMesh = 0; aMesh < meshNo; aMesh++ )
	{ 
		mesh &thisMesh = meshes[aMesh];

		// go through the vertices and translate and rotate them into position
		for (int aVertex = 0; aVertex < thisMesh.virticesNo; aVertex++)
		{
			vertex &thisVertex	= thisMesh.vertices[aVertex];
			int thisBone		= thisVertex.boneIndex;
			// continue if no bone for vertex
			if (-1 == thisBone){ continue;}

			// translate and rotate the vertex by its bones rotation and transformation
			thisVertex.currentXYZ = 
				bones[thisBone].final.multiplyVector(&thisVertex.originalXYZ);
		}

		// go through all the triangles
		for ( int tri = 0; tri < thisMesh.trianglesNo; tri++ )
		{	// referance variable for speed and readability
			triangle &thisTri	= thisMesh.triangles[tri]; 
			int triMultipliedByThree = (tri*3);
			// go through three triangle vertices
			for ( int aVertex = 0; aVertex < 3; aVertex++ )
			{	// referance variables for speed and readability
				int vertexRequired	= thisTri.vert[aVertex];
				vertex &thisVertex	= thisMesh.vertices[vertexRequired];
				// if their is a bone
				if (-1 != thisVertex.boneIndex)
				{	// referance variable
					singleVertex &vert = thisMesh.currentVirtices[triMultipliedByThree+aVertex];
					/*	get the current vertex (incorporating rotation and translation) from
						the vertex structure */
					vert.vertex = thisVertex.currentXYZ;
						
					/*	rotate this vertices normal according to the vertices associated bones final 
						matrix (no need to translate as normal has no position) */
					vert.Normal = 
						bones[thisVertex.boneIndex].final.multiplyRotateVector(&thisTri.normals[aVertex]);
					// ensure the normal is still normalised
					// vert.Normal.normalise(); 
				}// end else a bone exists
			} // end for triangle vertices
		} // end for mesh triangles

		// update vertex buffer with latest vert info
		thisMesh.meshBuffer.UpdateVertexArray((float*)thisMesh.currentVirtices);

		// draw all the virtices for this mesh using vertex arrays
		thisMesh.meshBuffer.Draw();
	}// end for meshes

	Globals::Instance().gl.GLDisable(GL_BLEND);
}
#endif

// public draw method which choses between display list and normal drawing
void CModel::draw()
{
#ifdef USE_SHADERS
	DrawUsingMeshBuffers();
#else
	if(0 == bonesNo){ // if the model has no bones 
		drawModelUsingDisplayList();} // draw using precompiled display list
	else{ // the model has bones 
		drawArticulatedModel();} // draw the articulated figure moving all the vertices
#endif
}

/*	private draw method, draws a model with no bones (so will not move)
	using a display list */
void CModel::drawModelUsingDisplayList( void)
{
#ifndef USE_SHADERS
	glCallList(modelDisplayList);
#endif
}

// private draw method, draws an articulated figure (one which has bones)
void CModel::drawArticulatedModel( void)
{
	// see if texture is already enabled so context can be returned to
	GLboolean texEnabled = glIsEnabled(GL_TEXTURE_2D);

	// draw all the meshes
	for ( int aMesh = 0; aMesh < meshNo; aMesh++ )
	{ 
		mesh &thisMesh = meshes[aMesh];
		int matIndex = thisMesh.matIndex;
		if (matIndex >= 0 ) // if their is a texture for this mesh
		{
			material &aMaterial = materials[matIndex]; // referance variable (speed and readability)
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // blend type (for transparency)
			Globals::Instance().gl.GLEnable(GL_BLEND); // enable blending (for transparency)
			glMaterialfv( GL_FRONT, GL_AMBIENT, aMaterial.ambient);
			glMaterialfv( GL_FRONT, GL_DIFFUSE, aMaterial.diffuse);
			glMaterialfv( GL_FRONT, GL_SPECULAR, aMaterial.specular);
			glMaterialfv( GL_FRONT, GL_EMISSION, aMaterial.emissive);
			glMaterialf( GL_FRONT, GL_SHININESS, aMaterial.shinnines);

			if(aMaterial.clrTexBndNo > 0) // if this material has a texture
			{	// bind texture
				glBindTexture(GL_TEXTURE_2D, aMaterial.clrTexBndNo);
				Globals::Instance().gl.GLEnable(GL_TEXTURE_2D); // enable texturing
			}
			else{ // material does not have a texture
				Globals::Instance().gl.GLDisable(GL_TEXTURE_2D);}
		}
		else{ // no texture for this mesh
			Globals::Instance().gl.GLDisable(GL_TEXTURE_2D);}

		// go through the vertices and translate and rotate them into position
		for (int aVertex = 0; aVertex < thisMesh.virticesNo; aVertex++)
		{
			vertex &thisVertex	= thisMesh.vertices[aVertex];
			int thisBone		= thisVertex.boneIndex;
			// continue if no bone for vertex
			if (-1 == thisBone){ continue;}

			// translate and rotate the vertex by its bones rotation and transformation
			thisVertex.currentXYZ = 
				bones[thisBone].final.multiplyVector(&thisVertex.originalXYZ);
		}

		// go through all the triangles
		for ( int tri = 0; tri < thisMesh.trianglesNo; tri++ )
		{	// referance variable for speed and readability
			triangle &thisTri	= thisMesh.triangles[tri]; 
			int triMultipliedByThree = (tri*3);
			// go through three triangle vertices
			for ( int aVertex = 0; aVertex < 3; aVertex++ )
			{	// referance variables for speed and readability
				int vertexRequired	= thisTri.vert[aVertex];
				vertex &thisVertex	= thisMesh.vertices[vertexRequired];
				// if their is a bone
				if (-1 != thisVertex.boneIndex)
				{	// referance variable
					singleVertex &vert = thisMesh.currentVirtices[triMultipliedByThree+aVertex];
					/*	get the current vertex (incorporating rotation and translation) from
						the vertex structure */
					vert.vertex = thisVertex.currentXYZ;
						
					/*	rotate this vertices normal according to the vertices associated bones final 
						matrix (no need to translate as normal has no position) */
					vert.Normal = 
						bones[thisVertex.boneIndex].final.multiplyRotateVector(&thisTri.normals[aVertex]);
					// ensure the normal is still normalised
					// vert.Normal.normalise(); 
				}// end else a bone exists
			} // end for triangle vertices
		} // end for mesh triangles

		// draw all the virtices for this mesh using vertex arrays
		glInterleavedArrays(GL_T2F_N3F_V3F, sizeof(singleVertex), thisMesh.currentVirtices);
		glDrawArrays(GL_TRIANGLES, 0, thisMesh.trianglesNo*3);
	}// end for meshes

	// if texture was initially enabled before entering this function
	if (texEnabled){ 
		Globals::Instance().gl.GLEnable(GL_TEXTURE_2D);} // re-enable texturing
	else{ // texture was not initially enabled
		Globals::Instance().gl.GLDisable(GL_TEXTURE_2D);} // disable texturing

	Globals::Instance().gl.GLDisable(GL_BLEND); // disable blending
} // end CModel::draw()

// restart the animation
void CModel::restart() 
{	// go through all the bones
	for (int bone = 0; bone < bonesNo; bone++) 
	{	// set back to key frame 0
		bones[bone].currentRotationKeyFrame = bones[bone].currentTranslationKeyFrame = 0;
		// set final matrix to absolute matrix, thus loosing the changes occuring during
		// the animation
		bones[bone].final = bones[bone].absolute;
	}// end for bones
}// end CModel::restart() 

void CModel::advanceFrame(GLuint elapsedTime, GLfloat &frame, GLuint frameRate,
									bool loop, int startingFrame, int endingFrame, float *amtOver)
{
	/*	if ending frame has the default parameter of -1, this means no parameter has been 
		explicitly given, in this case set ending frame to the total frames value of the 
		model */
	endingFrame = (-1 == endingFrame) ? totalFrames : endingFrame;

	// set frame to starting frame if it is less than starting frame
	frame = (frame < startingFrame) ? startingFrame : frame;

	/*	if the frame rate is not equal to 0 work out what frame the animation is now upto.
		if the frame rate is zero, no frames would have elapsed (also dividing by zero can 
		cause problems)*/
	if (0 != frameRate)
	{
		float frameTime = 1000.0f/frameRate; //work out frame interval (in milliseconds)
		// add on the percentage of a frame which has elapsed 
		frame += ((float)elapsedTime)/frameTime; 
	}

	// if e.g the frame has reached 30.5 and their are only 30 frames in this animation
	if (frame > endingFrame) // totalFrames) 
	{
		if (loop){ // if the animation should loop
			/*	start on the amount by which the ending frame value has been exceeded by
				e.g start on 0.5 if ending frame have been exceeded by 0.5 (30.5-30.0) 
				and startingFrame is 0 */
			frame = startingFrame + (frame-(float)endingFrame);}
		else{ // not looping
		{
			// set the amount by which the end frame has been exceeded
			if (amtOver != 0) {*amtOver = (frame-(float)endingFrame);}
			frame = (float)endingFrame;} // stay on last frame
		}
	}// end if total frames have been exceed
}

void CModel::setBonesAtFrame(GLfloat frame)
{
	// rotate and position all the bones
	for (int bone = 0; bone < bonesNo; bone++) 
	{
		CVector translation; // keeps the current translation at the current frame time
		CMatrix rotation;	// keeps the current rotation at the current frame time
		
		// if their are position Globals::Instance().keys 
		if (bones[bone].positionKeysAmt != 0)
		{
			// search for position Globals::Instance().keys
			int posKeyInterpolatingFrom = -1, posKeyInterpolatingTo = -1;
			/*	go through the position Globals::Instance().keys to find the key interpolating from
				and the key interpolating to */
		
			for (int posKey = 0; posKey < bones[bone].positionKeysAmt; posKey++)
			{	/*	if this is the first key found which is after the current time 
					in the animation then this key is being interpolated towards */
				if (bones[bone].posKeyFrames[posKey].time >= frame)
				{
					posKeyInterpolatingTo = posKey; // set key
					break; // break out of for loop
				}
				posKeyInterpolatingFrom = posKey; /*	this key is before the current stage in 
														the animation.  When the key interpolating
														to is found this will be left set as the
														key directly previous to the current stage
														in the animation */
			}// end for posKeys

			/*	if both Globals::Instance().keys have been set (i.e both not equal to -1) their are more than one key frames
				in the animation and interpolation between these frames can occur */
			if (posKeyInterpolatingFrom != -1 && posKeyInterpolatingTo != -1)
			{	// referance variables for readability
				keyFrame	&to = bones[bone].posKeyFrames[posKeyInterpolatingTo],
							&from = bones[bone].posKeyFrames[posKeyInterpolatingFrom];
	
				// work out difference in time between frames being interpolated between
				float delta = to.time - from.time;
					/*	work out the percentage of the way between the interpolated frames the current 
				frame is (ends with value between 0 - 1) */
				float perCent = (frame - from.time) / delta;
				// work out the x, y, z translation positions for the current time in the animation
				translation.setX(from.xyz.getX() + (to.xyz.getX() - from.xyz.getX()) * perCent);
				translation.setY(from.xyz.getY() + (to.xyz.getY() - from.xyz.getY()) * perCent);
				translation.setZ(from.xyz.getZ() + (to.xyz.getZ() - from.xyz.getZ()) * perCent);
			}// end if both Globals::Instance().keys set
			/*	if their is only one key frame, the final frame has been passed or the first frame has not
				been reached, the animation will not move.  The following two else if 
				statements see which key frame has been set (to or from) and the translation
				vector is set accordingly */
			else if (posKeyInterpolatingFrom == -1) /*	if their is only one key frame and it is 
														after the current time in the animation (or the first
														frame has not been reached) */
			{	// from is not set so to must be
				translation = bones[bone].posKeyFrames[posKeyInterpolatingTo].xyz;
			} // end if to set
			else if (posKeyInterpolatingTo == -1) /*	if their is only one frame and it is before 
														the current time in the animation (or the final frame 
														has been passed) */
			{	// to is not set so from must be
				translation = bones[bone].posKeyFrames[posKeyInterpolatingFrom].xyz;
			}// end if from set
		}// enf if their are position Globals::Instance().keys

		// if their are rotation Globals::Instance().keys
		if (bones[bone].rotationKeysAmt != 0) 
		{
			// search for rotation Globals::Instance().keys
			int rotKeyInterpolatingFrom = -1, rotKeyInterpolatingTo = -1;
			// go through the rotation Globals::Instance().keys to find the key interpolating from
			// and the key interpolating to
			for (int rotKey = 0; rotKey < bones[bone].rotationKeysAmt; rotKey++)
			{	/*	if this is the first key found which is after the current time 
					in the animation then this key is being interpolated towards */
				if (bones[bone].rotKeyFrames[rotKey].time >= frame)
				{
					rotKeyInterpolatingTo = rotKey; // set key
					break; // break out of for loop
				}
				rotKeyInterpolatingFrom = rotKey; /*	this key is before the current stage in 
														the animation.  When the key interpolating
														to is found this will be left set as the
														key directly previous to the current stage
														in the animation */
			}// end for rotKeys
	
			/*	if both Globals::Instance().keys have been set (i.e both not equal to -1) their are more than one key 
				frames in the animation and interpolation between these frames can occur */
			if (rotKeyInterpolatingFrom != -1 && rotKeyInterpolatingTo != -1)
			{	// referance variables for readability
				keyFrame	&to = bones[bone].rotKeyFrames[rotKeyInterpolatingTo],
							&from = bones[bone].rotKeyFrames[rotKeyInterpolatingFrom];

				// work out difference in time between frames being interpolated between
				float delta = to.time - from.time;
				/*	work out the percentage of the way between the interpolated frames the current 
					frame is (ends with value between 0 - 1) */
				float perCent = (frame - from.time) / delta;
	
				CQuaternion lastQuat, toQuat, finalQuat;
				// make a quaternion representing the 'from' frames rotation
				lastQuat.makeQuaternionFromEularAngles(&from.xyz);
				// make a quaternion representing the 'to' frmaes rotation
				toQuat.makeQuaternionFromEularAngles(&to.xyz);
				// slerp between the 'from' and 'to' quaternions
				finalQuat.quaternionSlerp(&lastQuat, &toQuat, perCent);
				// turn the final quaternion resulting from the slerp to a matrix
				rotation.quaternionToMatrix(finalQuat);
			}// end if both Globals::Instance().keys set
	
			else if (rotKeyInterpolatingFrom == -1){ /*	if their is only one key frame and it is 
														after the current time in the animation */
				// from is not set so to must be
				rotation.createXYZRotationMatrix(&bones[bone].rotKeyFrames[rotKeyInterpolatingTo].xyz);}
			else if (rotKeyInterpolatingTo == -1){	/*	if their is only one key frame and it is 
														before the current time in the animation */
				// to is not set so from must be
				rotation.createXYZRotationMatrix(&bones[bone].rotKeyFrames[rotKeyInterpolatingFrom].xyz);}
		}// end if their are rotation Globals::Instance().keys

		rotation.SetMatrixTranslation(CVector4(translation, 1.0f));

		CMatrix relativeFinal = bones[bone].relative;
		relativeFinal*=&rotation; // add in the additional rotation and translation
	
		/*	the following if statement relies on the assumption that the rotation and 
			translation of bones higher in the bone hierarchy will be computed before 
			bones lower in the hierarchy.  This is so bones lower in the hierarchy will
			be able to multiply in the final matrices of its parent bone, which in turn 
			will be an accumulation of its parent bones, which should all be correct for 
			the current frame */

		if ( bones[bone].parentBone == -1 ){ // no parent
			bones[bone].final = relativeFinal;} // no parent matrix to multiply in
		else{ // this bone has a parent
			// multiply by parents final matrix
			bones[bone].final = bones[bones[bone].parentBone].final;
			//multiply in the relativeFinal matrix
			bones[bone].final*=&relativeFinal;}
	}// for bones
}
