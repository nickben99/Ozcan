//SystenWindows.h - useful defines

#ifndef _SystemWindows_h_
#define _SystemWindows_h_

#define SPRINTF sprintf_s

#define VSPRINTF vsprintf_s

#define SSCANF sscanf_s

inline void ErrorOutput(const char* text) {
	MessageBox(0,text,"ERROR",MB_OK|MB_ICONEXCLAMATION);
}

#endif // ifndef _SystemWindows_