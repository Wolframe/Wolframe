#include "mylangFunctionCall.hpp"

using namespace _Wolframe;
using namespace _Wolframe::langbind;

mylang::Context::Context( )
{
}

std::vector<std::string> mylang::Context::loadProgram( const std::string& name)
{
	std::vector<std::string> rt;
	// TODO: myLang has sort of entry points which are added here to the list
	// of entry points (functions)?
	return rt;
}

mylang::InstanceR mylang::Context::getInstance( const std::string& name) const
{
	return mylang::InstanceR( 0 );
}

mylang::Structure* mylang::Structure::addSubstruct( const types::Variant& elemid_)
{
	return 0;
}

void mylang::Structure::setValue( const types::Variant& value)
{
}

const types::Variant& mylang::Structure::getValue() const
{
	static types::Variant null;
	return null;
}

bool mylang::Structure::atomic() const
{
	return true;
}

bool mylang::Structure::array() const
{
	return false;
}

mylang::Structure::const_iterator mylang::Structure::begin() const
{
	mylang::Structure::const_iterator it;
	return it;
}

mylang::Structure::const_iterator mylang::Structure::end() const
{
	mylang::Structure::const_iterator it;
	return it;
}

mylang::StructureR mylang::call( const proc::ProcessorProvider* provider, Instance* instance, const mylang::StructureR& arg)
{
	// IMPLEMENT CALL HERE
	return StructureR();
}

