#include "mylangInterpreterInstance.hpp"
#include "mylangStructureBuilder.hpp"
#include <boost/algorithm/string.hpp>

using namespace _Wolframe;
using namespace _Wolframe::langbind;

static void mapStructure( mylang::StructureBuilder& dest, const mylang::Structure* src)
{
	if (src->atomic())
	{
		types::Variant element( src->getValue());
		switch (element.type())
		{
			case types::Variant::Custom:
				dest.setValue( boost::algorithm::to_upper_copy( element.tostring()));
				break;
			case types::Variant::Null:
				dest.setValue( types::Variant());
				break;
			case types::Variant::Bool:
				dest.setValue( !element.tobool());
				break;
			case types::Variant::Double:
				dest.setValue( element.todouble() + 1);
				break;
			case types::Variant::Int:
				dest.setValue( element.toint() + 1);
				break;
			case types::Variant::UInt:
				dest.setValue( element.touint() + 1);
				break;
			case types::Variant::String:
				dest.setValue( boost::algorithm::to_upper_copy( element.tostring()));
				break;
			case types::Variant::Timestamp:
				dest.setValue( boost::algorithm::to_upper_copy( element.tostring()));
				break;
			case types::Variant::BigNumber:
				dest.setValue( boost::algorithm::to_upper_copy( element.tostring()));
				break;
			default:
				throw std::runtime_error( std::string("cannot handle this type '") + element.typeName() + "'");
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

mylang::StructureR mylang::InterpreterInstance::call( const proc::ProcessorProviderInterface*, const mylang::StructureR& arg)
{
	mylang::StructureBuilder rt;
	mapStructure( rt, arg.get());
	return rt.get();
}

