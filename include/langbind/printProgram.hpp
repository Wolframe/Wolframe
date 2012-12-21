/************************************************************************

 Copyright (C) 2011, 2012 Project Wolframe.
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
///\brief Interface for programs for printing
///\file langbind/printProgram.hpp
#ifndef _LANGBIND_PRINT_PROGRAM_HPP_INCLUDED
#define _LANGBIND_PRINT_PROGRAM_HPP_INCLUDED
#include "types/keymap.hpp"
#include "module/printFunctionBuilder.hpp"
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace langbind {

class PrintProgram
	:public types::keymap<prnt::PrintFunctionR>
{
public:
	typedef types::keymap<prnt::PrintFunctionR> Parent;

	PrintProgram(){}
	PrintProgram( const PrintProgram& o)
		:types::keymap<prnt::PrintFunctionR>(o)
		,m_constructormap(o.m_constructormap){}
	~PrintProgram(){}

	void addConstructor( const module::PrintFunctionConstructorR& c);

	bool is_mine( const std::string& filename) const;
	void loadProgram( const std::string& filename);

	const prnt::PrintFunction* get( const std::string& name) const
	{
		types::keymap<prnt::PrintFunctionR>::const_iterator rt = Parent::find( name);
		return (rt == Parent::end())?0:rt->second.get();
	}

private:
	types::keymap<module::PrintFunctionConstructorR> m_constructormap;
};

}}
#endif


