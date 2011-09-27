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
///
///\file luaAppProcessor.hpp
///\brief interface to the lua application processor
///
#ifndef _Wolframe_LUATYPES_HPP_INCLUDED
#define _Wolframe_LUATYPES_HPP_INCLUDED
#include "luaConfig.hpp"
#include "appObjects.hpp"
#include "appProcessor.hpp"
extern "C"
{
#include "lua.h"
}

namespace _Wolframe {
namespace iproc {
namespace lua {

///\class AppProcessor
///\brief application processor instance for processing calls as Lua script
class AppProcessor :public app::AppProcessorBase
{
public:
	///\class State
	///\brief State of the application processor instance
	struct State;

	///\brief Constructor
	///\param[in] config read only reference to the configuration of this application processor
	AppProcessor( const lua::Configuration* config);
	///\brief Destructor
	~AppProcessor();

	///\brief Get command implemenation (see app::AppProcessorBase::getCommand(const char*,const char*&,bool&)const)
	///\param[in] protocolCmd protocol command
	///\param[out] functionName associated Lua script function name
	///\param[out] hasIO true if associated Lua script function is processing content from network I/O
	///\return true, if the command is defined in configuration
	virtual bool getCommand( const char* protocolCmd, const char*& functionName, bool& hasIO) const
	{
		functionName = m_config->scriptFunctionName( protocolCmd);
		hasIO = m_config->scriptFunctionHasIO( protocolCmd);
		return (functionName != 0);
	}

	///\brief Get command implemenation (see app::AppProcessorBase::setIO( const InputFilterR&, const FormatOutputR&)
	///\param[in] in input filter reference
	///\param[in] out format output reference
	virtual void setIO( const protocol::InputFilterR& in, const protocol::FormatOutputR& out)
	{
		m_input.m_inputfilter = in;
		m_output.m_formatoutput = out;
	}

	///\brief Execute the Lua script (see app::AppProcessorBase::call(unsigned int, const char**,bool))
	///\param[in] argc number of arguments
	///\param[in] argc array of arguments
	///\return call state
	virtual CallResult call( unsigned int argc, const char** argv);

	///\brief Get the current lua state (not the thread!)
	///\return the current lua state
	lua_State* getLuaState() const;

private:
	const lua::Configuration* m_config;	///< reference to configuration
	app::Input m_input;			///< input
	app::Output m_output;			///< output
	State* m_state;				///< application procesor instance state
};

}}}//namespace
#endif

