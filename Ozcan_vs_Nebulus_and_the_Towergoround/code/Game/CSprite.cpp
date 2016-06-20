//CSprite.cpp, a sprite class

// system includes --------
#include <math.h>
//-------------------------

// header files -----------
#include "CSprite.h"
#include "ReplayManager.h"
#include "Game/Globals.h"
//-------------------------

// constructor
CSprite::CSprite()
{
	model = 0;
}

// overloaded constructor
CSprite::CSprite(CModel * aModel)
	: model(aModel) // initialise pointer (model = aModel)
{
	mpReplayData = 0;

	initialise();
}

// initialise the class
void CSprite::initialise()
{
	forces.reset();
	velocity.reset(); // don't carry over any forces or velocity if sprite is reset
	rotation.identity(); // reset rotation matrix to identity if reset

	currentFrame = 0.0f; // start at frame 0;
	loopingAnimation = true; // the animation will loop
	framesPerSecond = 20; /*	20 actual animation frames will be interpolated 
								through each second */
	scale = CVector(1.0f, 1.0f, 1.0f); // full size
	mass = 1.0f; // 1 kg
	collisionType = TRIANGLES; /* this sprite uses its triangles for collision detection */
	spriteType = UNSPECIFIED;	// sprite type is not important for this sprite
	dynamicProperty = STATIC_ENVIRONMENT; // most sprites don't move
	transparency = false; /* no transparency unless its found (below) that the model
															is partially transparent */
	/*	initialise model bounding sphere and box 
		(these will have to be updated if sprite is scaled) */
	if (model)
	{ 
		boundingSphereRadius = model->boundingSphereRadUnAnimated;
		boundingBox = model->boundingRectangleUnAnimated;
		scaledBoundingBox = boundingBox * &scale;
		scaledHalfBoundingBox = scaledBoundingBox * 0.5f;
		transparency = model->IsTransparent; // get transparency from model
	}
	/*	set ellipsoid radius vector on three axii (IF THEIR IS NO MODEL, THE VALUES 
		boundingSphereRadius WILL BE UNDEFINED) */
	elipsoidRadiusVector = CVector(boundingSphereRadius, boundingSphereRadius, 
									boundingSphereRadius);
	scaledElipsoidRadiusVector = elipsoidRadiusVector * &scale;
	// set gravitational acceleration
	gravitation = CVector(0.0f, -9.81f, 0.0f);
	active = true; // the sprite is active

	midAirGroundPlaneMaxSpeed = 2.5f; // max ground plane velocity when in mid air (per second)
	surfaceFrictionConstant = 1.52f;
	airFrictionConstant = 0.52f;

	killValue = 0; // for enemies, when killed by nebulus' fireball

	oldPosition = position;
	calculateFinalMatrix(); /*	do this here, as no need to do it again if 
								sprite doesn't move and ensures its in the 
								correct position after reinitialising */
	soundPlayedThisFrame = SOUNDS_COUNT;
}

// reinitialise sprite to starting state
void CSprite::reinitialise( void)
{
	CSprite::initialise(); /*	needs the 'CSprite::' part as this method could 
								be called from a base class, which would mean the base
								class' initialise would be called rather than the 
								CSprite one which is required */
								
}

void CSprite::ReplayInit()
{
	if (0 == mpReplayData)
	{
		mpReplayData = new ReplayData[ReplayManager::instance()->GetNumFramesForEndReplay()];
	}

	//if (ReplayManager::instance()->IsRecording())
	//{
	//	mRecorderStartFrame = -1;
	//	mRecorderEndFrame = -1;
	//}
}

void CSprite::saveReplayUpdate(int frame)
{
	if (0 != mpReplayData)
	{
		if (ReplayManager::instance()->IsRecording())
		{		
			if (!ReplayManager::instance()->IsRecordingEndFrames())
			{
				return; // we already have our end sequence recorded
			}
			
			ReplayData& data = mpReplayData[ReplayManager::instance()->GetRecordingBufferIndex()];
			data.frame = frame;
			data.position = position;
			data.currentAnimationFrame = currentFrame;
			data.orientation = rotation.matrixToQuaternion();			
			data.scale = scale;
			data.active = active;
			data.transparency = transparency;
			data.soundToPlay = soundPlayedThisFrame;
			saveReplayUpdate(data, 0 == frame);
		}
		else if (ReplayManager::instance()->IsReplayingEndLevel()) // replaying end level, we don't need to do anything for replaing the whole level
		{
			ReplayData& data = mpReplayData[ReplayManager::instance()->GetEndReplayBufferIndex()];

			position = data.position;
			rotation.quaternionToMatrix(data.orientation);
			currentFrame = data.currentAnimationFrame;
			scale = data.scale;
			active = data.active;
			transparency = data.transparency;

			if (SOUNDS_COUNT != data.soundToPlay)
			{
				Globals::Instance().sound.PlaySound( data.soundToPlay, false ); // play enemy explode sound once
			}

			saveReplayUpdate(data, 0 == frame);
			calculateFinalMatrix();
		}
	}
}

void CSprite::saveReplayUpdate(ReplayData&, bool)
{
}

// draw the sprite at appropriate frame
void CSprite::draw(int)
{
	// if the sprite has an associated model
	if (model) 
	{
		model->setBonesAtFrame(currentFrame);
		
		Globals::Instance().modelMatrixStack.PushMatrix();
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); /*	set polygon mode (using GL_LINE 
														massivly slows down the swapbuffers 
														command) */  
			Globals::Instance().modelMatrixStack.MultMatrix(finalMatrix);

			// draw the model
			model->draw();
		Globals::Instance().modelMatrixStack.PopMatrix();
	}	
}

// destructor
CSprite::~CSprite()
{
	model = 0; //set to 0

	if (0 != mpReplayData)
	{
		delete[] mpReplayData;
		mpReplayData = 0;
	}
}

// apply a force to the sprite
void CSprite::applyForce(CVector * force)
{
	forces += force;		
}

// apply a force to the sprite
void CSprite::applyForce(const CVector& force)
{
	forces += force;		
}

// move the sprite
void CSprite::move(int timeChange, GLfloat perCentOfSecond)
{
	oldPosition = position; // record the previous position

	CVector acceleration = (forces / mass); // determine acceleration
	
	velocity += ( acceleration * perCentOfSecond);	// add acceleration to velocity
																								
	position += (velocity * perCentOfSecond); // add velocity to position

	calculateFinalMatrix();

	// if the sprite has an associated model
	if (model) 
	{
		// advance the animation
		model->advanceFrame(timeChange, currentFrame, framesPerSecond, loopingAnimation);
	}	
}

// calculate the final matrix
void CSprite::calculateFinalMatrix()
{
	finalMatrix.identity(); // set final matrix to identity
	finalMatrix.SetMatrixTranslation(CVector4(position, 1.0f)); // set translation
	finalMatrix *= &rotation; // set rotation
	static CMatrix theScale; 
	theScale.creatScalingMatrix(scale.x, scale.y, scale.z); // create scaling matrix
	finalMatrix *= &theScale; // set scale
}
