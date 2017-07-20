/*--------------------------------------------
CODE BASED ON TUTORIAL AT http://rsn.gamedev.net/tutorials/ms3danim.asp

AND TUTORIAL AT http://hsfortuna.pwp.blueyonder.co.uk
--------------------------------------------*/

//CModelLoader.cpp, the CModelLoader class

//system includes-------
#include <stdio.h>
#include <string.h> // for strcmp (string comparison)
//----------------------

//header files----------
#include "CModelLoader.h"
#include "FileReading/texture.h" // REUSED CLASS FROM OPENGL SUPERBIBLE - sets up textures
#include "Game/defines.h"
#include <System/Interfaces/SystemInterface.h>
//----------------------

extern GLuint TextureLoad(const char *filename, bool alpha, GLenum minfilter, GLenum magfilter, GLenum wrap);

//defines---------------
#define DELETE_MODEL(MODEL) delete MODEL; MODEL = 0;
//----------------------

CModelLoader::CModelLoader()
{
}

/*	load the model file sent as a parameter and copy
	to the CModel pointer sent as a parameter */
int CModelLoader::load(char* filename, CModel **model)
{
	CTextFileReader fileReader; // used to read a text file
	char buffer[256]; // char buffer holds the model filename

    SPRINTF(buffer, "%smodels/%s.txt", GetDirectoryPath(), filename); // create file name with path
	
	if (!fileReader.openFile(buffer, ios::in)){
		return(false);} // return false if file could not be opened

	CModel * aModel = new CModel();

	fileReader.getNextLine(); /*	returns the next line which is not blank
									or a comment line */
	 // get total frames
    SSCANF(fileReader.currentLineOfFile, "Frames: %d", &aModel->totalFrames);

	fileReader.getNextLine();
    SSCANF(fileReader.currentLineOfFile, "Frame: %d", &aModel->currentFrame); // get current frame

	fileReader.getNextLine();
    SSCANF(fileReader.currentLineOfFile, "Meshes: %d", &aModel->meshNo); // get number of meshes

	// delete anything in the array
	if(aModel->meshes)delete[]aModel->meshes;
	
	aModel->meshes = new mesh[aModel->meshNo];		// allocate memory for the meshes

	if(!aModel->meshes){ 
		// memory allocation failed so return false
		DELETE_MODEL(aModel) fileReader.closeFile(); return(false);} 

	fileReader.getNextLine();
	// go through all the meshes and initialise dynamic arrays
	for (int mesh = 0; mesh < aModel->meshNo; mesh++)
	{
		// initialise dynamic arrays to 0
		aModel->meshes[mesh].normals = 0;
		aModel->meshes[mesh].triangles = 0;
		aModel->meshes[mesh].vertices = 0;
		aModel->meshes[mesh].currentVirtices = 0;
	} // end for mesh dynamic array set-up

	CVector tempMostRemoteVirtex, // keeps track of most remote virtex for sphere radius
			tempBoundingBoxMostRemoteVertex; /* keeps track of the most remote x, y, z 
												measurements for the model bounding box */

	// go through all the meshes and set up parameters
	for (int mesh = 0; mesh < aModel->meshNo; mesh++)
	{
		// get this meshes name, flags and material index.
		// return false if any fields are not set
        
        int res = SSCANF(fileReader.currentLineOfFile, "\"%[^\"]\" %d %d", SSCANF_STR_PARAM(aModel->meshes[mesh].name, mesh::kNameStrLen), &aModel->meshes[mesh].flags, &aModel->meshes[mesh].matIndex);
		if (res != 3){
			DELETE_MODEL(aModel) fileReader.closeFile(); return(false);}

		fileReader.getNextLine();
		// return false if no fields set
        res = SSCANF(fileReader.currentLineOfFile, "%d", &aModel->meshes[mesh].virticesNo);
		if (res != 1){ /*	set virtices
																									no for this
																									mesh */
			DELETE_MODEL(aModel) fileReader.closeFile(); return(false);}

		// delete anything in the array
		if(aModel->meshes[mesh].vertices)delete[]aModel->meshes[mesh].vertices;

		// allocate memory for this mesh's vertices
		aModel->meshes[mesh].vertices = new vertex[aModel->meshes[mesh].virticesNo];
		fileReader.getNextLine();

		if(!aModel->meshes[mesh].vertices){ 
			// memory allocation failed so return false
			DELETE_MODEL(aModel) fileReader.closeFile(); return(false);}

		// get all the vertices for this mesh
		for (int vertex = 0; vertex < aModel->meshes[mesh].virticesNo; vertex++)
		{
            res = SSCANF(fileReader.currentLineOfFile, "%d %f %f %f %f %f %d",
             &aModel->meshes[mesh].vertices[vertex].flags,
             &aModel->meshes[mesh].vertices[vertex].originalXYZ.x,
             &aModel->meshes[mesh].vertices[vertex].originalXYZ.y,
             &aModel->meshes[mesh].vertices[vertex].originalXYZ.z,
             &aModel->meshes[mesh].vertices[vertex].u,
             &aModel->meshes[mesh].vertices[vertex].v,
             &aModel->meshes[mesh].vertices[vertex].boneIndex);
            
			if (res != 7)
            { // return false if their has been an error
				DELETE_MODEL(aModel) fileReader.closeFile(); return(false);}
			fileReader.getNextLine();

			/*	set current to original xyz vertices (the original xyz vertices are inversely 
				rotated (in setUpBones below) by their associated bone if they have one, the 
				current xyz is then updated at each frame of the programme.  If the vertex has 
				no bone, current and original xyz will always stay the same */
			aModel->meshes[mesh].vertices[vertex].currentXYZ = 
				aModel->meshes[mesh].vertices[vertex].originalXYZ;

			/*	keep track of which virtex is the most remote in order to find the 
				radius of the models bounding sphere */
			if ( aModel->meshes[mesh].vertices[vertex].originalXYZ.getMagnitude() > 
				tempMostRemoteVirtex.getMagnitude() ){
				tempMostRemoteVirtex = aModel->meshes[mesh].vertices[vertex].originalXYZ;}

			// find the models bounding box
			CVector tempPositive = aModel->meshes[mesh].vertices[vertex].originalXYZ;
			tempPositive.makeVectorPositive();
			if (tempPositive.x > tempBoundingBoxMostRemoteVertex.x){ 
				tempBoundingBoxMostRemoteVertex.x = tempPositive.x;}
			if (tempPositive.y > tempBoundingBoxMostRemoteVertex.y){ 
				tempBoundingBoxMostRemoteVertex.y = tempPositive.y;}
			if (tempPositive.z > tempBoundingBoxMostRemoteVertex.z){ 
				tempBoundingBoxMostRemoteVertex.z = tempPositive.z;}
		}
        
        res = SSCANF(fileReader.currentLineOfFile, "%d", &aModel->meshes[mesh].normalsNo);
		if (res !=1){ /*	set normals
																			no for this
																			mesh */
			DELETE_MODEL(aModel) fileReader.closeFile(); return(false);}

		// delete anything in the array
		if(aModel->meshes[mesh].normals)delete[]aModel->meshes[mesh].normals;
		// allocate memory for this meshes normal vectors
		aModel->meshes[mesh].normals = new CVector[aModel->meshes[mesh].normalsNo];

		if(!aModel->meshes[mesh].normals){ 
			// memory allocation failed so return false
			DELETE_MODEL(aModel) fileReader.closeFile(); return(false);}
		
		fileReader.getNextLine();

		// get all the normals
		for (int normal = 0; normal < aModel->meshes[mesh].normalsNo; normal++)
		{
            res = SSCANF(fileReader.currentLineOfFile, "%f %f %f", &aModel->meshes[mesh].normals[normal].x,
             &aModel->meshes[mesh].normals[normal].y,
             &aModel->meshes[mesh].normals[normal].z);
			if (res != 3)
            { // return false if their has been an error
				DELETE_MODEL(aModel) fileReader.closeFile(); return(false);}
			fileReader.getNextLine();
		}
        
        res = SSCANF(fileReader.currentLineOfFile, "%d", &aModel->meshes[mesh].trianglesNo);
		// set triangle no
		if (res != 1){
            DELETE_MODEL(aModel) fileReader.closeFile(); return(false);}

		// delete anything in the array
		if(aModel->meshes[mesh].triangles){ delete[]aModel->meshes[mesh].triangles;}
		// allocate memory for the triangles
		aModel->meshes[mesh].triangles = new triangle[aModel->meshes[mesh].trianglesNo];

		if(!aModel->meshes[mesh].triangles){ 
			// memory allocation failed so return false
			DELETE_MODEL(aModel) fileReader.closeFile(); return(false);}

		// delete anything in the array
		if(aModel->meshes[mesh].currentVirtices){ 
			delete[]aModel->meshes[mesh].currentVirtices;}
		// allocate memory for the currentVirtices
		aModel->meshes[mesh].currentVirtices = 
			new singleVertex[aModel->meshes[mesh].trianglesNo*3];

		if(!aModel->meshes[mesh].currentVirtices){ 
			// memory allocation failed so return false
			DELETE_MODEL(aModel) fileReader.closeFile(); return(false);}
		
		fileReader.getNextLine();
		// get all the triangles
		for (int triangle = 0; triangle < aModel->meshes[mesh].trianglesNo; triangle++)
		{
            res = SSCANF(fileReader.currentLineOfFile, "%d %d %d %d %d %d %d %d",
             &aModel->meshes[mesh].triangles[triangle].flags,
             &aModel->meshes[mesh].triangles[triangle].vert[0],
             &aModel->meshes[mesh].triangles[triangle].vert[1],
             &aModel->meshes[mesh].triangles[triangle].vert[2],
             &aModel->meshes[mesh].triangles[triangle].norm[0],
             &aModel->meshes[mesh].triangles[triangle].norm[1],
             &aModel->meshes[mesh].triangles[triangle].norm[2],
             &aModel->meshes[mesh].triangles[triangle].smoothinggroup);
            
			if (res != 8){
				DELETE_MODEL(aModel) fileReader.closeFile(); return(false);}
			fileReader.getNextLine();
			
			// initialise the current triangle vertex values
			for (int verts = 0; verts < 3; verts++)
			{
				aModel->meshes[mesh].triangles[triangle].currentVertices[verts] = 
				aModel->meshes[mesh].vertices[aModel->meshes[mesh].triangles[triangle].vert[verts]].originalXYZ;

				// fill in each vertex structure
				aModel->meshes[mesh].currentVirtices[(triangle*3)+verts].vertex = 
				aModel->meshes[mesh].vertices[aModel->meshes[mesh].triangles[triangle].vert[verts]].originalXYZ;

				aModel->meshes[mesh].currentVirtices[(triangle*3)+verts].u = 
				aModel->meshes[mesh].vertices[aModel->meshes[mesh].triangles[triangle].vert[verts]].u;
				
				aModel->meshes[mesh].currentVirtices[(triangle*3)+verts].v = 
				aModel->meshes[mesh].vertices[aModel->meshes[mesh].triangles[triangle].vert[verts]].v;
			}

			// fill in normals and currentNormals in CVectors in triangle structure
			for (int norms = 0; norms < 3; norms++)
			{
				aModel->meshes[mesh].triangles[triangle].normals[norms] =   
				aModel->meshes[mesh].normals[aModel->meshes[mesh].triangles[triangle].norm[norms]];

				aModel->meshes[mesh].triangles[triangle].currentNormals[norms] = 
					aModel->meshes[mesh].triangles[triangle].normals[norms];

				// fill in each vertex structure
				aModel->meshes[mesh].currentVirtices[(triangle*3)+norms].Normal = 
					aModel->meshes[mesh].triangles[triangle].normals[norms];
			} // end for normals
		}// end for triangles
	} // end for all the meshes
	
	// set models bounding sphere and rectangle
	aModel->boundingSphereRadUnAnimated = tempMostRemoteVirtex.getMagnitude();
	aModel->boundingRectangleUnAnimated = (tempBoundingBoxMostRemoteVertex*2.0f);
	
    int res = SSCANF(fileReader.currentLineOfFile, "Materials: %d", &aModel->materialNo);
	// set material number
	if (res != 1){
		DELETE_MODEL(aModel) fileReader.closeFile(); return(false);}
	if (aModel->materialNo > 0) // alloc memory for the materials
	{
		// delete anything in the array
		if(aModel->materials)delete[]aModel->materials;
		aModel->materials = new material[aModel->materialNo];
		if(!aModel->materials){ 
			// memory allocation failed so return false
			DELETE_MODEL(aModel) fileReader.closeFile(); return(false);}
	} 

	fileReader.getNextLine();
	
	// get all the materials
	for (int material = 0; material < aModel->materialNo; material++)
	{ // get material name
        res = SSCANF(fileReader.currentLineOfFile, "\"%[^\"]\"", SSCANF_STR_PARAM(aModel->materials[material].name, material::kStrLen));
		if (res != 1){
			DELETE_MODEL(aModel) fileReader.closeFile(); return(false);}
		fileReader.getNextLine();

        res = SSCANF(fileReader.currentLineOfFile, "%f %f %f %f",	&aModel->materials[material].ambient[0],
         &aModel->materials[material].ambient[1],
         &aModel->materials[material].ambient[2],
         &aModel->materials[material].ambient[3]);
		//get ambient values
		if (res != 4){
			DELETE_MODEL(aModel) fileReader.closeFile(); return(false);}
		fileReader.getNextLine();
		
        res = SSCANF(fileReader.currentLineOfFile, "%f %f %f %f",	&aModel->materials[material].diffuse[0],
         &aModel->materials[material].diffuse[1],
         &aModel->materials[material].diffuse[2],
         &aModel->materials[material].diffuse[3]);
		//get diffuse values
		if (res != 4){
			DELETE_MODEL(aModel) fileReader.closeFile(); return(false);}
		fileReader.getNextLine();
		
		//get specular values
        res = SSCANF(fileReader.currentLineOfFile, "%f %f %f %f",	&aModel->materials[material].specular[0],
         &aModel->materials[material].specular[1],
         &aModel->materials[material].specular[2],
         &aModel->materials[material].specular[3]);
		if (res != 4){
			DELETE_MODEL(aModel) fileReader.closeFile(); return(false);}

		fileReader.getNextLine();

		//get emissive values
        res = SSCANF(fileReader.currentLineOfFile, "%f %f %f %f",	&aModel->materials[material].emissive[0],
         &aModel->materials[material].emissive[1],
         &aModel->materials[material].emissive[2],
         &aModel->materials[material].emissive[3]);
		if (res != 4){
			DELETE_MODEL(aModel) fileReader.closeFile(); return(false);}

		fileReader.getNextLine();
		
		// get shinniness value
        res = SSCANF(fileReader.currentLineOfFile, "%f",	&aModel->materials[material].shinnines);
		if (res != 1){
			DELETE_MODEL(aModel) fileReader.closeFile(); return(false);}

#ifdef USE_SHADERS
		aModel->materials[material].ambient[0] *= 0.5f;
		aModel->materials[material].ambient[1] *= 0.5f; 
		aModel->materials[material].ambient[2] *= 0.5f;
		aModel->materials[material].ambient[3] *= 0.5f;

		aModel->materials[material].emissive[0] = aModel->materials[material].diffuse[0];
		aModel->materials[material].emissive[1] = aModel->materials[material].diffuse[1];
		aModel->materials[material].emissive[2] = aModel->materials[material].diffuse[2];
		aModel->materials[material].emissive[3] = aModel->materials[material].diffuse[3];

		aModel->materials[material].specular[0] = aModel->materials[material].specular[1] = 
			aModel->materials[material].specular[2] = aModel->materials[material].specular[3] = 1.0f;

		aModel->materials[material].shinnines = 1.652f;
#endif

		fileReader.getNextLine();
		
		// get transparency value
        res = SSCANF(fileReader.currentLineOfFile, "%f",	&aModel->materials[material].transparency);
		if (res != 1){
			DELETE_MODEL(aModel) fileReader.closeFile(); return(false);}
		fileReader.getNextLine();

		// if the transparency value is less than 1.0f, the model will be partially transparent
		if (aModel->materials[material].transparency < 1.0f){
			aModel->IsTransparent = true;} // model is partially transparent
		
        defines::ReverseString(fileReader.currentLineOfFile); // reverse the latest line of file
		
		// get colour map diffuse texture, get just filename at end of line (not path)
        defines::CopyString (aModel->materials[material].clrMapTex, "", sizeof(aModel->materials[material].clrMapTex)); // set to null in case no tex

		SSCANF(fileReader.currentLineOfFile, "\"%[^\\\"]", SSCANF_STR_PARAM(aModel->materials[material].clrMapTex, material::kStrLen));
        defines::ReverseString(aModel->materials[material].clrMapTex); // reverse string so filename is not reversed
		fileReader.getNextLine();

		// get alpha map texture, initially set to null in case their's no alpha map tex
        defines::CopyString(aModel->materials[material].alphaMapTex, "", sizeof(aModel->materials[material].alphaMapTex));

		SSCANF(fileReader.currentLineOfFile, "\"%[^\"]\"", SSCANF_STR_PARAM(aModel->materials[material].alphaMapTex, material::kStrLen));
		fileReader.getNextLine();
	} // end for all materials
	
	if (!reloadTextures(aModel)){ // if all the textures did not load succesfully
		DELETE_MODEL(aModel) fileReader.closeFile(); return(false);} // delete model and return false

	// set the number of bones
    res = SSCANF(fileReader.currentLineOfFile, "Bones: %d", &aModel->bonesNo);
	if (res != 1){
		DELETE_MODEL(aModel) fileReader.closeFile(); return(false);}
	if (aModel->bonesNo > 0) // allocate memory for bone structs
	{
		// delete anything in the array
		if(aModel->bones)delete[]aModel->bones;
		aModel->bones = new bone[aModel->bonesNo];
		if(!aModel->bones){ 
			// memory allocation failed so return false
			DELETE_MODEL(aModel) fileReader.closeFile(); return(false);}
	}

	fileReader.getNextLine();

	// initialise bone dynamic arrays
	for (int bone = 0; bone < aModel->bonesNo; bone++)
	{	
		// dynamic arrays to 0
		aModel->bones[bone].posKeyFrames =
		aModel->bones[bone].rotKeyFrames = 0;
	} // end for bone dynamic array set-up

	// go through all the bones
	for (int bone = 0; bone < aModel->bonesNo; bone++)
	{
		// get bone name
        res = SSCANF(fileReader.currentLineOfFile, "\"%[^\"]\"", SSCANF_STR_PARAM(aModel->bones[bone].name, bone::kStrLen));
		if (res != 1){
			DELETE_MODEL(aModel) fileReader.closeFile(); return(false);}
		fileReader.getNextLine();

		// get parent name, initially set to null
        defines::CopyString (aModel->bones[bone].parentName, "", sizeof(aModel->bones[bone].parentName));
		SSCANF(fileReader.currentLineOfFile, "\"%[^\"]\"", SSCANF_STR_PARAM(aModel->bones[bone].parentName, bone::kStrLen));
		fileReader.getNextLine();
		
		// get bone information
        res = SSCANF(fileReader.currentLineOfFile, "%d %f %f %f %f %f %f", &aModel->bones[bone].flags,
         &aModel->bones[bone].posXYZ.x,
         &aModel->bones[bone].posXYZ.y,
         &aModel->bones[bone].posXYZ.z,
         &aModel->bones[bone].rotXYZ.x,
         &aModel->bones[bone].rotXYZ.y,
         &aModel->bones[bone].rotXYZ.z
         );
		if (res != 7){
			DELETE_MODEL(aModel) fileReader.closeFile(); return(false);}
		fileReader.getNextLine();

		// get number of position Globals::Instance().keys
        res = SSCANF(fileReader.currentLineOfFile, "%d", &aModel->bones[bone].positionKeysAmt);
		if (res != 1){
			DELETE_MODEL(aModel) fileReader.closeFile(); return(false);}
		// allocate memory for the position Globals::Instance().keys
		if (aModel->bones[bone].positionKeysAmt > 0)
		{
			// delete anything in the array
			if(aModel->bones[bone].posKeyFrames)delete[]aModel->bones[bone].posKeyFrames;
			
			aModel->bones[bone].posKeyFrames = 
				new keyFrame[aModel->bones[bone].positionKeysAmt];}

			if(!aModel->bones[bone].posKeyFrames){ 
				// memory allocation failed so return false
				DELETE_MODEL(aModel) fileReader.closeFile(); return(false);
		}

		fileReader.getNextLine();
		// get all the position Globals::Instance().keys
		for (int posKey = 0; posKey < aModel->bones[bone].positionKeysAmt; posKey++)
		{
            res = SSCANF(fileReader.currentLineOfFile, "%f %f %f %f",	&aModel->bones[bone].posKeyFrames[posKey].time,
             &aModel->bones[bone].posKeyFrames[posKey].xyz.x,
             &aModel->bones[bone].posKeyFrames[posKey].xyz.y,
             &aModel->bones[bone].posKeyFrames[posKey].xyz.z
             );
			if (res != 4){
				DELETE_MODEL(aModel) fileReader.closeFile(); return(false);}
			fileReader.getNextLine();
		}

		// get number of rotation Globals::Instance().keys
        res = SSCANF(fileReader.currentLineOfFile, "%d", &aModel->bones[bone].rotationKeysAmt);
		if (res != 1){
			DELETE_MODEL(aModel) fileReader.closeFile(); return(false);}
		// allocate memory for the rotation Globals::Instance().keys
		if (aModel->bones[bone].rotationKeysAmt > 0)
		{
			// delete anything in the array
			if(aModel->bones[bone].rotKeyFrames)delete[]aModel->bones[bone].rotKeyFrames;
			
			aModel->bones[bone].rotKeyFrames = 
				new keyFrame[aModel->bones[bone].rotationKeysAmt];

			if(!aModel->bones[bone].rotKeyFrames){ 
				// memory allocation failed so return false
				DELETE_MODEL(aModel) fileReader.closeFile(); return(false);}
		}

		fileReader.getNextLine();
		// get all the rotation Globals::Instance().keys
		for (int rotKey = 0; rotKey < aModel->bones[bone].rotationKeysAmt; rotKey++)
		{
            res = SSCANF(fileReader.currentLineOfFile, "%f %f %f %f",	&aModel->bones[bone].rotKeyFrames[rotKey].time,
             &aModel->bones[bone].rotKeyFrames[rotKey].xyz.x,
             &aModel->bones[bone].rotKeyFrames[rotKey].xyz.y,
             &aModel->bones[bone].rotKeyFrames[rotKey].xyz.z
             );
			if (res != 4){
				DELETE_MODEL(aModel) fileReader.closeFile(); return(false);}
			fileReader.getNextLine();
		}
	}// end for bones

	// go through all the bones to set up parent child relationships
	for (int bone = 0; bone < aModel->bonesNo; bone++)
	{
		aModel->bones[bone].parentBone = -1; /*	initially -1 (this will be updated unless this 
												is the parent bone) */
		if (strlen(aModel->bones[bone].parentName) > 0) // if this bone holds a parent name
		{	// go through all the bones looking for this bones parent bone
			for (int i = 0; i < aModel->bonesNo; i++ )
			{	// compare this bones name with the parent bone name being sought
				if (strcmp(aModel->bones[i].name, aModel->bones[bone].parentName) == 0)
				{
					aModel->bones[bone].parentBone = i; /*	set the index as this is the
															parent bone */
					break; // parent found so break out of for loop
				}
			}
			if (-1 == aModel->bones[bone].parentBone){ /*	this bone has a parent name which
															cannot be found, therefore return 
															false */
				DELETE_MODEL(aModel) fileReader.closeFile(); return(false);}
		}
	}// end for bones (setting up parent child relationships)

	setUpBones(aModel);

	// create the models display list if required
	aModel->createDisplayList();

	fileReader.closeFile();

	*model = aModel; // save to parameter
	aModel = 0;
    return(true);
}

void CModelLoader::setUpBones(CModel * model) 
{	// go through all the bones
	for (int bone = 0; bone < model->bonesNo; bone++ ) 
	{	// set initial rotation and translation for the relative bone matrix
		model->bones[bone].relative.createXYZRotationMatrix(&model->bones[bone].rotXYZ);
		model->bones[bone].relative.SetMatrixTranslation(CVector4(model->bones[bone].posXYZ, 1.0f));

		if (-1 != model->bones[bone].parentBone) // if the bone has a parent bone
		{	// set the absolute matrix to the parents absolute matrix
			model->bones[bone].absolute = model->bones[model->bones[bone].parentBone].absolute;
			/*	multiply bone absolute matrix by bone relative matrix
				i.e multiply the parents absolute matrix by this bones relative matrix
				to get this bones absolute matrix */
			model->bones[bone].absolute *= &model->bones[bone].relative; 
		}
		else{ /*	this bone has no parent therefore the bones relative and absolute matrices
					will be the same.  This is the root bone and its matrices define the bones
					offset in terms of position and rotation from the identity matrix */
			model->bones[bone].absolute = model->bones[bone].relative;}
	}// end for bones 

	// go through the meshes and move all the vertices and normals into position
	for (int mesh = 0; mesh < model->meshNo; mesh++)
	{	// go through this mesh's vertices
		for (int vertex = 0; vertex < model->meshes[mesh].virticesNo; vertex++) 
		{	// if this vertex has an associated bone
			int joint;
			if ((joint = model->meshes[mesh].vertices[vertex].boneIndex) != -1) 
			{	// referance variable used for readability
				CMatrix &m = model->bones[joint].absolute;
				
				// inversely rotate and translate the virtex
				model->meshes[mesh].vertices[vertex].originalXYZ =
					m.multiplyVectorInverseMatrix(&model->meshes[mesh].vertices[vertex].originalXYZ);
			}// end if bone
		}// end for vertices
		
		// go through the meshes triangles
		for (int triangles = 0; triangles < model->meshes[mesh].trianglesNo; triangles++ ) 
		{	// referance variable for readability
			triangle &aTriangle = model->meshes[mesh].triangles[triangles];
			// go through triangle vertices
			for (int j = 0; j < 3; j++) 
			{	// get the bone index of the first triangle vertex
				int theBone = model->meshes[mesh].vertices[aTriangle.vert[j]].boneIndex;
				// if this vertex has an associated bone
				if (theBone != -1) 
				{	// referance variable for readability
					CMatrix &m = model->bones[theBone].absolute;
					// rotate the normal by the absolute matrix of the normals associated
					// vertices associated bone
					aTriangle.normals[j] = 
						m.multiplyRotateVectorInverse(&aTriangle.normals[j]);
				}// end if bone
			}// end for triangle vertices
		}// end for triangles
	}// end for mesh's
	model->restart();
}// end setUpBones

// load model textures
int CModelLoader::reloadTextures(CModel *thisModel)
{
	for (int material = 0; material < thisModel->materialNo; material++ )
	{
		const int kFilenameBufferLen = 400;
		char filename[kFilenameBufferLen];
        	SPRINTF(filename, "%smodels/model textures/", GetDirectoryPath());
		if (strlen( thisModel->materials[material].clrMapTex) > 0) // if theirs a texture
		{            
			STRCAT(filename, kFilenameBufferLen, thisModel->materials[material].clrMapTex);
			thisModel->materials[material].clrTexBndNo = // bind the texture
				//TextureLoad(filename, GL_FALSE, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT);
                TextureLoad(filename, GL_FALSE, GL_LINEAR, GL_LINEAR, GL_REPEAT);

			if (thisModel->materials[material].clrTexBndNo <= 0){ // if the texture did not load properly
                		ErrorOutput("A texture did not load");
				return(0);} // loading error
		}
		else{ // no colour map diffuse texture
			thisModel->materials[material].clrTexBndNo = 0;}
	}// end for materials

	return(1); // everything loaded ok
}



