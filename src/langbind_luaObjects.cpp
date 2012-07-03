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
#include "langbind/luaException.hpp"
#include "langbind/luaGetFunctionClosure.hpp"
#include "filter/luafilter.hpp"
#include "filter/typingfilter.hpp"
#include "filter/tostringfilter.hpp"
#include "ddl/structTypeBuild.hpp"
#include "logger-v1.hpp"
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <cstddef>
#include <cstdarg>
#include <boost/lexical_cast.hpp>
extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}
#include "langbind/luaDebug.hpp"

using namespace _Wolframe;
using namespace langbind;

namespace luaname
{
	static const char* Logger = "wolframe.Logger";
	static const char* Input = "wolframe.Input";
	static const char* Output = "wolframe.Output";
	static const char* Filter = "wolframe.Filter";
	static const char* RedirectFilterClosure = "wolframe.RedirectFilterClosure";
	static const char* DDLForm = "wolframe.DDLForm";
	static const char* DDLStructParser = "wolframe.DDLStructParser";
	static const char* DDLStructSerializer = "wolframe.DDLStructSerializer";
	static const char* GlobalContext = "wolframe.ctx";
	static const char* InputFilterClosure = "wolframe.InputFilterClosure";
	static const char* TypedInputFilterR = "wolframe.TypedInputFilterR";
	static const char* TypedInputFilterClosure = "wolframe.TypedInputFilterClosure";
	static const char* FormFunctionClosure = "wolframe.FormFunctionClosure";
	static const char* PeerFormFunctionClosure = "wolframe.PeerFormFunctionClosure";
	static const char* StructSerializer = "wolframe.StructSerializer";
}

namespace
{
template <class ObjectType>
const char* metaTableName()						{return 0;}
template <> const char* metaTableName<Logger>()				{return luaname::Logger;}
template <> const char* metaTableName<Input>()				{return luaname::Input;}
template <> const char* metaTableName<Output>()				{return luaname::Output;}
template <> const char* metaTableName<Filter>()				{return luaname::Filter;}
template <> const char* metaTableName<RedirectFilterClosure>()		{return luaname::RedirectFilterClosure;}
template <> const char* metaTableName<DDLForm>()			{return luaname::DDLForm;}
template <> const char* metaTableName<serialize::DDLStructParser>()	{return luaname::DDLStructParser;}
template <> const char* metaTableName<serialize::DDLStructSerializer>()	{return luaname::DDLStructSerializer;}
template <> const char* metaTableName<GlobalContext>()			{return luaname::GlobalContext;}
template <> const char* metaTableName<InputFilterClosure>()		{return luaname::InputFilterClosure;}
template <> const char* metaTableName<TypedInputFilterR>()		{return luaname::TypedInputFilterR;}
template <> const char* metaTableName<TypedInputFilterClosure>()	{return luaname::TypedInputFilterClosure;}
template <> const char* metaTableName<FormFunctionClosure>()		{return luaname::FormFunctionClosure;}
template <> const char* metaTableName<PeerFormFunctionClosure>()	{return luaname::PeerFormFunctionClosure;}
template <> const char* metaTableName<serialize::StructSerializer>()	{return luaname::StructSerializer;}
}//anonymous namespace

static const luaL_Reg empty_methodtable[ 1] =
{
	{0,0}
};

template <class ObjectType>
struct LuaObject
{
	LuaObject( const ObjectType& o)
		:m_name(metaTableName<ObjectType>())
		,m_obj(o) {}
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
		LuaObject* obj = (LuaObject*) luaL_testudata( ls, -1, metaTableName<ObjectType>());
		if (!obj) return false;
		*obj = instance;
		return true;
	}

	static ObjectType* getGlobal( lua_State* ls, const char* name)
	{
		lua_getglobal( ls, name);
		LuaObject* obj = (LuaObject*) luaL_testudata( ls, -1, metaTableName<ObjectType>());
		if (!obj)
		{
			luaL_error( ls, "reserved global variable '%s' has been changed", name);
			return 0;
		}
		return obj->ref();
	}

	static ObjectType* getSelf( lua_State* ls, const char* name, const char* method)
	{
		LuaObject* self;
		if (lua_gettop( ls) == 0 || (self=(LuaObject*)luaL_testudata( ls, 1, metaTableName<ObjectType>())) == 0)
		{
			luaL_error( ls, "'%s' (metatable '%s') needs self parameter (%s:%s() instead of %s.%s())", name, metaTableName<ObjectType>(), name, method, name, method);
			return 0;
		}
		return self->ref();
	}

	static ObjectType* get( lua_State* ls, int index)
	{
		LuaObject* rt = (LuaObject*) luaL_testudata( ls, index, metaTableName<ObjectType>());
		return rt?rt->ref():0;
	}

	const ObjectType* ref() const		{return &m_obj;}
	ObjectType* ref()			{return &m_obj;}
	const char* name() const		{return m_name;}
private:
	const char* m_name;
	ObjectType m_obj;
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
	ObjectType** objref = (ObjectType**) luaL_testudata( ls, -1, metaTableName<ObjectType>());
	if (!objref)
	{
		luaL_error( ls, "reserved global variable '%s' has been changed", metaTableName<ObjectType>());
		return 0;
	}
	lua_pop( ls, 1);
	return *objref;
}

static void check_parameters( lua_State* ls, int si, int nn, ...)
{
	va_list aa;
	va_start( aa, nn);
	int anum = lua_gettop( ls);
	if (anum != nn+si)
	{
		if (nn == 0) throw std::runtime_error( "no arguments expected");
		if (anum > nn+si) throw std::runtime_error( "too many arguments");
		if (anum < nn+si) throw std::runtime_error( "too few arguments");
	}
	for (int ii = si; ii < nn; ++ii)
	{
		int expect = va_arg( aa, int);
		int typ = lua_type( ls, -ii-1);
		if (typ != expect)
		{
			const char* expectname = lua_typename( ls, expect);
			const char* typname = lua_typename( ls, typ);
			std::ostringstream msg;
			msg << "expected " << (expectname?expectname:"?") << "instead of " << (typname?typname:"?") << "as argument " << (ii-si+1);
			throw std::runtime_error( msg.str());
		}
	}
	va_end( aa);
}


template <class Object>
static int function__LuaObject__index( lua_State* ls)
{
	LuaErrorMessage luaerr;
	Object* obj = LuaObject<Object>::getSelf( ls, metaTableName<Object>(), "__index");
	const char* key = lua_tostring( ls, 2);
	try
	{
		if (!key)
		{
			throw std::runtime_error( "invalid element name");
		}
		std::string val;
		if (!obj->getValue( key, val))
		{
			if (lua_getmetatable( ls, 1)) return 1;
			throw std::runtime_error( "unknown element name");
		}
		else
		{
			LuaExceptionHandlerScope escope(ls);
			{
				lua_pushlstring( ls, val.c_str(), val.size());
				return 1;
			}
		}
		return 0;
	}
	catch (const std::bad_alloc&)
	{
		luaerr.init( "__index", "out of memory");
	}
	catch (const std::exception& e)
	{
		luaerr.init( "__index", e.what());
	}
	return luaerr.luaerror( ls);
}

template <class Object>
static int function__LuaObject__newindex( lua_State* ls)
{
	LuaErrorMessage luaerr;
	Object* obj = LuaObject<Object>::getSelf( ls, metaTableName<Object>(), "__newindex");
	const char* key = lua_tostring( ls, 2);
	try
	{
		if (!key)
		{
			throw std::runtime_error( "invalid element name");
		}
		const char* val = 0;
		int tp = lua_type( ls, 3);

		if (tp == LUA_TBOOLEAN)
		{
			lua_pushvalue( ls, 3);
			val = lua_toboolean( ls, -1)?"true":"false";
		}
		else
		{
			lua_pushvalue( ls, 3);
			val = lua_tostring( ls, -1);
		}
		if (!val)
		{
			lua_pop( ls, 1);
			if (lua_getmetatable( ls, 1)) return 1;
			throw std::runtime_error( "value is not convertible to a string");
		}
		if (!obj->setValue( key, val))
		{
			lua_pop( ls, 1);
			if (lua_getmetatable( ls, 1)) return 1;
			throw std::runtime_error( "unknown element name");
		}
		return 0;
	}
	catch (const std::bad_alloc&)
	{
		luaerr.init( "__newindex", "out of memory");
	}
	catch (const std::exception& e)
	{
		luaerr.init( "__newindex", e.what());
	}
	return luaerr.luaerror( ls);
}

static TypedInputFilterR get_operand_TypedInputFilter( lua_State* ls, int idx)
{
	TypedInputFilterR rt;
	int typ = lua_type( ls, idx);
	switch (typ)
	{
		case LUA_TTABLE:
			rt.reset( new LuaTableInputFilter( ls));
			break;

		case LUA_TFUNCTION:
			if (lua_getupvalue( ls, idx, 1))
			{
				InputFilterClosure* ic = LuaObject<InputFilterClosure>::get( ls, -1);
				if (ic)
				{
					if (!ic->isValidAsOperand())
					{
						throw std::runtime_error( "iterator used as function argument in an intermediate state");
					}
					rt.reset( new TypingInputFilter( ic->inputfilter()));
					break;
				}
				TypedInputFilterClosure* tc = LuaObject<TypedInputFilterClosure>::get( ls, -1);
				if (tc)
				{
					if (!tc->isValidAsOperand())
					{
						throw std::runtime_error( "iterator used as function argument in an intermediate state");
					}
					rt = tc->inputfilter();
					break;
				}
			}
			/*no break here*/
		default:
		{
			const char* typnam = typ>0?lua_typename( ls, typ):"NIL";
			std::ostringstream msg;
			msg << "expected table, form or generator function and got '" << typnam << "' as argument";
			throw std::runtime_error( msg.str());
		}
	}
	return rt;
}

#define LUA_FUNCTION_THROWS(fname,DECLNAME)\
struct DECLNAME ## _functor {static int call( lua_State* ls);};\
static int DECLNAME( lua_State* ls)\
{\
	LuaFunctionCppCall<DECLNAME ## _functor> func;\
	return func.run( fname, ls);\
}\
int DECLNAME ## _functor::call( lua_State* ls)\





LUA_FUNCTION_THROWS( "<structure>:get()", function_inputFilter_get)
{
	InputFilterClosure* closure = LuaObject<InputFilterClosure>::get( ls, lua_upvalueindex( 1));
	LuaExceptionHandlerScope escope(ls);
	switch (closure->fetch( ls))
	{
		case InputFilterClosure::DoYield:
			lua_yieldk( ls, 0, 1, function_inputFilter_get);

		case InputFilterClosure::EndOfData:
			return 0;

		case InputFilterClosure::Data:
			return 2;
	}
	throw std::runtime_error( "illegal state when fetching next element");
}



LUA_FUNCTION_THROWS( "<structure>:get()", function_typedinputfilterClosure_get)
{
	TypedInputFilterClosure* closure = LuaObject<TypedInputFilterClosure>::get( ls, lua_upvalueindex( 1));
	LuaExceptionHandlerScope escope(ls);
	switch (closure->fetch( ls))
	{
		case TypedInputFilterClosure::DoYield:
			lua_yieldk( ls, 0, 1, function_typedinputfilterClosure_get);

		case TypedInputFilterClosure::EndOfData:
			return 0;

		case TypedInputFilterClosure::Data:
			return 2;
	}
	throw std::runtime_error( "illegal state when fetching next element");
}


LUA_FUNCTION_THROWS( "scope(..)", function_scope)
{
	check_parameters( ls, 0, 1, LUA_TFUNCTION);
	if (lua_getupvalue( ls, 1, 1))
	{
		InputFilterClosure* ic = LuaObject<InputFilterClosure>::get( ls, -1);
		if (ic)
		{
			if (!ic->isValidAsOperand())
			{
				throw std::runtime_error( "iterator used as function argument in an intermediate state");
			}
			LuaObject<InputFilterClosure>::push_luastack( ls, InputFilterClosure( ic->inputfilter()));
			lua_pushcclosure( ls, function_inputFilter_get, 1);
			return 1;
		}
		TypedInputFilterClosure* tc = LuaObject<TypedInputFilterClosure>::get( ls, -1);
		if (tc)
		{
			if (!tc->isValidAsOperand())
			{
				throw std::runtime_error( "iterator used as function argument in an intermediate state");
			}
			LuaObject<TypedInputFilterClosure>::push_luastack( ls, TypedInputFilterClosure( tc->inputfilter()));
			lua_pushcclosure( ls, function_typedinputfilterClosure_get, 1);
			return 1;
		}
	}
	throw std::runtime_error( "iterator on input or on a form expected as argument");
}


LUA_FUNCTION_THROWS( "form:__tostring()", function_form_tostring)
{
	DDLForm* form = LuaObject<DDLForm>::getSelf( ls, "form", "__tostring");
	check_parameters( ls, 1, 0);

	std::string content = form->tostring();
	LuaExceptionHandlerScope escope(ls);
	{
		lua_pushlstring( ls, content.c_str(), content.size());
		return 1;
	}
}


LUA_FUNCTION_THROWS( "form:fill()", function_form_fill)
{
	DDLForm* form = LuaObject<DDLForm>::getSelf( ls, "form", "fill");
	serialize::DDLStructParser* closure;
	int ctx;
	if (lua_getctx( ls, &ctx) != LUA_YIELD)
	{
		serialize::Context::Flags flags = serialize::Context::None;
		if (lua_gettop( ls) == 3)
		{
			if (!lua_isstring( ls, 3))
			{
				throw std::runtime_error( "2nd argument is not a string");
			}
			const char* mode = lua_tostring( ls, 3);
			if (std::strcmp( mode, "strict") == 0)
			{
				flags = serialize::Context::ValidateAttributes;
			}
			else
			{
				throw std::runtime_error( "2nd argument does not specify a mode of validating input (e.g. \"strict\")");
			}
		}
		TypedInputFilterR inp = get_operand_TypedInputFilter( ls, 2);
		LuaObject<serialize::DDLStructParser>::push_luastack( ls, serialize::DDLStructParser( form->structure()));
		closure = LuaObject<serialize::DDLStructParser>::get( ls, -1);
		closure->init( inp, flags);
		lua_pushvalue( ls, 2);		//... iterator argument (table, generator function, etc.)
	}
	else
	{
		closure = (serialize::DDLStructParser*)lua_touserdata( ls, -1);
		lua_pop( ls, 1);
	}
	if (!closure->call())
	{
		lua_pushlightuserdata( ls, closure);
		lua_yieldk( ls, 0, 1, function_form_fill);
	}
	return 0;
}


LUA_FUNCTION_THROWS( "form:table()", function_form_table)
{
	DDLForm* form = LuaObject<DDLForm>::getSelf( ls, "form", "table");

	serialize::DDLStructSerializer* result;
	int ctx;
	if (lua_getctx( ls, &ctx) != LUA_YIELD)
	{
		check_parameters( ls, 1, 0);
		TypedOutputFilterR outp( new LuaTableOutputFilter( ls));
		LuaObject<serialize::DDLStructSerializer>::push_luastack( ls, serialize::DDLStructSerializer( form->structure()));
		result = LuaObject<serialize::DDLStructSerializer>::get( ls, -1);
		result->init( outp, serialize::Context::SerializeWithIndices);
	}
	else
	{
		result = (serialize::DDLStructSerializer*)lua_touserdata( ls, -1);
		lua_pop( ls, 1);
	}
	if (!result->call())
	{
		lua_pushlightuserdata( ls, result);
		lua_yieldk( ls, 0, 1, function_form_table);
	}
	return 1;
}


LUA_FUNCTION_THROWS( "form:get()", function_form_get)
{
	DDLForm* result = LuaObject<DDLForm>::getSelf( ls, "form", "get");
	check_parameters( ls, 1, 0);

	TypedInputFilterR itr( new serialize::DDLStructSerializer( result->structure()));
	LuaObject<TypedInputFilterClosure>::push_luastack( ls, TypedInputFilterClosure(itr));
	lua_pushcclosure( ls, function_typedinputfilterClosure_get, 1);
	return 1;
}



LUA_FUNCTION_THROWS( "form()", function_form)
{
	if (lua_isstring( ls, 1))
	{
		check_parameters( ls, 0, 1, LUA_TSTRING);

		const char* name = lua_tostring( ls, 1);
		GlobalContext* ctx = getGlobalSingletonPointer<GlobalContext>( ls);
		if (!ctx)
		{
			throw std::runtime_error( "lost global context");
		}
		DDLForm frm;
		if (!ctx->getForm( name, frm))
		{
			throw std::runtime_error( "form not defined");
		}
		LuaObject<DDLForm>::push_luastack( ls, frm);
		return 1;
	}
	else if (lua_istable( ls, 1))
	{
		check_parameters( ls, 0, 1, LUA_TTABLE);
		TypedInputFilterR inp = get_operand_TypedInputFilter( ls, 1);
		if (!inp.get()) throw std::runtime_error( "unexpected null object intead of table argument");
		DDLForm frm( ddl::StructTypeR( new ddl::StructTypeBuild( *inp)));
		LuaObject<DDLForm>::push_luastack( ls, frm);
		return 1;
	}
	throw std::runtime_error( "expected string or table as argument of form");
}


LUA_FUNCTION_THROWS( "<structure>:__tostring()", function_struct_tostring)
{
	serialize::StructSerializer* obj = LuaObject<serialize::StructSerializer>::getSelf( ls, "<structure>", "__tostring");
	check_parameters( ls, 1, 0);

	ToStringFilter* flt = new ToStringFilter;
	TypedOutputFilterR out( flt);
	obj->init( out, serialize::Context::SerializeWithIndices);
	if (!obj->call())
	{
		if (out->state() == OutputFilter::EndOfBuffer)
		{
			throw std::logic_error( "internal: tostring serialization with yield");
		}
		else
		{
			throw std::runtime_error( out->getError());
		}
	}
	std::string content = flt->content();
	LuaExceptionHandlerScope escope(ls);
	{
		lua_pushlstring( ls, content.c_str(), content.size());
		return 1;
	}
}


LUA_FUNCTION_THROWS( "<structure>:table()", function_struct_table)
{
	serialize::StructSerializer* obj = LuaObject<serialize::StructSerializer>::getSelf( ls, "<structure>", "table");

	int ctx;
	if (lua_getctx( ls, &ctx) != LUA_YIELD)
	{
		check_parameters( ls, 1, 0);
		obj->init( TypedOutputFilterR( new LuaTableOutputFilter( ls)), serialize::Context::SerializeWithIndices);
	}
	else
	{
		obj = (serialize::StructSerializer*)lua_touserdata( ls, -1);
		lua_pop( ls, 1);
	}
	if (!obj->call())
	{
		lua_pushlightuserdata( ls, obj);
		lua_yieldk( ls, 0, 1, function_struct_table);
	}
	return 1;
}


LUA_FUNCTION_THROWS( "<structure>:get()", function_struct_get)
{
	serialize::StructSerializer* obj = LuaObject<serialize::StructSerializer>::getSelf( ls, "<structure>", "get");
	check_parameters( ls, 1, 0);

	TypedInputFilterR itr( new serialize::StructSerializer( *obj));
	LuaObject<TypedInputFilterClosure>::push_luastack( ls, TypedInputFilterClosure( itr));
	lua_pushcclosure( ls, function_typedinputfilterClosure_get, 1);
	return 1;
}


LUA_FUNCTION_THROWS( "<structure>:__tostring()", function_typedinputfilter_tostring)
{
	TypedInputFilterR* obj = LuaObject<TypedInputFilterR>::getSelf( ls, "<structure>", "__tostring");
	check_parameters( ls, 1, 0);

	ToStringFilter* flt = new ToStringFilter();
	TypedOutputFilterR out( flt);
	RedirectFilterClosure exc( *obj, out);
	if (!exc.call())
	{
		throw std::logic_error( "internal: tostring serialization with yield");
	}
	std::string content = flt->content();
	LuaExceptionHandlerScope escope(ls);
	{
		lua_pushlstring( ls, content.c_str(), content.size());
		return 1;
	}
}


LUA_FUNCTION_THROWS( "<structure>:table()", function_typedinputfilter_table)
{
	TypedInputFilterR* obj = LuaObject<TypedInputFilterR>::getSelf( ls, "<structure>", "table");

	int ctx;
	if (lua_getctx( ls, &ctx) != LUA_YIELD)
	{
		TypedOutputFilterR outp( new LuaTableOutputFilter( ls));
		LuaObject<RedirectFilterClosure>::push_luastack( ls, RedirectFilterClosure( *obj, outp));
		RedirectFilterClosure* closure = LuaObject<RedirectFilterClosure>::get( ls, -1);
		lua_pushvalue( ls, 2);			//... iterator argument
		lua_pushlightuserdata( ls, closure);	//... redirect closure object
	}
	RedirectFilterClosure* closure = (RedirectFilterClosure*)lua_touserdata( ls, -1);
	lua_pop( ls, 1);
	if (!closure->call())
	{
		lua_pushlightuserdata( ls, closure);
		lua_yieldk( ls, 0, 1, function_typedinputfilter_table);
	}
	return 1;
}


LUA_FUNCTION_THROWS( "<structure>:get()", function_typedinputfilter_get)
{
	TypedInputFilterR* obj = LuaObject<TypedInputFilterR>::getSelf( ls, "<structure>", "get");
	check_parameters( ls, 1, 0);

	LuaObject<TypedInputFilterClosure>::push_luastack( ls, TypedInputFilterClosure( *obj));
	lua_pushcclosure( ls, function_typedinputfilterClosure_get, 1);
	return 1;
}


LUA_FUNCTION_THROWS( "<formfunction>(..)", function_formfunction_call)
{
	FormFunctionClosure* closure = LuaObject<FormFunctionClosure>::get( ls, lua_upvalueindex( 1));
	int ctx;
	if (lua_getctx( ls, &ctx) != LUA_YIELD)
	{
		if (lua_gettop( ls) != 1)
		{
			throw std::runtime_error( "one argument expected");
		}
		else
		{
			TypedInputFilterR inp = get_operand_TypedInputFilter( ls, 1);
			closure->init( inp, serialize::Context::None);
		}
		lua_pushvalue( ls, 1);		//... iterator argument (table, generator function, etc.)
	}
	if (!closure->call())
	{
		lua_yieldk( ls, 0, 1, function_formfunction_call);
	}
	LuaObject<serialize::StructSerializer>::push_luastack( ls, closure->result());
	return 1;
}

LUA_FUNCTION_THROWS( "<formfunction>(..)", function_peerformfunction_call)
{
	PeerFormFunctionClosure* closure = LuaObject<PeerFormFunctionClosure>::get( ls, lua_upvalueindex( 1));
	int ctx;
	if (lua_getctx( ls, &ctx) != LUA_YIELD)
	{
		if (lua_gettop( ls) != 1)
		{
			throw std::runtime_error( "one argument expected");
		}
		else
		{
			TypedInputFilterR inp = get_operand_TypedInputFilter( ls, 1);
			closure->init( inp);
		}
		lua_pushvalue( ls, 1);		//... iterator argument (table, generator function, etc.)
	}
	if (!closure->call())
	{
		lua_yieldk( ls, 0, 1, function_peerformfunction_call);
	}
	LuaObject<DDLForm>::push_luastack( ls, closure->result());
	return 1;
}


LUA_FUNCTION_THROWS( "formfunction(..)", function_formfunction)
{
	check_parameters( ls, 0, 1, LUA_TSTRING);

	const char* name = lua_tostring( ls, 1);
	GlobalContext* ctx = getGlobalSingletonPointer<GlobalContext>( ls);
	if (!ctx)
	{
		throw std::runtime_error( "lost global context");
	}
	FormFunction ff;
	PeerFunction pf;
	PeerFormFunction pff;

	if (ctx->getFormFunction( name, ff))
	{
		LuaObject<FormFunctionClosure>::push_luastack( ls, FormFunctionClosure( ff));
		lua_pushcclosure( ls, function_formfunction_call, 1);
		return 1;
	}
	else if (ctx->getPeerFormFunction( name, pff))
	{
		LuaObject<PeerFormFunctionClosure>::push_luastack( ls, PeerFormFunctionClosure( pff));
		lua_pushcclosure( ls, function_peerformfunction_call, 1);
		return 1;
	}
	else if (ctx->getPeerFunction( name, pf))
	{
		throw std::runtime_error( "calling peer function without parameter/result form directly. Only functions configured as formfunction or peerformfunction are supported");
	}
	else
	{
		throw std::runtime_error( "form function not found");
	}
}


static bool get_printop( lua_State* ls, int index, const char*& elem, std::size_t& elemsize)
{
	int tp;
	LuaExceptionHandlerScope escope(ls);
	switch (tp=lua_type( ls, index))
	{
		case LUA_TNIL:
			elem = 0;
			elemsize = 0;
			return true;
		case LUA_TBOOLEAN:
			if (!lua_toboolean( ls, index))
			{
				elem = 0;
				elemsize = 0;
				return true;
			}
			else
			{
				const char* tn = lua_typename( ls, tp);
				luaL_error( ls, "invalid type (%s) of print argument %d", tn?tn:"unknown", index-1);
			}
		case LUA_TNUMBER:
		case LUA_TSTRING:
			elem = lua_tolstring( ls, index, &elemsize);
			return true;
	}
	return false;
}


LUA_FUNCTION_THROWS( "output:print(..)", function_output_print_object)
{
	RedirectFilterClosure* closure = (RedirectFilterClosure*)lua_touserdata( ls, -1);
	lua_pop( ls, 1);
	if (!closure->call())
	{
		lua_pushlightuserdata( ls, closure);
		lua_yieldk( ls, 0, 1, function_output_print_object);
	}
	return 0;
}


LUA_FUNCTION_THROWS( "output:print(..)", function_output_print)
{
	const char* item[2] = {0,0};
	std::size_t itemsize[2] = {0,0};

	Output* output = LuaObject<Output>::getSelf( ls, "output", "print");
	switch (lua_gettop( ls))
	{
		case 1:
			break;
		case 3:
			if (!get_printop( ls, 3, item[1], itemsize[1]))
			{
				throw std::runtime_error( "invalid type of 2nd argument");
			}
			/*no break here !*/
		case 2:
			if (!get_printop( ls, 2, item[0], itemsize[0]))
			{
				if (item[1] != 0) throw std::runtime_error( "invalid type of first argument");
				{
					TypedInputFilterR inp = get_operand_TypedInputFilter( ls, 2);
					TypedOutputFilterR outp( new TypingOutputFilter( output->outputfilter()));
					LuaObject<RedirectFilterClosure>::push_luastack( ls, RedirectFilterClosure( inp, outp));
					RedirectFilterClosure* closure = LuaObject<RedirectFilterClosure>::get( ls, -1);
					lua_pushvalue( ls, 2);			//... iterator argument
					lua_pushlightuserdata( ls, closure);	//... redirect closure object
				}
				return function_output_print_object( ls);
			}
			break;
		default:
			throw std::runtime_error( "too many arguments");
	}
	if (!output->print( item[1]/*tag*/, itemsize[1], item[0]/*val*/, itemsize[0]))
	{
		lua_yieldk( ls, 0, 1, function_output_print);
	}
	return 0;
}


LUA_FUNCTION_THROWS( "output:opentag(..)", function_output_opentag)
{
	Output* output = LuaObject<Output>::getSelf( ls, "output", "opentag");
	check_parameters( ls, 1, 1, LUA_TSTRING);
	const char* tag = lua_tostring( ls, 2);
	std::size_t tagsize = std::strlen( tag);

	if (!output->print( tag, tagsize, 0/*val*/, 0))
	{
		lua_yieldk( ls, 0, 1, function_output_opentag);
	}
	return 0;
}


LUA_FUNCTION_THROWS( "output:closetag(..)", function_output_closetag)
{
	Output* output = LuaObject<Output>::getSelf( ls, "output", "closetag");
	check_parameters( ls, 1, 0);

	if (!output->print( 0/*tag*/, 0, 0/*val*/, 0))
	{
		lua_yieldk( ls, 0, 1, function_output_closetag);
	}
	return 0;
}


LUA_FUNCTION_THROWS( "filter(..)", function_filter)
{
	switch (lua_gettop( ls))
	{
		case 0:
		{
			LuaExceptionHandlerScope escope(ls);
			{
				Input* input = LuaObject<Input>::getGlobal( ls, "input");
				Output* output = LuaObject<Output>::getGlobal( ls, "output");
				Filter flt( input->inputfilter(), output->outputfilter());
				LuaObject<Filter>::push_luastack( ls, flt);
				return 1;
			}
		}
		case 1:
		{
			const char* name = lua_tostring( ls, 1);
			if (!name) throw std::runtime_error( "filter name is not a string");

			GlobalContext* ctx = getGlobalSingletonPointer<GlobalContext>( ls);
			if (!ctx)
			{
				throw std::runtime_error( "lost global context");
			}
			Filter flt;
			if (!ctx->getFilter( name, flt))
			{
				throw std::runtime_error( "filter not defined");
			}
			LuaObject<Filter>::push_luastack( ls, flt);
			return 1;
		}
		default: throw std::runtime_error( "too many arguments");
	}
}


LUA_FUNCTION_THROWS( "input:as(..)", function_input_as)
{
	Input* input = LuaObject<Input>::getSelf( ls, "input", "as");
	check_parameters( ls, 1, 1, LUA_TUSERDATA);

	Filter* filter = LuaObject<Filter>::get( ls, 2);
	if (!filter)
	{
		throw std::runtime_error( "filter object expected as first argument");
	}
	InputFilter* ff = 0;
	if (filter->inputfilter().get())
	{
		ff = filter->inputfilter()->copy();
		if (input->inputfilter().get())
		{
			// assign the rest of the input to the new filter attached
			const void* chunk;
			std::size_t chunksize;
			bool chunkend;
			input->inputfilter()->getRest( chunk, chunksize, chunkend);
			ff->putInput( chunk, chunksize, chunkend);
		}
		input->inputfilter().reset( ff);
	}
	else
	{
		throw std::runtime_error( "called with undefined input for the argument filter object");
	}
	return 0;
}



LUA_FUNCTION_THROWS( "input:doctype()", function_input_doctype)
{
	Input* input = LuaObject<Input>::getSelf( ls, "input", "doctype");
	check_parameters( ls, 1, 0, LUA_TUSERDATA);

	if (!input->inputfilter().get())
	{
		lua_pushnil( ls);
		return 1;
	}
	{
		std::string doctype;
		if (input->inputfilter()->getDocType( doctype))
		{
			LuaExceptionHandlerScope escope(ls);
			{
				lua_pushlstring( ls, doctype.c_str(), doctype.size());
				return 1;
			}

		}
	}
	lua_yieldk( ls, 0, 1, function_input_doctype);
	return 0;
}



LUA_FUNCTION_THROWS( "output:as(..)", function_output_as)
{
	Output* output = LuaObject<Output>::getSelf( ls, "output", "as");	//< self argument (mandatory)
	Filter* filter;								//< 1st argument (mandatory)
	const char* doctype = 0;						//< 2nd argument (optional)
	switch (lua_gettop( ls))
	{
		case 3:
			doctype = lua_tostring( ls, 3);
			if (!doctype) throw std::runtime_error( "string expected as second argument");
			/*no break here!*/
		case 2:
			filter = LuaObject<Filter>::get( ls, 2);
			if (!filter) throw std::runtime_error( "filter object expected as first argument");
			break;
		case 1:
			throw std::runtime_error( "too few arguments");
		default:
			throw std::runtime_error( "too many arguments");
	}
	OutputFilter* ff = 0;
	if (filter->outputfilter().get())
	{
		ff = filter->outputfilter()->copy();
		if (output->outputfilter().get())
		{
			ff->assignState( *output->outputfilter());
			if (doctype) ff->setDocType( doctype);
		}
	}
	else
	{
		throw std::runtime_error( "called with undefined output for the argument filter object");
	}
	output->outputfilter().reset( ff);
	return 0;
}



LUA_FUNCTION_THROWS( "input:get()", function_input_get)
{
	Input* input = LuaObject<Input>::getSelf( ls, "input", "get");
	check_parameters( ls, 1, 0);

	if (!input->inputfilter().get())
	{
		return luaL_error( ls, "no filter defined for input with input:as(..)");
	}
	LuaObject<InputFilterClosure>::push_luastack( ls, input->inputfilter());
	lua_pushcclosure( ls, function_inputFilter_get, 1);
	return 1;
}



LUA_FUNCTION_THROWS( "input:table()", function_input_table)
{
	RedirectFilterClosure* obj;
	int ctx;
	if (lua_getctx( ls, &ctx) != LUA_YIELD)
	{
		Input* input = LuaObject<Input>::getSelf( ls, "input", "table");
		check_parameters( ls, 1, 0);
		TypedInputFilterR inp( new TypingInputFilter( input->inputfilter()));
		TypedOutputFilterR outp( new LuaTableOutputFilter( ls));
		LuaObject<RedirectFilterClosure>::push_luastack( ls, RedirectFilterClosure( inp, outp));
		obj = LuaObject<RedirectFilterClosure>::get( ls, -1);
	}
	else
	{
		obj = (RedirectFilterClosure*)lua_touserdata( ls, -1);
		lua_pop( ls, 1);
	}
	if (!obj->call())
	{
		lua_pushlightuserdata( ls, obj);
		lua_yieldk( ls, 0, 1, function_input_table);
	}
	return 1;
}




LUA_FUNCTION_THROWS( "logger.print(..)", function_logger_print)
{
	/* first parameter maps to a log level, rest gets printed depending on
	 * whether it's a string or a number
	 */
	int ii,nn = lua_gettop(ls);
	if (nn <= 0)
	{
		throw std::runtime_error( "missing arguments");
	}
	const char *logLevel = lua_tostring( ls, 1);
	if (!logLevel)
	{
		throw std::runtime_error( "first argument log level is not a string");
	}
	std::string logmsg;

	for (ii=2; ii<=nn; ii++)
	{
		if (!getDescription( ls, ii, logmsg))
		{
			throw std::runtime_error( "failed to map arguments to strings");
		}
	}
	_Wolframe::log::LogLevel::Level lv = _Wolframe::log::LogLevel::strToLogLevel( logLevel);
	if (lv == _Wolframe::log::LogLevel::LOGLEVEL_UNDEFINED)
	{
		throw std::runtime_error( "first argument is an undefined loglevel");
	}
	else
	{
		_Wolframe::log::Logger( _Wolframe::log::LogBackend::instance() ).Get( lv )
			<< _Wolframe::log::LogComponent::LogLua
			<< logmsg;
	}
	return 0;
}



static const luaL_Reg logger_methodtable[ 2] =
{
	{"print",&function_logger_print},
	{0,0}
};

static const luaL_Reg input_methodtable[ 5] =
{
	{"as",&function_input_as},
	{"table",&function_input_table},
	{"doctype",function_input_doctype},
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

static const luaL_Reg typedinputfilter_methodtable[ 4] =
{
	{"table",&function_typedinputfilter_table},
	{"get", &function_typedinputfilter_get},
	{"__tostring",&function_typedinputfilter_tostring},
	{0,0}
};

static const luaL_Reg struct_methodtable[ 4] =
{
	{"table",&function_struct_table},
	{"get", &function_struct_get},
	{"__tostring",&function_struct_tostring},
	{0,0}
};

static const luaL_Reg ddlform_methodtable[ 5] =
{
	{"table",&function_form_table},
	{"get",&function_form_get},
	{"fill",&function_form_fill},
	{"__tostring",&function_form_tostring},
	{0,0}
};


LuaScript::LuaScript( const std::string& path_)
	:m_path(path_)
{
	// Load the source of the script from file
	char buf;
	std::fstream ff;
	ff.open( path_.c_str(), std::ios::in);
	while (ff.read( &buf, sizeof(buf)))
	{
		m_content.push_back( buf);
	}
	if ((ff.rdstate() & std::ifstream::eofbit) == 0)
	{
		std::ostringstream msg;
		msg << "failed to read lua script from file: '" << path_ << "'";
		throw std::runtime_error( msg.str());
	}
	ff.close();

	// Check the script syntax and get the list of all global functions
	LuaScriptInstance instance( this);
	lua_State* ls = instance.ls();

	lua_rawgeti( ls, LUA_REGISTRYINDEX, LUA_RIDX_GLOBALS);
	lua_pushnil(ls);
	while (lua_next( ls, -2))
	{
		if (lua_isfunction( ls, -1) && lua_isstring( ls, -2))
		{
			m_functions.push_back( lua_tostring( ls, -2));
		}
		lua_pop( ls, 1);
	}
}

LuaScriptInstance::LuaScriptInstance( const LuaScript* script_)
	:m_ls(0),m_thread(0),m_threadref(0),m_script(script_)
{
	m_ls = luaL_newstate();
	if (!m_ls) throw std::runtime_error( "failed to create lua state");

	LuaExceptionHandlerScope luaThrows(m_ls);
	{
		// create thread and prevent garbage collecting of it (http://permalink.gmane.org/gmane.comp.lang.lua.general/22680)
		m_thread = lua_newthread( m_ls);
		lua_pushvalue( m_ls, -1);
		m_threadref = luaL_ref( m_ls, LUA_REGISTRYINDEX);

		if (luaL_loadbuffer( m_ls, m_script->content().c_str(), m_script->content().size(), m_script->path().c_str()))
		{
			std::ostringstream buf;
			buf << "Failed to load script '" << m_script->path() << "':" << lua_tostring( m_ls, -1);
			throw std::runtime_error( buf.str());
		}
		// open standard lua libraries
		luaL_openlibs( m_ls);

		// register logging function already here because then it can be used in the script initilization part
		Logger logger_;
		LuaObject<Logger>::createGlobal( m_ls, "logger", logger_, logger_methodtable);

		// open additional libraries defined for this script
		std::vector<LuaScript::Module>::const_iterator ii=m_script->modules().begin(), ee=m_script->modules().end();
		for (;ii!=ee; ++ii)
		{
			if (ii->m_initializer( m_ls))
			{
				std::ostringstream buf;
				buf << "module '" << ii->m_name << "' initialization failed: " << lua_tostring( m_ls, -1);
				throw std::runtime_error( buf.str());
			}
		}

		// call main, we may have to initialize LUA modules there
		if (lua_pcall( m_ls, 0, LUA_MULTRET, 0) != 0)
		{
			std::ostringstream buf;
			buf << "Unable to call main entry of script: " << lua_tostring( m_ls, -1 );
			throw std::runtime_error( buf.str());
		}
	}
}

LuaScriptInstance::~LuaScriptInstance()
{
	if (m_ls)
	{
		luaL_unref( m_ls, LUA_REGISTRYINDEX, m_threadref);
		lua_close( m_ls);
	}
}

LuaFunctionMap::~LuaFunctionMap()
{
	std::vector<LuaScript*>::iterator ii=m_ar.begin(),ee=m_ar.end();
	while (ii != ee)
	{
		delete *ii;
		++ii;
	}
}

void LuaFunctionMap::defineLuaFunction( const std::string& name, const LuaScript& script)
{
	std::string nam( name);
	std::transform( nam.begin(), nam.end(), nam.begin(), ::tolower);
	{
		std::map<std::string,std::size_t>::const_iterator ii=m_procmap.find( nam),ee=m_procmap.end();
		if (ii != ee)
		{
			std::ostringstream buf;
			buf << "duplicate definition of function '" << nam << "'";
			throw std::runtime_error( buf.str());
		}
	}
	std::size_t scriptId;
	std::map<std::string,std::size_t>::const_iterator ii=m_pathmap.find( script.path()),ee=m_pathmap.end();
	if (ii != ee)
	{
		scriptId = ii->second;
	}
	else
	{
		scriptId = m_ar.size();
		m_ar.push_back( new LuaScript( script));
		m_pathmap[ script.path()] = scriptId;
	}
	m_procmap[ nam] = scriptId;
}

bool LuaFunctionMap::getLuaScriptInstance( const std::string& procname, LuaScriptInstanceR& rt) const
{
	std::string nam( procname);
	std::transform( nam.begin(), nam.end(), nam.begin(), ::tolower);

	std::map<std::string,std::size_t>::const_iterator ii=m_procmap.find( nam),ee=m_procmap.end();
	if (ii == ee) return false;
	rt = LuaScriptInstanceR( new LuaScriptInstance( m_ar[ ii->second]));
	return true;
}

bool LuaFunctionMap::initLuaScriptInstance( LuaScriptInstance* lsi, const Input& input_, const Output& output_)
{
	GlobalContext* gc = dynamic_cast<GlobalContext*>( this);
	if (!gc)
	{
		std::ostringstream msg;
		msg << "cannot get global context via RTTI from Lua function map";
		throw std::runtime_error( msg.str());
	}
	lua_State* ls = lsi->ls();
	try
	{
		LuaExceptionHandlerScope luaThrows(ls);
		{
			LuaObject<RedirectFilterClosure>::createMetatable( ls, 0, 0, 0);
			LuaObject<DDLForm>::createMetatable( ls, 0, 0, ddlform_methodtable);
			LuaObject<serialize::DDLStructParser>::createMetatable( ls, 0, 0, 0);
			LuaObject<serialize::DDLStructSerializer>::createMetatable( ls, 0, 0, 0);
			LuaObject<serialize::StructSerializer>::createMetatable( ls, 0, 0, struct_methodtable);
			LuaObject<InputFilterClosure>::createMetatable( ls, 0, 0, 0);
			LuaObject<TypedInputFilterR>::createMetatable( ls, 0, 0, typedinputfilter_methodtable);
			LuaObject<TypedInputFilterClosure>::createMetatable( ls, 0, 0, 0);
			LuaObject<FormFunctionClosure>::createMetatable( ls, 0, 0, 0);
			LuaObject<PeerFormFunctionClosure>::createMetatable( ls, 0, 0, 0);

			LuaObject<Input>::createGlobal( ls, "input", input_, input_methodtable);
			LuaObject<Output>::createGlobal( ls, "output", output_, output_methodtable);
			LuaObject<Filter>::createMetatable( ls, &function__LuaObject__index<Filter>, &function__LuaObject__newindex<Filter>, 0);
			setGlobalSingletonPointer<GlobalContext>( ls, gc);
			lua_pushcfunction( ls, &function_filter);
			lua_setglobal( ls, "filter");
			lua_pushcfunction( ls, &function_form);
			lua_setglobal( ls, "form");
			lua_pushcfunction( ls, &function_formfunction);
			lua_setglobal( ls, "formfunction");
			lua_pushcfunction( ls, &function_scope);
			lua_setglobal( ls, "scope");
		}
		return true;
	}
	catch (const std::exception& e)
	{
		std::ostringstream msg;
		msg << "error intializing lua script instance. " << e.what();
		throw std::runtime_error( msg.str());
	}
	return false;
}

