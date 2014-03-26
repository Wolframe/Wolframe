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
///\file inputfilterImpl.hpp
///\brief Input filter abstraction for the cJSON library

#ifndef _Wolframe_CJSON_INPUT_FILTER_HPP_INCLUDED
#define _Wolframe_CJSON_INPUT_FILTER_HPP_INCLUDED
#include "filter/inputfilter.hpp"
#include "types/countedReference.hpp"
#include "types/string.hpp"
#include "types/doctype.hpp"
extern "C"
{
#include "cJSON.h"
}
#include <cstdlib>
#include <vector>
#include <string>
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace langbind {

struct InputFilterImpl :public InputFilter
{
	typedef InputFilter Parent;

	InputFilterImpl()
		:utils::TypeSignature("langbind::InputFilterImpl (cjson)", __LINE__)
		,InputFilter("cjson")
		,m_encattr_defined(false)
		,m_firstnode(0)
	{
		setFlags( langbind::FilterBase::PropagateNoAttr);
	}

	InputFilterImpl( const InputFilterImpl& o)
		:utils::TypeSignature("langbind::InputFilterImpl (cjson)", __LINE__)
		,InputFilter(o)
		,m_content(o.m_content)
		,m_encattr(o.m_encattr)
		,m_encattr_defined(o.m_encattr_defined)
		,m_doctype(o.m_doctype)
		,m_root(o.m_root)
		,m_firstnode(o.m_firstnode)
		,m_stk(o.m_stk)
		{}

	virtual ~InputFilterImpl(){}

	///\brief Implements InputFilter::copy()
	virtual InputFilter* copy() const
	{
		return new InputFilterImpl(*this);
	}
	///\brief Implements InputFilter::initcopy()
	virtual InputFilter* initcopy() const
	{
		return new InputFilterImpl();
	}

	///\brief Implements InputFilter::getRest( const void*&,std::size_t&,bool&);
	virtual void getRest( const void*& ptr, std::size_t& size, bool& end);

	///\brief Implements FilterBase::getValue(const char*,std::string&) const
	virtual bool getValue( const char* name, std::string& val) const;

	///\brief Implements InputFilter::getDocType(std::string&)
	virtual bool getDocType( types::DocType& doctype);

	///\brief Implements FilterBase::setValue(const char*,const std::string&)
	virtual bool setValue( const char* name, const std::string& value);

	///\brief Implements InputFilter::putInput(const void*,std::size_t,bool)
	virtual void putInput( const void* content, std::size_t contentsize, bool end);

	///\brief Implements 'ContentFilterAttributes::getEncoding() const'
	virtual const char* getEncoding() const;

	///\brief Implements InputFilter::getMetadata()
	virtual bool getMetadata();

	///\brief implement interface member InputFilter::getNext( typename FilterBase::ElementType&,const void*&,std::size_t&)
	virtual bool getNext( InputFilter::ElementType& type, const void*& element, std::size_t& elementsize);

	///\brief Implements FilterBase::setFlags()
	virtual bool setFlags( Flags f);

private:
	boost::shared_ptr<cJSON> parse( const std::string& content);
	bool getNodeValue( const void*& element, std::size_t& elementsize);

private:
	std::string m_content;
	types::String::EncodingAttrib m_encattr;	//< character set encoding attributes
	bool m_encattr_defined;				//< true, if character set encoding is defined
	types::DocType m_doctype;			//< document type
	boost::shared_ptr<cJSON> m_root;		//< data structure holding the whole tree
	const cJSON* m_firstnode;			//< first node (to detect if getNext has been called)

	struct StackElement
	{
		explicit StackElement( const cJSON* node_, const char* tag_=0)
			:m_state(StateOpen),m_node(node_),m_tag(tag_){}
		StackElement( const StackElement& o)
			:m_state(o.m_state),m_node(o.m_node),m_tag(o.m_tag){}

		enum State {StateOpen,StateAttributeValue,StateContentValue,StateChild,StateValue,StateNext,StateCheckEnd,StateReopen};
		static const char* stateName( State i)
		{
			static const char* ar[] = {"StateOpen","StateAttributeValue","StateContentValue","StateChild","StateValue","StateNext","StateCheckEnd","StateReopen"};
			return ar[i];
		}
		State m_state;				//< current state
		const cJSON* m_node;			//< current node
		const char* m_tag;			//< current tag name
	};
	std::vector<StackElement> m_stk;		//< state stack
};

}}//namespace
#endif
