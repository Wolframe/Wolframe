extern "C" {
	#include "rlib.h"
}

#include "rlibfilter.hpp"

int main( void )
{
	rlib *r;

	r = rlib_init( );
	rlib_add_datasource( r, "wolf", static_cast<input_filter *>( wolf_rlib_new_input_filter( ) ) );
	rlib_add_query_as( r, "wolf", "a", "b" );
	rlib_add_report( r, "rlibreport.xml" );
	rlib_set_output_format( r, RLIB_FORMAT_PDF );
	rlib_execute( r );
	rlib_spool( r );

	rlib_free( r );

	return 0;
}
