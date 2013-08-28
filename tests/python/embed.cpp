#include <Python.h>

#include <iostream>
#include <cstdlib>

#define FUNC_NAME "func"
#define DIRECT_PARAMS

static const char *pyGetReprStr( PyObject *o )
{
	PyObject *repr = PyObject_Repr( o );
#if PY_MAJOR_VERSION >= 3 && PY_MINOR_VERSION >= 3
	const char *s = PyUnicode_AsUTF8( repr );
#else
	PyObject *pstr = PyUnicode_AsUTF8String( repr );
	const char *s = PyBytes_AsString( pstr );
#endif
	return s;
}

int main( int argc, char *argv[] )
{
	if( argc < 2 ) {
		std::cerr << "Usage: embed <module>" << std::endl;
		return 1;
	}

	// set name of program, this is used for resolving path names
	// for libraries (TODO: set to what in wolframe?!)
	wchar_t progname[FILENAME_MAX + 1];
	mbstowcs( progname, argv[0], strlen( argv[0] ) + 1 );
	Py_SetProgramName( progname ); 

	// initialize Python interpreter, avoid registration of signal handlers,
	// let's Wolframe do this..
	Py_InitializeEx( 0 );
	
	// needed to load module from current path, othwerwise python
	// searches the default pathes only
	PyRun_SimpleString( "import sys, os; sys.path.insert( 0, os.getcwd( ) )" );
	
	// avoid creation of __pycache__ directory, we would like to
	// redirect it later and make it an option in the wolframe
	// configuration
	PyRun_SimpleString( "sys.dont_write_bytecode = True" );

	// Build the name object, use 'PyUnicode_FromString' not 'PyString_FromString'!
	PyObject *name = PyUnicode_FromString( argv[1] );

	// Load the module object
	PyObject *module = PyImport_Import( name );
	Py_DECREF( name );

	// Check if the module could be loaded and if it is valid
	if( module == NULL || !PyModule_Check( module ) ) {
		PyObject *exceptionType, *exceptionValue, *exceptionTraceBack;
		PyErr_Fetch( &exceptionType, &exceptionValue, &exceptionTraceBack );	
		std::cerr << "Unable to load '" << argv[1] << "'" << std::endl;
		std::cerr << "Exception type: " << pyGetReprStr( exceptionType ) << std::endl;
		std::cerr << "Exception value: " << pyGetReprStr( exceptionValue ) << std::endl;
		std::cerr << "Exception traceback: " << pyGetReprStr( exceptionTraceBack ) << std::endl;
		Py_Finalize( );
		return 1;
	}

	PyObject *f = PyObject_GetAttrString( module, FUNC_NAME );
	if( !f || !PyCallable_Check( f ) ) {
		PyObject *exceptionType, *exceptionValue, *exceptionTraceBack;
		PyErr_Fetch( &exceptionType, &exceptionValue, &exceptionTraceBack );	
		std::cerr << "Unable to find function 'func' in module" << std::endl;
		std::cerr << "Exception type: " << pyGetReprStr( exceptionType ) << std::endl;
		std::cerr << "Exception value: " << pyGetReprStr( exceptionValue ) << std::endl;
		std::cerr << "Exception traceback: " << pyGetReprStr( exceptionTraceBack ) << std::endl;
		Py_DECREF( module );
		Py_Finalize( );
		return 1;
	}

	int argInt = 47;
	double argDouble = 4.711;

#ifdef DIRECT_PARAMS
	
	// build argument list (old style, but with 3.x functions)
	// this may be better for the iterator style of structure we
	// have in Wolframe

	PyObject *args = PyTuple_New( 2 );
	PyObject *arg1 = PyLong_FromLong( argInt );
	PyObject *arg2 = PyFloat_FromDouble( argDouble );
	PyTuple_SetItem( args, 0, arg1 );
	PyTuple_SetItem( args, 1, arg2 );

#else
		
	// build argument list (method style)
	// more high-level

	PyObject *args = Py_BuildValue("(if)", argInt, argDouble );
	if( !args ) {
		PyObject *exceptionType, *exceptionValue, *exceptionTraceBack;
		PyErr_Fetch( &exceptionType, &exceptionValue, &exceptionTraceBack );	
		std::cerr << "Unable to build arguments for functions" << std::endl;
		std::cerr << "Exception type: " << pyGetReprStr( exceptionType ) << std::endl;
		std::cerr << "Exception value: " << pyGetReprStr( exceptionValue ) << std::endl;
		std::cerr << "Exception traceback: " << pyGetReprStr( exceptionTraceBack ) << std::endl;
		Py_DECREF( module );
		Py_Finalize( );
		return 1;
	}

#endif

	// call function
	PyObject *value = PyObject_CallObject( f, args );
	if( !value ) {
		PyObject *exceptionType, *exceptionValue, *exceptionTraceBack;
		PyErr_Fetch( &exceptionType, &exceptionValue, &exceptionTraceBack );	
		std::cerr << "Unable to call function '" << FUNC_NAME << "'" << std::endl;
		std::cerr << "Exception type: " << pyGetReprStr( exceptionType ) << std::endl;
		std::cerr << "Exception value: " << pyGetReprStr( exceptionValue ) << std::endl;
		std::cerr << "Exception traceback: " << pyGetReprStr( exceptionTraceBack ) << std::endl;
		Py_DECREF( args );
		Py_DECREF( f );
		Py_DECREF( module );
		Py_Finalize( );
		return 1;
	}
	
	// get result of function
	puts( pyGetReprStr( value ) );
		
	// clean up objects
	Py_DECREF( args );
	Py_DECREF( f );
	Py_DECREF( module );

	// shut own Python interpreter
	Py_Finalize( );

	return 0;
}
