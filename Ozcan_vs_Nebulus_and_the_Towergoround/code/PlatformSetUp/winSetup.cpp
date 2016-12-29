/*
	CODE BASED ON NEHE BASE CODE AND OPENGL SUPERBIBLE BASECODE
 */

#define WIN32_LEAN_AND_MEAN   // Not using Microsoft Foundation Classes
#ifndef INITGUID
  #define INITGUID				// for direct input and direct sound (must be defined here at top of code)
#endif
//----- system includes-----------------------------------------------
#include <windows.h>		// header file for windows
#include <stdio.h>			// header file for standard input/output
#include <glew-1.12.0\include\GL/glew.h> ////////////////////////////////////////////////////////////////////
//--------------------------------------------------------------------

//---header files-----------------------------------------------------
#include "resource.h"		// for the icon
#include "Audio/SoundsEnum.h"
#include "Game/main.h"			// the main game code
#include "Game/Game.h"
#include "Game/Globals.h"		// for direct input
#include "Rendering\Text.h"
//--------------------------------------------------------------------

//---- global variables ----------------------------------------------
HDC			hDC=0;		// private GDI device context
HGLRC		hRC=0;		// permanent rendering context for OpenGL
HWND		hWnd=0;		// holds the window handle
HINSTANCE	hInst;			// holds the instance of the application
HPALETTE hPalette = 0;	// pallette, if required

bool fullscreen = true;	// fullscreen flag set to fullscreen mode by default
char *className = "Ozcan vs Nebulus & The Towergoround";	// name of the window class

//----------------------------------------------------------------------

LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);	// declaration for WndProc

// If necessary, creates a 3-3-2 palette for the device context listed.
HPALETTE GetOpenGLPalette(HDC theHDC)
{
	HPALETTE hRetPal = 0;	// handle to palette to be created
	PIXELFORMATDESCRIPTOR pfd;	// pixel Format Descriptor
	LOGPALETTE *pPal;			// pointer to memory for logical palette
	int nPixelFormat;			// pixel format index
	int nColors;				// number of entries in palette
	int i;						// counting variable
	BYTE RedRange,GreenRange,BlueRange;
								// Range for each color entry (7,7,and 3)


	// get the pixel format index and retrieve the pixel format description
	nPixelFormat = GetPixelFormat(theHDC);
	DescribePixelFormat(theHDC, nPixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

	/*	does this pixel format require a palette?  If not, do not create a
		palette and just return 0 */
	if(!(pfd.dwFlags & PFD_NEED_PALETTE))
		return 0;

	// number of entries in palette.  8 bits yeilds 256 entries
	nColors = 1 << pfd.cColorBits;	

	// allocate space for a logical palette structure plus all the palette entries
	pPal = (LOGPALETTE*)malloc(sizeof(LOGPALETTE) +nColors*sizeof(PALETTEENTRY));

	// Fill in palette header 
	pPal->palVersion = 0x300;		// Windows 3.0
	pPal->palNumEntries = (WORD)nColors; // table size

	// build mask of all 1's.  This creates a number represented by having
	// the low order x bits set, where x = pfd.cRedBits, pfd.cGreenBits, and
	// pfd.cBlueBits.  
	RedRange = (1 << pfd.cRedBits) -1;
	GreenRange = (1 << pfd.cGreenBits) - 1;
	BlueRange = (1 << pfd.cBlueBits) -1;

	// Loop through all the palette entries
	for(i = 0; i < nColors; i++)
	{
		// Fill in the 8-bit equivalents for each component
		pPal->palPalEntry[i].peRed = (i >> pfd.cRedShift) & RedRange;
		pPal->palPalEntry[i].peRed = (unsigned char)(
			(double) pPal->palPalEntry[i].peRed * 255.0 / RedRange);

		pPal->palPalEntry[i].peGreen = (i >> pfd.cGreenShift) & GreenRange;
		pPal->palPalEntry[i].peGreen = (unsigned char)(
			(double)pPal->palPalEntry[i].peGreen * 255.0 / GreenRange);

		pPal->palPalEntry[i].peBlue = (i >> pfd.cBlueShift) & BlueRange;
		pPal->palPalEntry[i].peBlue = (unsigned char)(
			(double)pPal->palPalEntry[i].peBlue * 255.0 / BlueRange);

		pPal->palPalEntry[i].peFlags = (unsigned char) 0;
	}
		
	// create the palette
	hRetPal = CreatePalette(pPal);

	// select and realize the palette for this device context
	SelectPalette(theHDC,hRetPal,FALSE);
	RealizePalette(theHDC);

	// Free the memory used for the logical palette structure
	free(pPal);

	// Return the handle to the new palette
	return hRetPal;
}

// kill the window
GLvoid KillGameWindow(GLvoid)								
{
	// in fullscreen mode?
	if (fullscreen)										
	{
		ChangeDisplaySettings(0,0); // switch back to the desktop
		ShowCursor(TRUE); // show mouse pointer
	}

	// if theirs a rendering context
	if (hRC) 
	{
		if (!wglMakeCurrent(0,0))	// able to release the DC and RC contexts?
		{
			MessageBox(0,"Release Of DC And RC Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}

		if (!wglDeleteContext(hRC))	// able to delete the RC?
		{
			MessageBox(0,"Release Rendering Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}

		hRC = 0; // set RC to 0
	}

	// able to release the DC
	if (hDC && !ReleaseDC(hWnd,hDC))		
	{
		MessageBox(0,"Release Device Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hDC=0; // set DC to 0
	}

	if (hWnd && !DestroyWindow(hWnd)) // able to destroy the window?
	{
		MessageBox(0,"Could Not Release hWnd.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hWnd=0; // set hWnd to 0
	}

	if (!UnregisterClass(className, hInst)) // able to unregister class
	{
		MessageBox(0,"Could Not Unregister Class.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hInst=0;									// set hInstance to 0
	}

	// if a palette was created
	if(hPalette != 0)
	{
		if (!DeleteObject(hPalette)) // if palette could not be deleted
		{
			MessageBox(0,"Could Not Delete palette.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
			hPalette = 0; // set hPalette to null
		}
	}
}

BOOL CreateGameWindow()
{	
	const int bits = 32; // bits per pixel

	const int width = 1024;	// window width
	const int height = 768; // window height

	GLuint		PixelFormat; // holds the results after searching for a match
	WNDCLASSEX	wc;	// windows class structure
	DWORD		dwExStyle; // window extended style
	DWORD		dwStyle; // window style
	RECT		WindowRect; // grabs rectangle upper left / lower right values
	WindowRect.left=(long)0; // set left value to 0
	WindowRect.right=(long)width; // set right value to requested width
	WindowRect.top=(long)0; // set top value to 0
	WindowRect.bottom=(long)height; // set bottom value to requested height

	hInst				= GetModuleHandle(0); // instance for window
	wc.cbSize			= sizeof(WNDCLASSEX);					
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC; // redraw on size, and own DC for window.
	wc.lpfnWndProc		= (WNDPROC) WndProc; // WndProc handles messages
	wc.cbClsExtra		= 0; // no extra window data
	wc.cbWndExtra		= 0; // no extra window data
	wc.hInstance		= hInst; // set the instance
	wc.hIcon			= LoadIcon(hInst, MAKEINTRESOURCE(ICON_NEBULUS)); // load nebuls icon
	wc.hCursor			= LoadCursor(hInst, MAKEINTRESOURCE(CUR_NEBULUS)); // load nebulus pointer
	wc.hbrBackground	= 0;	// No Background Required
	wc.lpszMenuName		= 0;	// We Don't Want A Menu
	wc.lpszClassName	= className; // Set The Class Name
	wc.hIconSm			= LoadIcon(hInst, MAKEINTRESOURCE(ICON_NEBULUS)); // load nebulus icon (for window top left)

	if (!RegisterClassEx(&wc)) // try to register the window class
	{
		MessageBox(0,"Failed To Register The Window Class.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE; // Return FALSE
	}
	
	if (fullscreen) // attempt fullscreen mode
	{
		DEVMODE dmScreenSettings; // device mode
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings)); // makes sure memory's cleared
		dmScreenSettings.dmSize=sizeof(dmScreenSettings); // size of the devmode structure
		dmScreenSettings.dmPelsWidth	= width; // selected screen width
		dmScreenSettings.dmPelsHeight	= height; // selected screen height
		dmScreenSettings.dmBitsPerPel	= bits; // selected bits per pixel
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

		// set selected mode and get results (CDS_FULLSCREEN gets rid of start bar)
		if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
		{
			// if fullscreen mode failed, quit or use windowed mode.
			if (MessageBox(0,"Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?",
				"Screen Settings (Ozcan vs Nebulus & The Towergoround)",MB_YESNO|MB_ICONEXCLAMATION)==IDYES)
			{
				fullscreen=FALSE;		// windowed mode selected, Fullscreen = FALSE
			}
			else
			{
				// pop up a message box letting user know the program is closing.
				MessageBox(0,"Program Will Now Close.", 
					"Ozcan vs Nebulus & The Towergoround",MB_OK|MB_ICONSTOP);
				return FALSE;									// Return FALSE
			}
		}
	}

	if (fullscreen)	// if fullscreen is required
	{
		dwExStyle=WS_EX_APPWINDOW; // window extended style
		dwStyle=WS_POPUP; // windows style
		ShowCursor(FALSE); // hide mouse pointer
	}
	else // windowed mode
	{
		dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE; // window extended style
		// window style (close button, not re-sizable) - replace with WS_OVERLAPPEDWINDOW to make it resizable
		dwStyle= WS_OVERLAPPED |  WS_SYSMENU | WS_CAPTION | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	}

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);	// adjust window to true requested size

	char *title = "Ozcan vs Nebulus & The Towergoround";

	// create the window
	hWnd=CreateWindowEx(dwExStyle, // extended style for the window
						className, // class name
						title, // window title
						dwStyle, // defined window style
						0, 0, // window position
						WindowRect.right-WindowRect.left, // calculate window width
						WindowRect.bottom-WindowRect.top, // calculate window height
						0, // no parent window
						0, // no menu
						hInst, // instance
						0);	// dont pass anything to WM_CREATE
	if (!hWnd)
	{
		KillGameWindow();	// reset the display
		MessageBox(0,"Window Creation Error.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE; // return FALSE
	}
	
	// have menu if in windowed mode
	if (false == fullscreen) SetMenu(hWnd,LoadMenu(hInst, MAKEINTRESOURCE(MAIN_MENU)));

	static	PIXELFORMATDESCRIPTOR pfd= // pfd tells windows the pixel format required
	{
		sizeof(PIXELFORMATDESCRIPTOR), // size of this pixel format descriptor
		1, // version number
		PFD_DRAW_TO_WINDOW | // format must support window
		PFD_SUPPORT_OPENGL | // format must support OpenGL
		PFD_DOUBLEBUFFER, // must support double buffering
		PFD_TYPE_RGBA, // request an RGBA format
		(BYTE)bits, // select our color depth
		0, 0, 0, 0, 0, 0, // color bits ignored
		0, // no alpha buffer
		0, // shift bit ignored
		0, // no accumulation buffer
		0, 0, 0, 0, // accumulation bits ignored
		16, // 16Bit Z-Buffer (depth buffer)  
		0, // no stencil buffer
		0, // no auxiliary buffer
		PFD_MAIN_PLANE, // main drawing layer
		0, // reserved
		0, 0, 0 // layer masks ignored
	};
	
	// was a device context acquired?
	hDC=GetDC(hWnd);
	if (!hDC)
	{
		KillGameWindow(); // reset the display
		MessageBox(0,"Can't Create A GL Device Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE; // return FALSE
	}

	// was a matching pixel format found?
	PixelFormat=ChoosePixelFormat(hDC, &pfd);
	if (!PixelFormat)	
	{
		KillGameWindow();	// reset the display
		MessageBox(0,"Can't Find A Suitable PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE; // return FALSE
	}
	
	// was the pixel format set?
	if(!SetPixelFormat(hDC,PixelFormat, &pfd)) 
	{
		KillGameWindow(); // reset the display
		MessageBox(0,"Can't Set The PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE; // return FALSE
	}

	// was a rendering context acquired?
	hRC=wglCreateContext(hDC);
	if (!hRC) 
	{
		KillGameWindow(); // reset the display
		MessageBox(0,"Can't Create A GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE; // return FALSE
	}

	// activate rendering context
	if(!wglMakeCurrent(hDC,hRC)) 
	{
		KillGameWindow();	// reset the display
		MessageBox(0,"Can't Activate The GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE; // Return FALSE
	}

	ShowWindow(hWnd,SW_SHOW); // show the window
	SetForegroundWindow(hWnd); // slightly higher priority
	SetFocus(hWnd);	// sets keyboard focus to the window

	// create the palette
	hPalette = GetOpenGLPalette(hDC); // returns 0 if no pallette required

	return TRUE; // success
}

LRESULT CALLBACK WndProc(	HWND	theHWnd,			// Handle For This Window
							UINT	uMsg,			// Message For This Window
							WPARAM	wParam,			// Additional Message Information
							LPARAM	lParam)			// Additional Message Information
{
	// check for windows messages
	switch (uMsg)									
	{
		case WM_SYSCOMMAND: // intercept system commands
			switch (wParam)	// check system calls
			{
				case SC_SCREENSAVE:	// screensaver trying to start?
				case SC_MONITORPOWER: // monitor trying to enter powersave?
					return 0; // prevent from happening
				default:break;
			}
			break;								
		case WM_CLOSE: // close message
			PostQuitMessage(WM_QUIT); // send a quit message
			return(0);							
		case WM_KEYDOWN: // a key is being held down
			switch (wParam)
			{
				case VK_ESCAPE: // escape key
					PostQuitMessage(WM_QUIT); // post quit message 
					break;
				default: break;
			}							
			break;
		case WM_SIZE: // resize the OpenGL window
			Globals::Instance().gl.ReSizeGLScene(LOWORD(lParam),HIWORD(lParam));  // LoWord=width, HiWord=height
#ifdef USE_SHADERS
			if (Globals::Instance().gl.HasBeenInitialized())
			{
				CMenu::setToPerspectiveView();
			}
#endif
			break;							
		// process menu messages
		case WM_COMMAND: // respond to menu item selections
			switch(wParam) // see which menu item has been selected
            {
				case MENUID_EXIT: // exit
					PostQuitMessage(WM_QUIT); // post quit message
					break;
				case MENUID_ABOUT: // about info
					MessageBox(theHWnd,"Ben Nickson's 'Ozcan vs Nebulus & The Towergoround'\
					\n\n\tto contact Ben, e-mail ben_nickson@yahoo.co.uk","About",MB_OK);
					break;
				default: break;
			} // end switch
			break;
		case WM_QUERYNEWPALETTE:
		{ /*	Windows is telling the application that it may modify
				the system palette.  This message in essance asks the 
				application for a new palette.*/
			if(hPalette) // If the palette was created.
			{
				int nRet;
				// Selects the palette into the current device context
				SelectPalette(hDC, hPalette, FALSE);
				// Map entries from the currently selected palette to
				// the system palette.  The return value is the number 
				// of palette entries modified.
				nRet = RealizePalette(hDC);
				// Repaint, forces remap of palette in current window
				InvalidateRect(theHWnd,0,FALSE);
				return nRet;
			}
			break;
		}	
		// This window may set the palette, even though it is not the 
		// currently active window.
		case WM_PALETTECHANGED:
			// don't do anything if the palette does not exist, or if
			// this is the window that changed the palette.
			if((hPalette != 0) && ((HWND)wParam != theHWnd))
			{
				// Select the palette into the device context
				SelectPalette(hDC,hPalette,FALSE);

				// Map entries to system palette
				RealizePalette(hDC);
				
				// Remap the current colors to the newly realized palette
				UpdateColors(hDC);
				return 0;
			}
			break;
		default: break;
	}// end switch uMsg
	// pass all unhandled messages to DefWindowProc
	return DefWindowProc(theHWnd,uMsg,wParam,lParam);
}

void DrawTriangleWithShader(MSG&, BOOL&);
//void DrawTriangleWithShaderSetUp(MSG&, BOOL&);
void DrawTriangleWithShaderNoLoop();

int WINAPI WinMain(	HINSTANCE, // instance
					HINSTANCE, // previous instance
					LPSTR, // command line parameters
					int) // window show state
{
	MSG		msg; // windows message structure
	memset(&msg, 0, sizeof(msg));
	BOOL	done = FALSE; // Bool variable to exit game loop

	// ask the user for screen mode preferance
	if (MessageBox(0,"Would You Like To Play In Fullscreen? (Recommended)", 
		"Screen Settings (Ozcan vs Nebulus & The Towergoround)",MB_YESNO|MB_ICONQUESTION)==IDNO){
		fullscreen=false;} // windowed mode
	
	if (!CreateGameWindow())
	{
		MessageBox(0,"Initialization Failure","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return 0;// quit if window was not created
	}

	if (!Globals::Instance().gl.InitGL())
	{
		MessageBox(0,"Initialization Failure","ERROR",MB_OK|MB_ICONEXCLAMATION);
#ifdef USE_SHADERS
		Text::DeleteMesh();
		Globals::Instance().gl.DestroyGL(); // must be donw before killing the game window
#endif
		KillGameWindow();
		return 0;// quit if window was not created
	}

    int modelMatrixLocation = Globals::Instance().gl.GetUniformLocation("uModelMatrix");
    Globals::Instance().modelMatrixStack.SetMatrixLocation(modelMatrixLocation);
    
    int viewMatrixLocation = Globals::Instance().gl.GetUniformLocation("uViewMatrix");
    Globals::Instance().viewMatrixStack.SetMatrixLocation(viewMatrixLocation);
    if (Globals::Instance().gl.IsUsingSubRoutines())
    {
        Globals::Instance().gl.SetSubroutineUniformIndex(Globals::Instance().gl.GetSubroutineUniformIndex("mainRender", GL_FRAGMENT_SHADER),
                                                     Globals::Instance().gl.GetSubroutineIndex("RenderScene", GL_FRAGMENT_SHADER), GL_FRAGMENT_SHADER);
        Globals::Instance().gl.SetSubroutineUniformIndex(Globals::Instance().gl.GetSubroutineUniformIndex("mainRender", GL_VERTEX_SHADER),
                                                     Globals::Instance().gl.GetSubroutineIndex("RenderScene", GL_VERTEX_SHADER), GL_VERTEX_SHADER);
    }
    else
    {
        int mainRenderVertexShaderLocation = Globals::Instance().gl.GetUniformLocation("uMainRenderVertexShader");
        Globals::Instance().gl.SetUniformBool(mainRenderVertexShaderLocation, false);
        
        int mainRenderFragmentShaderLocation = Globals::Instance().gl.GetUniformLocation("uMainRenderFragmentShader");
        Globals::Instance().gl.SetUniformBool(mainRenderFragmentShaderLocation, false);
    }
    
    int viewProjectionLightMatrixLocation = Globals::Instance().gl.GetUniformLocation("uViewProjectionLightMatrix");
    Globals::Instance().gl.SetUniformMatrix(viewProjectionLightMatrixLocation, CMatrix());

#ifdef USE_SHADERS
	CMenu::SetPerspectiveProjectionMatrix();
#endif

	Game game;
	if(!game.Init()) 
	{
		MessageBox(0,"Initialization Failure","ERROR",MB_OK|MB_ICONEXCLAMATION);
#ifdef USE_SHADERS
		Text::DeleteMesh();
		Globals::Instance().gl.DestroyGL(); // must be donw before killing the game window
#endif
		KillGameWindow(); // kill window and exit if QueryPerformanceFrequency didn't work
		return (0); // failure
	}

	Globals::Instance().sound.PlaySound( SOUNDS_MAINMUSIC, true, false ); //start repeatedly playing main music

	while(!done) // loop that runs while done = FALSE
	{	
		if (PeekMessage(&msg,0,0,0,PM_REMOVE)) // is there a message waiting?
		{
			if (msg.message==WM_QUIT){ // is this quit message?
				done=TRUE;}	// if so done = TRUE
			else // not a quit message (so send message to message handler WndProc)
			{
				TranslateMessage(&msg);	// translate the message
				DispatchMessage(&msg); // dispatch the message
			}
		}
		else // their are no messages
		{
			done = !game.Update();
			//DrawTriangleWithShaderNoLoop();

#if _DEBUG
			Globals::Instance().debug.printDebug();
#endif
			SwapBuffers(hDC); // swap the frame buffers
		} // end if peekMessage
	} // end while not done 

#ifdef USE_SHADERS
	CHECK_GL_ERROR;
#endif

	// Shutdown
	game.DeleteGameObjects(); // delete all game specific objects
//#endif
#ifdef USE_SHADERS
	Text::DeleteMesh();
	Globals::Instance().gl.DestroyGL(); // must be donw before killing the game window
#endif
	KillGameWindow();	// destroy the window
	return (msg.wParam); // exit the program
} // end WinMain