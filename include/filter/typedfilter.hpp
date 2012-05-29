/************************************************************************
Copyright (C) 2011 Project Wolframe.
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
///\file filter/typedfilter.hpp
///\brief Typed interface for input/output filter

#ifndef _Wolframe_FILTER_TYPEDFILTER_INTERFACE_HPP_INCLUDED
#define _Wolframe_FILTER_TYPEDFILTER_INTERFACE_HPP_INCLUDED
#include "countedReference.hpp"
#include "filter/filterbase.hpp"
#include "filter/inputfilter.hpp"
#include "filter/outputfilter.hpp"
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace langbind {


///\class TypedInputFilter
///\brief Input filter with atomic values having a type
class TypedInputFilter :public TypedFilterBase
{
public:
	TypedInputFilter(){}
	virtual ~TypedInputFilter(){}

	///\brief Get next element
	///\param [out] type element type parsed
	///\param [out] element reference to element returned
	///\return true, if success, false, if not.
	///\remark Check the state when false is returned
	virtual bool getNext( ElementType& type, Element& element)=0;
};


///\class TypedOutputFilter
///\brief Output filter with atomic values having a type
class TypedOutputFilter :public TypedFilterBase
{
public:
	TypedOutputFilter(){}
	virtual ~TypedOutputFilter(){}

	///\brief Get next element
	///\param [out] type element type parsed
	///\param [out] element reference to element returned
	///\return true, if success, false, if not.
	///\remark Check the state when false is returned
	virtual bool print( ElementType type, const Element& element)=0;
};



}}//namespace
#endif


