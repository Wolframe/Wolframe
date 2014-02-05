/************************************************************************
Copyright (C) 2011 - 2013 Project Wolframe.
All rights reserved.

This file is part of Project Wolframe.

Commercial Usage
Licensees holding valid Project Wolframe Commercial licenses may
use this file in accordance with the Project Wolframe
Commercial License Agreement provided with the Software or,
alternatively, in accordance with the terms contained
in a written agreement between the licensee and Project Wolframe.

GNU General Public License Usage
Alternatively, you can redistribute this file and/or modify it
under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Wolframe is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
//\file comauto/procProviderDispatch.cpp
//\brief Implementation of Dispatch Interface for calls of processor provider function from .NET via interop callback
#include "comauto/procProviderDispatch.hpp"
#include "comauto/variantInputFilter.hpp"
#include "comauto/variantAssignment.hpp"
#include "comauto/utils.hpp"
#include "comauto/typelib.hpp"
#include "comauto/variantToString.hpp"
#include "logger-v1.hpp"
#include <iostream>

using namespace _Wolframe;
using namespace _Wolframe::comauto;

//\brief GUID of ProcProvider (orig "CE320901-8DBD-459A-821F-423E6C14D661")
static const CLSID g_CLSID_ProcProvider =
	{ 0xCE320901,0x8DBD,0x459A,{0x82,0x1F,0x42,0x3E,0x6C,0x14,0xD6,0x61}};

GUID comauto::ProcessorProviderDispatch::uuid()
{
	return g_CLSID_ProcProvider;
}

static langbind::TypedInputFilterR callProcProvider( const proc::ProcessorProvider* provider_, const std::string& funcname_, const langbind::TypedInputFilterR& input_)
{
	const langbind::FormFunction* func = provider_->formFunction( funcname_);
	if (func == 0)
	{
		throw std::runtime_error( std::string( "function '") + funcname_ + "' not found (processor provider function call)");
	}
	langbind::FormFunctionClosureR funcexec( func->createClosure());
	if (!funcexec.get())
	{
		throw std::runtime_error( "creation of function execution context failed (processor provider function call)");
	}
	serialize::Context::Flags flags = serialize::Context::CaseInsensitiveCompare;
	funcexec->init( provider_, input_, flags);
	if (!funcexec->call())
	{
		throw std::runtime_error( std::string( "call of function '") + funcname_ + "' failed (processor provider function call)");
	}
	return funcexec->result();
}

HRESULT comauto::ProcessorProviderDispatch::GetTypeInfoCount( UINT* pCountTypeInfo)
{
	*pCountTypeInfo = 1;
	return NOERROR;
}

HRESULT comauto::ProcessorProviderDispatch::GetTypeInfo( UINT tidx, LCID, ITypeInfo** typeinfo_)
{
	if (typeinfo_ == NULL) return E_INVALIDARG;
	*typeinfo_ = 0;

	if (tidx == 0)
	{
		m_typeinfo->AddRef();
		*typeinfo_ = m_typeinfo;
		return NOERROR;
	}
	else
	{
		return DISP_E_BADINDEX;
	}
}

//\brief Ascii case insensitive compare of two strings
static int compareIdentifier( LPOLESTR aa, LPOLESTR bb)
{
		int ii = 0;
		for (; bb[ii] != 0; ++ii)
		{
			if (aa[ii] > 127)
			{
				if (aa[ii] != bb[ii]) break;
			}
			else
			{
				if ((aa[ii]|32) != (bb[ii]|32)) break;
			}
		}
		return (aa[0] == 0);
}

HRESULT comauto::ProcessorProviderDispatch::GetIDsOfNames( REFIID , LPOLESTR* rgszNames, UINT cNames, LCID /*lcid ignored*/,  DISPID* rgDispId)
{
	return DispGetIDsOfNames( m_typeinfo, rgszNames, cNames, rgDispId);
}

static CLSID getUUIDfromVariantArg( const VARIANT& arg, const IRecordInfo* recordinfo, const ITypeInfo* typeinfo)
{
	CLSID rt;
	std::memset( &rt, 0, sizeof(rt));

	TYPEATTR *typeattr = 0;
	int varidx = 0;
	wchar_t fieldname[3];
	fieldname[0] = '_';
	fieldname[1] = 'a';
	fieldname[2] = 0;
	VARIANT elem;
	elem.vt = VT_EMPTY;
	try
	{
		WRAP( const_cast<ITypeInfo*>(typeinfo)->GetTypeAttr( &typeattr));

		if (typeattr->cVars != 11/*1+2+8*/)
		{
			throw std::runtime_error( "invalid argument for Guid (3rd argument of ProcProvider::call)");
		}
		for (; varidx < typeattr->cVars; ++varidx,++fieldname[1])
		{
			if (S_OK != const_cast<IRecordInfo*>(recordinfo)->GetField( arg.pvRecord, fieldname, &elem))
			{
				throw std::runtime_error( "invalid argument for Guid (3rd argument of ProcProvider::call)");
			}
			if (elem.vt == VT_I4)
			{
				if (varidx != 0)
				{
					throw std::runtime_error( "invalid argument for Guid (3rd argument of ProcProvider::call)");
				}
				rt.Data1 = elem.intVal;
			}
			else if (elem.vt == VT_I2)
			{
				if (varidx == 1)
				{
					rt.Data2 = elem.iVal;
				}
				else if (varidx == 2)
				{
					rt.Data3 = elem.iVal;
				}
				else
				{
					throw std::runtime_error( "invalid argument for Guid (3rd argument of ProcProvider::call)");
				}
			}
			else if (elem.vt == VT_UI1)
			{
				if (varidx < 3)
				{
					throw std::runtime_error( "invalid argument for Guid (3rd argument of ProcProvider::call)");
				}
				rt.Data4[varidx-3] = elem.bVal;
			}
			else
			{
				throw std::runtime_error( "invalid argument for Guid (3rd argument of ProcProvider::call)");
			}
		}
	}
	catch (const std::runtime_error& e)
	{
		if (typeinfo && typeattr) const_cast<ITypeInfo*>(typeinfo)->ReleaseTypeAttr( typeattr);
		throw e;
	}
	if (typeinfo && typeattr) const_cast<ITypeInfo*>(typeinfo)->ReleaseTypeAttr( typeattr);
	return rt;
}


HRESULT comauto::ProcessorProviderDispatch::Invoke( DISPID dispIdMember, REFIID /*riid*/, LCID /*lcid*/, WORD /*wFlags*/, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* /*puArgErr*/)
{
	try
	{
		/*[-]*/std::cout << "++++ CALL comauto::ProcessorProviderDispatch::Invoke " << (int)dispIdMember << std::endl;
		if (dispIdMember == (DISPID)DispID_CALL || dispIdMember == (DISPID)DispID_CALL_NORES)
		{
			/*[-]*/std::cout << "++++ LINE " << (int)__LINE__ << std::endl;
			if (pDispParams->cNamedArgs != 0)
			{
				throw std::runtime_error( "handling of named parameters not implemented");
			}
			/*[-]*/std::cout << "++++ LINE " << (int)__LINE__ << std::endl;
			std::string funcname;
			VARIANT* inputarg = 0;
			VARIANT* resultarg = 0;
			ITypeInfo* resultGuidTypeInfo = 0;
			ITypeInfo* resultTypeInfo = 0;
			ITypeInfo* inputTypeInfo = 0;
			HRESULT hr = S_OK;

			if (dispIdMember == (DISPID)DispID_CALL_NORES)
			{
				if (pDispParams->cArgs != 2)
				{
					throw std::runtime_error( "illegal number of arguments (2 arguments expected)");
				}
				funcname = utf8string( pDispParams->rgvarg[ 1].bstrVal);
				inputarg  = &pDispParams->rgvarg[0];
			}
			else
			{
				if (pDispParams->cArgs != 3)
				{
					throw std::runtime_error( "illegal number of arguments (3 arguments expected)");
				}
				funcname = utf8string( pDispParams->rgvarg[ 2].bstrVal);
				inputarg  = &pDispParams->rgvarg[1];
				resultarg = &pDispParams->rgvarg[0];
				if (resultarg->vt != VT_RECORD || !resultarg->pRecInfo) throw std::runtime_error( "result parameter of provider call is not a valid structure (VT_RECORD) with public type info");
			}

			try
			{
				/*[-]*/std::cout << "++++ LINE " << (int)__LINE__ << std::endl;
				hr = inputarg->pRecInfo->GetTypeInfo( &inputTypeInfo);
				if (hr != S_OK) throw std::runtime_error( "cannot get type info from record info of input (is it declared as public ?)");
				if (resultarg)
				{
					hr = resultarg->pRecInfo->GetTypeInfo( &resultGuidTypeInfo);
					if (hr != S_OK) throw std::runtime_error( "cannot get type info from record info of result (is it declared as public ?)");
					/*[-]*/std::cout << "++++ LINE " << (int)__LINE__ << std::endl;
					CLSID resultUUID = getUUIDfromVariantArg( *resultarg, resultarg->pRecInfo, resultGuidTypeInfo);
					WRAP (const_cast<ITypeLib*>(m_typelib->typelib())->GetTypeInfoOfGuid( resultUUID, &resultTypeInfo));
				}

				int fs = (int)(serialize::Context::CaseInsensitiveCompare)|(int)(serialize::Context::ValidateInitialization);
				serialize::Context::Flags flags = (serialize::Context::Flags)fs;

				/*[-]*/std::cout << "++++ LINE " << (int)__LINE__ << std::endl;
				langbind::TypedInputFilterR input( new VariantInputFilter( m_typelib, inputTypeInfo, *inputarg, flags));
				langbind::TypedInputFilterR result = callProcProvider( m_provider, funcname, input);

				if (resultarg)
				{
					/*[-]*/std::cout << "++++ LINE " << (int)__LINE__ << std::endl;
					AssignmentClosure resultassign( m_typelib, result, resultTypeInfo);
					/*[-]*/std::cout << "++++ LINE " << (int)__LINE__ << std::endl;
					VARIANT res;
					res.vt = VT_EMPTY;
					/*[-]*/std::cout << "++++ LINE " << (int)__LINE__ << std::endl;
					if (!resultassign.call( res))
					{
						/*[-]*/std::cout << "++++ LINE " << (int)__LINE__ << std::endl;
						throw std::runtime_error( "failed to assign result of processor provider call");
					}
					/*[-]*/std::cout << "++++ LINE " << (int)__LINE__ << std::endl;
					*pVarResult = res;
				}
				if (resultTypeInfo) resultTypeInfo->Release();
				if (resultGuidTypeInfo) resultGuidTypeInfo->Release();
				if (inputTypeInfo) inputTypeInfo->Release();
			}
			catch (const std::runtime_error& e)
			{
				if (resultGuidTypeInfo) resultGuidTypeInfo->Release();
				if (resultTypeInfo) resultTypeInfo->Release();
				if (inputTypeInfo) inputTypeInfo->Release();
				throw e;
			}
		}
		/*[-]*/std::cout << "++++ DONE comauto::ProcessorProviderDispatch::Invoke" << std::endl;
		return S_OK;
	}
	catch (const std::runtime_error& e)
	{
		pExcepInfo->wCode = 0;
		pExcepInfo->wReserved = 0;
		pExcepInfo->bstrSource = createBSTR( "Wolframe Processor Provider");
		pExcepInfo->bstrDescription = createBSTR( e.what());
		pExcepInfo->bstrHelpFile = 0;
		pExcepInfo->dwHelpContext = 0;
		pExcepInfo->pvReserved = 0;
		pExcepInfo->pfnDeferredFillIn = 0;
		pExcepInfo->scode = 0;
		return DISP_E_EXCEPTION;
	}
}

HRESULT comauto::ProcessorProviderDispatch::QueryInterface( REFIID riid, void** ppvObj)
{
	if (!ppvObj) return E_INVALIDARG;
	*ppvObj = 0;
	if (riid == IID_IUnknown)
	{
		*ppvObj = (LPVOID)this;
		AddRef();
		return NOERROR;
	}
	else if (riid == IID_IDispatch)
	{
		*ppvObj = (LPVOID)this;
		AddRef();
		return NOERROR;
	}
	else if (riid == IID_NULL)
	{
		*ppvObj = (LPVOID)this;
		AddRef();
		return NOERROR;
	}
	else if (riid == g_CLSID_ProcProvider)
	{
		*ppvObj = (LPVOID)this;
		AddRef();
		return NOERROR;
	}
	return E_NOINTERFACE;
}

ULONG comauto::ProcessorProviderDispatch::AddRef()
{
	::InterlockedIncrement( &m_refcount);
	return m_refcount;
}

ULONG comauto::ProcessorProviderDispatch::Release()
{
	ULONG ulRefCount = ::InterlockedDecrement( &m_refcount);
	if (0 == m_refcount)
	{
		delete this;
	}
	return ulRefCount;
}

IDispatch* ProcessorProviderDispatch::create(  const proc::ProcessorProvider* provider_, const TypeLib* typelib_, ITypeInfo* typeinfo_)
{
	ProcessorProviderDispatch* inst = new ProcessorProviderDispatch( provider_, typelib_, typeinfo_);
	IDispatch* rt = inst;
	return rt;
}



