#ifndef _Wolframe_COM_AUTOMATION_TYPELIB_HPP_INCLUDED
#define _Wolframe_COM_AUTOMATION_TYPELIB_HPP_INCLUDED
#include "comauto_utils.hpp"
#include "comauto_function.hpp"
#include "comauto_type.hpp"
#include <oaidl.h>
#include <comdef.h>
#include <atlbase.h>
#include <atlcom.h>
#include <cstring>
#include <string>
#include <vector>
#include <map>
#include <boost/shared_ptr.hpp>

namespace comauto {

class TypeLib
{
public:
	explicit TypeLib( const std::string& file);
	virtual ~TypeLib();
	void print( std::ostream& out) const;

private:
	void defineFunction( ITypeInfo* typeinfo, const std::string& classname, unsigned short fidx);

	ITypeLib* m_typelib;
	std::map<std::string,FunctionR> m_funcmap;
	std::map<std::string,TypeR> m_typemap;
};

} //namespace
#endif

