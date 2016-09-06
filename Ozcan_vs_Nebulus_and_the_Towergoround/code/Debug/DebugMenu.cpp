//DebugMenu.cpp

#ifdef _DEBUG
#ifdef USE_SHADERS

// system includes --------
//-------------------------

// header files -----------
#include "DebugMenu.h"
#include "Game/Globals.h"
#include "Game/defines.h"
#include "Math/CVector.h"
//-------------------------

CVector4 selectedColor;
CVector4 subMenuColor;
CVector4 normalColor;

DebugMenu::DebugMenu()
	: mainMenu(NULL)
	, isVisible(false)
#ifdef _WINDOWS
	, font(NULL)
#endif
{
	selectedColor = CVector4::red;
	subMenuColor = CVector4::blue;
	normalColor = CVector4::black;
}

DebugMenu::~DebugMenu()
{
#ifdef _WINDOWS
	Stop();
#endif
}

float xDrawPosDefault = -500.0f;
float yDrawPosDefault = -200.0f;

DebugMenu::DebugMenuItemContainer::~DebugMenuItemContainer()
{
	Stop();
}

void DebugMenu::DebugMenuItemContainer::Stop()
{
	int numMenuItems = (int)menuItems.size();
	for (int item = 0; item < numMenuItems; ++item)
	{
		delete menuItems[item];
	}
	menuItems.clear();
}

void DebugMenu::DebugMenuItemContainer::Update()
{
	int numMenuItems = (int)menuItems.size();
	if (0 == numMenuItems)
	{
		currentlySelected = 0;
		return;
	}

	currentlySelected = defines::Clamp<int>(currentlySelected, 0, numMenuItems-1);

	if (menuItems[currentlySelected]->IsVisible() && menuItems[currentlySelected]->HasSubItems())
	{
		menuItems[currentlySelected]->Update();
		return;
	}

	if(Globals::Instance().keys.GetKeyPressed(eKeyCode_UP))
	{
		--currentlySelected;
	}

	if(Globals::Instance().keys.GetKeyPressed(eKeyCode_DOWN))
	{
		++currentlySelected;
	}

	currentlySelected = defines::Clamp<int>(currentlySelected, 0, numMenuItems-1);

	if(Globals::Instance().keys.GetKeyStatus(eKeyCode_LEFT))
	{
		menuItems[currentlySelected]->Decrement();
	}

	if(Globals::Instance().keys.GetKeyStatus(eKeyCode_RIGHT))
	{
		menuItems[currentlySelected]->Increment();
	}
}
#ifdef _WINDOWS
void DebugMenu::DebugMenuItemContainer::Draw(Text& font, float& xDrawPos, float& yDrawPos)
{
	font.SetSize(25.0f);

	DebugMenuItem::Draw(font, xDrawPos, yDrawPos);

	if (!IsVisible() || !HasSubItems())
	{
		return;
	}

	const float Indent = 20.0f;
	xDrawPos += Indent;

	int numMenuItems = (int)menuItems.size();
	for (int item = 0; item < numMenuItems; ++item)
	{		
		CVector4 color = (menuItems[item]->HasSubItems()) ? subMenuColor : normalColor;
		color = (item == currentlySelected) ? selectedColor : color;
		font.SetColor(color);
		menuItems[item]->Draw(font, xDrawPos, yDrawPos);
	}
	xDrawPos -= Indent;
}

void DebugMenu::Start()
{
	if (NULL == font)
	{
		font = new Text();
	}

	if (NULL == mainMenu)
	{
		mainMenu = new DebugMenu::DebugMenuItemContainer("Debug Menu");
		
		DebugMenuItemContainer* positioning = new DebugMenuItemContainer("Debug Menu Position");

		positioning->AddVariable(new DebugMenu::FloatDebugMenuItem("Menu pos x", &(xDrawPosDefault), 1.0f, -1.0f));
		positioning->AddVariable(new DebugMenu::FloatDebugMenuItem("Menu pos y", &(yDrawPosDefault), 1.0f, -1.0f));

		DebugMenuItemContainer* colors = new DebugMenuItemContainer("Debug Menu Colors");
	
		CVector4 increment(0.01f, 0.01f, 0.01f, 0.01f);
		colors->AddVariable(new DebugMenu::CVector4DebugMenuItem("selected color", &selectedColor, increment, -increment));
		colors->AddVariable(new DebugMenu::CVector4DebugMenuItem("sub menu color", &subMenuColor, increment, -increment));
		colors->AddVariable(new DebugMenu::CVector4DebugMenuItem("normal color", &normalColor, increment, -increment));

		DebugMenuItemContainer* positioningAndColor = new DebugMenuItemContainer("Debug Menu Position/Color");	
		positioningAndColor->AddVariable(positioning);
		positioningAndColor->AddVariable(colors);

		AddVariable(positioningAndColor);
	}
}

void DebugMenu::Stop()
{
	if (NULL != font)
	{
		delete font;
		font = NULL;
	}

	if (NULL != mainMenu)
	{
		delete mainMenu;
		mainMenu = NULL;
	}
}

void DebugMenu::Update()
{
	if (NULL == font)
	{
		return;
	}

	bool wasVisible = isVisible;
	isVisible = (Globals::Instance().keys.GetKeyPressed(eKeyCode_SPACE)) ? !isVisible : isVisible;
	if (!isVisible)
	{
		return;
	}

	if (NULL != mainMenu)
	{
		if (!wasVisible) // has the debug menu been turned on right now
		{
			mainMenu->SetVisible();
		}
		mainMenu->Update();
	}
}

void DebugMenu::Draw()
{
	if (NULL == font)
	{
		return;
	}

	if (!isVisible)
	{
		return;
	}

	float xDrawPos = xDrawPosDefault;
	float yDrawPos = yDrawPosDefault;
	font->SetColor(selectedColor);

	if (NULL != mainMenu)
	{
		mainMenu->Draw(*font, xDrawPos, yDrawPos);
	}
}
#endif // #ifdef _WINDOWS

#endif // USE_SHADERS
#endif // #if _DEBUG