//
// testing FreeImagePlus library (C++ wrapper)
//

#include "gtest/gtest.h"

#include <iostream>

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
	ASSERT_EQ( width, 312 );
	ASSERT_EQ( height, 312 );
	
// make thumbnail
	fipImage thumb( image );
	thumb.makeThumbnail( 50, true );

// get info about the thumb
	unsigned int widthThumb = thumb.getWidth( );
	unsigned int heightThumb = thumb.getHeight( );
	//std::cout << "size of thumb is " << widthThumb << " x " << heightThumb << std::endl;
	ASSERT_EQ( widthThumb, 50 );
	ASSERT_EQ( heightThumb, 50 );
	
// save image (for debugging mainly)
	thumb.save( THUMB, 0 );
}

int main( int argc, char **argv )
{
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS( );
}