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
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
//\file luaDump.hpp
//\brief Implementation of a dumper/loader of untouched (just loaded without execution) lua state based on lua_dump/lua_load
#include "luaDump.hpp"
#include <cstdlib>
#include <cstring>
#include <stdexcept>

extern "C" {
#include "lualib.h"
#include "lauxlib.h"
#include "lua.h"
}

namespace _Wolframe {
namespace langbind {

class LuaDump
{
public:
	LuaDump()
		:m_pos(0),m_size(0),m_ar(0){}
	~LuaDump()
	{
		if (m_ar) std::free( m_ar);
	}

	//\brief Append 'addsize' bytes starting at 'addptr' to buffer
	bool write( const void* addptr, std::size_t addsize)
	{
		enum {StartSize=1024};
		if (addsize > m_size - m_pos)
		{
			std::size_t mm = (m_size)?(m_size*2):(std::size_t)StartSize;
			while (mm < (addsize + m_pos) && mm >= StartSize)
			{
				mm *= 2;
			}
			if (mm < StartSize) return false;

			void* newar = std::realloc( m_ar, mm);
			if (!newar) return false;

			m_ar = newar;
			m_size = mm;
		}
		std::memcpy( (char*)m_ar + m_pos, addptr, addsize);
		m_pos += addsize;
		return true;
	}

	//\brief Adjust size of buffer to keep allocated only the number of bytes written
	void close()
	{
		if (m_pos)
		{
			void* newar = std::realloc( m_ar, m_pos?m_pos:1);
			if (newar)
			{
				m_ar = newar;
				m_size = m_pos;
			}
		}
		else
		{
			std::free( m_ar);
			m_ar = 0;
			m_size = 0;
		}
	}

	const char* baseptr() const
	{
		return (const char*)m_ar;
	}

	std::size_t size() const
	{
		return m_size;
	}

private:
	std::size_t m_pos;
	std::size_t m_size;
	void* m_ar;
};
}}

using namespace _Wolframe;
using namespace _Wolframe::langbind;

namespace {
class LuaDumpReader
{
public:
	explicit LuaDumpReader( const LuaDump* d)
		:m_dump(d),m_consumed(false){}
	~LuaDumpReader()
	{}

	const char* read( std::size_t* size)
	{
		if (m_consumed)
		{
			*size = 0;		//... terminated
		}
		else
		{
			*size = m_dump->size();
			m_consumed = true;	//... signal termination
		}
		return m_dump->baseptr();
	}

private:
	const LuaDump* m_dump;
	bool m_consumed;
};
}//anonymous namespace


static int luaDumpWriter( lua_State*, const void* p, size_t sz, void* ud)
{
	langbind::LuaDump* dump = reinterpret_cast<langbind::LuaDump*>(ud);
	return dump->write( p, sz)?0:1;
}

static const char* luaDumpReader( lua_State*, void* ud, size_t* size)
{
	LuaDumpReader* dump = reinterpret_cast<LuaDumpReader*>(ud);
	return dump->read( size);
}

LuaDump* langbind::luaCreateDump( lua_State *ls)
{
	LuaDump* dump = new LuaDump();
	lua_dump( ls, luaDumpWriter, (void*)dump);
	dump->close();
	return dump;
}

void langbind::luaLoadDump( lua_State *ls, const LuaDump* dump)
{
	LuaDumpReader reader( dump);
	int res = lua_load( ls, luaDumpReader, (void*)&reader, dump->baseptr(), "b");
	switch (res)
	{
		case LUA_OK: break;
		case LUA_ERRSYNTAX: throw std::runtime_error("Lua syntax error during loading of chunk");
		case LUA_ERRGCMM: throw std::runtime_error("internal Lua error during loading of chunk (LUA_ERRGCMM)");
		case LUA_ERRMEM: throw std::bad_alloc();
	}
}

void langbind::freeLuaDump( LuaDump* d)
{
	delete d;
}

