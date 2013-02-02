#include "comauto_recordinfo.hpp"
#include "comauto_utils.hpp"
#include <cstring>
#include <limits>
#pragma warning(disable:4996)

#define RETURN_ON_ERROR(RV,CALL) {if ((RV = (CALL)) != S_OK) return RV;}

using namespace _Wolframe;

comauto::RecordInfo::~RecordInfo()
{
/*[-]*/std::cerr << "CALL comauto::RecordInfo::~RecordInfo()" << std::endl;

	if (m_typeattr) m_typeinfo->ReleaseTypeAttr( m_typeattr);
	m_typeinfo->Release();
}

comauto::RecordInfo::RecordInfo( ITypeInfo* typeinfo_)
	:m_typeinfo(typeinfo_),m_typeattr(0)
{
/*[-]*/std::cerr << "CALL comauto::RecordInfo::RecordInfo( " << std::hex << "0x0" << (uintptr_t)typeinfo_ << ")" << std::dec << std::endl;
	WRAP( m_typeinfo->GetTypeAttr( &m_typeattr))
	if (m_typeattr->typekind != TKIND_RECORD)
	{
		m_typeinfo->ReleaseTypeAttr( m_typeattr);
	}
	m_typeinfo->AddRef();
	initDescr();
}

HRESULT comauto::RecordInfo::QueryInterface( REFIID  riid, LPVOID* ppvObj)
{
/*[-]*/std::cerr << "CALL comauto::RecordInfo::QueryInterface( " << std::hex << "0x0" << (uintptr_t)ppvObj << ")" << std::dec << std::endl;
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

HRESULT comauto::RecordInfo::RecordFill( PVOID pvNew, comauto::RecordInfo::InitType initType, bool doThrow, PVOID pvOld)
{
/*[-]*/std::cerr << "comauto::RecordInfo::RecordFill(" << initTypeName(initType) << ": " << std::hex << (uintptr_t)pvNew << ", " << (uintptr_t)pvOld << ")" << std::dec << std::endl;
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
				WRAP( rec.RecordFill( nfield, initType, doThrow, pvOld))
				rectypeinfo->Release();
				rectypeinfo = 0;
			}
			else if (vd->elemdescVar.tdesc.vt == VT_BSTR)
			{
				switch (initType)
				{
					case ClearInit:
						if (*((BSTR*)nfield) != NULL) ::SysFreeString( *((BSTR*)nfield));
						/*no break here!*/
					case DefaultConstructor:
						*((BSTR*)nfield) = ::SysAllocString( L"");
						break;

					case CopyInit:
						if (*((BSTR*)nfield) != NULL) ::SysFreeString( *((BSTR*)nfield));
						/*no break here!*/
					case CopyConstructor:
						if (*((BSTR*)ofield))
						{
							*((BSTR*)nfield) = ::SysAllocString( *((BSTR*)ofield));
						}
						else
						{
							*((BSTR*)nfield) = ::SysAllocString( L"");
						}
						break;

					case DefaultConstructorZero:
						throw std::logic_error( "should not pass here !");

					case Destructor:
						if (*((BSTR*)nfield) != NULL)
						{
							::SysFreeString( *((BSTR*)nfield));
							*(BSTR*)nfield = NULL;
						}
						break;
				}
			}
			else if (vd->elemdescVar.tdesc.vt == VT_LPWSTR)
			{
				switch (initType)
				{
					case ClearInit:
						if (*((LPWSTR*)nfield) != NULL) comauto::freeMem( *((LPWSTR*)nfield));
					case DefaultConstructor:
						*((LPWSTR*)nfield) = NULL;
						break;

					case CopyInit:
						if (*((LPWSTR*)nfield) != NULL) comauto::freeMem( *((LPWSTR*)nfield));
					case CopyConstructor:
						*((LPWSTR*)nfield) = comauto::createLPWSTR( *(LPWSTR*)ofield);
						break;

					case DefaultConstructorZero:
						throw std::logic_error( "should not pass here !");

					case Destructor:
						if (*((LPWSTR*)nfield) != NULL)
						{
							comauto::freeMem( *((LPWSTR*)nfield));
							*((LPWSTR*)nfield) = NULL;
						}
						break;
				}
			}
			else if (vd->elemdescVar.tdesc.vt == VT_LPSTR)
			{
/*[-]*/char* cstr = pvOld?*((LPSTR*)ofield):"(NULL)";
/*[-]*/std::cout << "DEBUG init (RecordFill) " << comauto::typestr(vd->elemdescVar.tdesc.vt) <<  " at 0x0" << std::hex << (uintptr_t)nfield << std::dec << " with '" << (cstr?cstr:"(-> NULL)") << "'" << std::endl;
				switch (initType)
				{
					case ClearInit:
						if (*((LPSTR*)nfield) != NULL) comauto::freeMem( *((LPSTR*)nfield));
					case DefaultConstructor:
						*((LPSTR*)nfield) = NULL;
						break;

					case CopyInit:
						if (*((LPSTR*)nfield) != NULL) comauto::freeMem( *((LPSTR*)nfield));
					case CopyConstructor:
						*((LPSTR*)nfield) = comauto::createLPSTR( *(LPSTR*)ofield);
						break;

					case DefaultConstructorZero:
						throw std::logic_error( "should not pass here !");

					case Destructor:
						if (*((LPSTR*)nfield) != NULL)
						{
							comauto::freeMem( *((LPSTR*)nfield));
							*((LPSTR*)nfield) = NULL;
						}
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
	catch (const std::runtime_error& e)
	{
		if (rectypeinfo)
		{
				rectypeinfo->Release();
				rectypeinfo = 0;
		}
		if (doThrow) throw e;
		return E_INVALIDARG;
	}
	catch (const std::logic_error& e)
	{
		if (rectypeinfo)
		{
				rectypeinfo->Release();
				rectypeinfo = 0;
		}
		if (doThrow) throw e;
		return E_INVALIDARG;
	}
}

HRESULT comauto::RecordInfo::RecordInit( PVOID pvNew)
{
/*[-]*/std::cerr << "comauto::RecordInfo::RecordInit(..)" << std::endl;
	return comauto::RecordInfo::RecordFill( pvNew, DefaultConstructorZero, false);
}

HRESULT comauto::RecordInfo::RecordClear( PVOID pvExisting)
{
/*[-]*/std::cerr << "comauto::RecordInfo::RecordClear(..)" << std::endl;
	return comauto::RecordInfo::RecordFill( pvExisting, ClearInit, false);
}

HRESULT comauto::RecordInfo::RecordCopy( PVOID pvExisting, PVOID pvNew)
{
/*[-]*/std::cerr << "comauto::RecordInfo::RecordCopy(..)" << std::endl;
	return comauto::RecordInfo::RecordFill( pvNew, CopyInit, false, pvExisting);
}

HRESULT comauto::RecordInfo::GetGuid( GUID* pguid)
{
/*[-]*/std::cerr << "comauto::RecordInfo::GetGuid(..)" << std::endl;
	if (!m_typeattr) return E_INVALIDARG;
	*pguid = m_typeattr->guid;
	return S_OK;
}

HRESULT comauto::RecordInfo::GetName( BSTR* pbstrName)
{
/*[-]*/std::cerr << "comauto::RecordInfo::GetName(..)" << std::endl;
	HRESULT hr;
	if (!pbstrName) return E_INVALIDARG;
	RETURN_ON_ERROR( hr, m_typeinfo->GetDocumentation(-1, pbstrName, NULL, NULL, NULL));
	return S_OK;
}

HRESULT comauto::RecordInfo::GetSize( ULONG* pcbSize)
{
/*[-]*/std::cerr << "comauto::RecordInfo::GetSize(..)" << std::endl;
	if (!pcbSize) return E_INVALIDARG;
	*pcbSize = m_typeattr->cbSizeInstance;
	return S_OK;
}

HRESULT comauto::RecordInfo::GetTypeInfo( ITypeInfo** ppTypeInfo)
{
/*[-]*/std::cerr << "comauto::RecordInfo::GetTypeInfo(..)" << std::endl;
	m_typeinfo->AddRef();
	*ppTypeInfo = m_typeinfo;
	return S_OK;
}

HRESULT comauto::RecordInfo::GetField( PVOID pvData, LPCOLESTR szFieldName, VARIANT* pvarField)
{
/*[-]*/std::cerr << "comauto::RecordInfo::GetField(..)" << std::endl;
	HRESULT hr = S_OK;

	VARIANT refvarField;
	::VariantInit( &refvarField);

	PVOID field = NULL;
	RETURN_ON_ERROR( hr, GetFieldNoCopy( pvData, szFieldName, &refvarField, &field));

	//copy element value referenced (resolve VT_BYREF | VT_XXX -> VT_XXX):
	comauto::wrapVariantClear( pvarField);
	hr = comauto::wrapVariantCopyInd( pvarField, &refvarField);
	return hr;
}

HRESULT comauto::RecordInfo::GetFieldNoCopy( PVOID pvData, LPCOLESTR szFieldName, VARIANT* pvarField, PVOID* ppvDataCArray)
{
/*[-]*/std::cerr << "comauto::RecordInfo::GetFieldNoCopy(..)" << std::endl;
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
		RETURN_ON_ERROR( hr, comauto::wrapVariantClear( pvarField));
		RETURN_ON_ERROR( hr, elemrecinfo.CopyTo( &pvarField->pRecInfo));
		pvarField->pvRecord = field;
		pvarField->vt = VT_RECORD;
	}
	else
	{
		//... inititialize atomic value reference to return:
		RETURN_ON_ERROR( hr, comauto::wrapVariantClear( pvarField));
		pvarField->byref = field;
		pvarField->vt = bindptr.lpvardesc->elemdescVar.tdesc.vt | VT_BYREF;
	}
	return S_OK;
}

HRESULT comauto::RecordInfo::PutField( ULONG wFlags, PVOID pvData, LPCOLESTR szFieldName, VARIANT* pvarField)
{
/*[-]*/std::cerr << "comauto::RecordInfo::PutField(..)" << std::endl;
	HRESULT hr = S_OK;

	//copy parameter to put:
	VARIANT varCopy;
	::VariantInit( &varCopy);
	RETURN_ON_ERROR( hr, comauto::wrapVariantCopy( &varCopy, pvarField));

	return PutFieldNoCopy( wFlags, pvData, szFieldName, &varCopy);
}

HRESULT comauto::RecordInfo::PutFieldNoCopy( ULONG, PVOID pvData, LPCOLESTR szFieldName, VARIANT* pvarField)
{
/*[-]*/std::cerr << "comauto::RecordInfo::PutFieldNoCopy(..)" << std::endl;
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
		RETURN_ON_ERROR( hr, comauto::wrapVariantChangeType( pvarField, pvarField, 0, bindptr.lpvardesc->elemdescVar.tdesc.vt));
	}

	char elemsize = sizeofAtomicType( bindptr.lpvardesc->elemdescVar.tdesc.vt);
	if (elemsize)
	{
		std::memcpy( field, comauto::arithmeticTypeAddress( pvarField), elemsize);  //... atomic type copy by value
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

			case VT_LPWSTR:
				*((LPWSTR*)field) = (LPWSTR)(void*)pvarField->pcVal;
				break;

			case VT_LPSTR:
				*((LPSTR*)field) = pvarField->pcVal;
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
/*[-]*/std::cerr << "comauto::RecordInfo::GetFieldNames(..)" << std::endl;
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
/*[-]*/std::cerr << "comauto::RecordInfo::IsMatchingType(..)" << std::endl;
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

PVOID comauto::RecordInfo::createRecord()
{
/*[-]*/std::cerr << "comauto::RecordInfo::createRecord(..)" << std::endl;
	PVOID rt = 0;
	try
	{
		rt = comauto::allocMem( m_typeattr->cbSizeInstance);
		if (rt == NULL) throw std::bad_alloc();
		comauto::RecordInfo::RecordFill( rt, DefaultConstructorZero, true);
		return rt;
	}
	catch (const std::runtime_error& e)
	{
		if (rt) comauto::freeMem( rt);
		throw e;
	}
}

PVOID comauto::RecordInfo::RecordCreate()
{
/*[-]*/std::cerr << "comauto::RecordInfo::RecordCreate(..)" << std::endl;
	PVOID rt = comauto::allocMem( m_typeattr->cbSizeInstance);
	if (rt == NULL) return NULL;

	HRESULT hr = RecordInit( rt);
	if (hr != S_OK)
	{
		comauto::freeMem( rt);
		rt = NULL;
	}
	return rt;
}

HRESULT comauto::RecordInfo::RecordCreateCopy( PVOID pvSource, PVOID* ppvDest)
{
/*[-]*/std::cerr << "comauto::RecordInfo::RecordCreateCopy(..)" << std::endl;
	if (!ppvDest) return E_INVALIDARG;
	*ppvDest = comauto::allocMem( m_typeattr->cbSizeInstance);
	if (!*ppvDest) return E_OUTOFMEMORY;

	HRESULT hr = comauto::RecordInfo::RecordFill( pvSource, CopyConstructor, false, *ppvDest);
	if (hr != S_OK)
	{
		RecordDestroy( *ppvDest);
		*ppvDest = NULL;
	}
	return hr;
}

HRESULT comauto::RecordInfo::RecordDestroy( PVOID pvRecord)
{
/*[-]*/std::cerr << "comauto::RecordInfo::RecordDestroy(..)" << std::endl;
	comauto::RecordInfo::RecordFill( pvRecord, Destructor, false, 0);
	comauto::freeMem( pvRecord);
	return S_OK;
}

static void getRecordInfoMap( unsigned short depht, std::map<comauto::RecordInfoKey,comauto::RecordInfoR>& rimap, ITypeInfo* typeinfo, std::size_t ofs)
{
/*[-]*/std::cerr << "getRecordInfoMap(..)" << std::endl;
	TYPEATTR* typeattr = 0;
	VARDESC* vd = 0;
	ITypeInfo* rectypeinfo = 0;
	try
	{
		WRAP( typeinfo->GetTypeAttr( &typeattr))
		for (WORD iv = 0; iv < typeattr->cVars; ++iv)
		{
			WRAP( typeinfo->GetVarDesc( iv, &vd))

			if (vd->elemdescVar.tdesc.vt == VT_USERDEFINED)
			{
				WRAP( typeinfo->GetRefTypeInfo( vd->elemdescVar.tdesc.hreftype, &rectypeinfo))
				comauto::RecordInfoR rc( new comauto::RecordInfo( rectypeinfo));
				rimap[ comauto::recordInfoKey( depht, ofs + vd->oInst)] = rc;
				if (depht >= 0xFFFF) throw std::runtime_error( "record info structure too complex");
				getRecordInfoMap( depht+1, rimap, rectypeinfo, ofs + vd->oInst);

				rectypeinfo->Release();
				rectypeinfo = 0;
			}
			typeinfo->ReleaseVarDesc( vd);
			vd = 0;
		}
		typeinfo->ReleaseTypeAttr( typeattr);
		typeattr = 0;
	}
	catch (const std::runtime_error& e)
	{
		if (vd) typeinfo->ReleaseVarDesc( vd);
		if (typeattr) typeinfo->ReleaseTypeAttr( typeattr);
		if (rectypeinfo) rectypeinfo->Release();
		throw e;
	}
}

unsigned int comauto::recordInfoKey( unsigned int depht_, unsigned int ofs_)
{
	if ((depht_|ofs_) > 0xFFFF) throw std::runtime_error( "record info key out of range");
	return ((ofs_<<16)+depht_);
}

comauto::RecordInfoMap comauto::getRecordInfoMap( ITypeInfo* typeinfo)
{
	RecordInfoMap rt;
	RecordInfo* baserecinfo = new RecordInfo( typeinfo);
	rt[recordInfoKey(0,0)] = RecordInfoR( baserecinfo );
	if (baserecinfo->typeattr()->cbSizeInstance > 0xFFFF) throw std::runtime_error( "structure size too big for record info structure");
	getRecordInfoMap( 1, rt, typeinfo, 0);
	return rt;
}

bool comauto::RecordInfo::getVariableDescriptor( const std::string& name, VariableDescriptor& descr) const
{
	VariableDescriptorMap::const_iterator di = m_descrmap.find( name);
	if (di == m_descrmap.end()) return false;
	descr.type = di->second.type;
	descr.ofs = di->second.ofs;
	descr.varnum = di->second.varnum;
	return true;
}

void comauto::RecordInfo::initDescr()
{
	VARDESC* vd = 0;
	BSTR name = 0;
	try
	{
		VariableDescriptor descr;

		for (WORD iv = 0; iv < m_typeattr->cVars; ++iv)
		{
			WRAP( m_typeinfo->GetVarDesc( iv, &vd))
			descr.ofs = vd->oInst;
			descr.type = vd->elemdescVar.tdesc.vt;
			descr.varnum = iv;
			UINT nn;
			WRAP( (hr=m_typeinfo->GetNames( vd->memid, &name, 1, &nn)))
			std::string id( comauto::utf8string( name));
			::SysFreeString( name);
			name = 0;
			m_typeinfo->ReleaseVarDesc( vd);
			vd = 0;

			m_descrmap[ id] = descr;
		}
	}
	catch (const std::runtime_error& e)
	{
		if (name) ::SysFreeString( name);
		if (vd) m_typeinfo->ReleaseVarDesc( vd);
		throw e;
	}
}
