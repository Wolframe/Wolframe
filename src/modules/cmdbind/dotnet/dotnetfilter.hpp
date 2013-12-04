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
///\file dotnetfilter.hpp
///\brief Interface to a .NET (Windows) VARIANT data type (structure) as filter

#ifndef _Wolframe_DOTNET_FILTER_HPP_INCLUDED
#define _Wolframe_DOTNET_FILTER_HPP_INCLUDED
#include "filter/typedfilter.hpp"
#include <vector>
#include <stdexcept>

#if defined(_WIN32)
#include <oaidl.h>
#include <comdef.h>
#else
#error .NET support not enabled (Windows only)
#endif

namespace _Wolframe {
namespace langbind {

///\class DotnetInputFilter
///\brief .NET VARIANT data type as typed input filter
class DotnetInputFilter :public TypedInputFilter
{
public:
	///\brief Constructor
	///\remark Expects that the stack is not modified by anyone but this class in the lifetime after the first call of DotnetInputFilter::getNext(ElementType&,types::VariantConst&)
	explicit DotnetInputFilter();

	///\brief Copy constructor
	///\param[in] o input filter to copy
	DotnetInputFilter( const DotnetInputFilter& o)
		:types::TypeSignature("langbind::DotnetInputFilter", __LINE__)
		,TypedInputFilter(o)
		,m_ls(o.m_ls)
		,m_stk(o.m_stk)
	{
		if (m_stk.size() > 0) throw std::runtime_error( "copy of .NET input filter not allowed in this state");
	}

	///\brief Destructor
	virtual ~DotnetInputFilter(){}

	///\brief Get a self copy
	///\return allocated pointer to copy of this
	virtual TypedInputFilter* copy() const		{return new DotnetInputFilter(*this);}

	///\brief Implementation of TypedInputFilter::getNext(ElementType&,types::VariantConst&)
	virtual bool getNext( ElementType& type, Element& element);

private:
	variant_t m_obj;			//< object
	std::vector<FetchState> m_stk;		//< stack of iterator states
};


///\class DotnetOutputFilter
///\brief typed output filter for a NET variant type
class DotnetOutputFilter :public TypedOutputFilter
{
public:
	///\brief Constructor
	///\remark Expects that the stack is not modified by anyone but this class in the lifetime after the first call of DotnetOutputFilter::print(ElementType,const types::VariantConst&)
	explicit DotnetOutputFilter()
		:types::TypeSignature("langbind::DotnetOutputFilter", __LINE__)
		,m_type(OpenTag){}

	///\brief Copy constructor
	///\param[in] o output filter to copy
	DotnetOutputFilter( const DotnetOutputFilter& o)
		:types::TypeSignature("langbind::DotnetOutputFilter", __LINE__)
		,TypedOutputFilter(o)
		,m_type(o.m_type)
	{
		if (m_statestk.size() > 0) throw std::runtime_error( "copy of .NET output filter not allowed in this state");
	}

	///\brief Destructor
	virtual ~DotnetOutputFilter(){}

	///\brief Get a self copy
	///\return allocated pointer to copy of this
	virtual TypedOutputFilter* copy() const		{return new DotnetOutputFilter(*this);}

	///\brief Implementation of TypedOutputFilter::print(ElementType,const types::Variant&)
	virtual bool print( ElementType type, const types::Variant& element);

private:
	struct StackElem {};
	ElementType m_type;
	std::vector<StackElem> m_statestk;
};

}}//namespace
#endif

