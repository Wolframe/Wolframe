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
///\file outputfilterImpl.hpp
///\brief Output filter abstraction for the cjson library

#ifndef _Wolframe_CJSON_OUTPUT_FILTER_HPP_INCLUDED
#define _Wolframe_CJSON_OUTPUT_FILTER_HPP_INCLUDED
#include "filter/outputfilter.hpp"
#include "types/countedReference.hpp"
#include "types/doctype.hpp"
extern "C"
{
#include "cjson/cJSON.h"
}
#include <cstdlib>
#include <vector>
#include <string>
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace langbind {

class OutputFilterImpl
	:public OutputFilter
{
public:
	typedef OutputFilter Parent;

	explicit OutputFilterImpl( const ContentFilterAttributes* attr=0)
		:types::TypeSignature("langbind::OutputFilterImpl (cjson)", __LINE__)
		,OutputFilter(attr)
		{}

	OutputFilterImpl( const OutputFilterImpl& o)
		:types::TypeSignature("langbind::OutputFilterImpl (cjson)", __LINE__)
		,OutputFilter(o)
		,m_attribname(o.m_attribname)
		,m_elembuf(o.m_elembuf)
		,m_elemitr(o.m_elemitr)
		,m_doctype_root(o.m_doctype_root)
		,m_doctype_system(o.m_doctype_system)
		,m_encoding(o.m_encoding){}

	virtual ~OutputFilterImpl(){}

	///\brief Implementation of OutputFilter::copy()
	virtual OutputFilterImpl* copy() const
	{
		return new OutputFilterImpl( *this);
	}

	///\brief Implementation of OutputFilter::setDocType( const std::string&)
	virtual void setDocType( const std::string& value);

	///\brief Implementation of OutputFilter::print( ElementType, const void*,std::size_t)
	virtual bool print( ElementType type, const void* element, std::size_t elementsize);

	///\brief Implementation of FilterBase::getValue( const char*, std::string&)
	virtual bool getValue( const char* name, std::string& val);

	///\brief Implementation of FilterBase::setValue( const char*, const std::string&)
	virtual bool setValue( const char* name, const std::string& value);

	void setEncoding( const std::string& value)
	{
		m_encoding = value;
	}

	const char* encoding() const;

private:
	bool flushBuffer();

private:
	std::string m_attribname;				//< attribute name buffer
	std::string m_elembuf;					//< buffer for current element
	std::size_t m_elemitr;					//< iterator on current element
	std::string m_doctype_root;				//< !DOCTYPE root element (1)
	std::string m_doctype_system;				//< !DOCTYPE system element (3)
	std::string m_encoding;					//< character set encoding
};

}}//namespace
#endif

