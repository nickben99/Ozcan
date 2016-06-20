//CHUD.cpp, a Head Up Display HUD class

// system includes --------
#include <math.h>
//-------------------------

// header files -----------
#include "CHUDFader.h"
#include "CMenu.h"
#include "CHUD.h"
#include "Game/Globals.h"
//-------------------------

const float kFadeTimeSeconds = 1.0f; // 

// constructor
CHUDFader::CHUDFader( void)
	: alpha(0.0f)
	, currentState(CHUDFader::state::eFadingOut)
	, nextState(CHUDFader::state::eStateMax)
{
}

// destructor
CHUDFader::~CHUDFader()
{
}

void CHUDFader::update(float delta)
{
	float prevAlpha = alpha;
	switch (currentState)
	{
	case state::eFadingOut:		
		alpha -= (delta/kFadeTimeSeconds);
		if (state::eFadingIn == nextState && prevAlpha > 0.0f && alpha <= 0.0f)
		{
			currentState = nextState;
			nextState = state::eStateMax;
		}
		alpha = (alpha > 0.0f) ? alpha : 0.0f; // don't let alpha get less than zero
		break;
	case state::eFadingIn:		
		alpha += (delta/kFadeTimeSeconds);
		if (state::eFadingOut == nextState && prevAlpha < 1.0f && alpha >= 1.0f)
		{
			currentState = nextState;
			nextState = state::eStateMax;
		}
		alpha = (alpha < 1.0f) ? alpha : 1.0f; // don't let alpha get less than zero
		break;
	default: break;
	}
}

// draw the HUD to the screen
void CHUDFader::draw( void)
{
	if (0.0f == alpha)
	{
		return;
	}

	CMenu::setToOrthograthicView(); // go into orthograthic view
	GLboolean wasBlendEnabled = glIsEnabled(GL_BLEND); 
	if (!wasBlendEnabled)
	{
		Globals::Instance().gl.GLEnable(GL_BLEND); // enable blending
	}

	GLboolean wasTextureEnabled = glIsEnabled(GL_TEXTURE_2D); 
	if (wasTextureEnabled)
	{
		Globals::Instance().gl.GLDisable(GL_TEXTURE_2D); // disable blending
	}
		
	glColor4f(0.0f, 0.0f, 0.0f, alpha); // set drawing colour to black
	glBegin(GL_QUADS);		// draw background texture
		glVertex2f(	0.0f, 0.0f);
		glVertex2f(	0.0f, CHUD::instance()->GetScreenHeight());
		glVertex2f(	CHUD::instance()->GetScreenWidth(), CHUD::instance()->GetScreenHeight());
		glVertex2f(	CHUD::instance()->GetScreenWidth(), 0.0f);
	glEnd();

	if (wasTextureEnabled)
	{
		Globals::Instance().gl.GLEnable(GL_TEXTURE_2D); // disable blending
	}

	if (!wasBlendEnabled)
	{
		Globals::Instance().gl.GLDisable(GL_BLEND); // enable blending
	}
	
	CMenu::setToPerspectiveView(); // go into perspective view
}

void CHUDFader::FadeIn()
{
	currentState = eFadingIn;
	nextState = eStateMax;
}

void CHUDFader::FadeOut()
{
	currentState = eFadingOut;
	nextState = eStateMax;
}

void CHUDFader::FadeInThenOut()
{
	currentState = eFadingIn;
	nextState = eFadingOut;
}

void CHUDFader::FadeOutThenIn()
{
	currentState = eFadingOut;
	nextState = eFadingIn;
}