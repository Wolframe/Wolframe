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
///\file appdevel/module/runtimeEnvironmentConstructor.hpp
///\brief Interface to constructors of runtime environment for executing functions that need it
#ifndef _Wolframe_MODULE_RUNTIME_ENVIRONMENT_CONSTRUCTOR_HPP_INCLUDED
#define _Wolframe_MODULE_RUNTIME_ENVIRONMENT_CONSTRUCTOR_HPP_INCLUDED
#include "module/constructor.hpp"
#include "langbind/runtimeEnvironment.hpp"
#include "module/moduleInterface.hpp"
#include <string>
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace proc {
	///\brief Forward declaration
	class ProcessorProvider;
}
namespace module {

///\class RuntimeEnvironmentConstructor
///\brief Constructor of a runtime environment for executing functions
class RuntimeEnvironmentConstructor
	:public ConfiguredObjectConstructor<langbind::RuntimeEnvironment>
{
public:
	///\brief Destructor
	virtual ~RuntimeEnvironmentConstructor(){}

	///\brief Get the module object type
	///\return the object type
	virtual ObjectConstructorBase::ObjectType objectType() const
	{
		return ObjectConstructorBase::RUNTIME_ENVIRONMENT_OBJECT;
	}

	virtual const char* objectClassName() const
	{
		return "RuntimeEnvironment";
	}
};

typedef boost::shared_ptr<RuntimeEnvironmentConstructor> RuntimeEnvironmentConstructorR;


}} //namespace

#endif
