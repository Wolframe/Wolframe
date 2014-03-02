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
//\file langbind/runtimeEnvironment.hpp
//\brief Definition of language runtime environment
#ifndef _Wolframe_LANGBIND_RUNTIME_ENVIRONMENT_HPP_INCLUDED
#define _Wolframe_LANGBIND_RUNTIME_ENVIRONMENT_HPP_INCLUDED
#include "langbind/formFunction.hpp"
#include "processor/procProviderInterface.hpp"
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace langbind {

//\class RuntimeEnvironment
//\brief Runtime environment for functions that need it. (for example as structure hosting the CLR for .NET function calls)
class RuntimeEnvironment
{
public:
	//\brief Destructor
	virtual ~RuntimeEnvironment(){}
	//\brief createClosure create a closure to execute function with name 'funcname'
	//\param [in] funcname function name
	virtual FormFunctionClosure* createClosure( const std::string& funcname) const=0;

	//\brief Get the names of the functions implemented
	//\return the function names
	virtual std::vector<std::string> functions() const=0;

	virtual const char* name() const=0;
};

//\brief Shared ownership reference to a runtime environment
typedef boost::shared_ptr<RuntimeEnvironment> RuntimeEnvironmentR;


//\class RuntimeEnvironmentFormFunction
//\brief Form function that needs an execution environment
class RuntimeEnvironmentFormFunction
	:public FormFunction
{
public:
	//\brief Constructor
	//\param[in] funcname_ name of the function
	//\param[in] env_ runtime environment of the function
	RuntimeEnvironmentFormFunction( const std::string& funcname_, const RuntimeEnvironment* env_)
		:m_env(env_),m_funcname(funcname_){}

	//\brief Create a function closure for execution with help of the runtime environment
	//\return created closure
	virtual langbind::FormFunctionClosure* createClosure() const
	{
		FormFunctionClosure* rt = m_env->createClosure( m_funcname);
		return rt;
	}

private:
	const RuntimeEnvironment* m_env;	//< runtime environment reference
	std::string m_funcname;			//< name of the function
};

}}//namespace
#endif

