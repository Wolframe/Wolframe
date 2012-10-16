#include <stdio.h>

void printMemory( const char* title, const unsigned char* start, unsigned int length )
{
	int row, col, i = 0;

	//iterate through the rows, which will be 16 bytes of memory wide
	printf( "%s:\n", title );
	for ( row = 0; (i + 1) < length; row++ )	{
		//print hex representation
		for ( col = 0; col <16; col++ )	{
			//calculate the current index
			i = row * 16 + col;
			//divides a row of 16 into two columns of 8
			if ( col == 8 )
				printf( " " );
			//print the hex value if the current index is in range.
			if ( i < length)
				printf( "%02X", start[ i ] );
			//print a blank if the current index is past the end
			else
				printf( "  " );
			//print a space to keep the values separate
			printf( " " );
		}
		//create a vertial seperator between hex and ascii representations
		printf( " " );
		//print ascii representation
		for ( col = 0; col < 16; col++ )	{
			//calculate the current index
			i = row * 16 + col;
			//divides a row of 16 into two coumns of 8
			if( col == 8 )
				printf( "  " );
			//print the value if it is in range
			if( i < length )	{
				//print the ascii value if applicable
				if( start[ i ] > 0x20 && start[ i ] < 0x7F )  //A-Z
					printf( "%c", start[ i ] );
				//print a period if the value is not printable
				else
					printf( "." );
			}
			//nothing else to print, so break out of this for loop
			else
				break;
		}
		//create a new row
		printf( "\n" );
	}
}

