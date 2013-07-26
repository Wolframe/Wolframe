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
///\file filter/tostringfilter.hpp
///\brief Interface for tostring methods in language bindings

#ifndef _Wolframe_FILTER_TOSTRING_FILTER_INTERFACE_HPP_INCLUDED
#define _Wolframe_FILTER_TOSTRING_FILTER_INTERFACE_HPP_INCLUDED
#include "filter/typedfilter.hpp"
#include <string>

namespace _Wolframe {
namespace langbind {

///\class ToStringFilter
///\brief Output filter for tostring methods in language bindings
class ToStringFilter :public TypedOutputFilter
{
public:
	///\brief Constructor
	explicit ToStringFilter( const std::string indentstr_="")
		:types::TypeSignature("langbind::ToStringFilter", __LINE__)
		,m_lasttype(FilterBase::OpenTag)
		,m_indentstr(indentstr_) {}

	///\brief Copy constructor
	///\param[in] o typed output filter to copy
	ToStringFilter( const ToStringFilter& o)
		:types::TypeSignature("langbind::ToStringFilter", __LINE__)
		,TypedOutputFilter(o)
		,m_content(o.m_content)
		,m_lasttype(o.m_lasttype)
		,m_indent(o.m_indent)
		,m_indentstr(o.m_indentstr){}
	///\brief Destructor
	virtual ~ToStringFilter(){}

	///\brief Get a self copy
	///\return allocated pointer to copy of this
	virtual TypedOutputFilter* copy() const		{return new ToStringFilter(*this);}

	///\brief Implementation of TypedOutputFilter::print(ElementType,const types::VariantConst&)
	virtual bool print( ElementType type, const types::VariantConst& element);

	///\brief Get the content
	const std::string& content() const		{return m_content;}

private:
	std::string m_content;				//< content string
	FilterBase::ElementType m_lasttype;		//< last parsed element type
	std::string m_indent;				//< indent array
	std::string m_indentstr;			//< indentiation string
};

}}//namespace
#endif
