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
///\file inputfilterImpl.hpp
///\brief Input filter abstraction for the cJSON library

#ifndef _Wolframe_CJSON_INPUT_FILTER_HPP_INCLUDED
#define _Wolframe_CJSON_INPUT_FILTER_HPP_INCLUDED
#include "filter/inputfilter.hpp"
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

struct InputFilterImpl :public InputFilter
{
	InputFilterImpl()
		:types::TypeSignature("langbind::InputFilterImpl (cjson)", __LINE__)
		,m_state(Buffering)
	{
		setFlags( langbind::FilterBase::PropagateNoAttr);
	}

	InputFilterImpl( const InputFilterImpl& o)
		:types::TypeSignature("langbind::InputFilterImpl (cjson)", __LINE__)
		,InputFilter(o)
		,m_content(o.m_content)
		,m_encoding(o.m_encoding)
		,m_state(o.m_state)
		{}

	///\brief Implements InputFilter::copy()
	virtual InputFilter* copy() const
	{
		return new InputFilterImpl(*this);
	}

	///\brief Implements FilterBase::getValue(const char*,std::string&)
	virtual bool getValue( const char* name, std::string& val);

	///\brief Implements InputFilter::getDocType(std::string&)
	virtual bool getDocType( std::string& val);

	///\brief Implements FilterBase::setValue(const char*,const std::string&)
	virtual bool setValue( const char* name, const std::string& value);

	///\brief Implements InputFilter::putInput(const void*,std::size_t,bool)
	virtual void putInput( const void* content, std::size_t contentsize, bool end);

	virtual bool getMetadata()
	{
		return (m_state == Processing);
	}

	bool getDocType( types::DocType& doctype);

	///\brief implement interface member InputFilter::getNext( typename FilterBase::ElementType&,const void*&,std::size_t&)
	virtual bool getNext( InputFilter::ElementType& type, const void*& element, std::size_t& elementsize);

	virtual const char* getEncoding() const
	{
		return m_encoding.empty()?0:m_encoding.c_str();
	}

	virtual bool setFlags( Flags f);

private:
	std::string m_content;
	std::string m_encoding;
	enum State {Buffering,Processing};
	State m_state;
	cJSON* m_root;

	class StackElement
	{
		StackElement( const cJSON* node_)
			:m_node(node_){}
		StackElement( const StackElement& o)
			:m_node(o.m_node){}
	private:
		const cJSON* m_node;
	};
	std::vector<cJSON*> m_stk;
};

}}//namespace
#endif
