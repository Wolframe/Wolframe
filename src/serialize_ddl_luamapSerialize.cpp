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
#include "serialize/ddl/luamapSerialize.hpp"

using namespace _Wolframe;
using namespace serialize;

// forward declaration
static bool parseObject( ddl::StructType& st, lua_State* ls, Context& ctx);
static bool printObject( const ddl::StructType& st, lua_State* ls, Context& ctx);

static bool parseAtom( ddl::AtomicType& val, lua_State* ls, Context& ctx)
{
	bool rt = true;
	try
	{
		switch (lua_type( ls, -1))
		{
			case LUA_TNIL:
				val.init();
				break;
			break;
			case LUA_TNUMBER:
				if (val.set( (double)lua_tonumber(ls,-1)))
				{
					ctx.setError( "illegal value for number");
					return false;
				}
			break;
			case LUA_TBOOLEAN:
				if (val.set( lua_toboolean(ls,-1)?1:0))
				{
					ctx.setError( "illegal value for boolean");
					return false;
				}
			break;
			case LUA_TSTRING:
				if (val.set( std::string( lua_tostring(ls,-1))))
				{
					ctx.setError( "illegal value for string");
					return false;
				}
			break;
			case LUA_TTABLE: ctx.setError( "arithmetic value expected instead of table"); rt = false; break;
			case LUA_TFUNCTION: ctx.setError( "arithmetic value expected instead of function"); rt = false; break;
			case LUA_TUSERDATA: ctx.setError( "arithmetic value expected instead of userdata"); rt = false; break;
			case LUA_TTHREAD: ctx.setError( "arithmetic value expected instead of thread"); rt = false; break;
			case LUA_TLIGHTUSERDATA: ctx.setError( "arithmetic value expected instead of lightuserdata"); rt = false; break;
		}
	}
	catch (const std::exception& e)
	{
		ctx.setError( e.what());
		rt = false;
	}
	return rt;
}

static bool parseStruct( ddl::StructType& st, lua_State* ls, Context& ctx)
{
	if (!lua_istable( ls, -1))
	{
		ctx.setError( "table expected for structure");
		return false;
	}
	lua_pushnil( ls);
	while (lua_next( ls, -2))
	{
		lua_pushvalue( ls, -2);
		const char* key = lua_tostring( ls, -1);
		if (!key)
		{
			ctx.setError( "string expected as key for struct in table instead of ", lua_typename( ls, lua_type( ls, -1)));
			return false;
		}
		ddl::StructType::Map::iterator itr = st.find( key);

		if (itr == st.end())
		{
			ctx.setError( "element not defined ", key);
			return false;
		}
		lua_pop( ls, 1);

		if (!parseObject( itr->second, ls, ctx))
		{
			ctx.setTag( itr->first.c_str());
			return false;
		}
		lua_pop( ls, 1);
	}
	return true;
}

static bool parseVector( ddl::StructType& st, lua_State* ls, Context& ctx)
{
	std::size_t index = 0;
	if (!lua_istable( ls, -1))
	{
		/// ... If the element parsed as a vector is not a table,
		/// ... then we assume it to be the single atomic
		/// ... element of the vector parsed. If this hypothesis
		/// ... works, everything is fine. If not we have to give
		/// ... up with an error.
		st.push();
		if (!parseObject( st.back(), ls, ctx))
		{
			ctx.setError( "table expected for vector or one vector element");
			return false;
		}
	}
	else
	{
		lua_pushnil( ls);
		while (lua_next( ls, -2))
		{
			if (lua_type( ls,-2) != LUA_TNUMBER)
			{
				if (index++ == 0)
				{
					ctx.setError( "parsed table is not a vector");
					return false;
				}
				else
				{
					lua_pop( ls, 2);
					lua_pushvalue( ls, -3);
					st.push();
					if (!parseObject( st.back(), ls, ctx))
					{
						ctx.setError( "table expected for vector or one vector element");
						return false;
					}
					lua_pop( ls, 3);
					return true;
				}
			}
			st.push();
			if (!parseObject( st.back(), ls, ctx))
			{
				return false;
			}
			lua_pop( ls, 1);
		}
	}
	return true;
}

static bool parseObject( ddl::StructType& st, lua_State* ls, Context& ctx)
{
	bool rt = false;
	switch (st.contentType())
	{
		case ddl::StructType::Atomic:
			rt = parseAtom( st.value(), ls, ctx);

		case ddl::StructType::Vector:
			rt = parseVector( st, ls, ctx);

		case ddl::StructType::Struct:
			rt = parseStruct( st, ls, ctx);
	}
	return rt;
}

static bool printAtom( const ddl::StructType& st, lua_State* ls, Context& ctx)
{
	const ddl::AtomicType& atm = st.value();
	switch (atm.type())
	{
		case ddl::AtomicType::double_:
		{
			double val;
			if (!atm.get( val))
			{
				ctx.setError( "could not convert value to double");
				return false;
			}
			lua_pushnumber( ls, val);
			break;
		}
		case ddl::AtomicType::float_:
		{
			float val;
			if (!atm.get( val))
			{
				ctx.setError( "could not convert value to float");
				return false;
			}
			lua_pushnumber( ls, val);
			break;
		}
		case ddl::AtomicType::bigint_:
		{
			if (!atm.get( ctx.content()))
			{
				ctx.setError( "could not convert value to bigint");
				return false;
			}
			lua_pushlstring( ls, ctx.content().c_str(), ctx.content().size());
			break;
		}
		case ddl::AtomicType::int_:
		{
			int val;
			if (!atm.get( val))
			{
				ctx.setError( "could not convert value to int");
				return false;
			}
			lua_pushnumber( ls, val);
			break;
		}
		case ddl::AtomicType::uint_:
		{
			unsigned int val;
			if (!atm.get( val))
			{
				ctx.setError( "could not convert value to unsigned int");
				return false;
			}
			lua_pushnumber( ls, val);
			break;
		}
		case ddl::AtomicType::short_:
		{
			short val;
			if (!atm.get( val))
			{
				ctx.setError( "could not convert value to short");
				return false;
			}
			lua_pushnumber( ls, val);
			break;
		}
		case ddl::AtomicType::ushort_:
		{
			unsigned short val;
			if (!atm.get( val))
			{
				ctx.setError( "could not convert value to unsigned short");
				return false;
			}
			lua_pushnumber( ls, val);
			break;
		}
		case ddl::AtomicType::char_:
		{
			char val;
			if (!atm.get( val))
			{
				ctx.setError( "could not convert value to char");
				return false;
			}
			lua_pushlstring( ls, &val, 1);
			break;
		}
		case ddl::AtomicType::string_:
		{
			std::string val;
			if (!atm.get( val))
			{
				ctx.setError( "could not convert value to string");
				return false;
			}
			lua_pushlstring( ls, val.c_str(), val.size());
			break;
		}
	}
	return true;
}

static bool printStruct( const ddl::StructType& st, lua_State* ls, Context& ctx)
{
	lua_newtable( ls);
	ddl::StructType::Map::const_iterator itr = st.begin();
	while (itr != st.end())
	{
		lua_pushlstring( ls, itr->first.c_str(), itr->first.size());
		if (!printObject( itr->second, ls, ctx))
		{
			ctx.setTag( itr->first.c_str());
			return false;
		}
		lua_settable( ls, -3);
		++itr;
	}
	return true;
}

static bool printVector( const ddl::StructType& st, lua_State* ls, Context& ctx)
{
	lua_newtable( ls);
	ddl::StructType::Map::const_iterator itr = st.begin();
	std::size_t index = 0;

	while (itr != st.end())
	{
		lua_pushnumber( ls, (lua_Number)(++index));
		if (!printObject( itr->second, ls, ctx))
		{
			ctx.setTag( itr->first.c_str());
			return false;
		}
		lua_settable( ls, -3);
		++itr;
	}
	return true;
}

static bool printObject( const ddl::StructType& st, lua_State* ls, Context& ctx)
{
	bool rt = false;
	switch (st.contentType())
	{
		case ddl::StructType::Atomic:
			rt = printAtom( st.value(), ls, ctx);

		case ddl::StructType::Vector:
			rt = printVector( st, ls, ctx);

		case ddl::StructType::Struct:
			rt = printStruct( st, ls, ctx);
	}
	return rt;
}

static int luaException( lua_State* ls)
{
	const char* errmsg = lua_tostring( ls, -1);
	throw std::runtime_error( errmsg?errmsg:"unspecified lua exception");
	return 0;
}

void _Wolframe::serialize::parse( ddl::StructType& st, lua_State* ls)
{
	bool gotError = false;
	Context ctx;
	lua_CFunction old_panicf = lua_atpanic( ls, luaException);
	try
	{
		gotError = !parseObject( st, ls, ctx);
	}
	catch (std::exception& e)
	{
		ctx.setError( e.what());
		gotError = true;
	}
	lua_atpanic( ls, old_panicf);
	if (gotError)
	{
		luaL_error( ls, ctx.getLastError());
	}
}


void _Wolframe::serialize::print( const ddl::StructType& st, lua_State* ls)
{
	bool gotError = false;
	Context ctx;
	lua_CFunction old_panicf = lua_atpanic( ls, luaException);
	try
	{
		gotError = !printObject( st, ls, ctx);
	}
	catch (std::exception& e)
	{
		ctx.setError( e.what());
		gotError = true;
	}
	lua_atpanic( ls, old_panicf);
	if (gotError)
	{
		luaL_error( ls, ctx.getLastError());
	}
}


