#include "mylangInterpreterInstance.hpp"
#include <boost/algorithm/string.hpp>

using namespace _Wolframe;
using namespace _Wolframe::langbind;

static void mapStructure( mylang::Structure* dest, const mylang::Structure* src)
{
	if (src->atomic())
	{
		types::Variant val( src->getValue());
		if (val.type() == types::Variant::UInt)
		{
			dest->setValue( val.touint() + 1);
		}
		else if (val.type() == types::Variant::Int)
		{
			dest->setValue( val.toint() + 1);
		}
		else if (val.type() == types::Variant::Double)
		{
			dest->setValue( val.todouble() + 1);
		}
		else if (val.type() == types::Variant::Bool)
		{
			dest->setValue( !val.tobool());
		}
		else if (val.type() == types::Variant::String)
		{
			dest->setValue( boost::algorithm::to_upper_copy( val.tostring()));
		}
		else
		{
			throw std::runtime_error( std::string("cannot handle this type '") + val.typeName() + "'");
		}
	}
	else if (src->array())
	{
		mylang::Structure::const_iterator si = src->begin(), se = src->end();
		for (; si != se; ++si)
		{
			mylang::Structure* elem = dest->addArrayElement();
			mapStructure( elem, si->second);
		}
	}
	else
	{
		mylang::Structure::const_iterator si = src->begin(), se = src->end();
		for (; si != se; ++si)
		{
			mylang::Structure* elem = dest->addStructElement( si->first.tostring());
			mapStructure( elem, si->second);
		}
	}
}

mylang::StructureR mylang::InterpreterInstance::call( const proc::ProcessorProvider* provider, const mylang::StructureR& arg)
{
	mylang::StructureR rt( new mylang::Structure( this));
	mapStructure( rt.get(), arg.get());
	return rt;
}

