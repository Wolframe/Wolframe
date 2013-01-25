#include "comauto_type.hpp"

comauto::Type::Type( ITypeInfo* typeinfo_)
	:m_typeinfo(typeinfo_),m_typeattr(0)
{
	WRAP( m_typeinfo->GetTypeAttr( &m_typeattr))
}

comauto::Type::~Type()
{
	if (m_typeinfo) 
	{
		m_typeinfo->ReleaseTypeAttr( m_typeattr);
		m_typeinfo->Release();
	}
}



