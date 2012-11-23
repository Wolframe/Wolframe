//
// testing FreeImage library
//

#include "gtest/gtest.h"

#include <iostream>

#include "FreeImage.h"

static void FreeImageErrorHandler( FREE_IMAGE_FORMAT fif, const char *message ) {
	std::cerr << "\n*** ";
	if(fif != FIF_UNKNOWN) {
		std::cerr << FreeImage_GetFormatFromFIF(fif) << " Format\n";
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

TEST_F( FreeImageFixture, SimpleTest )
{
//	EXPECT_EQ( output, must );
}

int main( int argc, char **argv )
{
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS( );
}
