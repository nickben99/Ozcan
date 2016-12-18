// upto here - ios refactor

//------------------------------------------------------------------


// CLASS REUSED - FROM THE OPENGL SUPERBIBLE


//------------------------------------------------------------------
/*
 * Windows BMP file functions for OpenGL.
 *
 * Written by Michael Sweet.
 */

#include "imageFileLoaders.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#ifdef OSX
#include <string.h> // for memcmp
#endif

/*
 * 'LoadDIBitmap()' - Load a DIB/BMP file from disk.
 *
 * Returns a pointer to the bitmap if successful, 0 otherwise...
 */

GLubyte *							/* O - Bitmap data */
LoadDIBitmap(const char *filename,	/* I - File to load */
             unsigned short &uHeight,	// image height
			 unsigned short &uWidth,	// image width
			 unsigned char &uBytesPerPixel )
{
    FILE             *fp;			/* Open file pointer */
    GLubyte          *bits;			/* Bitmap pixel bits */
    int              bitsize;		/* Size of bitmap */
    unsigned int     infosize;		/* Size of header information */
    BITMAPFILEHEADER header;		/* File header */
	BITMAPINFO *info;				// Bitmap information 

    /* Try opening the file; use "rb" mode to read this *binary* file. */
#ifdef _WINDOWS
	fopen_s(&fp, filename, "rb");
#endif
#ifdef OSX
    fp = fopen(filename, "rb");
#endif
    if (fp == 0){
        return (0);}

    /* Read the file header and any following bitmap information... */
    if (fread(&header, sizeof(BITMAPFILEHEADER), 1, fp) < 1)
    {
        /* Couldn't read the file header - return 0... */
		fclose(fp);
        return (0);
    }

    unsigned short bmReversed = ('M' << 8) | 'B';
    if (header.bfType != bmReversed)	/* Check for BM reversed... */
    {
        /* Not a bitmap file - return 0... */
#ifdef _WINDOWS
        char errorMsg[256];
        sprintf_s( errorMsg, "%s has a .bmp extension, but is not a bitmap file", filename );
        MessageBox( 0, errorMsg, "ERROR",MB_OK|MB_ICONEXCLAMATION );
#endif
        fclose(fp);
        return (0);
    }

    infosize = header.bfOffBits - sizeof(BITMAPFILEHEADER);
    if ((info = (BITMAPINFO *)malloc(infosize)) == 0)
    {
        /* Couldn't allocate memory for bitmap info - return 0... */
        fclose(fp);
        return (0);
    }

    size_t infoSizeRead = fread(info, 1, infosize, fp);
    if (infoSizeRead < infosize)
    {
        /* Couldn't read the bitmap header - return 0... */
        free(info);
        fclose(fp);
        return (0);
    }

    /* Now that we have all the header info read in, allocate memory for *
     * the bitmap and read *it* in...                                    */
    if ((bitsize = (info)->bmiHeader.biSizeImage) == 0)
	{
		bitsize = ((info)->bmiHeader.biWidth * (info)->bmiHeader.biBitCount + 7) / 8 * abs((info)->bmiHeader.biHeight); // width * bytesPerPixel * height
	}

    if ((bits = (unsigned char*)malloc(bitsize)) == 0)
    {
        /* Couldn't allocate memory - return 0! */
        free(info);
        fclose(fp);
        return (0);
    }

	unsigned int pixel = 0;
	int bytesPerPixel = (info->bmiHeader.biBitCount / 8);
	int pixelCount = bitsize / bytesPerPixel; // number of pixels

	// Start on the last line and work way up the image
	// as bitmaps are stored upside down

	unsigned int y = abs((info)->bmiHeader.biHeight) - 1;
	for (int i = 0, x = 0; i < pixelCount; i++, x++)
	{
		// finished a whole row move up one line
		// as i is a multiple of biWidth
		if (i > 0 && (i % (info)->bmiHeader.biWidth == 0))
		{
			--y;
			x = 0;
		}
		fread(&pixel, 1, bytesPerPixel, fp); // Read the pixel in
		GLubyte *pPtr = (GLubyte*) &pixel;
		int offset = bytesPerPixel *(y *(info)->bmiHeader.biWidth + x);

		if (3 == bytesPerPixel)
		{
			bits[offset] = pPtr[2];
			// Write the pixel to image space also swap red and blue channels
			bits[offset + 1] = pPtr[1];
			bits[offset + 2] = pPtr[0];
		}
		else
		{			
			bits[offset] = pPtr[3];
			// Write the pixel to image space also swap red and blue channels
			bits[offset + 1] = pPtr[2];
			bits[offset + 2] = pPtr[1];

			bits[offset + 3] = pPtr[0]; // alpha
		}
	}
	fclose(fp);

	// set referance variables
	uHeight = static_cast<unsigned short>( info->bmiHeader.biHeight );
	uWidth = static_cast<unsigned short>( info->bmiHeader.biWidth );
	uBytesPerPixel = (unsigned char)bytesPerPixel; // currently, this loader only accepts a 24 bit bitmap
	free( info );
	info = 0;
	return (bits);
}

// this method loads an uncompressed targa (BASED ON METHOD FROM NEHE LESSON 24)
GLubyte* LoadUncompressedTarga( const char *filename,	unsigned short &uHeight, // image height
														unsigned short &uWidth,	// image width
														unsigned char &ubytesPerPixel ) // no of bytes per pixel
{
	GLubyte		TGAheader[12]={0,0,2,0,0,0,0,0,0,0,0,0}; // uncompressed TGA Header
	GLubyte		TGAcompare[12];	// used to compare TGA header
	GLubyte		header[6]; // first 6 useful bytes from the header
	GLuint		imageSize; // used to store the image size
	GLuint		temp;									
	GLubyte*	imageData; // stores the pixel data from the tga file

	FILE *file = 0;
#ifdef _WINDOWS
    fopen_s(&file, filename, "rb"); // open the TGA file
#endif
#ifdef OSX
    file = fopen(filename, "rb"); // open the TGA file
#endif
	if(	file==0 || // does file exist
		fread(TGAcompare,1,sizeof(TGAcompare),file)!=sizeof(TGAcompare) ||	// are there 12 bytes to read?
		memcmp(TGAheader,TGAcompare,sizeof(TGAheader))!=0				||	// is it an uncompressed header?
		fread(header,1,sizeof(header),file)!=sizeof(header)) // read the rest of the header			
	{
		if (file == 0){ // did the file exist? 
			return( 0 );}								
		else
		{
			fclose(file); // if anything failed, close the file
			file = 0;
			return( 0 );
		}
	}

	uWidth  = header[1] * 256 + header[0]; // determine the TGA width	(highbyte*256+lowbyte)
	uHeight = header[3] * 256 + header[2]; // determine the TGA height	(highbyte*256+lowbyte)
    
 	if(	uWidth	== 0	|| // is the width less than or equal to zero
		uHeight	== 0	|| // is the height less than or equal to zero
		(header[4] != 24 && header[4] != 32 )) // is the TGA 24 or 32 bit?
	{
		fclose(file); // close The File	
		file = 0; 
		return( 0 );								
	}

	ubytesPerPixel	= header[4]/8; // divide by 8 to get the bytes per pixel
	imageSize		= uWidth*uHeight*ubytesPerPixel; // calculate the memory required in bytes for the TGA

	imageData=(GLubyte *)malloc(imageSize);	// allocate memory

	if(	imageData==0 || // was the memory allocated?
		fread(imageData, 1, imageSize, file) != imageSize)	// was the info read ok?
	{
		if(imageData != 0){ // was any data loaded
			free( imageData ); // release any data loaded
			imageData = 0;}

		fclose(file); // close the file
		file = 0;
		return( 0 );				
	}

	for(GLuint i=0; i<int(imageSize); i+=ubytesPerPixel) // loop through the image data
	{	// swaps the 1st and 3rd bytes ('R'ed and 'B'lue)
		temp=imageData[i]; // temporarily store the value At Image Data 'i'
		imageData[i] = imageData[i + 2]; // set the 1st byte to the value of the 3rd byte
		imageData[i + 2] = (GLubyte)temp; // set the 3rd byte to the value in 'temp' (1st byte value)
	}

	fclose (file); // close The File
	file = 0;
	return( imageData ); // return the rgb(a) pixel data
}

// this method loads a Run Length Encoded (RLE) compressed targa
GLubyte* LoadRLECompressedTarga( const char *filename,	unsigned short &uHeight,	// image height
														unsigned short &uWidth,	// image width
														unsigned char &uBytesPerPixel )
{
	GLubyte		TGAheader[12]={0,0,10,0,0,0,0,0,0,0,0,0}; // RLE compressed TGA header
	GLubyte		TGAcompare[12];	// used to compare TGA header
	GLubyte		header[6]; // first 6 useful bytes from the header
	GLuint		imageSize; // used to store the size of the rgba data in bytes
	GLubyte*	imageData = 0; // stores the pixel data from the tga file

	FILE *file = 0;
#ifdef _WINDOWS
    fopen_s(&file, filename, "rb"); // open the TGA file
#endif
#ifdef OSX
    file = fopen(filename, "rb"); // open the TGA file
#endif
	if(	file==0 || // does the file exist?
		fread(TGAcompare,1,sizeof(TGAcompare),file)!=sizeof(TGAcompare) ||	// are there 12 bytes to read?
		memcmp(TGAheader,TGAcompare,sizeof(TGAheader))!=0				||	// is it a compressed tga header?
		fread(header,1,sizeof(header),file)!=sizeof(header)) // read the rest of the header		
	{
		if (file == 0){ // did the file exist 
			return( 0 );}					
		else
		{
			fclose(file); // close the file
			file = 0;
			return( 0 );				
		}
	}

	uWidth  = header[1] * 256 + header[0]; // determine the TGA width	(highbyte*256+lowbyte) or ( (highbyte<<8)+lowbyte )
	uHeight = header[3] * 256 + header[2]; // determine the TGA height	(highbyte*256+lowbyte) or ( (highbyte<<8)+lowbyte )
    
 	if(	uWidth	== 0	|| // is the width less than or equal to zero
		uHeight	== 0	|| // is the height less than or equal to zero
		(header[4] != 24 && header[4] != 32 )) // is the TGA 24 or 32 bit?
	{
		fclose(file); // close The File
		file = 0;								
		return( 0 );								
	}

	uBytesPerPixel	= header[4]/8; // divide by 8 to get the bytes per pixel
	imageSize		= uWidth*uHeight*uBytesPerPixel; // calculate the memory required for the TGA data

	imageData=(GLubyte *)malloc(imageSize); // allocate memory
	if ( !imageData ){ // memory was not allocated correctly
		fclose( file );
		file = 0;
		return( 0 );}


	GLuint pixelcount	= uHeight * uWidth;	// get number of pixels in the image
	GLuint currentpixel	= 0; // current pixel being read
	GLuint currentbyte	= 0; // current byte 
	GLubyte *colorbuffer = (GLubyte*)malloc(uBytesPerPixel); // allocate storage for 1 pixel

	if ( !colorbuffer ){ // check to ensure colorbuffer was allocated properly
		if( file ){ fclose(file); file = 0;}		
		if( imageData ){ free(imageData); imageData = 0;}
		return( 0 );} // error

	do
	{
		GLubyte chunkheader = 0; // storage for "chunk" header

		if(fread(&chunkheader, sizeof(GLubyte), 1, file) == 0) // read in the 1 byte header
		{
			// an error has occurred, so close the file and dealocate any data
			if(file){ fclose(file); file = 0;}		
			if(colorbuffer){ free(colorbuffer); colorbuffer = 0;}
			if(imageData){ free(imageData); imageData = 0;}
			return( 0 ); // error
		}

		// if ( !( chunkheader & 0x80 ) )
		if(chunkheader < 128) /*	if the ehader is < 128, it means the that is the number of RAW 
									color packets minus 1 that follow the header */
		{	
			chunkheader++; // add 1 to get number of following color values
			for(short counter = 0; counter < chunkheader; counter++) // read RAW color values
			{
				if(fread(colorbuffer, 1, uBytesPerPixel, file) != uBytesPerPixel) // Try to read 1 pixel
				{
					// an error has occurred, so close the file and dealocate any data
					if(file){ fclose(file); file = 0;}		
					if(colorbuffer){ free(colorbuffer); colorbuffer = 0;}
					if(imageData){ free(imageData); imageData = 0;}
					return( 0 ); // Return failed
				}
				imageData[currentbyte		] = colorbuffer[2]; // Flip R and B vcolor values around in the process 
				imageData[currentbyte + 1	] = colorbuffer[1];
				imageData[currentbyte + 2	] = colorbuffer[0];

				if(uBytesPerPixel == 4){	// if its a 32 bpp image
					imageData[currentbyte + 3] = colorbuffer[3];} // copy the 4th byte

				currentbyte += uBytesPerPixel; // increase the current byte by the number of bytes per pixel
				currentpixel++;	

				if(currentpixel > pixelcount) // make sure we havent read too many pixels
				{
					// an error has occurred, so close the file and dealocate any data
					if(file){ fclose(file); file = 0;}		
					if(colorbuffer){ free(colorbuffer); colorbuffer = 0;}
					if(imageData){ free(imageData); imageData = 0;}
					return( 0 ); // return failed
				}
			}
		}
		else // chunkheader > 128 RLE data, next color reapeated (chunkheader - 127) times
		{	
			chunkheader -= 127; // Subteact 127 to get rid of the ID bit (alternately ( chunkheader ^= 0x80 ) )
			if(fread(colorbuffer, 1, uBytesPerPixel, file) != uBytesPerPixel) // attempt to read following color values
			{	
				// an error has occurred, so close the file and dealocate any data
				if(file){ fclose(file); file = 0;}		
				if(colorbuffer){ free(colorbuffer); colorbuffer = 0;}
				if(imageData){ free(imageData); imageData = 0;}
				return( 0 );	// return failed
			}

			for( short counter = 0; counter < chunkheader; counter++ )	/*	copy the color into the image data as many 
																				times as dictated by the header */
			{
				imageData[currentbyte		] = colorbuffer[2];	// switch R and B bytes areound while copying
				imageData[currentbyte + 1	] = colorbuffer[1];
				imageData[currentbyte + 2	] = colorbuffer[0];

				if ( uBytesPerPixel == 4 ){	// if TGA image is 32 bpp
					imageData[currentbyte + 3] = colorbuffer[3];} // copy 4th byte

				currentbyte += uBytesPerPixel; // increase current byte by the number of bytes per pixel
				currentpixel++; // increase pixel count by 1

				if(currentpixel > pixelcount) // make sure we havent written too many pixels
				{
					// an error has occurred, so close the file and dealocate any memory
					if(file){ fclose(file); file = 0;}		
					if(colorbuffer){ free(colorbuffer); colorbuffer = 0;}
					if(imageData){ free(imageData); imageData = 0;}
					return( 0 );	// return failed
				}
			}
		}
	} while(currentpixel < pixelcount); // loop while there are still pixels left
	
	fclose(file); // close the file
	file = 0;

	free( colorbuffer );
	colorbuffer = 0;

	return( imageData ); // return success
}

// this method loads a RLE compressed or uncompressed targa file
GLubyte* LoadTarga(const char *filename,	unsigned short &uHeight,	// image height
											unsigned short &uWidth,	// image width
											unsigned char &uBytesPerPixel )
{
	GLubyte TGAheaderUncompressed[12] = {0,0,2,0,0,0,0,0,0,0,0,0};	// uncompressed TGA Header
	GLubyte TGAheaderRLECompressed[12] = {0,0,10,0,0,0,0,0,0,0,0,0};	// RLE compressed TGA Header
	GLubyte	TGAcompare[12]; // Used To Compare TGA Header

	FILE *file = 0;
#ifdef _WINDOWS
    fopen_s(&file, filename, "rb"); // Open The TGA File
#endif
#ifdef OSX
    file = fopen(filename, "rb"); // Open The TGA File
#endif
	if(	!file ){ // if the file does not exist
		return( 0 );} // return 0

	if (fread( TGAcompare, 1, sizeof(TGAcompare), file ) != sizeof( TGAcompare ) ){	// Are There 12 Bytes To Read?
		fclose(file);
		file = 0;
		return( 0 );}

	// is this an uncompressed tga?
	if ( memcmp( TGAheaderUncompressed, TGAcompare, sizeof( TGAheaderUncompressed ) ) == 0 ){
		fclose( file ); // close the file
		file = 0;
		return( LoadUncompressedTarga( filename, uHeight, uWidth, uBytesPerPixel ) );}

	// is this an RLE compressed tga?
	if ( memcmp( TGAheaderRLECompressed, TGAcompare, sizeof( TGAheaderRLECompressed ) ) == 0 ){
		fclose( file ); // close the file
		file = 0;
		return( LoadRLECompressedTarga( filename, uHeight, uWidth, uBytesPerPixel ) );}

#ifdef _WINDOWS
	char errorMsg[256];
	sprintf_s( errorMsg, "%s has a .tga extension, but is not an uncompressed or RLE compressed targa file", filename );
	MessageBox( 0, errorMsg, "ERROR",MB_OK|MB_ICONEXCLAMATION );
#endif
	fclose( file ); // close the file
	file = 0;
	return( 0 );
}

