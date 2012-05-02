/************************************************************************
Copyright (C) 2011 Project Wolframe.
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
///\file langbind/appGlobalContext.hpp
///\brief interface for application processor scripting language to all system object instances (global context)

#ifndef _Wolframe_langbind_APP_GLOBAL_CONTEXT_HPP_INCLUDED
#define _Wolframe_langbind_APP_GLOBAL_CONTEXT_HPP_INCLUDED
#include "langbind/appObjects.hpp"
#include "langbind/appConfig.hpp"

namespace _Wolframe {
namespace langbind {

///\class GlobalContext
///\brief Reference to all available processing resources seen from scripting language binding
class GlobalContext
	:public FilterMap
	,public DDLFormMap
	,public TransactionFunctionMap
	,public DDLCompilerMap
	,public PluginFunctionMap
	,public LuaFunctionMap
	,public LuaPluginFunctionMap
{
public:
	GlobalContext(){}
	bool load( const ApplicationEnvironmentConfig& config);


	///\brief Get the TransactionFunction and its input and output specification by its composite name (function:paramform:resultform)
	///\param[in] name composite transaction function name
	///\param[out] tf returned transaction function
	///\param[out] par returned transaction function parameters form
	///\param[out] res returned transaction function result form
	bool getTransactionFunctionElements( const std::string& name, TransactionFunction& rt, DDLFormR& param, DDLFormR& result);
};
typedef CountedReference<GlobalContext> GlobalContextR;


void defineGlobalContext( const GlobalContextR& context);
GlobalContext* getGlobalContext();

}}//namespace
#endif

