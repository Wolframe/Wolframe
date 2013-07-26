#include "serialize/tostringUtils.hpp"
#include "serialize/ddl/filtermapDDLSerialize.hpp"
#include "filter/tostringfilter.hpp"

using namespace _Wolframe;
using namespace _Wolframe::serialize;

std::string serialize::ddlStructureToString( const types::VariantStruct& st)
{
	langbind::ToStringFilter* res;
	langbind::TypedOutputFilterR out( res = new langbind::ToStringFilter( "\t"));
	serialize::DDLStructSerializer serializer( &st);
	serializer.init( out);
	if (!serializer.call()) throw std::runtime_error( "illegal state in DDL structure serializer");
	return res->content();
}

std::string serialize::typedInputFilterToString( const langbind::TypedInputFilterR& inp)
{
	langbind::ToStringFilter* res;
	langbind::TypedOutputFilterR out( res = new langbind::ToStringFilter( "\t"));
	langbind::TypedInputFilter::ElementType type;
	types::VariantConst element;
	int taglevel = 0;
	while (taglevel >= 0 && inp->getNext( type, element))
	{
		if (type == langbind::TypedInputFilter::OpenTag) ++taglevel;
		else if (type == langbind::TypedInputFilter::CloseTag) --taglevel;
		if (taglevel >= 0) out->print( type, element);
	}
	switch (inp->state())
	{
		case langbind::InputFilter::Open: break;
		case langbind::InputFilter::Error: throw std::runtime_error( inp->getError());
		case langbind::InputFilter::EndOfMessage: throw std::runtime_error( "input not complete and cannot yield execution here. tostring failed");
	}
	return res->content();
}

