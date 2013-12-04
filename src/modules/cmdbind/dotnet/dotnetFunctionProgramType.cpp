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
///\file dotnetFunctionProgramType.cpp
///\brief Implementation of the function to create a form function program type object for calling .NET via COM interop
#include "dotnetFunctionProgramType.hpp"
#include "langbind/formFunction.hpp"
#include "comauto/function.hpp"
#include "processor/procProvider.hpp"
#include "utils/stringUtils.hpp"
#include "utils/parseUtils.hpp"
#include "logger-v1.hpp"
#include <boost/algorithm/string.hpp>

using namespace _Wolframe;
using namespace _Wolframe::langbind;

namespace {

class DotnetFormFunction
	:public langbind::FormFunction
{
public:
	DotnetFormFunction( const std::string& clrversion_, const std::string& programdef_, const std::string& functionname_)
		:m_clrversion(clrversion_),m_programdef(programdef_),m_functionname(functionname_){}

	virtual ~DotnetFormFunction(){}

	virtual langbind::FormFunctionClosure* createClosure() const
	{
		
	}

private:
	std::string m_clrversion;
	std::string m_programdef;
	std::string m_functionname;
};

static bool isAlpha( char ch)
{
	if (ch >= 'A' && ch <= 'Z') return true;
	if (ch >= 'a' && ch <= 'z') return true;
	if (ch == '_') return true;
	return false;
}

class DotnetProgramType
	:public prgbind::Program
{
public:
	DotnetProgramType()
		:prgbind::Program( prgbind::Program::Function){}

	virtual ~DotnetProgramType(){}

	struct ProgramDescription
	{
		std::string modulename;
		std::string clrversion;
		std::string def;

		ProgramDescription( const std::string& description)
		{
			std::vector<std::string> defs;
			splitString( defs, description, ",");
	
			std::vector<std::string>::const_iterator di = defs.begin(), de = defs.end()
			for (; di != de; ++di)
			{
				std::string::const_iterator ei = di->begin(), ee = di->end();
				std::string id;

				if (!isAlpha( utils::parseNextToken( id, ei, ee)))
				{
					throw std::runtime_error( std::string("identifier expected in assembly description element: '") + *di + "'");
				}
				char ch = utils::gotoNextToken( ei, ee);
				if (!ch)
				{
					if (def.empty())
					{
						modulename = id;
						def = id;
						continue;
					}
					else
					{
						throw std::runtime_error( std::string("module name must be the first element in the assembly description: '") + def + "' and '" + *di + "'");
					}
				}
				else if (ch != '=')
				{
					throw std::runtime_error( std::string("identifier expected in assembly description: '") + *di + "'");
				}
				++ei;
				if (boost::algorithm::iequals( id, "clrversion"))
				{
					if (!clrversion.empty())
					{
						throw std::runtime_error( std::string("duplicate definition of CLR version ('clrversion' attribute) in assembly description") + *di + "'");
					}
					clrversion = std::string( ei, ee);
				}
				else
				{
					if (def.empty())
					{
						throw std::runtime_error( std::string("module name missing at start of assembly description: '") + *di + "'");
					}
					def.append( ", ");
					def.append( id);
					def.append( "=");
					def.append( ei, ee);
				}
			}
			if (tlbpath.empty())
			{
				throw std::runtime_error( "missing 'typelib' declaration with the path of the type library in the assembly description");
			}
			if (clrversion.empty())
			{
				throw std::runtime_error( "missing 'clrversion' declaration with the version of the CLR to load for the function execution");
			}
		}
		ProgramDescription( const ProgramDescription& o)
			:modulename(o.modulename),clrversion(o.clrversion),def(o.def){}
	};

	virtual bool is_mine( const std::string& description) const
	{
		bool rt = false;
		std::vector<std::string> defs;
		splitString( defs, description, ",");

		std::vector<std::string>::const_iterator di = defs.begin(), de = defs.end()
		for (; di != de; ++di)
		{
			std::string::const_iterator ei = di->begin(), ee = di->end();
			std::string id;

			if (!isAlpha( utils::parseNextToken( id, ei, ee)))
			{
				return false;
			}
			if (utils::gotoNextToken( ei, ee) == '='
			&&  boost::algorithm::iequals( id, "clrversion"))
			{
				rt = true;
			}
		}
		return rt;
	}

	virtual void loadProgram( prgbind::ProgramLibrary& library, db::Database* /*transactionDB*/, const std::string& description)
	{
		ProgramDescription pd( description);
		std::string assembly( "Functions, Version=1.0.0.30, Culture=neutral, PublicKeyToken=1c1d731dc6e1cbe1, processorArchitecture=MSIL");
		comauto::TypeLib typelib( pd.tlbpath);

		if (_Wolframe::log::LogBackend::instance().minLogLevel() == _Wolframe::log::LogLevel::LOGLEVEL_DEBUG)
		{
			std::ostringstream typelibtxt;
			typelib.print( typelibtxt);
			LOG_DEBUG << "[.NET load program] " << typelibtxt.str();
		}
		comauto::CommonLanguageRuntime clr( "v4.0.30319");

		
		std::vector<std::string> funcs = m_context.loadProgram( filename);
		std::vector<std::string>::const_iterator fi = funcs.begin(), fe = funcs.end();
		for (; fi != fe; ++fi)
		{
			langbind::FormFunctionR ff( new DotnetFormFunction( &m_context, *fi));
			library.defineFormFunction( *fi, ff);
		}
	}
};
}//anonymous namespace

prgbind::Program* langbind::createDotnetProgramType()
{
	return new DotnetProgramType();
}


