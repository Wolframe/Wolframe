#include "pythonFunctionCall.hpp"

using namespace _Wolframe;
using namespace _Wolframe::langbind;

python::Context::Context( )
{
}

std::vector<std::string> python::Context::loadProgram( const std::string& /*prgfile*/)
{
	std::vector<std::string> rt;
	// TODO: load all public functions from python module
	// problem, what to do with methods of objects? where to
	// create the objects?
	//~ langbind::LuaScript script( prgfile);
	//~ std::vector<std::string>::const_iterator ni = script.functions().begin(), ne = script.functions().end();
	//~ for (; ni != ne; ++ni)
	//~ {
		//~ rt.push_back( *ni);
		//~ funcmap.defineLuaFunction( *ni, script);
	//~ }
	return rt;
}

python::InstanceR python::Context::getInstance( const std::string& name) const
{
	return python::InstanceR( 0 );
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

