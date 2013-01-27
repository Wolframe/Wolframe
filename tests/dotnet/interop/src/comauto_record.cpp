#include "comauto_record.hpp"
#include "comauto_utils.hpp"

using namespace _Wolframe;

comauto::Record::Record( ITypeInfo* typeinfo_, PVOID data_)
	:m_typeinfo(typeinfo_),m_typeattr(0),m_data(data_),m_allocated(data_==0)
{
	WRAP( m_typeinfo->AddRef())
	WRAP( m_typeinfo->GetTypeAttr( &m_typeattr))
	if (!m_data)
	{
		m_data = CoTaskMemAlloc( m_typeattr->cbSizeInstance);
		if (!m_data)
		{
			if (m_typeattr) m_typeinfo->ReleaseTypeAttr( m_typeattr);
			m_typeinfo->Release();
		}
		std::memset( m_data, 0, m_typeattr->cbSizeInstance);
	}
}

comauto::Record::~Record()
{
	if (m_allocated) CoTaskMemFree( m_data);

	if (m_typeinfo)
	{
		if (m_typeattr) m_typeinfo->ReleaseTypeAttr( m_typeattr);
		m_typeinfo->Release();
	}
}

void comauto::Record::init()
{
	for (WORD iv = 0; iv < m_typeattr->cVars; ++iv) {

		VARDESC* vd;
		PVOID field;
		WRAP( m_typeinfo->GetVarDesc( iv, &vd))

		if ( (vd->elemdescVar.tdesc.vt & VT_BYREF) != 0) throw std::runtime_error( "cannot handle VT_BYREF in structure");

		field = (BYTE*)m_data + vd->oInst;
		
		if (vd->elemdescVar.tdesc.vt == VT_USERDEFINED)
		{
			ITypeInfo* rectypeinfo;
			WRAP( m_typeinfo->GetRefTypeInfo(vd->elemdescVar.tdesc.hreftype, &rectypeinfo))
			Record rec( rectypeinfo, field);
			rec.init();
			break;
		}
		else if (vd->elemdescVar.tdesc.vt == VT_BSTR)
		{
			*((BSTR*)field) = SysAllocString (L""); break;
		}
		else
		{
			// ... already zero initialized
		}
	}
}
