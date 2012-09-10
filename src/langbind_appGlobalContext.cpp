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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
///\file langbind_appObjects.cpp
///\brief implementation of scripting global context
#include "langbind/appGlobalContext.hpp"
#include "utils/miscUtils.hpp"
#include "utils/doctype.hpp"
#include "logger-v1.hpp"
#include <sstream>
#include <boost/algorithm/string.hpp>

using namespace _Wolframe;
using namespace _Wolframe::langbind;

static GlobalContextR g_context;

void _Wolframe::langbind::defineGlobalContext( const GlobalContextR& context)
{
	g_context = context;
}

GlobalContext* _Wolframe::langbind::getGlobalContext()
{
	GlobalContext* gct = g_context.get();
	if (!gct) throw std::runtime_error( "global context not defined");
	return gct;
}

bool GlobalContext::loadScript( const ScriptCommandConfigStruct& config, std::string& itemname, std::string& error)
{
	try
	{
		itemname.clear();
		std::string type( config.type);
		if (type.empty())
		{
			std::string ext = utils::getFileExtension( config.sourcepath);
			if (!ext.size())
			{
				error = "no type of script specified (file extension missing)";
				return false;
			}
			type = std::string( ext.c_str()+1);
		}

		std::string name( config.name);
		if (boost::algorithm::iequals( type, "lua"))
		{
#if WITH_LUA
			langbind::LuaScript script( config.sourcepath);
			if (name.empty())
			{
				if (script.functions().empty())
				{
					error = "no functions defined in script";
					return false;
				}
				else
				{
					// no function name specified, then the last function in the script is the name of the exported function
					name = script.functions().back();
				}
			}
			defineLuaFunction( itemname = name, script);
			return true;
#else
			error = "cannot load script (lua script support not built in)";
			return false;
#endif
		}
		else
		{
			error = "cannot load script (unknown type of script)";
			return false;
		}
		return true;
	}
	catch (std::runtime_error& e)
	{
		error = e.what();
		return false;
	}
}

bool GlobalContext::loadPrintLayout( const PrintLayoutConfigStruct& config, std::string& itemname, std::string& error)
{
	try
	{
		itemname.clear();
		std::string type( config.type);
		if (type.empty())
		{
			std::string ext = utils::getFileExtension( config.sourcepath);
			if (!ext.size())
			{
				error = "no type of print layout specified (file extension missing) for print layout file";
				return false;
			}
			type = std::string( ext.c_str()+1);
		}
		prnt::CreatePrintFunction pf;
		if (!getPrintFunctionType( type, pf))
		{
			error = std::string( "Unknown print layout description type '") + type + "'";
			return false;
		}
		else
		{
			prnt::PrintFunctionR func;
			try
			{
				func.reset( pf( utils::readSourceFileContent( config.sourcepath)));
			}
			catch (const std::exception& e)
			{
				std::ostringstream msg;
				msg << "could not compile file '" << config.sourcepath << "': " << e.what() << std::endl;
				throw std::runtime_error( msg.str());
			}
			std::string name( config.name);
			if (name.empty())
			{
				name = utils::getFileStem( config.sourcepath);
				if (name.empty())
				{
					error = "no name defined for print layout";
					return false;
				}
			}
			definePrintFunction( itemname = name, func);
		}
		return true;
	}
	catch (const std::runtime_error& e)
	{
		error = std::string( "error in print layout definition: ") + e.what();
		return false;
	}
}

bool GlobalContext::loadDDLForm( const DDLFormConfigStruct& config, std::string& itemname, std::string& error)
{
	try
	{
		itemname.clear();
		std::string DDL( config.DDL);
		if (DDL.empty())
		{
			std::string ext = utils::getFileExtension( config.sourcepath);
			if (!ext.size())
			{
				error = "no DDL specified (file extension missing) for form file";
				return false;
			}
			DDL = std::string( ext.c_str()+1);
		}
		ddl::DDLCompilerR ci;
		if (!getDDLCompiler( DDL, ci))
		{
			error = std::string( "Unknown DDL of form '") + DDL + "'";
			return false;
		}
		else
		{
			ddl::StructTypeR form = ddl::StructTypeR( new ddl::StructType());
			try
			{
				*form = ci->compile( utils::readSourceFileContent( config.sourcepath));
			}
			catch (const std::exception& e)
			{
				std::ostringstream msg;
				msg << "could not compile file '" << config.sourcepath << "': " << e.what() << std::endl;
				throw std::runtime_error( msg.str());
			}

			std::string name;
			if (form->doctype())
			{
				name = utils::getIdFromDoctype( form->doctype());
			}
			else
			{
				name = utils::getFileStem( config.sourcepath);
			}
			defineForm( itemname = name, form);
		}
		return true;
	}
	catch (std::runtime_error& e)
	{
		error = std::string( "error in form data definition source: ") + e.what();
		return false;
	}
}

bool GlobalContext::loadTransactionFunction( const TransactionFunctionConfigStruct& config, std::string& itemname, std::string& error)
{
	try
	{
		itemname.clear();
		CreateTransactionFunction creatf;
		if (!getTransactionFunctionType( config.type, creatf))
		{
			error = std::string("unknown transaction function type '") + config.type + "'";
			return false;
		}

		defineTransactionFunction( itemname = config.name, TransactionFunctionR( creatf( config.call)));
		return true;
	}
	catch (std::runtime_error& e)
	{
		error = e.what();
		return false;
	}
}

bool GlobalContext::load( const EnvironmentConfigStruct& config)
{
	bool rt = true;
	std::map<std::string,std::string> functions;
	std::string itemname;
	std::string error;
	{
		std::vector<DDLFormConfigStruct>::const_iterator itr=config.form.begin(),end=config.form.end();
		for (;itr!=end; ++itr)
		{
			if (!loadDDLForm( *itr, itemname, error))
			{
				LOG_ERROR << "Cannot load DDL form '" << itemname << " from '" << itr->sourcepath << "' (" << error << ")";
				rt = false;
			}
			else if (functions.find( itemname) != functions.end())
			{
				LOG_ERROR << "Name clash with " << functions[ itemname] << " loading DDL form '" << itemname << " from '" << itr->sourcepath << "' (" << error << ")";
				rt = false;
			}
			else
			{
				functions[ itemname] = std::string( "form definition '") + itemname + "' in '" + itr->sourcepath + "'";
				LOG_DEBUG << "Loaded DDL form '" << itemname << "'";
			}
		}
	}
	{
		std::vector<PrintLayoutConfigStruct>::const_iterator itr=config.printlayout.begin(),end=config.printlayout.end();
		for (;itr!=end; ++itr)
		{
			if (!loadPrintLayout( *itr, itemname, error))
			{
				LOG_ERROR << "Cannot load print layout '" << itemname << " from '" << itr->sourcepath << "' (" << error << ")";
				rt = false;
			}
			else if (functions.find( itemname) != functions.end())
			{
				LOG_ERROR << "Name clash with " << functions[ itemname] << " loading print layout '" << itemname << " from '" << itr->sourcepath << "' (" << error << ")";
				rt = false;
			}
			else
			{
				functions[ itemname] = std::string( "print layout '") + itemname + "' in '" + itr->sourcepath + "'";
				LOG_DEBUG << "Loaded print layout '" << itemname << "'";
			}
		}
	}
	{
		std::vector<TransactionFunctionConfigStruct>::const_iterator itr=config.transaction.begin(),end=config.transaction.end();
		for (;itr!=end; ++itr)
		{
			if (!loadTransactionFunction( *itr, itemname, error))
			{
				LOG_ERROR << "Cannot load transaction function '" << itemname << "' (" << error << ")";;
				rt = false;
			}
			else if (functions.find( itemname) != functions.end())
			{
				LOG_ERROR << "Name clash with " << functions[ itemname] << " loading transaction function '" << itemname << "' (" << error << ")";;
				rt = false;
			}
			else
			{
				functions[ itemname] = std::string( "transaction function '") + itemname + "'";
				LOG_DEBUG << "Loaded transaction function '" << itemname << "'";
			}
		}
	}
	{
		std::vector<ScriptCommandConfigStruct>::const_iterator itr=config.script.begin(),end=config.script.end();
		for (;itr!=end; ++itr)
		{
			if (!loadScript( *itr, itemname, error))
			{
				LOG_ERROR << "Cannot load script function '" << itemname << "' from '"<< itr->sourcepath << "' (" << error << ")";
				rt = false;
			}
			else if (functions.find( itemname) != functions.end())
			{
				LOG_ERROR << "Name clash with " << functions[ itemname] << " loading script function '" << itemname << "' from '"<< itr->sourcepath << "' (" << error << ")";
				rt = false;
			}
			else
			{
				functions[ itemname] = std::string( "script function '") + itemname + "' in '" + itr->sourcepath + "'";
				LOG_DEBUG << "Loaded script function '" << itemname << "'";
			}
		}
	}
	return rt;
}

bool GlobalContext::getFilter( const std::string& name, const std::string& arg, Filter& fl) const
{
	Filter* fp = m_provider->filter( name, arg);
	if (!fp)
	{
		bool rt = FilterMap::getFilter( name, arg, fl);
		return rt;
	}
	else
	{
		fl = *fp;
		delete fp;
		return true;
	}
}

bool GlobalContext::getFormFunction( const std::string& name, FormFunction& func) const
{
	FormFunction* fp = m_provider->formfunction( name);
	if (!fp)
	{
		bool rt = FormFunctionMap::getFormFunction( name, func);
		return rt;
	}
	else
	{
		func = *fp;
		delete fp;
		return true;
	}
}

bool GlobalContext::getDDLCompiler( const std::string& name, ddl::DDLCompilerR& dc) const
{
	ddl::DDLCompiler* fp = m_provider->ddlcompiler( name);
	if (!fp)
	{
		bool rt = DDLCompilerMap::getDDLCompiler( name, dc);
		return rt;
	}
	else
	{
		dc.reset( fp);
		return true;
	}
}

