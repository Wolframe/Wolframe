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
///\file filter/typecastfilter.hpp
///\brief Typed input/output filter wrapper template

#ifndef _Wolframe_SERIALIZE_FILTER_HPP_INCLUDED
#define _Wolframe_SERIALIZE_FILTER_HPP_INCLUDED
#include "filter/inputfilter.hpp"
#include "filter/outputfilter.hpp"
#include "filter/typedfilter.hpp"

namespace _Wolframe {
namespace langbind {

class SerializeInputFilter :public TypedInputFilter
{
public:
	SerializeInputFilter( InputFilter* inp=0)
		:m_inputfilter(inp){}
	SerializeInputFilter( const SerializeInputFilter& o)
		:m_inputfilter(o.m_inputfilter){}
	virtual ~SerializeInputFilter(){}

	///\brief Implementation of TypedInputFilter::getNext(ElementType&,Element&)
	virtual bool getNext( ElementType& type, Element& element);

private:
	InputFilter* m_inputfilter;
};


class SerializeOutputFilter :public TypedOutputFilter
{
public:
	SerializeOutputFilter( OutputFilter* out=0)
		:m_outputfilter(out){}
	SerializeOutputFilter( const SerializeOutputFilter& o)
		:m_outputfilter(o.m_outputfilter){}
	virtual ~SerializeOutputFilter(){}

	///\brief Implementation of TypedOutputFilter::print( ElementType type, const Element& element)
	virtual bool print( ElementType type, const Element& element);

private:
	OutputFilter* m_outputfilter;
};

}}//namespace
#endif


