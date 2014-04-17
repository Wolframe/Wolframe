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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
///\file luaObjectTemplate.hpp
///\brief Template for Wolframe objects used in the Lua interpreter context
#ifndef _Wolframe_langbind_LUA_OBJECT_TEMPLATE_HPP_INCLUDED
#define _Wolframe_langbind_LUA_OBJECT_TEMPLATE_HPP_INCLUDED
#include "luaObjects.hpp"
#include "luaGetFunctionClosure.hpp"
#include "processor/procProviderInterface.hpp"
#include "types/normalizeFunction.hpp"
#include "types/doctype.hpp"
#include "types/datetime.hpp"
#include "types/bignumber.hpp"
#include "types/customDataType.hpp"
#include "types/form.hpp"
#include "filter/typingfilter.hpp"
#include "filter/typedfilterScope.hpp"
#include "filter/inputfilterScope.hpp"
#include "filter/tostringfilter.hpp"
#include "filter/redirectFilterClosure.hpp"
#include "serialize/ddlFormParser.hpp"
#include "serialize/ddlFormSerializer.hpp"
#include "serialize/struct/structParser.hpp"
#include "serialize/struct/structSerializer.hpp"
#include "utils/fileUtils.hpp"
#include <limits>
#include <stdexcept>
#include <string>

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

namespace _Wolframe {
namespace langbind {

template <class ObjectType>
struct MetaTable {static const char* name()						{return 0;}};
template <> struct MetaTable<Input> {static const char* name()				{return "wolframe.Input";}};
template <> struct MetaTable<Output> {static const char* name()				{return "wolframe.Output";}};
template <> struct MetaTable<Filter> {static const char* name()				{return "wolframe.Filter";}};
template <> struct MetaTable<RedirectFilterClosure> {static const char* name()		{return "wolframe.RedirectFilterClosure";}};
template <> struct MetaTable<types::FormR> {static const char* name()			{return "wolframe.Form";}};
template <> struct MetaTable<types::CustomDataValueR> {static const char* name()	{return "wolframe.CustomValue";}};
template <> struct MetaTable<types::CustomDataInitializerR> {static const char* name()	{return "wolframe.CustomInitializer";}};
template <> struct MetaTable<types::DateTime> {static const char* name()		{return "wolframe.DateTime";}};
template <> struct MetaTable<types::BigNumber> {static const char* name()		{return "wolframe.BigNumber";}};
template <> struct MetaTable<serialize::DDLFormParser> {static const char* name()	{return "wolframe.DDLFormParser";}};
template <> struct MetaTable<serialize::DDLFormSerializer> {static const char* name()	{return "wolframe.DDLFormSerializer";}};
template <> struct MetaTable<InputFilterClosure> {static const char* name()		{return "wolframe.InputFilterClosure";}};
template <> struct MetaTable<TypedInputFilterR> {static const char* name()		{return "wolframe.TypedInputFilterR";}};
template <> struct MetaTable<TypedInputFilterClosure> {static const char* name()	{return "wolframe.TypedInputFilterClosure";}};
template <> struct MetaTable<FormFunctionClosureR> {static const char* name()		{return "wolframe.FormFunctionClosureR";}};
template <> struct MetaTable<types::NormalizeFunctionR> {static const char* name()	{return "wolframe.NormalizeFunctionR";}};
template <> struct MetaTable<serialize::StructSerializer> {static const char* name()	{return "wolframe.StructSerializer";}};
template <> struct MetaTable<proc::ProcessorProviderInterface> {static const char* name(){return "wolframe.ProcessorProvider";}};
template <> struct MetaTable<LuaModuleMap> {static const char* name()			{return "wolframe.LuaModuleMap";}};

template <class ObjectType>
struct LuaObject
{
	LuaObject( const ObjectType& o)
		:m_name(MetaTable<ObjectType>::name())
		,m_obj(o) {}
	LuaObject()
		:m_name(MetaTable<ObjectType>::name()) {}

	static int destroy( lua_State* ls)
	{
		LuaObject *THIS = (LuaObject*)lua_touserdata( ls, 1);
		if (THIS) THIS->~LuaObject();
		return 0;
	}

	static void createMetatable( lua_State* ls, lua_CFunction indexf, lua_CFunction newindexf, const luaL_Reg* mt, const char* tpname)
	{
		luaL_newmetatable( ls, MetaTable<ObjectType>::name());
		lua_pushliteral( ls, "__index");
		if (indexf)
		{
			lua_pushcfunction( ls, indexf);
		}
		else
		{
			lua_pushvalue( ls, -2);
		}
		lua_rawset( ls, -3);

		lua_pushliteral( ls, "__newindex");
		if (newindexf)
		{
			lua_pushcfunction( ls, newindexf);
		}
		else
		{
			lua_pushvalue( ls, -2);
		}
		lua_rawset( ls, -3);

		if (tpname)
		{
			lua_pushliteral( ls, "type");
			lua_pushstring( ls, tpname);
			lua_rawset( ls, -3);
		}

		lua_pushliteral( ls, "__gc");
		lua_pushcfunction( ls, destroy);
		lua_rawset( ls, -3);

		if (mt)
		{
			unsigned int ii;
			for (ii=0; mt[ii].name; ++ii)
			{
				lua_pushcfunction( ls, mt[ii].func);
				lua_setfield( ls, -2, mt[ii].name);
			}
		}
		lua_pop( ls, 1);
	}

	void* operator new (std::size_t num_bytes, lua_State* ls) throw (std::bad_alloc)
	{
		void* rt = lua_newuserdata( ls, num_bytes);
		if (rt == 0) throw std::bad_alloc();
		return rt;
	}

	void* operator new (std::size_t num_bytes, lua_State* ls, const char* mt) throw (std::bad_alloc)
	{
		void* rt = lua_newuserdata( ls, num_bytes);
		if (rt == 0) throw std::bad_alloc();
		luaL_getmetatable( ls, mt);
		lua_setmetatable( ls, -2);
		return rt;
	}

	/// \brief does nothing because the LUA garbage collector does the job.
	/// \warning CAUTION: DO NOT CALL THIS FUNCTION ! DOES NOT WORK ON MSVC 9.0. (The compiler links with the std delete)
	/// (just avoids C4291 warning)
	void operator delete (void *, lua_State*) {}
	void operator delete (void *, lua_State*, const char*) {}

	static void push_luastack( lua_State* ls, const ObjectType& o)
	{
		try
		{
			const char* mt = MetaTable<ObjectType>::name();
			new (ls,mt) LuaObject( o);
		}
		catch (const std::bad_alloc&)
		{
			luaL_error( ls, "memory allocation error in lua context");
		}
	}

	static void createGlobal( lua_State* ls, const char* name, const ObjectType& instance)
	{
		new (ls) LuaObject( instance);
		luaL_getmetatable( ls, MetaTable<ObjectType>::name());
		if (lua_type( ls, -1) == LUA_TNIL)
		{
			lua_pop( ls, 1);
			luaL_error( ls, "no metatable defined to defined global object '%s'", name);
		}
		lua_setmetatable( ls, -2);
		lua_setglobal( ls, name);
	}

	static bool setGlobal( lua_State* ls, const char* name, const ObjectType& instance)
	{
		lua_getglobal( ls, name);
		LuaObject* obj = (LuaObject*) luaL_testudata( ls, -1, MetaTable<ObjectType>::name());
		if (!obj) return false;
		*obj = instance;
		return true;
	}

	static ObjectType* getGlobal( lua_State* ls, const char* name)
	{
		lua_getglobal( ls, name);
		LuaObject* obj = (LuaObject*) luaL_testudata( ls, -1, MetaTable<ObjectType>::name());
		if (!obj) return 0;
		ObjectType* rt = obj->ref();
		lua_pop( ls, 1);
		return rt;
	}

	static ObjectType* getSelf( lua_State* ls, const char* name, const char* method)
	{
		LuaObject* self;
		if (lua_gettop( ls) == 0 || (self=(LuaObject*)luaL_testudata( ls, 1, MetaTable<ObjectType>::name())) == 0)
		{
			luaL_error( ls, "'%s' (metatable '%s') needs self parameter (%s:%s() instead of %s.%s())", name, MetaTable<ObjectType>::name(), name, method, name, method);
			return 0;
		}
		return self->ref();
	}

	static ObjectType* get( lua_State* ls, int index)
	{
		LuaObject* rt = (LuaObject*) luaL_testudata( ls, index, MetaTable<ObjectType>::name());
		return rt?rt->ref():0;
	}

	const ObjectType* ref() const		{return &m_obj;}
	ObjectType* ref()			{return &m_obj;}
	const char* name() const		{return m_name;}
private:
	const char* m_name;
	ObjectType m_obj;
};

}}//namespace
#endif


