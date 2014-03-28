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
//\file database/vm/subroutine.hpp
//\brief Interface for mapping names to subroutine definitions
#ifndef _DATABASE_VIRTUAL_MACHINE_SUBROUTINE_HPP_INCLUDED
#define _DATABASE_VIRTUAL_MACHINE_SUBROUTINE_HPP_INCLUDED
#include "database/virtualMachine.hpp"
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
	Subroutine( const std::string& name_, const std::vector<std::string>& templateParams_, const std::vector<std::string>& params_, const VirtualMachineR& vm_)
		:m_name(name_),m_templateParams(templateParams_),m_params(params_),m_vm(vm_){}
	Subroutine( const std::string& name_, const std::vector<std::string>& params_, const VirtualMachineR& vm_)
		:m_name(name_),m_params(params_),m_vm(vm_){}
	Subroutine( const Subroutine& o)
		:m_name(o.m_name),m_templateParams(o.m_templateParams),m_params(o.m_params),m_vm(o.m_vm){}

	void setName( const std::string& name_)			{m_name = name_;}

	const std::string& name() const				{return m_name;}
	const std::vector<std::string>& templateParams() const	{return m_params;}
	const std::vector<std::string>& params() const		{return m_params;}
	const VirtualMachineR& vmref() const			{return m_vm;}

	void substituteStatementTemplates( const std::vector<std::string>& templateParamValues);

private:
	std::string m_name;
	std::vector<std::string> m_templateParams;
	std::vector<std::string> m_params;
	VirtualMachineR m_vm;
};

}}}//namespace
#endif


