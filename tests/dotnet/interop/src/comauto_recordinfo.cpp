#include "comauto_recordinfo.hpp"
#include "comauto_record.hpp"
#include "comauto_type.hpp"
#include "comauto_utils.hpp"
#include <cstring>

#define RETURN_ON_ERROR(RV,CALL) {if ((RV = (CALL)) != S_OK) return RV;}

using namespace _Wolframe;

comauto::RecordInfo::~RecordInfo()
{
	if (m_typeattr) m_typeinfo->ReleaseTypeAttr( m_typeattr);
	m_typeinfo->Release();
}

comauto::RecordInfo::RecordInfo( ITypeInfo* typeinfo_)
	:m_typeinfo(typeinfo_),m_typeattr(0)
{
	WRAP( m_typeinfo->GetTypeAttr( &m_typeattr))
	if (m_typeattr->typekind != TKIND_RECORD)
	{
		m_typeinfo->ReleaseTypeAttr( m_typeattr);
	}
	WRAP( m_typeinfo->AddRef())
}

HRESULT comauto::RecordInfo::QueryInterface( REFIID  riid, LPVOID* ppvObj)
{
    if (!ppvObj) return E_INVALIDARG;
    *ppvObj = NULL;

	if (riid == IID_IUnknown || riid == IID_IRecordInfo)
    {
        *ppvObj = (LPVOID)this;
        AddRef();
        return S_OK;
    }
    return E_NOINTERFACE;
}

static char sizeofAtomicType( int tp)
{
	struct AtomicTypes
	{
		char ar[80];
		AtomicTypes()
		{
			std::memset( ar, 0, sizeof(ar));
			ar[ VT_I2] = 2;
			ar[ VT_I4] = 4;
			ar[ VT_R4] = 4;
			ar[ VT_R8] = 8;
			ar[ VT_CY] = sizeof(CY);
			ar[ VT_DATE] = sizeof(DATE);
			ar[ VT_BOOL] = sizeof(BOOL);
			ar[ VT_DECIMAL] = sizeof(DECIMAL);
			ar[ VT_I1] = 1;
			ar[ VT_UI1] = 1;
			ar[ VT_UI2] = 2;
			ar[ VT_UI4] = 4;
			ar[ VT_I8] = 8;
			ar[ VT_UI8] = 8;
			ar[ VT_INT] = sizeof(int);
			ar[ VT_UINT] = sizeof(unsigned int);
			ar[ VT_HRESULT] = sizeof(HRESULT);
		}

		char operator[]( int i) const
		{
			if (i >= 0 && i <= sizeof(ar)) return (ar[i] != 0);
			return 0;
		}
	};
	static AtomicTypes at;
	return at[tp];
}

HRESULT comauto::RecordInfo::RecordFill( PVOID pvNew, comauto::RecordInfo::InitType initType, PVOID pvOld)
{
	HRESULT hr;
	ITypeInfo* rectypeinfo = 0;
	if (initType == DefaultConstructorZero)
	{
		std::memset( pvNew, 0, m_typeattr->cbSizeInstance);
		initType = DefaultConstructor;
	}
	try
	{
		for (WORD iv = 0; iv < m_typeattr->cVars; ++iv)
		{
			char elemsize;
			VARDESC* vd;
			PVOID nfield;
			PVOID ofield;
			WRAP( m_typeinfo->GetVarDesc( iv, &vd))

			nfield = (BYTE*)pvNew + vd->oInst;
			ofield = (BYTE*)pvOld + vd->oInst;
		
			if (vd->elemdescVar.tdesc.vt == VT_USERDEFINED)
			{
				RETURN_ON_ERROR( hr, m_typeinfo->GetRefTypeInfo( vd->elemdescVar.tdesc.hreftype, &rectypeinfo))
				comauto::RecordInfo rec( rectypeinfo);
				WRAP( rec.RecordFill( nfield, initType, pvOld))
				rectypeinfo->Release();
				rectypeinfo = 0;
			}
			else if (vd->elemdescVar.tdesc.vt == VT_BSTR)
			{
				switch (initType)
				{
					case ClearInit:
						if (*((BSTR*)nfield) != NULL) ::SysFreeString( *((BSTR*)nfield));
						*((BSTR*)nfield) = ::SysAllocString( L"");
						break;

					case CopyInit:
						if (*((BSTR*)nfield) != NULL) ::SysFreeString( *((BSTR*)nfield));
						*((BSTR*)nfield) = ::SysAllocString( *((BSTR*)ofield));
						break;

					case DefaultConstructor:
						*((BSTR*)nfield) = ::SysAllocString( L"");
						break;

					case DefaultConstructorZero:
						throw std::logic_error( "should not pass here !");

					case CopyConstructor:
						*((BSTR*)nfield) = ::SysAllocString( *((BSTR*)ofield));
						break;

					case Destructor:
						if (*((BSTR*)nfield) != NULL) ::SysFreeString( *((BSTR*)nfield));
						break;
				}
			}
			else if ((elemsize=sizeofAtomicType( vd->elemdescVar.tdesc.vt)) > 0)
			{
				switch (initType)
				{
					case CopyInit:
					case CopyConstructor:
						//copy atomic type
						std::memcpy( nfield, ofield, elemsize);
						break;
					case ClearInit:
					case DefaultConstructor:
					case DefaultConstructorZero:
						// .... others are alread set to zero
						break;
					case Destructor:
						// .... no destructor
						break;
				}
			}
			else
			{
				throw std::runtime_error( "illegal type in POD IRecordInfo structure");
			}
		}
		return S_OK;
	}
	catch (...)
	{
		if (rectypeinfo)
		{
				rectypeinfo->Release();
				rectypeinfo = 0;
		}
		return E_INVALIDARG;
	}
}

HRESULT comauto::RecordInfo::RecordInit( PVOID pvNew)
{
	return comauto::RecordInfo::RecordFill( pvNew, DefaultConstructorZero);
}

HRESULT comauto::RecordInfo::RecordClear( PVOID pvExisting)
{
	return comauto::RecordInfo::RecordFill( pvExisting, ClearInit);
}

HRESULT comauto::RecordInfo::RecordCopy( PVOID pvExisting, PVOID pvNew)
{
	return comauto::RecordInfo::RecordFill( pvExisting, CopyInit, pvNew);
}

HRESULT comauto::RecordInfo::GetGuid( GUID* pguid)
{
	if (!m_typeattr) return E_INVALIDARG;
	*pguid = m_typeattr->guid;
	return S_OK;
}

HRESULT comauto::RecordInfo::GetName( BSTR* pbstrName)
{
	HRESULT hr;
	if (!pbstrName) return E_INVALIDARG;
	RETURN_ON_ERROR( hr, m_typeinfo->GetDocumentation(-1, pbstrName, NULL, NULL, NULL));
	return S_OK;
}

HRESULT comauto::RecordInfo::GetSize( ULONG* pcbSize)
{
	if (!pcbSize) return E_INVALIDARG;
	*pcbSize = m_typeattr->cbSizeInstance;
	return S_OK;
}

HRESULT comauto::RecordInfo::GetTypeInfo( ITypeInfo** ppTypeInfo)
{
	HRESULT hr;
	RETURN_ON_ERROR( hr, m_typeinfo->AddRef());
	*ppTypeInfo = m_typeinfo;
	return S_OK;
}

HRESULT comauto::RecordInfo::GetField( PVOID pvData, LPCOLESTR szFieldName, VARIANT* pvarField)
{
	HRESULT hr = S_OK;

	VARIANT refvarField;
	::VariantInit( &refvarField);

	PVOID field = NULL;
	RETURN_ON_ERROR( hr, GetFieldNoCopy( pvData, szFieldName, &refvarField, &field));

	//copy element value referenced (resolve VT_BYREF | VT_XXX -> VT_XXX):
	::VariantClear( pvarField);
	hr = ::VariantCopyInd( pvarField, &refvarField);
	return hr;
}

HRESULT comauto::RecordInfo::GetFieldNoCopy( PVOID pvData, LPCOLESTR szFieldName, VARIANT* pvarField, PVOID* ppvDataCArray)
{
	HRESULT hr = S_OK;
	CComPtr<ITypeComp> typecomp;
	CComPtr<ITypeInfo> elemtypeinfo;
	DESCKIND desckind;
	BINDPTR bindptr;

	//Get type info and variable descriptor of addressed member (szFieldName):
	RETURN_ON_ERROR( hr, m_typeinfo->GetTypeComp( &typecomp));
	RETURN_ON_ERROR( hr, typecomp->Bind( const_cast<LPOLESTR>(szFieldName), 0, INVOKE_PROPERTYGET, &elemtypeinfo, &desckind, &bindptr));

	if (desckind != DESCKIND_VARDESC) return E_INVALIDARG;	//... only variables expected in a POD structure

	PVOID field = ((BYTE*)pvData) + bindptr.lpvardesc->oInst;  //... 'field' points to the member variable bound to 'szFieldName'
	*ppvDataCArray = field;

	if ((bindptr.lpvardesc->elemdescVar.tdesc.vt & VT_BYREF) != 0) return E_INVALIDARG;  //... only value members expected in a POD structure

	if (bindptr.lpvardesc->elemdescVar.tdesc.vt == VT_USERDEFINED)
	{
		//... structure
		CComPtr<ITypeInfo> refelemtypeinfo;
		//resolve indirection VT_USERDEFINED -> VT_RECORD:
		RETURN_ON_ERROR( hr, m_typeinfo->GetRefTypeInfo( bindptr.lpvardesc->elemdescVar.tdesc.hreftype, &refelemtypeinfo));

		CComPtr<IRecordInfo> elemrecinfo( new comauto::RecordInfo( refelemtypeinfo));

		//inititialize structure value reference to return:
		RETURN_ON_ERROR( hr, ::VariantClear( pvarField));
		RETURN_ON_ERROR( hr, elemrecinfo.CopyTo( &pvarField->pRecInfo));
		pvarField->pvRecord = field;
		pvarField->vt = VT_RECORD;
	}
	else
	{
		//... inititialize atomic value reference to return:
		RETURN_ON_ERROR( hr, ::VariantClear( pvarField));
		pvarField->byref = field;
		pvarField->vt = bindptr.lpvardesc->elemdescVar.tdesc.vt | VT_BYREF;
	}
	return S_OK;
}

HRESULT comauto::RecordInfo::PutField( ULONG wFlags, PVOID pvData, LPCOLESTR szFieldName, VARIANT* pvarField)
{
	HRESULT hr = S_OK;

	//copy parameter to put:
	VARIANT varCopy;
	::VariantInit( &varCopy);
	RETURN_ON_ERROR( hr, ::VariantCopy( &varCopy, pvarField));

	return PutFieldNoCopy( wFlags, pvData, szFieldName, &varCopy);
}

HRESULT comauto::RecordInfo::PutFieldNoCopy( ULONG wFlags, PVOID pvData, LPCOLESTR szFieldName, VARIANT* pvarField)
{
	HRESULT hr = S_OK;
	CComPtr<ITypeComp> typecomp;
	CComPtr<ITypeInfo> elemtypeinfo;
	DESCKIND desckind;
	BINDPTR bindptr;

	//Get type info and variable descriptor of addressed member:
	RETURN_ON_ERROR( hr, m_typeinfo->GetTypeComp( &typecomp));
	RETURN_ON_ERROR( hr, typecomp->Bind( const_cast<LPOLESTR>(szFieldName), 0, INVOKE_PROPERTYPUT | INVOKE_PROPERTYPUTREF, &elemtypeinfo, &desckind, &bindptr));

	if (desckind != DESCKIND_VARDESC) return E_INVALIDARG;	//... only variables expected in a POD structure

	PVOID field = ((BYTE*)pvData) + bindptr.lpvardesc->oInst;  //... 'field' points to the member variable bound to 'szFieldName'

	if (pvarField->vt != bindptr.lpvardesc->elemdescVar.tdesc.vt)
	{
		//... element to set if of different type -> convert it first to the type of the member to set:
		RETURN_ON_ERROR( hr, ::VariantChangeType( pvarField, pvarField, NULL, bindptr.lpvardesc->elemdescVar.tdesc.vt));
	}

	char elemsize = sizeofAtomicType( bindptr.lpvardesc->elemdescVar.tdesc.vt);
	if (elemsize)
	{
		std::memcpy( field, &pvarField->llVal, elemsize);  //... atomic type copy by value
	}
	else if ((bindptr.lpvardesc->elemdescVar.tdesc.vt & VT_ARRAY) != 0)
	{
		*((SAFEARRAY**)field) = pvarField->parray;	//... array of some type copy reference
	}
	else
	{
		switch (bindptr.lpvardesc->elemdescVar.tdesc.vt)
		{
			case VT_BSTR:
				*((BSTR*)field) = pvarField->bstrVal;
				break;

			case VT_USERDEFINED:
				*((PVOID*)field) = pvarField->pvRecord;
				break;

			default:
				return E_INVALIDARG;
		}
	}
	return S_OK;
}

HRESULT comauto::RecordInfo::GetFieldNames( ULONG* pcNames, BSTR* rgBstrNames)
{
	HRESULT hr = S_OK;

	*pcNames = m_typeattr->cVars;

	if (rgBstrNames != NULL) 
	{
		unsigned short ii;
		for (ii = 0; ii < m_typeattr->cVars; ++ii)
		{
			rgBstrNames[ ii] = NULL;
		}
		VARDESC* var = NULL;
		try
		{
			for (ii = 0; ii < m_typeattr->cVars; ++ii) 
			{
				WRAP( (hr=m_typeinfo->GetVarDesc( ii, &var)))
				UINT nn;
				WRAP( (hr=m_typeinfo->GetNames( var->memid, rgBstrNames+ii, 1, &nn)))
				m_typeinfo->ReleaseVarDesc( var);
				var = NULL;
			}
		}
		catch (...)
		{
			for (ii = 0; ii < m_typeattr->cVars; ++ii)
			{
				if (rgBstrNames[ii]) ::SysFreeString( rgBstrNames[ii]);
			}
			if (var) m_typeinfo->ReleaseVarDesc( var);
			return hr;
		}
	}
	return S_OK;
}

BOOL comauto::RecordInfo::IsMatchingType( IRecordInfo *pRecordInfo)
{
	BOOL rt = TRUE;
	ITypeInfo* otypeinfo = 0;
	ITypeInfo* rectypeinfo = 0;
	ITypeInfo* orectypeinfo = 0;
	TYPEATTR* otypeattr = 0;
	VARDESC* vd = 0;
	VARDESC* ovd = 0;
	BSTR varname = NULL;
	BSTR ovarname = NULL;
	try
	{
		WRAP( pRecordInfo->GetTypeInfo( &otypeinfo))
		WRAP( otypeinfo->GetTypeAttr( &otypeattr))
		if (m_typeattr->guid == otypeattr->guid) goto Cleanup;
		if (m_typeattr->cVars != otypeattr->cVars) {rt=FALSE; goto Cleanup;}
		unsigned short ii;
		for (ii = 0; ii < m_typeattr->cVars; ++ii)
		{
			if (vd) m_typeinfo->ReleaseVarDesc( vd);
			WRAP( m_typeinfo->GetVarDesc( ii, &vd))
			if (ovd) otypeinfo->ReleaseVarDesc( ovd);
			WRAP( otypeinfo->GetVarDesc( ii, &ovd))
			if (vd->elemdescVar.tdesc.vt != ovd->elemdescVar.tdesc.vt || vd->oInst != ovd->oInst) {rt=FALSE; goto Cleanup;}
			UINT nn;
			if (varname) ::SysFreeString( varname);
			WRAP( m_typeinfo->GetNames( vd->memid, &varname, 1, &nn))
			if (ovarname) ::SysFreeString( ovarname);
			WRAP( otypeinfo->GetNames( ovd->memid, &ovarname, 1, &nn))
			if (wcscmp( varname, ovarname) != 0) {rt=FALSE; goto Cleanup;}

			if (vd->elemdescVar.tdesc.vt == VT_USERDEFINED)
			{
				if (rectypeinfo) rectypeinfo->Release();
				WRAP( m_typeinfo->GetRefTypeInfo( vd->elemdescVar.tdesc.hreftype, &rectypeinfo))
				if (orectypeinfo) orectypeinfo->Release();
				WRAP( m_typeinfo->GetRefTypeInfo( ovd->elemdescVar.tdesc.hreftype, &orectypeinfo))

				comauto::RecordInfo rec( rectypeinfo);
				comauto::RecordInfo orec( orectypeinfo);
				if (rec.IsMatchingType( &orec) == FALSE) {rt=FALSE; goto Cleanup;}
			}
		}
	}
	catch (...)
	{
		rt = FALSE;
	}
Cleanup:
	if (vd) m_typeinfo->ReleaseVarDesc( vd);
	if (ovd) otypeinfo->ReleaseVarDesc( ovd);
	if (otypeattr) otypeinfo->ReleaseTypeAttr( otypeattr);
	if (rectypeinfo) rectypeinfo->Release();
	if (orectypeinfo) orectypeinfo->Release();
	if (otypeinfo) otypeinfo->Release();
	if (varname) ::SysFreeString( varname);
	if (ovarname) ::SysFreeString( ovarname);
	return rt;
}

PVOID comauto::RecordInfo::RecordCreate()
{
	PVOID rt = ::CoTaskMemAlloc( m_typeattr->cbSizeInstance);
	if (rt == NULL) return NULL;

	HRESULT hr = RecordInit( rt);
	if (hr != S_OK)
	{
		::CoTaskMemFree( rt);
		rt = NULL;
	}
	return rt;
}

HRESULT comauto::RecordInfo::RecordCreateCopy( PVOID pvSource, PVOID* ppvDest)
{
	if (!ppvDest) return E_INVALIDARG;
	*ppvDest = ::CoTaskMemAlloc( m_typeattr->cbSizeInstance);
	if (!*ppvDest) return E_OUTOFMEMORY;

	HRESULT hr = comauto::RecordInfo::RecordFill( pvSource, CopyConstructor, *ppvDest);
	if (hr != S_OK)
	{
		RecordDestroy( *ppvDest);
		*ppvDest = NULL;
	}
	return hr;
}

HRESULT comauto::RecordInfo::RecordDestroy( PVOID pvRecord)
{
	comauto::RecordInfo::RecordFill( pvRecord, Destructor, 0);
	::CoTaskMemFree( pvRecord);
	return S_OK;
}



