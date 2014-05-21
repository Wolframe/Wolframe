/************************************************************************
Copyright (C) 2011 - 2014 Project Wolframe.
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
///\file filter/bufferingfilter.hpp
///\brief Interface for input filter

#ifndef _Wolframe_FILTER_BUFFERING_FILTER_INTERFACE_HPP_INCLUDED
#define _Wolframe_FILTER_BUFFERING_FILTER_INTERFACE_HPP_INCLUDED
#include "types/countedReference.hpp"
#include "filter/inputfilter.hpp"
#include <string>

namespace _Wolframe {
namespace langbind {

///\class BufferingInputFilter
///\brief Input filter that buffers the input before redirecting to InputFilter::getNext(ElementType&,const void*&,std::size_t&)
class BufferingInputFilter :public InputFilter
{
public:
	BufferingInputFilter( InputFilter* ref, const char* name_)
		:utils::TypeSignature("langbind::BufferingInputFilter", __LINE__)
		,InputFilter(name_)
		,m_ref(ref->copy()),m_end(false),m_fetched(false){}

	BufferingInputFilter( const BufferingInputFilter& o)
		:utils::TypeSignature(o)
		,InputFilter(o)
		,m_ref(o.m_ref->copy())
		,m_buf(o.m_buf)
		,m_end(o.m_end)
		,m_fetched(o.m_fetched){}

	virtual ~BufferingInputFilter()
	{
		delete m_ref;
	}

	///\brief Implements InputFilter::copy()const
	virtual InputFilter* copy() const
	{
		return new BufferingInputFilter( *this);
	}

	///\brief Implements InputFilter::initcopy()const
	virtual InputFilter* initcopy() const
	{
		return new BufferingInputFilter( m_ref, m_ref->name());
	}

	///\brief Implements InputFilter::getRest(const void*&,std::size_t&,bool)
	virtual void getRest( const void*& ptr, std::size_t& size, bool& end_)
	{
		end_ = m_end;
		if (m_fetched)
		{
			ptr = 0;
			size = 0;
		}
		else
		{
			ptr = m_buf.c_str();
			size = m_buf.size();
		}
	}

	///\brief Implements InputFilter::putInput(const void*,std::size_t,bool)
	virtual void putInput( const void* ptr, std::size_t size, bool end_)
	{
		m_buf.append( (const char*)ptr, size);
		if (end_)
		{
			m_ref->putInput( m_buf.c_str(), m_buf.size(), m_end=true);
		}
	}

	///\brief Implements InputFilter::getNext(ElementType&,const void*&,std::size_t&)
	virtual bool getNext( ElementType& type, const void*& element, std::size_t& elementsize)
	{
		if (m_end)
		{
			m_fetched = true;
			bool rt = m_ref->getNext( type, element, elementsize);
			setState( m_ref->state(), m_ref->getError());
			return rt;
		}
		else
		{
			setState( EndOfMessage);
			return false;
		}
	}

	virtual const types::DocMetaData* getMetaData()
	{
		if (m_end)
		{
			const types::DocMetaData* rt = m_ref->getMetaData( doctype);
			setState( m_ref->state(), m_ref->getError());
			return rt;
		}
		else
		{
			setState( EndOfMessage);
			return false;
		}
	}

	virtual const char* getEncoding() const
	{
		return m_ref->getEncoding();
	}

	///\brief Implements FilterBase::getValue(const char*,std::string&) const
	virtual bool getValue( const char* id, std::string& val) const
	{
		return m_ref->getValue( id, val);
	}

	///\brief Implements FilterBase::setValue(const char*,const std::string&)
	virtual bool setValue( const char* id, const std::string& val)
	{
		return m_ref->setValue( id, val);
	}

	InputFilter* reference() const
	{
		return m_ref;
	}

	bool end() const
	{
		return m_end;
	}
private:
	InputFilter* m_ref;
	std::string m_buf;
	bool m_end;
	bool m_fetched;
};

}}//namespace
#endif


