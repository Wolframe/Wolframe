#include "mylangInterpreterInstance.hpp"
#include "mylangStructureBuilder.hpp"
#include <boost/algorithm/string.hpp>

using namespace _Wolframe;
using namespace _Wolframe::langbind;

static void mapStructure( mylang::StructureBuilder& dest, const mylang::Structure* src)
{
	if (src->atomic())
	{
		types::Variant val( src->getValue());
		if (val.type() == types::Variant::UInt)
		{
			dest.setValue( val.touint() + 1);
		}
		else if (val.type() == types::Variant::Int)
		{
			dest.setValue( val.toint() + 1);
		}
		else if (val.type() == types::Variant::Double)
		{
			dest.setValue( val.todouble() + 1);
		}
		else if (val.type() == types::Variant::Bool)
		{
			dest.setValue( !val.tobool());
		}
		else if (val.type() == types::Variant::String)
		{
			dest.setValue( boost::algorithm::to_upper_copy( val.tostring()));
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
			dest.openArrayElement();
			mapStructure( dest, si->val);
			dest.closeElement();
		}
	}
	else
	{
		mylang::Structure::const_iterator si = src->begin(), se = src->end();
		for (; si != se; ++si)
		{
			dest.openElement( si->key.tostring());
			mapStructure( dest, si->val);
			dest.closeElement();
		}
	}
}

mylang::StructureR mylang::InterpreterInstance::call( const proc::ProcessorProvider*, const mylang::StructureR& arg)
{
	mylang::StructureBuilder rt;
	mapStructure( rt, arg.get());
	return rt.get();
}

