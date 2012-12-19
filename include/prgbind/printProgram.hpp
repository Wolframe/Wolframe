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
///\brief Interface for loading programs for printing
///\file prgbind/transactionProgram.hpp

#ifndef _PRGBIND_PRINT_PROGRAM_HPP_INCLUDED
#define _PRGBIND_PRINT_PROGRAM_HPP_INCLUDED
#include "prgbind/program.hpp"
#include "database/printProgram.hpp"
#include <string>

namespace _Wolframe {
namespace prgbind {

class PrintProgram
	:public Program
	,public prnt::PrintProgram
{
public:
	PrintProgram( const std::string& name_, const module::PrintFunctionConstructorR& contructor_);
	virtual ~PrintProgram(){}

	virtual bool is_mine( const std::string& filename) const;
	virtual void loadProgram( proc::ProcessorProvider& provider, const std::string& filename);

private:
	std::string m_name;
	module::PrintFunctionConstructorR m_contructor;
};

}}//namespace
#endif


