/*--------------------------------------------
CODE BASED ON TUTORIAL AT http://rsn.gamedev.net/tutorials/ms3danim.asp
--------------------------------------------*/

//CModel.h, header file for CModel.cpp

//system includes------
#include <windows.h>	// header file for windows
#include <gl\gl.h>		// header file for the OpenGL32 library
#include <gl\glu.h>		// header file for the GLu32 library
//---------------------

#ifndef _CModel_h_
#define _CModel_h_

//header files---
#include "CModel.h"
#include "Math/CMatrix.h"
#include "Math/CVector.h"
#include "Math/CQuaternion.h"
#include "Rendering\MeshBuffer.h"
//---------------

//external definitions --
//-----------------------

//defines--------------
struct keyFrame
{
	GLfloat time; // keyframe time 
	CVector	xyz;	// x, y, z position/rotation
};

struct bone
{
	static const int kStrLen = 256;
	char	name[kStrLen], // joint name
			parentName[kStrLen]; // parent name

	GLint	flags;
	CVector	posXYZ, // x, y, z position 
			rotXYZ; // x, y, z rotation

	GLint	positionKeysAmt;
	keyFrame * posKeyFrames; // this bones position key frames

	GLint	rotationKeysAmt;
	keyFrame * rotKeyFrames; // this bones rotation key frames

	CMatrix absolute, relative, final;

	GLint	currentRotationKeyFrame,	// rotation key frame currently being interpolated towards
			currentTranslationKeyFrame;	// translation key frame currently being interpolated towards 

	GLint parentBone;
};

struct material
{
	material()
	{
		sort = ++latestSortNumber;
	}

	static int latestSortNumber; // this is just used for ordering triangles which use this material
	int sort;

	static const int kStrLen = 256;
	char	name[kStrLen],
			clrMapTex[kStrLen],
			alphaMapTex[kStrLen];

	GLint	clrTexBndNo,
			alphaTexBndNo;

	float	ambient[4],
			diffuse[4],
			specular[4],
			emissive[4],
			shinnines,
			transparency;
};

struct vertex
{
	int flags; // vertex flags
	CVector	originalXYZ, // x, y, z coords from MilkShape ASCII file
			currentXYZ; // the current vertex including transformations 
	GLfloat	u, v; // u, v texture coordinates
	int boneIndex; // bone this vertex is associated with
};

struct triangle
{
	int flags, 
		vert[3], // vertices used for the triangle
		norm[3], // normal indeces used for each vertex
		smoothinggroup;

	// holds the current values of the triangles vertices (incorporating transformations)
	CVector currentVertices[3];
	// normals used for each vertex 
	CVector normals[3], 
			currentNormals[3]; // normals used for each vertex (incorporating transformations)
};

/*	describes a single vertex, used to keep a record 
	of all the virtices in each mesh, incorporating the
	transformation of the normal and vertex */
struct singleVertex
{	
	float u, v;
	CVector Normal, 
			vertex;
};

struct mesh
{
#ifdef USE_SHADERS
	mesh()
		: meshBuffer(MeshBuffer::eFormat::SingleArray)
	{
	}
#endif

	static const int kNameStrLen = 256;
	char name[kNameStrLen]; // mesh name
	int flags, matIndex; // mesh flags, material index

	int virticesNo;
	vertex * vertices; // all the virtices required for triangles

	int normalsNo;
	CVector * normals;

	int trianglesNo;
	triangle * triangles; 

	/*	all the virtices of all the triangles at the current 
		stage in the animation (the amount will be trianglesNo*3) */
	singleVertex *currentVirtices;

#ifdef USE_SHADERS
	MeshBuffer meshBuffer;
#endif
};
//---------------------

//external variables --
//---------------------

class CModel
{
	private:
//private variables------------
//-----------------------------

//private methods--------------
		/*	private draw method, draws a model with no bones (so will not move)
			using a display list */
		void drawModelUsingDisplayList( void);

		// private draw method, draws an articulated figure (one which has bones)
		void drawArticulatedModel( void);		
		bool AreDynamicBuffersInUse();
#ifdef USE_SHADERS
		void CreateMeshBuffers();
		void DrawUsingMeshBuffers();
#endif
//-----------------------------
	public:
//public methods---------------
		CModel(); // constructor
		~CModel(); // destructor
		// create the models display list (if applicable i.e no bones)
		void createDisplayList( void);
		void draw(); // public draw method which choses between display list and normal drawing
		void restart(); // restart the animation 
		void advanceFrame(GLuint elapsedTime, GLfloat &frame, GLuint frameRate,
									bool loop, int startingFrame = 0, int endingFrame = -1, float *amtOver = 0); 
		// put animation at the specified frame
		void setBonesAtFrame(GLfloat frame);
//-----------------------------

//public variables-------------
	int totalFrames,
		currentFrame,
		meshNo,
		materialNo,
		bonesNo; 
	bool IsTransparent; // indicates if any of the model is partially transparent

	mesh * meshes; // dynamic array of meshes
	material * materials; // dynamic array of materials
	bone * bones;

	/*	the radius of a sphere which would compleatly encompass the 
		unnanimated model */
	float boundingSphereRadUnAnimated;
	/*	the dimensions of a box which would compleatly encompass the 
		unnanimated model */
	CVector boundingRectangleUnAnimated;

	int modelDisplayList; // a display list for the model if it does not have any bones
//-----------------------------
}; // class CModel

#endif // _CModel_h_






