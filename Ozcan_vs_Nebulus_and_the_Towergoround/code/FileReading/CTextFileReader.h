//CTextFileReader.h - header for the sprite class

//system includes------------
#include <fstream> // for reading files
#include <string.h> // for strcmp (string comparison)
using namespace std; // required to avoid problems when using fstream
//---------------------------

#ifndef _CTextFileReader_h_
#define _CTextFileReader_h_

//header files---------
//---------------------

//defines--------------
#define BUFFER_SIZE_WIN 1024 // line length
//---------------------

class CTextFileReader
{
	public:
//-----public variables-----------
		fstream inFile; // file handle
		char currentLineOfFile[BUFFER_SIZE];
//----------------------------------

//----public methods----------------------------------------
		CTextFileReader(); // constructor
		~CTextFileReader(); // destructor

		static bool DoesDirExists(const char* dirName);
		static bool ReadFile(const char* pFileName, std::string& fileContents);

		bool openFile(char * filename, int openOptions); // open a new text file
		void closeFile(); // close the currently opened text file
		void getNextLine(); // returns the next line of the text file
		// write a string to a text file
		void writeTextToFile(char * text);
		// overloaded operators

//-----------------------------------------------------------
}; // end class CTextFileReader

#endif // ifndef _CTextFileReader_h_ 