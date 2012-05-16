extern "C" {
	#include "rlib.h"
}

int main( void )
{
	rlib *r;

	r = rlib_init( );
	rlib_add_datasource_csv( r, "csv" );
	rlib_add_query_as( r, "csv", "rlibdata.csv", "data" );
	rlib_add_report( r, "rlibreport.xml" );
	rlib_set_output_format( r, RLIB_FORMAT_PDF );
	rlib_execute( r );
	rlib_spool( r );

	rlib_free( r );

	return 0;
}
