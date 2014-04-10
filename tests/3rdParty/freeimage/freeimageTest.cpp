//
// testing FreeImage library
//

#include "gtest/gtest.h"

#include <iostream>
#include <sstream>
#include <cassert>

#include "FreeImage.h"

static void FreeImageErrorHandler( FREE_IMAGE_FORMAT format, const char *message ) {
	std::cerr << "\n*** ";
	if(format != FIF_UNKNOWN) {
		std::cerr << FreeImage_GetFormatFromFIF(format) << " Format\n";
	}
	std::cerr << message;
	std::cerr << " ***\n";
}

// The fixture for testing class _Wolframe::module
class FreeImageFixture : public ::testing::Test
{
	protected:

	protected:
		FreeImageFixture( )
		{
// call this ONLY when linking with FreeImage as a static library
#ifdef FREEIMAGE_LIB
			FreeImage_Initialise( );
#endif // FREEIMAGE_LIB
			FreeImage_SetOutputMessage( FreeImageErrorHandler );
		}
		
		~FreeImageFixture( )
		{
// call this ONLY when linking with FreeImage as a static library
#ifdef FREEIMAGE_LIB
	FreeImage_DeInitialise( );
#endif // FREEIMAGE_LIB
		}
};

TEST_F( FreeImageFixture, VersionInfo )
{
	//std::cout << "FreeImage " << FreeImage_GetVersion( ) << "\n";
	//std::cout << FreeImage_GetCopyrightMessage( ) << "\n\n";

	const char *version = FreeImage_GetVersion( );
	std::ostringstream ss;
	ss << FREEIMAGE_MAJOR_VERSION << "." << FREEIMAGE_MINOR_VERSION << "." << FREEIMAGE_RELEASE_SERIAL;
	(void)ss.str( ).c_str( );
//	ASSERT_STREQ( version, ss.str( ).c_str( ) );
}

#define TESTFILE "test.png"
#define THUMB "thumb.png"

TEST_F( FreeImageFixture, ImageInfo )
{
// load the image
	FREE_IMAGE_FORMAT format = FIF_UNKNOWN;

// check the file signature and deduce its format
// (the second argument is currently not used by FreeImage)
	format = FreeImage_GetFileType( TESTFILE, 0 );
	if( format == FIF_UNKNOWN ) {
// no signature ?
// try to guess the file format from the file extension
		format = FreeImage_GetFIFFromFilename( TESTFILE );
	}
	
	FIBITMAP *image = 0;
	
// check that the plugin has reading capabilities ...
	if( ( format != FIF_UNKNOWN ) && FreeImage_FIFSupportsReading( format ) ) {
// ok, let's load the file
		image = FreeImage_Load( format, TESTFILE, 0 );
	}
	assert( image != 0 );

// get info about the image	
	unsigned int width = FreeImage_GetWidth( image );
	unsigned int height = FreeImage_GetHeight( image );
	//std::cout << "size is " << width << " x " << height << std::endl;
	ASSERT_EQ( width, 312u );
	ASSERT_EQ( height, 312u );

// make thumbnail
	FIBITMAP *thumb = FreeImage_MakeThumbnail( image, 50, true );

// get info about the thumb
	unsigned int widthThumb = FreeImage_GetWidth( thumb );
	unsigned int heightThumb = FreeImage_GetHeight( thumb );
	//std::cout << "size of thumb is " << widthThumb << " x " << heightThumb << std::endl;
	ASSERT_EQ( widthThumb, 50u );
	ASSERT_EQ( heightThumb, 50u );
	
// save image (for debugging mainly)
	FreeImage_Save( FIF_PNG, thumb, THUMB, 0 );
	
// Unload the bitmap
	FreeImage_Unload( image );
}

int main( int argc, char **argv )
{
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS( );
}
