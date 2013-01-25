#ifndef _Wolframe_COM_AUTOMATION_TYPE_HPP_INCLUDED
#define _Wolframe_COM_AUTOMATION_TYPE_HPP_INCLUDED
#include "comauto_utils.hpp"
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

class Type
{
public:
	Type( ITypeInfo* typeinfo_);
	~Type();

private:
	ITypeInfo* m_typeinfo;
	TYPEATTR* m_typeattr;
};

typedef boost::shared_ptr<Type> TypeR;

} //namespace
#endif

