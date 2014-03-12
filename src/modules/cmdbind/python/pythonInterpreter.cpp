#include "logger-v1.hpp"
#include "utils/fileUtils.hpp"
#include <stdexcept>
#include <sstream>
#include "pythonInterpreter.hpp"

extern "C" {
#include <Python.h>
}

using namespace _Wolframe;
using namespace _Wolframe::langbind;
using namespace _Wolframe::langbind::python;

InterpreterInstanceR python::Interpreter::getInstance( const std::string&) const
{
	return InterpreterInstanceR();
}

std::vector<std::string> python::Interpreter::loadProgram( const std::string& prgfile )
{
	LOG_TRACE << "[python] Loading module in file " << prgfile;
	if (m_modulePath.empty())
	{
		m_modulePath = utils::getParentPath( prgfile, 1 );
		LOG_TRACE << "[python] Primary Python load path '" << m_modulePath << "'";
	}
	std::vector<std::string> rt;
	std::string moduleName = utils::getFileStem( prgfile );
	LOG_TRACE << "[python] module name '" << moduleName << "'";
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
			LOG_TRACE 	<< "[python] function found "
					<< pyGetRepr( key ) << ": "
					<< pyGetRepr( value );
			m_functions.push_back( pyGetStr( key ) );
			rt.push_back( m_functions.back());
		}
	}

	Py_DECREF( module );
	return rt;
}

python::Interpreter::Interpreter()
{
	LOG_TRACE << "[python] Creating interpreter";

	// should be set to what? 'python' sets it to itself, setting it
	// to modulePath or wolframe leads to introspection problems later!
	//~ wchar_t progname[FILENAME_MAX + 1];
	//~ mbstowcs( progname, modulePath.c_str( ), modulePath.size( ) + 1 );
	//~ Py_SetProgramName( progname );

	Py_InitializeEx( 0 );

	PyRun_SimpleString( "import sys, os; sys.path.insert( 1, os.getcwd( ) )" );
	std::ostringstream s;

	//PF:NOTE: Do not know how to get the module path. There is no configuration avalable and no module path can be passed.
	//	The original source create the interpreter with one source file but this is obsolete. Open discussion how to proceed here.

	s << "sys.path.insert( 1, '" << m_modulePath << "' )";
	PyRun_SimpleString( s.str( ).c_str( ) );

	PyRun_SimpleString( "sys.dont_write_bytecode = True" );
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
	msg << ", exception type: " << pyGetRepr( exceptionType );
	msg << ", exception value: " << pyGetRepr( exceptionValue );
	msg << ", exception traceback: " << pyGetRepr( exceptionTraceBack );
	return msg.str( );
}

const char *python::Interpreter::pyGetRepr( PyObject *o )
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

const char *python::Interpreter::pyGetStr( PyObject *o )
{
	PyObject *str = PyObject_Str( o );
#if PY_MAJOR_VERSION >= 3 && PY_MINOR_VERSION >= 3
	const char *s = PyUnicode_AsUTF8( str );
#else
	PyObject *pstr = PyUnicode_AsUTF8String( str );
	const char *s = PyBytes_AsString( pstr );
#endif
	return s;
}
