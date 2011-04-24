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
/// \file luatypes.hpp
/// \brief interface to lua application processor
///
#ifndef _Wolframe_LUATYPES_HPP_INCLUDED
#define _Wolframe_LUATYPES_HPP_INCLUDED
#include "langbind.hpp"
#include "luaconfig.hpp"
#include "appProcessor.hpp"

namespace _Wolframe {
namespace iproc {
namespace lua {

class AppProcessor :public app::AppProcessorBase
{
public:
	struct State;

	AppProcessor( app::System* system, const lua::Configuration& config, app::Input& input, app::Output& output);
	~AppProcessor();

	virtual CallResult call( unsigned int argc, const char** argv);
private:
	app::Input m_input;
	app::Output m_output;
	app::System* m_system;
	State* m_state;
};

}}}//namespace
#endif

