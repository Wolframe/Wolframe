//
// testing FreeImagePlus library (C++ wrapper)
//

#include "gtest/gtest.h"
#include "wtest/testReport.hpp"

#include <iostream>
#include <fstream>
#include <string>

#ifdef _WIN32
#define WIN32_MEAN_AND_LEAN
#include <windows.h>
#endif
#include "FreeImage.h"
#include "FreeImagePlus.h"

// The fixture for testing class _Wolframe::module
class FreeImagePlusFixture : public ::testing::Test
{
	protected:

	protected:
		FreeImagePlusFixture( )
		{
// call this ONLY when linking with FreeImage as a static library
#ifdef FREEIMAGE_LIB
			FreeImage_Initialise( );
#endif // FREEIMAGE_LIB
		}
		
		~FreeImagePlusFixture( )
		{
// call this ONLY when linking with FreeImage as a static library
#ifdef FREEIMAGE_LIB
	FreeImage_DeInitialise( );
#endif // FREEIMAGE_LIB
		}
};

#define TESTFILE "test.png"
#define THUMB "thumbcpp.png"

TEST_F( FreeImagePlusFixture, ImageInfo )
{
// load the image
	fipImage image;
	image.load( TESTFILE, 0 );

// get info about the image
	unsigned int width = image.getWidth( );
	unsigned int height = image.getHeight( );
	//std::cout << "size is " << width << " x " << height << std::endl;
	ASSERT_EQ( width, 312U );
	ASSERT_EQ( height, 312U );
	
// make thumbnail
	fipImage thumb( image );
	thumb.makeThumbnail( 50, true );

// get info about the thumb
	unsigned int widthThumb = thumb.getWidth( );
	unsigned int heightThumb = thumb.getHeight( );
	//std::cout << "size of thumb is " << widthThumb << " x " << heightThumb << std::endl;
	ASSERT_EQ( widthThumb, 50U );
	ASSERT_EQ( heightThumb, 50U );
	
// save image (for debugging mainly)
	thumb.save( THUMB, 0 );
}

#define COPYFILE "testmem.png"

TEST_F( FreeImagePlusFixture, ImageFromMemory )
{
// load the file
	std::ifstream ifs( TESTFILE, std::ios::in | std::ios::binary );
	std::vector<char> raw( ( std::istreambuf_iterator<char>( ifs ) ),
		( std::istreambuf_iterator<char>( ) ) );
	//std::cout << raw.size( ) << std::endl;
		
// open a memory handle
	fipMemoryIO memIO( (BYTE *)&raw[0], raw.size( ) );

// load image from buffer
	fipImage image;
	image.loadFromMemory( memIO );

// output data of the image in memory
	unsigned int width = image.getWidth( );
	unsigned int height = image.getHeight( );
	//std::cout << "size is " << width << " x " << height << std::endl;
	ASSERT_EQ( width, 312U );
	ASSERT_EQ( height, 312U );

// create freeimage memory handle for result
	fipMemoryIO outIO;

// write image into the buffer
	image.saveToMemory( FIF_PNG, outIO );
	
// output buffer to file, for debugging
	BYTE *thumbData = NULL;
	DWORD thumbSize = 0;
	outIO.acquire( &thumbData, &thumbSize );
	//std::cout << "bytesize of image is " << thumbSize << std::endl;

// output to file
	std::ofstream ofs( COPYFILE );
	ofs.write( (const char *)thumbData, thumbSize );
}

int main( int argc, char **argv )
{
	::testing::InitGoogleTest( &argc, argv );
	WOLFRAME_GTEST_REPORT( argv[0], refpath.string());
	return RUN_ALL_TESTS( );
}
