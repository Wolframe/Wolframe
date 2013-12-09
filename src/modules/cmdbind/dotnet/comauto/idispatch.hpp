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

struct _GUID;
typedef _GUID* REFIID; 
#ifndef HRESULT
#define HRESULT __int32
#undef

namespace _Wolframe {
namespace comauto {

template <class Object>
class ObjectReference :public IDispatch
{
public:
	ObjectReference( const Object& obj_, REFIID iid_)
		:m_obj(obj_),m_iid(iid_){}

	~ObjectReference(){}

	typedef __uint32 ULONG;
	typedef __int32 LONG;

	HRESULT QueryInterface( REFIID riid, void** ppvObj)
	{
		if (!ppvObj) return E_INVALIDARG;
		*ppvObj = 0;
		if (riid == IID_IUnknown)
		{
			*ppvObj = (LPVOID)this;
			AddRef();
			return NOERROR;
		}
		else if (riid == m_iid)
		{
			*ppvObj = (LPVOID)this;
			AddRef();
			return NOERROR;
		}
		return E_NOINTERFACE;
	}

	ULONG AddRef()
	{
		::InterlockedIncrement( m_cRef);
		return m_cRef;
	}

	ULONG Release()
	{
		ULONG ulRefCount = ::InterlockedDecrement(m_cRef);
		if (0 == m_cRef)
		{
			delete this;
		}
		return ulRefCount;
	}

private:
	Object m_obj;
	REFIID m_iid;
};

}}//namespace
#endif

