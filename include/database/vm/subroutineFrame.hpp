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
///\file database/vm/subroutineFrame.hpp
///\brief Defines the instruction set of the virtual machine defining database transactions
#ifndef _DATABASE_VIRTUAL_MACHINE_SUBROUTINE_FRAME_HPP_INCLUDED
#define _DATABASE_VIRTUAL_MACHINE_SUBROUTINE_FRAME_HPP_INCLUDED
#include "database/vm/valueTupleSet.hpp"
#include "types/variant.hpp"
#include <string>
#include <vector>
#include <cstdlib>
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace db {
namespace vm {

class SubroutineFrame
{
public:
	SubroutineFrame(){}
	explicit SubroutineFrame( const std::vector<std::string>& paramnames_)
		:m_paramnames(paramnames_){}
	SubroutineFrame( const SubroutineFrame& o)
		:m_paramnames(o.m_paramnames)
		,m_paramvalues(o.m_paramvalues){}

	void clear()
	{
		m_paramnames.clear();
		m_paramvalues.clear();
	}

	void init( const std::vector<std::string>& paramnames_)
	{
		clear();
		m_paramnames = paramnames_;
	}

	void push( const types::Variant& p)
	{
		if (m_paramvalues.size() >= m_paramnames.size())
		{
			throw std::runtime_error("too many parameters passed to subroutine");
		}
		m_paramvalues.push_back( p);
	}
	ValueTupleSetR getParameters() const
	{
		if (m_paramvalues.size() < m_paramnames.size())
		{
			throw std::runtime_error("too few parameters passed to subroutine");
		}
		ValueTupleSetR rt( new ValueTupleSet( m_paramnames));
		rt->push( m_paramvalues);
		return rt;
	}

private:
	std::vector<std::string> m_paramnames;
	std::vector<types::Variant> m_paramvalues;
};

}}}//namespace
#endif

