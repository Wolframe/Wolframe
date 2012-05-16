#include "rlibfilter.hpp"

#include <cstring>
#include <iostream>

using namespace std;

static gint wolf_rlib_input_close( gpointer input_ptr )
{
	return 0;
}

static gint wolf_rlib_free_input_filter( gpointer input_ptr )
{
	struct input_filter *input = (struct input_filter *)input_ptr;
	g_free( input );
	return 0;
}

static gint wolf_rlib_first( gpointer input_ptr, gpointer result_ptr )
{
	return FALSE;
}

static gint wolf_rlib_next( gpointer input_ptr, gpointer result_ptr )
{
	return FALSE;
}

static gint wolf_rlib_previous( gpointer input_ptr, gpointer result_ptr )
{
	return FALSE;
}

static gint wolf_rlib_last( gpointer input_ptr, gpointer result_ptr )
{
	return FALSE;
}

static gint wolf_rlib_isdone( gpointer input_ptr, gpointer result_ptr )
{
	return TRUE;
}

static const gchar* wolf_rlib_get_error( gpointer input_ptr )
{
	return "ERROR";
}

static void *wolf_rlib_new_result_from_query( gpointer input_ptr, gchar *query ) 
{
	cerr << "query: " << query << endl;
	
	return NULL;
}

static gchar *wolf_rlib_get_field_value_as_string( gpointer input_ptr, gpointer result_ptr, gpointer field_ptr )
{
	return "";
}

static gpointer wolf_rlib_resolve_field_pointer( gpointer input_ptr, gpointer result_ptr, gchar *name ) 
{
	return NULL; 
}

static void wolf_rlib_free_result( gpointer input_ptr, gpointer result_ptr )
{
}

gpointer wolf_rlib_new_input_filter( )
{
	struct input_filter *input;
	
	input = (struct input_filter *)g_malloc( sizeof( struct input_filter ) );
	memset( input, 0, sizeof( struct input_filter ) );
	input->_private = 0;
	input->input_close = wolf_rlib_input_close;
	input->first = wolf_rlib_first;
	input->next = wolf_rlib_next;
	input->previous = wolf_rlib_previous;
	input->last = wolf_rlib_last;
	input->isdone = wolf_rlib_isdone;
	input->get_error = wolf_rlib_get_error;
	input->new_result_from_query = wolf_rlib_new_result_from_query;
	input->get_field_value_as_string = wolf_rlib_get_field_value_as_string;
	input->resolve_field_pointer = wolf_rlib_resolve_field_pointer;
	input->free = wolf_rlib_free_input_filter;
	input->free_result = wolf_rlib_free_result;
	
	return input;
}
