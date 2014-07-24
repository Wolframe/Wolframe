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
/// \file filter/tostringfilter.hpp
/// \brief Interface for tostring methods in language bindings

#ifndef _Wolframe_FILTER_TOSTRING_FILTER_INTERFACE_HPP_INCLUDED
#define _Wolframe_FILTER_TOSTRING_FILTER_INTERFACE_HPP_INCLUDED
#include "filter/typedfilter.hpp"
#include <string>

namespace _Wolframe {
namespace langbind {

/// \class ToStringFilter
/// \brief Output filter for tostring methods in language bindings
class ToStringFilter :public TypedOutputFilter
{
public:
	/// \brief Constructor
	explicit ToStringFilter( const std::string indentstr_="")
		:TypedOutputFilter("tostring")
		,m_contentpos(0)
		,m_lasttype(FilterBase::OpenTag)
		,m_indentstr(indentstr_)
		,m_taglevel(0){}

	/// \brief Copy constructor
	/// \param[in] o typed output filter to copy
	ToStringFilter( const ToStringFilter& o)
		:TypedOutputFilter(o)
		,m_content(o.m_content)
		,m_contentpos(o.m_contentpos)
		,m_lasttype(o.m_lasttype)
		,m_indent(o.m_indent)
		,m_indentstr(o.m_indentstr)
		,m_taglevel(o.m_taglevel){}
	/// \brief Destructor
	virtual ~ToStringFilter(){}

	/// \brief Get a self copy
	/// \return allocated pointer to copy of this
	virtual TypedOutputFilter* copy() const		{return new ToStringFilter(*this);}

	/// \brief Implementation of TypedOutputFilter::print(ElementType,const types::VariantConst&)
	virtual bool print( ElementType type, const types::VariantConst& element);

	virtual void getOutput( const void*& buf, std::size_t& bufsize)
	{
		buf = (const void*)(m_content.c_str() + m_contentpos);
		bufsize = m_content.size() - m_contentpos;
		m_contentpos = m_content.size();
	}

	/// \brief Get the content
	const std::string& content() const		{return m_content;}

private:
	std::string m_content;				///< content string
	std::size_t m_contentpos;			///< position in content string
	FilterBase::ElementType m_lasttype;		///< last parsed element type
	std::string m_indent;				///< indent array
	std::string m_indentstr;			///< indentiation string
	int m_taglevel;					///< count of open tags
};

}}//namespace
#endif
