/************************************************************************
Copyright (C) 2011, 2012 Project Wolframe.
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
#ifndef _Wolframe_TYPE_SIGNATURE_HPP_INCLUDED
#define _Wolframe_TYPE_SIGNATURE_HPP_INCLUDED
///\file types/typeSignature.hpp
///\brief Signature for structures to detect memory problems
#include "types/malloc.hpp"
#include <cstddef>
#include <cstring>
#include <stdexcept>
#include <boost/lexical_cast.hpp>

namespace _Wolframe {
namespace types {

struct ProtectedMem
{
	void* operator new( std::size_t size)
	{
		void* rt = types::malloc( size);
		if (!rt) throw std::bad_alloc();
		return rt;
	}

	void operator delete(void* ptr)
	{
		types::free( ptr);
	}
};

///\class TypeSignature
///\brief Signature of a type for pointer constraint check
class TypeSignature
{
	static unsigned int getStmp( unsigned int objid_) {return 2654435761U*(objid_+123);}
public:
	TypeSignature( const char* objname_, unsigned int objid_)
		:m_objid(objid_)
		,m_objinvid(getStmp(objid_))
	{
		unsigned int nn = std::strlen( objname_);
		if (nn >= objnamesize) nn = objnamesize-1;
		std::memset( m_objname, (char)m_objid, objnamesize);
		std::memcpy( m_objname, objname_, nn);
		m_objname[nn] = 0;
		m_chk = chk();
	}

	virtual ~TypeSignature()
	{
		verify();
	}

	void operator=( const TypeSignature& o)
	{
		m_objid = o.m_objid;
		m_objinvid = o.m_objinvid;
	}

private:
	void verify() const
	{
		if (m_objid != m_objid || m_objinvid != getStmp(m_objid) || m_chk != chk())
		{
			throw std::runtime_error( std::string( "internal: memory curruption detected for object ") + m_objname + " line:" + boost::lexical_cast<std::string>(m_objid) + ")");
		}
	}
	unsigned int chk() const
	{
		unsigned int xx = 123,ii=0;
		for (; ii<objnamesize; ++ii) xx += (m_objname[ii]+ii) * 123;
		return xx;
	}

private:
	enum {objnamesize=32};
	unsigned int m_objid;
	unsigned int m_chk;
	char m_objname[objnamesize];
	unsigned int m_objinvid;
};

}}//namespace
#endif



