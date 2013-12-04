/************************************************************************
Copyright (C) 2011 - 2013 Project Wolframe.
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
///\file dotnetRuntimeEnvironmentBuilder.hpp
///\brief Interface .NET runtime environment builder
#ifndef _Wolframe_DOTNET_RUNTIME_ENVIRONMENT_BUILDER_HPP_INCLUDED
#define _Wolframe_DOTNET_RUNTIME_ENVIRONMENT_BUILDER_HPP_INCLUDED
#include "prgbind/runtimeEnvironmentConstructor.hpp"
#include "langbind/formFunction.hpp"
#include "dotnetRuntimeEnvironmentConfig.hpp"
#include "dotnetRuntimeEnvironment.hpp"
#include "processor/moduleInterface.hpp"
#include "module/constructor.hpp"
#include <string>
#include <map>

namespace _Wolframe {
namespace module {

class DotnetRuntimeEnvironmentConstructor
	:public prgbind::RuntimeEnvironmentConstructor
{
public:
	DotnetRuntimeEnvironmentConstructor(){}

	virtual ~DotnetRuntimeEnvironmentConstructor(){}

	virtual DotnetRuntimeEnvironment* object( const config::NamedConfiguration& cfgi)
	{
		const DotnetRuntimeEnvironmentConfig* cfg = dynamic_cast<const DotnetRuntimeEnvironmentConfig*>(&cfgi);
		if (!cfg) throw std::logic_error( "internal: wrong configuration interface passed to runtime environment constructor");
		DotnetRuntimeEnvironment* rt = new DotnetRuntimeEnvironment( cfg);
		return rt;
	}

	virtual const char* objectClassName() const
	{
		return "DotnetRuntimeEnvironment";
	}

	virtual bool checkReferences( const config::NamedConfiguration&, const proc::ProcessorProvider*) const
	{
		return true;
	}
};


class DotnetRuntimeEnvironmentBuilder
	:public ConfiguredBuilder
{
public:
	DotnetRuntimeEnvironmentBuilder( const char* classname_, const char* title, const char* section, const char* keyword, const char* id);

	virtual ~DotnetRuntimeEnvironmentBuilder();

	virtual ObjectConstructorBase::ObjectType objectType() const
	{
		return ObjectConstructorBase::RUNTIME_ENVIRONMENT_OBJECT;
	}

	virtual config::NamedConfiguration* configuration( const char* logPrefix)
	{
		return new DotnetRuntimeEnvironmentConfig( m_className, m_title, logPrefix, m_keyword);
	}

	virtual ObjectConstructorBase* constructor()
	{
		return new DotnetRuntimeEnvironmentConstructor();
	}
};

}}//namespace

#endif

