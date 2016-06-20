// DebugMenu.h

#ifdef _DEBUG
#ifdef USE_SHADERS

//system includes------------
#include <vector>
//---------------------------

#ifndef _DebugMenu_h_
#define _DebugMenu_h_

//header files---------
#include "Rendering\Text.h"
//---------------------

//defines--------------
//---------------------

//forward declerations----
//------------------------

class DebugMenu
{
public:
	class DebugMenuItem
	{
	public:
		DebugMenuItem(const char* textIn)
		{
			if (NULL != textIn)
			{
				strncpy_s(text, TextLength-1, textIn, strlen(textIn));
			}
			else
			{
				text[0] = '\0';
			}
		}

		virtual ~DebugMenuItem() = 0 {}; // pure virtual destructor stops the class being instantiated (means it can only be a base class)
		
		virtual void Increment() = 0;
		virtual void Decrement() = 0;

		virtual const char* GetText() = 0;

		virtual bool HasSubItems()
		{
			return false;
		}

		virtual bool IsVisible()
		{
			return true;
		}

		virtual void Update(){};

		virtual void Draw(Text& font, float& xDrawPos, float& yDrawPos)
		{
			font.SetText(xDrawPos, yDrawPos, GetText());
			font.Draw();
			yDrawPos += font.GetTextHeight();
		}

	protected:
		static const int TextLength = 256;
		char text[TextLength];		
	};

	class DebugMenuItemContainer : public DebugMenuItem
	{
	public:
		DebugMenuItemContainer(const char* textIn)
			: DebugMenuItem(textIn)
			, currentlySelected(0)
			, visible(false)
		{
			AddVariable(new DebugMenu::BoolDebugMenuItem("Show Sub Menu", &(visible)));
		}

		virtual ~DebugMenuItemContainer();
		
		void AddVariable(DebugMenuItem* item)
		{
			menuItems.push_back(item);
		}

		void SetVisible()
		{
			visible = true;
		}

		void Stop();

		virtual void Update();
		virtual void Draw(Text& font, float& xDrawPos, float& yDrawPos);

		virtual void Increment()
		{
			visible = true;
		}

		virtual void Decrement()
		{
		}

		virtual bool HasSubItems()
		{
			return true;
		}

		virtual bool IsVisible()
		{
			return visible;
		}

		virtual const char* GetText()
		{
			return text;
		}

	protected:
		std::vector<DebugMenuItem*> menuItems;
		int currentlySelected;
		bool visible;
	};

	class FloatDebugMenuItem : public DebugMenuItem
	{
	public:
		FloatDebugMenuItem(const char* displayText, float* variableIn, float variableIncrement, float variableDecrement)
			:DebugMenuItem(displayText)
			, variable(variableIn)
			, variableIncrement(variableIncrement)
			, variableDecrement(variableDecrement)
		{
		}

		virtual ~FloatDebugMenuItem(){}

		virtual void Increment()
		{
			(*variable) += variableIncrement;			
		}

		virtual void Decrement()
		{
			(*variable) += variableDecrement;
		}

		virtual const char* GetText()
		{
			sprintf_s(displayText, "%s %f", text, *variable);
			return displayText;
		}

	private:
		float* variable;
		float variableIncrement;
		float variableDecrement;

		char displayText[DebugMenuItem::TextLength*2];
	};

	class BoolDebugMenuItem : public DebugMenuItem
	{
	public:
		BoolDebugMenuItem(const char* displayText, bool* variableIn)
			:DebugMenuItem(displayText)
			, variable(variableIn)
		{
		}

		virtual ~BoolDebugMenuItem(){}

		virtual void Increment()
		{
			(*variable) = true;			
		}

		virtual void Decrement()
		{
			(*variable) = false;
		}

		virtual const char* GetText()
		{
			sprintf_s(displayText, "%s %s", text, (*variable) ? "true" : "false");
			return displayText;
		}

	private:
		bool* variable;
		char displayText[DebugMenuItem::TextLength*2];
	};

	class CVectorDebugMenuItem : public DebugMenuItemContainer
	{
	public:
		CVectorDebugMenuItem(const char* displayText, CVector* variableIn, const CVector& increment, const CVector& decrement)
			:DebugMenuItemContainer(displayText)
			, variable(variableIn)
		{
			AddVariable(new DebugMenu::FloatDebugMenuItem("x", &(variableIn->x), increment.x, decrement.x));
			AddVariable(new DebugMenu::FloatDebugMenuItem("y", &(variableIn->y), increment.y, decrement.y));
			AddVariable(new DebugMenu::FloatDebugMenuItem("z", &(variableIn->z), increment.z, decrement.z));
		}

		virtual ~CVectorDebugMenuItem(){}

		virtual const char* GetText()
		{
			sprintf_s(displayText, "%s %.3f, %.3f, %.3f", text, variable->x, variable->y, variable->z);			
			return displayText;
		}

	protected:		
		char displayText[DebugMenuItem::TextLength*2];

	private:
		CVector* variable;
	};

	class CVector4DebugMenuItem : public CVectorDebugMenuItem
	{
	public:
		CVector4DebugMenuItem(const char* displayText, CVector4* variableIn, const CVector4& increment, const CVector4& decrement)
			:CVectorDebugMenuItem(displayText, &(variableIn->v3), increment.v3, decrement.v3)
			, vector4Variable(variableIn)
		{
			AddVariable(new DebugMenu::FloatDebugMenuItem("w", &(variableIn->w), increment.w, decrement.w));
		}

		virtual ~CVector4DebugMenuItem(){}

		virtual const char* GetText()
		{
			sprintf_s(displayText, "%s %.3f, %.3f, %.3f, %.3f", text, vector4Variable->v3.x, vector4Variable->v3.y, vector4Variable->v3.z, vector4Variable->w);			
			return displayText;
		}

	private:
		CVector4* vector4Variable;
	};

	DebugMenu();
	~DebugMenu();

	void AddVariable(DebugMenuItem* item)
	{
		Start();
		mainMenu->AddVariable(item);
	}

	void Update();
	void Draw();

private:
	void Start();
	void Stop();

private:
	DebugMenuItemContainer* mainMenu;
	bool isVisible;
	Text* font;
};

#endif // ifndef _DebugMenu_h_ 
#endif // USE_SHADERS
#endif // #if _DEBUG