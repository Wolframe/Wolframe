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
///\file serialize/struct/filtermapBase.cpp
///\brief Implements the non intrusive base class of serialization/deserialization of filters
#include "serialize/struct/filtermapBase.hpp"
#include <cstring>
#include <stdexcept>

using namespace _Wolframe;
using namespace serialize;

bool FiltermapDescriptionBase::parse( void* obj, langbind::BufferingInputFilter& in, Context& ctx) const
{
	bool rt = true;
	try
	{
		if (!m_parse) throw std::runtime_error( "null parser called");
		rt = m_parse( obj, in, ctx, false);
	}
	catch (std::exception& e)
	{
		ctx.setError( e.what());
		return false;
	}
	return rt;
}

bool FiltermapDescriptionBase::print( const void* obj, langbind::OutputFilter& out, Context& ctx) const
{
	bool rt = true;
	try
	{
		if (!m_print) throw std::runtime_error( "null printer called");
		char buf[ 4096];
		out.setOutputBuffer( buf, sizeof( buf));
		rt = m_print( 0, obj, out, ctx);
	}
	catch (std::exception& e)
	{
		ctx.setError( e.what());
		rt = false;
	}
	return rt;
}

