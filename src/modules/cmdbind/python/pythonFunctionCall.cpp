#include "pythonFunctionCall.hpp"
#include "pythonInterpreter.hpp"
#include "logger-v1.hpp"

using namespace _Wolframe;
using namespace _Wolframe::langbind;

python::Context::Context( )
{
}

std::vector<std::string> python::Context::loadProgram( const std::string& prgfile )
{
	std::vector<std::string> rt;

	MOD_LOG_TRACE << "[python] Loading Python program " << prgfile;

	python::Interpreter interpreter( prgfile );
	std::vector<std::string>::const_iterator ni = interpreter.functions( ).begin( ), ne = interpreter.functions( ).end( );
	for( ; ni != ne; ++ni ) {
		rt.push_back( *ni );
	}
	
	return rt;
}

python::InstanceR python::Context::getInstance( const std::string& /*name*/) const
{
	return python::InstanceR( new python::Instance( ) );
}

python::Structure* python::Structure::addSubstruct( const types::Variant& /*elemid_*/)
{
	return 0;
}

void python::Structure::setValue( const types::Variant& /*value*/)
{
}

const types::Variant& python::Structure::getValue() const
{
	static types::Variant null;
	return null;
}

bool python::Structure::atomic() const
{
	return true;
}

bool python::Structure::array() const
{
	return false;
}

python::Structure::const_iterator python::Structure::begin() const
{
	python::Structure::const_iterator it;
	return it;
}

python::Structure::const_iterator python::Structure::end() const
{
	python::Structure::const_iterator it;
	return it;
}

python::StructureR python::call( const proc::ProcessorProvider* /*provider*/, Instance* /*instance*/, const python::StructureR& /*arg*/)
{
	// IMPLEMENT CALL HERE
	return StructureR();
}

