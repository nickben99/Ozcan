//CTextFileReader.cpp, a text file reader class

// system includes --------
//-------------------------

// header files -----------
#include "CTextFileReader.h"
#include <System/Interfaces/SystemInterface.h>
//-------------------------

// constructor
CTextFileReader::CTextFileReader()
{
}

// destructor
CTextFileReader::~CTextFileReader()
{
	closeFile(); // close file if one is open
}

bool CTextFileReader::ReadFile(const char* pFileName, string& fileContents)
{
	std::ifstream ifs(pFileName);
    
    bool ret = false;
    
	if (ifs.is_open())
	{
		fileContents = std::string((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
		ret = true;
	}
    
    return ret;
}

// open a text file for reading or writing
bool CTextFileReader::openFile(char * filename, int openOptions)
{
	closeFile(); // close file if its attached to an open file

	/*	nocreate means don't create the 
		file if it doesn't exist (seems to not be required with 
		ifstream rather than ifstream.h) */
	inFile.open(filename, openOptions);
						
	// Check if there was an error opening the file
	if (!inFile){
		return(false);}

	return(true); // opened succesfully
}

// get the next line of a text file
void CTextFileReader::getNextLine()
{
	// traverse to next line which is not a comment or a blank line (and not end of file)
	inFile.getline(currentLineOfFile, BUFFER_SIZE, '\n');
	while( ((!strncmp (currentLineOfFile, "//", 2)) || (0 == currentLineOfFile[0])) && 
		(inFile.eof() == false) ){ 
		inFile.getline(currentLineOfFile, BUFFER_SIZE, '\n');}
}

// write a string to a text file
void CTextFileReader::writeTextToFile(char * text)
{
	inFile << text;
}

// close the currently opened text file
void CTextFileReader::closeFile()
{
	if (inFile.is_open()){ // if the file handle currently has a file
		inFile.close();} // close the old file
}

bool CTextFileReader::DoesDirExists(const char* dirName)
{
	return PlatformSpecificDoesDirExists(dirName);
}