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
#include "langbind/luaObjects.hpp"
#include "langbind/appObjects.hpp"
#include "langbind/appGlobalContext.hpp"
#include "langbind/luaDebug.hpp"
#include "logger-v1.hpp"
#include <stdexcept>
#include <cstddef>
#include <boost/lexical_cast.hpp>

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

using namespace _Wolframe;
using namespace langbind;

namespace luaname
{
	static const char* Input = "wolframe.Input";
	static const char* Output = "wolframe.Output";
	static const char* Filter = "wolframe.Filter";
	static const char* DDLForm = "wolframe.DDLForm";
	static const char* GlobalContext = "wolframe.ctx";
	static const char* InputFilterClosure = "wolframe.InputFilterClosure";
}

namespace
{
template <class ObjectType>
const char* metaTableName()						{return 0;}
template <> const char* metaTableName<Input>()				{return luaname::Input;}
template <> const char* metaTableName<Output>()				{return luaname::Output;}
template <> const char* metaTableName<Filter>()				{return luaname::Filter;}
template <> const char* metaTableName<DDLForm>()			{return luaname::DDLForm;}
template <> const char* metaTableName<GlobalContext>()			{return luaname::GlobalContext;}
template <> const char* metaTableName<InputFilterClosure>()		{return luaname::InputFilterClosure;}
}//anonymous namespace

static const luaL_Reg empty_methodtable[ 1] =
{
	{0,0}
};

template <class ObjectType>
struct LuaObject :public ObjectType
{
	const char* m_name;
	LuaObject( const ObjectType& o)
		:ObjectType(o),m_name(metaTableName<ObjectType>()) {}
	LuaObject()
		:m_name(metaTableName<ObjectType>()) {}

	static int destroy( lua_State* ls)
	{
		LuaObject *THIS = (LuaObject*)lua_touserdata( ls, 1);
		if (THIS) THIS->~LuaObject();
		return 0;
	}

	static void createMetatable( lua_State* ls, lua_CFunction indexf, lua_CFunction newindexf, const luaL_Reg* mt)
	{
		luaL_newmetatable( ls, metaTableName<ObjectType>());
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
	/// ABa: got a C4291 warning here again, not sure about the whole construct here..
	/// an indicator could be that started the 'iprocd.exe' results in an error when
	/// opening the processor!
	/// PF: Overloading new and delete as defined in the C++ standard
	void operator delete (void *, lua_State*, const char*) {}

	template <class Orig>
	static void push_luastack( lua_State* ls, const Orig& o)
	{
		try
		{
			const char* mt = metaTableName<ObjectType>();
			(void*)new (ls,mt) LuaObject( o);
		}
		catch (const std::bad_alloc&)
		{
			luaL_error( ls, "memory allocation error in lua context");
		}
	}

	static void createGlobal( lua_State* ls, const char* name, const ObjectType& instance, const luaL_Reg* mt=0)
	{
		createMetatable( ls, 0, 0, mt);
		new (ls) LuaObject( instance);
		luaL_getmetatable( ls, metaTableName<ObjectType>());
		lua_setmetatable( ls, -2);
		lua_setglobal( ls, name);
	}

	static bool setGlobal( lua_State* ls, const char* name, const ObjectType& instance)
	{
		lua_getglobal( ls, name);
		LuaObject* obj = (LuaObject*) luaL_checkudata( ls, -1, metaTableName<ObjectType>());
		if (!obj) return false;
		*obj = instance;
		return true;
	}

	static ObjectType* getGlobal( lua_State* ls, const char* name)
	{
		lua_getglobal( ls, name);
		LuaObject* obj = (LuaObject*) luaL_checkudata( ls, -1, metaTableName<ObjectType>());
		if (!obj)
		{
			return luaL_error( ls, "reserved global variable '%s' has been changed", name);
		}
		return obj;
	}

	static LuaObject* getSelf( lua_State* ls, const char* name, const char* method)
	{
		LuaObject* self;
		if (lua_gettop( ls) == 0 || (self=(LuaObject*)luaL_checkudata( ls, 1, metaTableName<ObjectType>())) == 0)
		{
			luaL_error( ls, "'%s' (metatable '%s') needs self parameter (%s:%s() instead of %s.%s())", name, metaTableName<ObjectType>(), name, method, name, method);
			return 0;
		}
		return self;
	}

	static LuaObject* get( lua_State* ls, int index)
	{
		LuaObject* rt = (LuaObject*) luaL_checkudata( ls, index, metaTableName<ObjectType>());
		return rt;
	}
};

template <class ObjectType>
static void setGlobalSingletonPointer( lua_State* ls, ObjectType* obj)
{
	luaL_newmetatable( ls, metaTableName<ObjectType>());
	lua_pushliteral( ls, "__index");
	lua_pushvalue( ls, -2);
	lua_rawset( ls, -3);
	lua_pushliteral( ls, "__newindex");
	lua_pushvalue( ls, -2);
	lua_rawset( ls, -3);
	ObjectType** objref = (ObjectType**)lua_newuserdata( ls, sizeof(ObjectType*));
	*objref = obj;
	luaL_getmetatable( ls, metaTableName<ObjectType>());
	lua_setmetatable( ls, -2);
	lua_setglobal( ls, metaTableName<ObjectType>());
}

template <class ObjectType>
static ObjectType* getGlobalSingletonPointer( lua_State* ls)
{
	lua_getglobal( ls, metaTableName<ObjectType>());
	ObjectType** objref = (ObjectType**) luaL_checkudata( ls, -1, metaTableName<ObjectType>());
	if (!objref)
	{
		luaL_error( ls, "reserved global variable '%s' has been changed", metaTableName<ObjectType>());
		return 0;
	}
	lua_pop( ls, 1);
	return *objref;
}


template <class Object>
static int function__LuaObject__index( lua_State* ls)
{
	Object* obj = LuaObject<Object>::getSelf( ls, metaTableName<Object>(), "__index");
	const char* key = lua_tostring( ls, 2);
	if (!key) luaL_error( ls, "%s __index called with an invalid argument 2 (key)", metaTableName<Object>());

	bool rt = false;
	bool overfl = false;
	char valbuf[ 256];
	try
	{
		std::string val;
		rt = obj->getValue( key, val);
		std::size_t nn = (overfl=(val.size() >= sizeof( valbuf)))?(sizeof( valbuf)-1):val.size();
		std::memcpy( valbuf, val.c_str(), nn);
		valbuf[ nn] = 0;
	}
	catch (const std::bad_alloc&)
	{
		return luaL_error( ls, "out of memory calling %s __index", metaTableName<Object>());
	}
	catch (const std::exception& e)
	{
		return luaL_error( ls, "%s __index called with illegal value (%s)", metaTableName<Object>(), e.what());
	}
	if (rt)
	{
		if (overfl)
		{
			return luaL_error( ls, "%s __index variable size exceeds maximum size (%u)", metaTableName<Object>(), sizeof(valbuf));
		}

		lua_pushstring( ls, valbuf);
		return 1;
	}
	else
	{
		return luaL_error( ls, "%s __index called with unknown variable name", metaTableName<Object>());
	}
	return 0;
}

template <class Object>
static int function__LuaObject__newindex( lua_State* ls)
{
	Object* obj = LuaObject<Object>::getSelf( ls, metaTableName<Object>(), "__newindex");
	const char* key = lua_tostring( ls, 2);
	if (!key) luaL_error( ls, "%s __newindex called with invalid argument 2 (key)", metaTableName<Object>());
	const char* val = 0;
	int tp = lua_type( ls, 3);

	if (tp == LUA_TBOOLEAN)
	{
		val = lua_toboolean( ls, 3)?"true":"false";
	}
	else
	{
		val = lua_tostring( ls, 3);
	}
	if (!val) luaL_error( ls, "%s __newindex called with invalid argument 3 (value)", metaTableName<Object>());

	try
	{
		if (!obj->setValue( key, val))
		{
			luaL_error( ls, "%s __newindex called with unknown variable name", metaTableName<Object>());
		}
	}
	catch (const std::bad_alloc&)
	{
		return luaL_error( ls, "out of memory calling %s __newindex", metaTableName<Object>());
	}
	catch (const std::exception& e)
	{
		return luaL_error( ls, "%s __newindex called with illegal value (%s)", metaTableName<Object>(), e.what());
	}
	return 0;
}

static void pushItem( lua_State* ls, const char* item, unsigned int itemsize)
{
	if (item)
	{
		lua_pushlstring( ls, item, (std::size_t)itemsize);
		lua_tostring( ls, -1); //PF:BUGFIX lua 5.1.4 needs this one
	}
	else
	{
		lua_pushboolean( ls, 0);
	}
}

static int function_inputFilter( lua_State* ls)
{
	const char* item[2];
	unsigned int itemsize[2];
	InputFilterClosure* closure = (InputFilterClosure*)lua_touserdata( ls, lua_upvalueindex( 1));

	try
	{
		switch (closure->fetch( item[1]/*tag*/, itemsize[1], item[0]/*val*/, itemsize[0]))
		{
			case InputFilterClosure::DoYield:
				lua_yieldk( ls, 0, 1, function_inputFilter);

			case InputFilterClosure::EndOfData:
				return 0;

			case InputFilterClosure::Error:
				luaL_error( ls, "error in iterator");
				return 0;

			case InputFilterClosure::Data:
				pushItem( ls, item[0], itemsize[0]);
				pushItem( ls, item[1], itemsize[1]);
				return 2;
		}
	}
	catch (const std::bad_alloc&)
	{
		return luaL_error( ls, "out of memory calling input filter fetch");
	}
	catch (const std::exception& e)
	{
		return luaL_error( ls, "got exception in input filter fetch: (%s)", e.what());
	}
	return luaL_error( ls, "illegal state produced by input filter fetch");
}

static const char* get_printop( lua_State* ls, int index, std::size_t& size)
{
	const char* rt = 0;
	if (lua_isnil( ls, index) || (lua_isboolean( ls, index) && !lua_toboolean( ls, index))) {}
	else if ((rt=lua_tolstring( ls, index, &size)) == 0)
	{
		const char* tn = lua_typename( ls, lua_type( ls, index));
		luaL_error( ls, "invalid type (%s) of argument %d (convertable to string or nil or false expected)", tn?tn:"unknown", index-1);
	}
	return rt;
}

static int function_output_print( lua_State* ls)
{
	const char* msg;
	const char* item[2] = {0,0};
	std::size_t itemsize[2] = {0,0};

	Output* output = LuaObject<Output>::getSelf( ls, "output", "print");
	if (lua_gettop( ls) == 1)
	{}
	else if (lua_gettop( ls) == 2)
	{
		item[0] = get_printop( ls, 2, itemsize[0]);
	}
	else if (lua_gettop( ls) == 3)
	{
		item[0] = get_printop( ls, 2, itemsize[0]);
		item[1] = get_printop( ls, 3, itemsize[1]);
	}
	else
	{
		return luaL_error( ls, "too many arguments in call of output:print");
	}

	try
	{
		switch (output->print( item[1]/*tag*/, itemsize[1], item[0]/*val*/, itemsize[0]))
		{
			case Output::DoYield:
				lua_yieldk( ls, 0, 1, function_output_print);

			case Output::Error:
				msg = output->outputfilter()->getError();
				return luaL_error( ls, "error in output:print (%s)", msg?msg:"unknown");

			case Output::Data:
				return 0;
		}
	}
	catch (const std::bad_alloc&)
	{
		return luaL_error( ls, "out of memory calling output:print");
	}
	catch (const std::exception& e)
	{
		return luaL_error( ls, "got exception in output:print: (%s)", e.what());
	}
	return luaL_error( ls, "illegal state produced by output:print");
}

static int function_output_opentag( lua_State* ls)
{
	const char* msg;
	const char* tag = 0;
	std::size_t tagsize = 0;

	Output* output = LuaObject<Output>::getSelf( ls, "output", "opentag");
	if (lua_gettop( ls) == 1)
	{
		return luaL_error( ls, "output:opentag called without tag name as argument");
	}
	else if (lua_gettop( ls) == 2)
	{
		tag = get_printop( ls, 2, tagsize);
	}
	else if (lua_gettop( ls) > 2)
	{
		return luaL_error( ls, "output:opentag called with too many arguments");
	}
	try
	{
		switch (output->print( tag, tagsize, 0/*val*/, 0))
		{
			case Output::DoYield:
				lua_yieldk( ls, 0, 1, function_output_opentag);

			case Output::Error:
				msg = output->outputfilter()->getError();
				return luaL_error( ls, "error in output:opentag (%s)", msg?msg:"unknown");

			case Output::Data:
				return 0;
		}
	}
	catch (const std::bad_alloc&)
	{
		return luaL_error( ls, "out of memory calling output:opentag");
	}
	catch (const std::exception& e)
	{
		return luaL_error( ls, "got exception in output:opentag: (%s)", e.what());
	}
	return luaL_error( ls, "illegal state produced by output:opentag");
}

static int function_output_closetag( lua_State* ls)
{
	const char* msg;

	Output* output = LuaObject<Output>::getSelf( ls, "output", "closetag");
	if (lua_gettop( ls) > 1)
	{
		return luaL_error( ls, "output:closetag called with too many arguments. no arguments expected");
	}
	try
	{
		switch (output->print( 0/*tag*/, 0, 0/*val*/, 0))
		{
			case Output::DoYield:
				lua_yieldk( ls, 0, 1, function_output_closetag);

			case Output::Error:
				msg = output->outputfilter()->getError();
				return luaL_error( ls, "error in output:closetag (%s)", msg?msg:"unknown");

			case Output::Data:
				return 0;
		}
	}
	catch (const std::bad_alloc&)
	{
		return luaL_error( ls, "out of memory calling output:closetag");
	}
	catch (const std::exception& e)
	{
		return luaL_error( ls, "got exception in output:closetag: (%s)", e.what());
	}
	return luaL_error( ls, "illegal state produced by output:closetag");
}

static int function_filter( lua_State* ls)
{
	unsigned int nn = lua_gettop( ls);
	if (nn == 0) return luaL_error( ls, "too few arguments for filter");
	if (nn > 1) return luaL_error( ls, "too many arguments for filter");
	if (!lua_isstring( ls, 1))
	{
		return luaL_error( ls, "invalid type of argument (string expected)");
	}
	const char* name = lua_tostring( ls, 1);
	GlobalContext* ctx = getGlobalSingletonPointer<GlobalContext>( ls);
	if (!ctx)
	{
		return luaL_error( ls, "internal error. filter function got no global context");
	}
	Filter flt;
	try
	{
		if (!ctx->getFilter( name, flt))
		{
			return luaL_error( ls, "could not get filter '%s'", name);
		}
	}
	catch (const std::exception& e)
	{
		return luaL_error( ls, "got exception in function filter: (%s)", e.what());
	}
	LuaObject<Filter>::push_luastack( ls, flt);
	return 1;
}

static int function_input_as( lua_State* ls)
{
	Input* input = LuaObject<Input>::getSelf( ls, "input", "as");
	if (lua_gettop( ls) != 2)
	{
		luaL_error( ls, "filter type value as parameter of method 'input:as' expected");
	}
	LuaObject<Filter>* filter = LuaObject<Filter>::get( ls, 2);

	if (!filter)
	{
		const char* tn = lua_typename( ls, lua_type( ls, 2));
		luaL_error( ls, "filter type value expected as first argument of input:as instead of %s", tn?tn:"UNKNOWN");
	}
	try
	{
		protocol::InputFilter* ff = 0;
		if (filter->inputfilter().get())
		{
			ff = filter->inputfilter()->copy();
			if (input->inputfilter().get())
			{
				ff->assignContent( *input->inputfilter());
			}
		}
		else
		{
			luaL_error( ls, "input:as called with a filter with undefined input");
		}
		input->inputfilter().reset( ff);
	}
	catch (const std::bad_alloc&)
	{
		return luaL_error( ls, "out of memory calling input:as");
	}
	catch (const std::exception& e)
	{
		return luaL_error( ls, "got exception in input:as: (%s)", e.what());
	}
	return 0;
}

static int function_output_as( lua_State* ls)
{
	Output* output = LuaObject<Output>::getSelf( ls, "output", "as");
	if (lua_gettop( ls) != 2)
	{
		luaL_error( ls, "filter type value as parameter of method 'output:as' expected");
	}
	Filter* filter = LuaObject<Filter>::get( ls, 2);
	if (!filter)
	{
		const char* tn = lua_typename( ls, lua_type( ls, 2));
		luaL_error( ls, "filter type value expected as first argument of output:as instead of %s", tn?tn:"UNKNOWN");
	}
	try
	{
		protocol::OutputFilter* ff = 0;
		if (filter->outputfilter().get())
		{
			ff = filter->outputfilter()->copy();
			if (output->outputfilter().get())
			{
				ff->assignContent( *output->outputfilter());
			}
		}
		else
		{
			luaL_error( ls, "output:as called with a filter with undefined output");
		}
		output->outputfilter().reset( ff);
	}
	catch (const std::bad_alloc&)
	{
		return luaL_error( ls, "out of memory calling output:as");
	}
	catch (const std::exception& e)
	{
		return luaL_error( ls, "got exception in output:as: (%s)", e.what());
	}
	return 0;
}

static int function_input_get( lua_State* ls)
{
	Input* input = LuaObject<Input>::getSelf( ls, "input", "get");
	if (lua_gettop( ls) != 1)
	{
		luaL_error( ls, "no arguments expected for input:get");
	}
	if (!input->inputfilter().get())
	{
		return luaL_error( ls, "no filter defined for input with input:as");
	}
	LuaObject<InputFilterClosure>::push_luastack( ls, input->inputfilter());
	lua_pushcclosure( ls, function_inputFilter, 1);
	return 1;
}

static int function_yield( lua_State* ls)
{
	int ii,nn=lua_gettop( ls);
	for (ii=0; ii<nn; ii++) lua_pushvalue( ls, ii);
	return lua_yieldk( ls, ii, 1, function_yield);
}

static const luaL_Reg input_methodtable[ 3] =
{
	{"as",&function_input_as},
	{"get",&function_input_get},
	{0,0}
};

static const luaL_Reg output_methodtable[ 5] =
{
	{"as",&function_output_as},
	{"print",&function_output_print},
	{"opentag",function_output_opentag},
	{"closetag",function_output_closetag},
	{0,0}
};

LuaScriptInstanceR _Wolframe::langbind::createLuaScriptInstance( const char* name, const Input& input_, const Output& output_)
{
	LuaScriptInstanceR rt;
	GlobalContext* gc = getGlobalContext();
	if (!gc->getLuaScriptInstance( name, rt)) throw std::runtime_error( "invalid command name");
	lua_State* ls = rt->ls();
	LuaObject<Input>::createGlobal( ls, "input", input_, input_methodtable);
	LuaObject<Output>::createGlobal( ls, "output", output_, output_methodtable);
	LuaObject<Filter>::createMetatable( ls, &function__LuaObject__index<Filter>, &function__LuaObject__newindex<Filter>, 0);
	LuaObject<InputFilterClosure>::createMetatable( ls, 0, 0, 0);
	setGlobalSingletonPointer<GlobalContext>( ls, getGlobalContext());
	lua_pushcfunction( ls, &function_yield);
	lua_setglobal( ls, "yield");
	lua_pushcfunction( ls, &function_filter);
	lua_setglobal( ls, "filter");
	return rt;
}

