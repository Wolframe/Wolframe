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
/// \file standardAuthenticator.cpp
/// \brief top-level header file for authentication interface
#include "AAAA/standardAuthenticator.hpp"
#include <string>

using namespace _Wolframe;
using namespace _Wolframe::AAAA;

// Standard authenticator
StandardAuthenticator::StandardAuthenticator()
{
}

StandardAuthenticator::~StandardAuthenticator()
{
}

std::vector<std::string> StandardAuthenticator::mechs() const
{
	std::vector<std::string> rt;
	rt.push_back( "NONE");
	return rt;
}

void StandardAuthenticator::chooseMech( const std::string& /*mech*/) const
{
}

void StandardAuthenticator::close()
{
	delete this;
}

// The FSM interface functions
// The input data.
void StandardAuthenticator::receiveData( const void* /*data*/, std::size_t /*size*/ )
{
}

// signal the FSM
void StandardAuthenticator::signal( FSM::Signal /*event*/ )
{
}

// Data not consumed.
std::size_t StandardAuthenticator::dataLeft( const void*& /*begin*/ )
{
	return 0;
}
