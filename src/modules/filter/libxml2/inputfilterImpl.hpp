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
///\brief Input filter abstraction for the libxml2 library

#ifndef _Wolframe_LIBXML2_INPUT_FILTER_HPP_INCLUDED
#define _Wolframe_LIBXML2_INPUT_FILTER_HPP_INCLUDED
#include "documentReader.hpp"
#include "xsltMapper.hpp"
#include "types/countedReference.hpp"
#include "filter/inputfilter.hpp"
#include "types/doctype.hpp"
#include "libxml/parser.h"
#include "libxml/tree.h"
#include "libxml/encoding.h"
#include "libxml/xmlwriter.h"
#include "libxml/xmlsave.h"
#include <cstdlib>
#include <vector>
#include <string>
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace langbind {

struct InputFilterImpl :public InputFilter
{
	explicit InputFilterImpl( const XsltMapper& xsltMapper_)
		:types::TypeSignature("langbind::InputFilterImpl (libxml2)", __LINE__)
		,m_xsltMapper(xsltMapper_)
		,m_node(0)
		,m_value(0)
		,m_prop(0)
		,m_propvalues(0)
		,m_taglevel(0)
		,m_withEmpty(false){}

	explicit InputFilterImpl()
		:types::TypeSignature("langbind::InputFilterImpl (libxml2)", __LINE__)
		,m_node(0)
		,m_value(0)
		,m_prop(0)
		,m_propvalues(0)
		,m_taglevel(0)
		,m_withEmpty(false){}

	InputFilterImpl( const InputFilterImpl& o)
		:types::TypeSignature("langbind::InputFilterImpl (libxml2)", __LINE__)
		,InputFilter(o)
		,m_doc(o.m_doc)
		,m_xsltMapper(o.m_xsltMapper)
		,m_node(o.m_node)
		,m_value(o.m_value)
		,m_prop(o.m_prop)
		,m_propvalues(o.m_propvalues)
		,m_taglevel(o.m_taglevel)
		,m_withEmpty(o.m_withEmpty)
		,m_elembuf(o.m_elembuf)
		,m_encoding(o.m_encoding)
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

	///\brief Implements InputFilter::getMetadata()
	virtual bool getMetadata()
	{
		return (m_doc.get());
	}

	///\brief implement interface member InputFilter::getNext( typename FilterBase::ElementType&,const void*&,std::size_t&)
	virtual bool getNext( InputFilter::ElementType& type, const void*& element, std::size_t& elementsize);

	///\brief Implements 'ContentFilterAttributes::getEncoding() const'
	virtual const char* getEncoding() const
	{
		return m_encoding.empty()?0:m_encoding.c_str();
	}

	///\brief Implements FilterBase::setFlags()
	virtual bool setFlags( Flags f);

private:
	std::string getElementString( const xmlChar* str);
	void getElement( const void*& element, std::size_t& elementsize, const xmlChar* str);
	bool getDocType( types::DocType& doctype);

private:
	DocumentReader m_doc;			//< document reader structure
	XsltMapper m_xsltMapper;		//< optional XSLT mapper
	xmlNode* m_node;			//< current node value
	xmlChar* m_value;			//< current node value
	xmlAttr* m_prop;
	xmlNode* m_propvalues;
	int m_taglevel;				//< tag hierarchy level
	std::vector<xmlNode*> m_nodestk;	//< stack of nodes
	bool m_withEmpty;			//< return empty tokens as W3C requires too
	std::string m_elembuf;			//< buffer for current element
	std::string m_encoding;			//< character set encoding
};

}}//namespace
#endif
