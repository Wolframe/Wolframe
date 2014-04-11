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
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
///\file database/vm/output.hpp
///\brief Defines the output of a virtual machine for database transactions
#ifndef _DATABASE_VIRTUAL_MACHINE_OUTPUT_HPP_INCLUDED
#define _DATABASE_VIRTUAL_MACHINE_OUTPUT_HPP_INCLUDED
#include "types/variant.hpp"
#include <vector>
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace db {
namespace vm {

class Output
{
public:
	Output(){}
	Output( const Output& o)
		:m_ar(o.m_ar){}

	class Element
	{
	public:
		enum Operation
		{
			Open,			//< open of a single element
			OpenArray,		//< open of an array 
			OpenArrayElement,	//< open of an array element
			Close,			//< close of a single element
			CloseArray,		//< close of an array
			CloseArrayElement,	//< close of an array element
			Value			//< content value
		};
		Element()
			:m_op(Value){}
		Element( const Element& o)
			:m_op(o.m_op),m_arg(o.m_arg){}
		Element( const Operation& op_, const types::Variant& arg_)
			:m_op(op_),m_arg(arg_){}
		explicit Element( const Operation& op_)
			:m_op(op_){}

		Operation op() const			{return m_op;}
		const types::Variant& arg() const	{return m_arg;}

	private:
		Operation m_op;
		types::Variant m_arg;
	};

	void add( const Element& elem)
	{
		m_ar.push_back( elem);
	}
	void addValue( const types::Variant& value)
	{
		m_ar.push_back( Element( Element::Value, value));
	}

	typedef std::vector<Element>::const_iterator const_iterator;
	const_iterator begin() const				{return m_ar.begin();}
	const_iterator end() const				{return m_ar.end();}

	const std::vector<Element>& elements() const;

private:
	std::vector<Element> m_ar;
};

typedef boost::shared_ptr<Output> OutputR;

}}}//namespace
#endif

