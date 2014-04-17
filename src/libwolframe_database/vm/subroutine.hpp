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
///\file subroutine.hpp
///\brief Local interface for mapping names to subroutine definitions
#ifndef _DATABASE_VM_SUBROUTINE_HPP_INCLUDED
#define _DATABASE_VM_SUBROUTINE_HPP_INCLUDED
#include "vm/program.hpp"
#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>

namespace _Wolframe {
namespace db {
namespace vm {

class Subroutine
{
public:
	Subroutine(){}
	Subroutine( const std::string& name_, const std::vector<std::string>& templateParams_, const std::vector<std::string>& params_, const ProgramR& program_)
		:m_name(name_),m_templateParams(templateParams_),m_params(params_),m_program(program_){}
	Subroutine( const std::string& name_, const std::vector<std::string>& params_, const ProgramR& program_)
		:m_name(name_),m_params(params_),m_program(program_){}
	Subroutine( const Subroutine& o)
		:m_name(o.m_name),m_templateParams(o.m_templateParams),m_params(o.m_params),m_program(o.m_program){}

	const std::string& name() const				{return m_name;}
	const std::vector<std::string>& templateParams() const	{return m_templateParams;}
	const std::vector<std::string>& params() const		{return m_params;}
	const ProgramR& program() const				{return m_program;}

	void substituteStatementTemplates( const std::vector<std::string>& templateParamValues);

private:
	std::string m_name;
	std::vector<std::string> m_templateParams;
	std::vector<std::string> m_params;
	ProgramR m_program;
};

}}}//namespace
#endif


