#include "mylangFunctionCall.hpp"

using namespace _Wolframe;
using namespace _Wolframe::langbind;

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

mylang::StructureR mylang::call( const proc::ProcessorProvider* provider, Instance* instance, const mylang::StructureR& arg)
{
	// IMPLEMENT CALL HERE
	return StructureR();
}

