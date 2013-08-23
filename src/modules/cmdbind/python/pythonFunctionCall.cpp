#include "pythonFunctionCall.hpp"

using namespace _Wolframe;
using namespace _Wolframe::langbind;

python::Structure* python::Structure::addSubstruct( const types::Variant& elemid_)
{
	return 0;
}

void python::Structure::setValue( const types::Variant& value)
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

python::StructureR python::call( const proc::ProcessorProvider* provider, Instance* instance, const python::StructureR& arg)
{
	// IMPLEMENT CALL HERE
	return StructureR();
}

