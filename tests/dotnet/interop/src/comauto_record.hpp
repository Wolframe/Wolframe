#ifndef _Wolframe_COM_AUTOMATION_RECORD_HPP_INCLUDED
#define _Wolframe_COM_AUTOMATION_RECORD_HPP_INCLUDED
#include "comauto_utils.hpp"
#include <cstring>
#include <oaidl.h>
#include <comdef.h>
#include <atlbase.h>
#include <atlcom.h>

namespace comauto {

class Record
{
public:
	explicit Record( ITypeInfo* typeinfo_, PVOID data_=0);
	virtual ~Record();

private:
	void init();

private:
	ITypeInfo* m_typeinfo;
	TYPEATTR* m_typeattr;
	PVOID m_data;
	bool m_allocated;
};

} //namespace
#endif
