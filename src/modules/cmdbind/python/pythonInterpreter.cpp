#include "pythonInterpreter.hpp"
#include "logger-v1.hpp"
#include "utils/fileUtils.hpp"

#include <stdexcept>
#include <sstream>

using namespace _Wolframe;
using namespace _Wolframe::langbind;

python::Interpreter::Interpreter( const std::string prgfile )
{
	MOD_LOG_TRACE << "[python] Creating Python interpreter with module " << prgfile;

	std::string modulePath = utils::getParentPath( prgfile, 1 );
	MOD_LOG_TRACE << "[python] Primary Python load path '" << modulePath << "'";

	// should be set to what? 'python' sets it to itself, setting it
	// to modulePath or wolframe leads to introspection problems later!
	//~ wchar_t progname[FILENAME_MAX + 1];
	//~ mbstowcs( progname, modulePath.c_str( ), modulePath.size( ) + 1 );
	//~ Py_SetProgramName( progname ); 
	
	Py_InitializeEx( 0 );
	
	PyRun_SimpleString( "import sys, os; sys.path.insert( 1, os.getcwd( ) )" );
	std::ostringstream s;
	s << "sys.path.insert( 1, '" << modulePath << "' )";
	PyRun_SimpleString( s.str( ).c_str( ) );

	PyRun_SimpleString( "sys.dont_write_bytecode = True" );
	
	std::string moduleName = utils::getFileStem( prgfile );
	MOD_LOG_TRACE << "[python] Python module name '" << moduleName << "'";
	PyObject *name = PyUnicode_FromString( moduleName.c_str( ) );
	
	PyObject *module = PyImport_Import( name );

	Py_DECREF( name );

	if( module == NULL || !PyModule_Check( module ) ) {
		throw std::runtime_error( lastErrorMsg( "Unable to load module" ) );
	}

	PyObject *symbols = PyModule_GetDict( module );

	PyObject *key, *value;
	Py_ssize_t pos = 0;
	while( PyDict_Next( symbols, &pos, &key, &value ) ) {
		if( PyFunction_Check( value ) ) {
			MOD_LOG_TRACE 	<< "[python] function found "
					<< pyGetReprStr( key ) << ": "
					<< pyGetReprStr( value );
			m_functions.push_back( pyGetReprStr( value ) );
		}
	}

	Py_DECREF( module );
}

python::Interpreter::~Interpreter( )
{
	Py_Finalize( );
}

std::string python::Interpreter::lastErrorMsg( const std::string& str )
{
	PyObject *exceptionType, *exceptionValue, *exceptionTraceBack;
	PyErr_Fetch( &exceptionType, &exceptionValue, &exceptionTraceBack );
	std::ostringstream msg;
	msg << str << ": ";
	msg << ", exception type: " << pyGetReprStr( exceptionType );
	msg << ", exception value: " << pyGetReprStr( exceptionValue );
	msg << ", exception traceback: " << pyGetReprStr( exceptionTraceBack );
	return msg.str( );
}

const char *python::Interpreter::pyGetReprStr( PyObject *o )
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
