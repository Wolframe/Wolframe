#include <Python.h>

#include <iostream>

static const char *pyGetReprStr( PyObject *o )
{
	PyObject *repr = PyObject_Repr( o );
	const char *s = PyUnicode_AsUTF8( repr );
	return s;
}

int main( int argc, char *argv[] )
{
	if( argc < 2 ) {
		std::cerr << "Usage: embed <module>" << std::endl;
		return 1;
	}

	Py_SetProgramName( (wchar_t *)argv[0] ); 

	Py_Initialize( );
	
	// needed to load module from current path, othwerwise python
	// searches the default pathes only
	PyRun_SimpleString( "import sys, os; sys.path.insert( 0, os.getcwd( ) )" );
	
	// avoid creation of __pycache__ directory, we would like to
	// redirect it later and make it an option in the wolframe
	// configuration
	PyRun_SimpleString( "sys.dont_write_bytecode = True" );

	PyObject *name = PyUnicode_FromString( argv[1] );

	PyObject *module = PyImport_Import( name );
	Py_DECREF( name );

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

	PyObject *f = PyObject_GetAttrString( module, "func" );
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

	PyObject *value = PyObject_CallObject( f, NULL );
	puts( pyGetReprStr( value ) );
		
	Py_DECREF( f );

	Py_DECREF( module );

	Py_Finalize( );

	return 0;
}
