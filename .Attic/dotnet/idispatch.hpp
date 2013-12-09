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
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
//\file comauto/idispatch.hpp
//\brief Implements IDispatch interface for a reference class to pass to .NET via interop
#ifndef _Wolframe_COM_AUTOMATION_IDISPATCH_HPP_INCLUDED
#define _Wolframe_COM_AUTOMATION_IDISPATCH_HPP_INCLUDED
#include <objbase.h>

namespace _Wolframe {
namespace comauto {

template <class Object>
class ObjectReference :public IDispatch
{
public:
	ObjectReference( const Object& obj_)
		:m_obj(obj_){}

	~ObjectReference(){}

    // IDispatch
    HRESULT __stdcall GetTypeInfoCount( UINT* pCountTypeInfo)
	{
		*pCountTypeInfo = 0;
		return 0;
	}

	HRESULT __stdcall GetTypeInfo( UINT, LCID, ITypeInfo**)
	{
		return E_NOTIMPL;
	}

    HRESULT __stdcall GetIDsOfNames( REFIID /*riid IID_NULL always*/, LPOLESTR* rgszNames, UINT cNames, LCID /*lcid ignored*/,  DISPID* rgDispId)
	{
		enum {strlen_call = 4};
		static LPOLESTR rgszName_call = L"call";
		if (riid == 0)
		{
			 if (cNames != 1) return DISP_E_UNKNOWNNAME;
			 int ii=0;
			 for (; ii<=(int)strlen_call && rgszNames[0][ii] == rgszName_call[ii]; ++ii){}
			 if (ii == (int)strlen_call+1)
			 {
				rgDispId[0] = 0x0001;
				return S_OK;
			 }
		}
		else
		{
			return DISP_E_UNKNOWNNAME;
		}
	}

    HRESULT __stdcall Invoke( DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr)
	{
		try
		{
			m_obj->call();
		}
		catch (const std::runtime_error& e)
		{
			pExcepInfo->wCode = 0;
			pExcepInfo->wReserved = 0;
			pExcepInfo->bstrSource = "Wolframe Processor Provider";
			pExcepInfo->bstrDescription = e.what();
			pExcepInfo->bstrHelpFile = 0;
			pExcepInfo->dwHelpContext = 0;
			pExcepInfo->pvReserved = 0;
			pExcepInfo->pfnDeferredFillIn = 0;
			pExcepInfo->scode = 0;
		}
	}
private:
	Object m_obj;
};

}}//namespace
#endif

