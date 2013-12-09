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
#include "comauto/utils.hpp"
#include <iostream>

using namespace _Wolframe;
using namespace _Wolframe::comauto;

static langbind::TypedInputFilterR callProcProvider( const proc::ProcessorProvider* provider_, const char* function_, const langbind::TypedInputFilterR& input_)
{
	std::cout << "CALLING FUNCTION '" << function_ << std::endl;
	langbind::TypedInputFilterR rt;
	return rt;
}

HRESULT comauto::ProcessorProviderDispatch::GetTypeInfoCount( UINT* pCountTypeInfo)
{
	*pCountTypeInfo = 0;
	return 0;
}

HRESULT comauto::ProcessorProviderDispatch::GetTypeInfo( UINT, LCID, ITypeInfo**)
{
	return E_NOTIMPL;
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

HRESULT comauto::ProcessorProviderDispatch::GetIDsOfNames( REFIID riid, LPOLESTR* rgszNames, UINT cNames, LCID /*lcid ignored*/,  DISPID* rgDispId)
{
	if (riid == IID_NULL)
	{
			if (cNames != 1) return DISP_E_UNKNOWNNAME;
			if (compareIdentifier( rgszNames[0], L"call"))
			{
				rgDispId[0] = 0x0001;
			}
			return S_OK;
	}
	else
	{
		return DISP_E_UNKNOWNNAME;
	}
}

HRESULT comauto::ProcessorProviderDispatch::Invoke( DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr)
{
	try
	{
		const char* function_ = "FUNCNAME";
		langbind::TypedInputFilterR input_;
		langbind::TypedInputFilterR result = callProcProvider( m_provider, function_, input_);
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



