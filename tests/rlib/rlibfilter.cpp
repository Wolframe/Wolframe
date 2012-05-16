#include "rlibfilter.hpp"

#include <cstring>

static gint wolf_rlib_free_input_filter( gpointer input_ptr )
{
	struct input_filter *input = (struct input_filter *)input_ptr;
	g_free( input );
	return 0;
}


void *wolf_rlib_new_result_from_query( gpointer input_ptr, gchar *query ) 
{
}

gpointer wolf_rlib_new_input_filter( )
{
	struct input_filter *input;
	
	input = (struct input_filter *)g_malloc( sizeof( struct input_filter ) );
	memset( input, 0, sizeof( struct input_filter ) );
	input->_private = 0;
	/*
	input->input_close = rlib_csv_input_close;
	input->first = rlib_csv_first;
	input->next = rlib_csv_next;
	input->previous = rlib_csv_previous;
	input->last = rlib_csv_last;
	input->isdone = rlib_csv_isdone;
	input->get_error = rlib_csv_get_error;
	* */
	input->new_result_from_query = wolf_rlib_new_result_from_query;
	/*
	input->get_field_value_as_string = rlib_csv_get_field_value_as_string;
	input->resolve_field_pointer = rlib_csv_resolve_field_pointer;
	*/
	input->free = wolf_rlib_free_input_filter;
/*
 * 	input->free_result = rlib_csv_rlib_free_result;*
	*/
	return input;
}
