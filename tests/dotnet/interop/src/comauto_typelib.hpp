#ifndef _Wolframe_COM_AUTOMATION_TYPELIB_HPP_INCLUDED
#define _Wolframe_COM_AUTOMATION_TYPELIB_HPP_INCLUDED
#include "comauto_utils.hpp"
#include <oaidl.h>
#include <comdef.h>
#include <atlbase.h>
#include <atlcom.h>
#include <cstring>
#include <string>

namespace comauto {

class TypeLib
{
public:
	explicit TypeLib( const std::string& file);
	virtual ~TypeLib();
	void print( std::ostream& out) const;

private:
	ITypeLib* m_typelib;
};

} //namespace
#endif

