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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
///\file luaObjects.cpp
#include "luaObjects.hpp"
#include "luaDebug.hpp"
#include "luafilter.hpp"
#include "luaGetFunctionClosure.hpp"
#include "langbind/appObjects.hpp"
#include "langbind/luaException.hpp"
#include "langbind/luaCppCall.hpp"
#include "langbind/normalizeFunction.hpp"
#include "filter/typingfilter.hpp"
#include "filter/typedfilterScope.hpp"
#include "filter/inputfilterScope.hpp"
#include "filter/tostringfilter.hpp"
#include "types/doctype.hpp"
#include "utils/fileUtils.hpp"
#include "logger-v1.hpp"
#include <limits>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <cstddef>
#include <cstdarg>
#include <boost/lexical_cast.hpp>
#include <boost/type_traits/remove_cv.hpp>
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
	static const char* Logger = "wolframe.Logger";
	static const char* Input = "wolframe.Input";
	static const char* Output = "wolframe.Output";
	static const char* Filter = "wolframe.Filter";
	static const char* RedirectFilterClosure = "wolframe.RedirectFilterClosure";
	static const char* Form = "wolframe.Form";
	static const char* DDLFormParser = "wolframe.DDLFormParser";
	static const char* DDLFormSerializer = "wolframe.DDLFormSerializer";
	static const char* InputFilterClosure = "wolframe.InputFilterClosure";
	static const char* TypedInputFilterR = "wolframe.TypedInputFilterR";
	static const char* TypedInputFilterClosure = "wolframe.TypedInputFilterClosure";
	static const char* FormFunctionClosureR = "wolframe.FormFunctionClosureR";
	static const char* Transaction = "wolframe.Transaction";
	static const char* NormalizeFunction = "wolframe.NormalizeFunction";
	static const char* StructSerializer = "wolframe.StructSerializer";
	static const char* ProcessorProvider = ":wolframe.ProcessorProvider";
	static const char* LuaModuleMap = ":wolframe.LuaModuleMap";
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
template <> const char* metaTableName<types::FormR>()			{return luaname::Form;}
template <> const char* metaTableName<DDLFormParser>()			{return luaname::DDLFormParser;}
template <> const char* metaTableName<DDLFormSerializer>()		{return luaname::DDLFormSerializer;}
template <> const char* metaTableName<InputFilterClosure>()		{return luaname::InputFilterClosure;}
template <> const char* metaTableName<TypedInputFilterR>()		{return luaname::TypedInputFilterR;}
template <> const char* metaTableName<TypedInputFilterClosure>()	{return luaname::TypedInputFilterClosure;}
template <> const char* metaTableName<FormFunctionClosureR>()		{return luaname::FormFunctionClosureR;}
template <> const char* metaTableName<db::TransactionR>()		{return luaname::Transaction;}
template <> const char* metaTableName<types::NormalizeFunction>()	{return luaname::NormalizeFunction;}
template <> const char* metaTableName<serialize::StructSerializer>()	{return luaname::StructSerializer;}
template <> const char* metaTableName<proc::ProcessorProvider>()	{return luaname::ProcessorProvider;}
template <> const char* metaTableName<langbind::LuaModuleMap>()		{return luaname::LuaModuleMap;}
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
	typedef typename boost::remove_cv<ObjectType>::type ObjectType_ncv;
	ObjectType_ncv* obj_ncv = const_cast<ObjectType_ncv*>( obj);
	lua_pushlightuserdata( ls, obj_ncv);
	lua_setglobal( ls, metaTableName<ObjectType_ncv>());
}

template <class ObjectType>
static ObjectType* getGlobalSingletonPointer( lua_State* ls)
{
	typedef typename boost::remove_cv<ObjectType>::type ObjectType_ncv;
	lua_getglobal( ls, metaTableName<ObjectType_ncv>());
	ObjectType* rt = (ObjectType*)lua_touserdata( ls, -1);
	if (!rt) throw std::runtime_error("global context not defined");
	lua_pop( ls, 1);
	return rt;
}

static void setGlobalModuleMap( lua_State* ls, const langbind::LuaModuleMap* modulemap_)
{
	return setGlobalSingletonPointer<const langbind::LuaModuleMap>( ls, modulemap_);
}

static const LuaModuleMap* getLuaModuleMap( lua_State* ls)
{
	const LuaModuleMap* rt = getGlobalSingletonPointer<const LuaModuleMap>( ls);
	if (!rt) throw std::runtime_error( "module map undefined");
	return rt;
}

static void setProcessorProvider( lua_State* ls, const proc::ProcessorProvider* provider_)
{
	return setGlobalSingletonPointer<const proc::ProcessorProvider>( ls, provider_);
}

static const proc::ProcessorProvider* getProcessorProvider( lua_State* ls)
{
	const proc::ProcessorProvider* rt = getGlobalSingletonPointer<proc::ProcessorProvider>( ls);
	if (!rt) throw std::runtime_error( "processor provider undefined");
	return rt;
}

class DDLTypeMap :public types::NormalizeFunctionMap
{
public:
	DDLTypeMap( lua_State* ls)
		:m_provider( getProcessorProvider( ls)){}

	virtual const types::NormalizeFunction* get( const std::string& name) const
	{
		return m_provider->normalizeFunction( name);
	}

private:
	const proc::ProcessorProvider* m_provider;
};


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
			msg << "expected " << (expectname?expectname:"?") << " instead of " << (typname?typname:"?") << " as argument " << (ii-si+1);
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
			lua_pushvalue( ls, idx);
			rt.reset( new LuaTableInputFilter( ls));
			break;

		case LUA_TFUNCTION:
			if (lua_getupvalue( ls, idx, 1))
			{
				InputFilterClosure* ic = LuaObject<InputFilterClosure>::get( ls, -1);
				if (ic)
				{
					if (!ic->isValidAsScope())
					{
						throw std::runtime_error( "Illegal use of iterator as function argument. Only allowed at the beginning or after an open tag");
					}
					rt.reset( new TypingInputFilter( ic->inputfilter()));
					break;
				}
				TypedInputFilterClosure* tc = LuaObject<TypedInputFilterClosure>::get( ls, -1);
				if (tc)
				{
					if (!tc->isValidAsScope())
					{
						throw std::runtime_error( "Illegal use of iterator as function argument. Only allowed at the beginning or after an open tag");
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




LUA_FUNCTION_THROWS( "module()", function_module)
{
	check_parameters( ls, 0, 1, LUA_TSTRING);
	const char* modulename = lua_tostring( ls, 1);
	if (!modulename) throw std::runtime_error( "module name is not a string");
	const LuaModuleMap* modmap = getLuaModuleMap( ls);
	LuaModule mod;
	if (!modmap->getLuaModule( modulename, mod)) throw std::runtime_error( std::string( "module '") + modulename + "' not defined");
	mod.load( ls);
	return 0;
}

LUA_FUNCTION_THROWS( "module()", function_module_dummy)
{
	check_parameters( ls, 0, 1, LUA_TSTRING);
	const char* modulename = lua_tostring( ls, 1);
	if (!modulename) throw std::runtime_error( "module name is not a string");
	return 0;
}


LUA_FUNCTION_THROWS( "<structure>:get()", function_inputfilterClosure_get)
{
	InputFilterClosure* closure = LuaObject<InputFilterClosure>::get( ls, lua_upvalueindex( 1));
	LuaExceptionHandlerScope escope(ls);
	switch (closure->fetch( ls))
	{
		case InputFilterClosure::DoYield:
			lua_yieldk( ls, 0, 1, function_inputfilterClosure_get);

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

static void getVariantValue( lua_State* ls, types::Variant& val, int idx)
{
	std::size_t val_len;
	const char* val_str;
	switch (lua_type( ls, idx))
	{
		case LUA_TBOOLEAN:
			val = lua_toboolean( ls, idx);
			break;

		case LUA_TNUMBER:
			val = lua_tonumber( ls, idx);
			break;

		case LUA_TSTRING:
			val_str = lua_tolstring( ls, idx, &val_len);
			val.initConstant( val_str, val_len);
			break;

		default:
			throw std::runtime_error( "atomic value expected for 'variant type' argument");
	}
}

static void pushVariantValue( lua_State* ls, const types::Variant& val)
{
	unsigned int val_uint;
	switch (val.type())
	{
		case types::Variant::Null:
			lua_pushnil( ls);
			break;

		case types::Variant::Bool:
			lua_pushboolean( ls, val.tobool());
			break;

		case types::Variant::Int:
			lua_pushinteger( ls, val.toint());
			break;

		case types::Variant::UInt:
			val_uint = val.touint();
			if (val_uint > (unsigned int)std::numeric_limits<int>::max())
			{
				lua_pushnumber( ls, val_uint);
			}
			else
			{
				lua_pushinteger( ls, val_uint);
			}
			break;

		case types::Variant::Double:
			lua_pushnumber( ls, val.todouble());
			break;

		case types::Variant::String:
		{
			LuaExceptionHandlerScope escope(ls);
			{
				lua_pushlstring( ls, val.charptr(), val.charsize());
			}
			break;
		}
		default:
			throw std::runtime_error( "atomic value expected for 'variant type' argument");
	}
}

LUA_FUNCTION_THROWS( "<normalizer>(..)", function_normalizer_call)
{
	types::NormalizeFunction* func = (types::NormalizeFunction*)lua_touserdata( ls, lua_upvalueindex( 1));
	if (lua_gettop( ls) != 1)
	{
		throw std::runtime_error( "atomic value expected for 'variant type' argument");

	}
	types::Variant param;
	getVariantValue( ls, param, 1);
	types::Variant result = func->execute( param);
	pushVariantValue( ls, result);
	return 1;
}

LUA_FUNCTION_THROWS( "normalizer(..)", function_normalizer)
{
	check_parameters( ls, 0, 1, LUA_TSTRING);
	const char* name = lua_tostring( ls, 1);
	const proc::ProcessorProvider* ctx = getProcessorProvider( ls);

	const types::NormalizeFunction* func = ctx->normalizeFunction( name);
	if (!func) throw std::runtime_error( std::string("normalizer '") + name + "' not defined");

	lua_pushlightuserdata( ls, const_cast<types::NormalizeFunction*>(func));
	lua_pushcclosure( ls, function_normalizer_call, 1);
	return 1;
}


LUA_FUNCTION_THROWS( "scope(..)", function_scope)
{
	check_parameters( ls, 0, 1, LUA_TFUNCTION);
	if (lua_getupvalue( ls, 1, 1))
	{
		InputFilterClosure* ic = LuaObject<InputFilterClosure>::get( ls, -1);
		if (ic)
		{
			LuaObject<InputFilterClosure>::push_luastack( ls, ic->scope());
			lua_pushcclosure( ls, function_inputfilterClosure_get, 1);
			return 1;
		}
		TypedInputFilterClosure* tc = LuaObject<TypedInputFilterClosure>::get( ls, -1);
		if (tc)
		{
			LuaObject<TypedInputFilterClosure>::push_luastack( ls, tc->scope());
			lua_pushcclosure( ls, function_typedinputfilterClosure_get, 1);
			return 1;
		}
	}
	throw std::runtime_error( "iterator on input or on a form expected as argument");
}


LUA_FUNCTION_THROWS( "form:__tostring()", function_form_tostring)
{
	types::FormR* form = LuaObject<types::FormR>::getSelf( ls, "form", "__tostring");
	check_parameters( ls, 1, 0);

	ToStringFilter* flt = new ToStringFilter();
	TypedOutputFilterR out( flt);

	DDLFormSerializer ser( *form);
	ser.init( out, serialize::Context::SerializeWithIndices);
	if (!ser.call())
	{
		if (out->state() == OutputFilter::EndOfBuffer)
		{
			throw std::logic_error( "internal: tostring serialization with yield");
		}
		else
		{
			throw std::runtime_error( ser.getError());
		}
	}
	std::string content;
	content.append( "FORM ");
	content.append( (*form)->description()->name());
	content.append( "\n");
	content.append( flt->content());

	LuaExceptionHandlerScope escope(ls);
	{
		lua_pushlstring( ls, content.c_str(), content.size());
		return 1;
	}
}


LUA_FUNCTION_THROWS( "form:name()", function_form_name)
{
	types::FormR* form = LuaObject<types::FormR>::getSelf( ls, "form", "doctype");
	check_parameters( ls, 1, 0);

	LuaExceptionHandlerScope escope(ls);
	{
		const types::FormDescription* descr = (*form)->description();
		lua_pushlstring( ls, descr->name().c_str(), descr->name().size());
		return 1;
	}
}

static types::VariantStruct* get_substructure( lua_State* ls, int index, types::VariantStruct* st)
{
	if (!lua_istable( ls, index)) throw std::runtime_error( "table (array) expected as substructure path");
	lua_pushnil(ls);
	while (lua_next( ls, index))
	{
		if (!lua_isnumber( ls, -2)) throw std::runtime_error( "array expected as substructure path");
		if (!lua_isstring( ls, -1)) throw std::runtime_error( "array of strings (identifiers) expected as substructure path");
		st = st->select( lua_tostring( ls, -1));
		if (!st) throw std::runtime_error( "selected substructure does not exist in form");
		lua_pop( ls, 1);
	}
	return st;
}

LUA_FUNCTION_THROWS( "form:fill()", function_form_fill)
{
	types::FormR* form = LuaObject<types::FormR>::getSelf( ls, "form", "fill");
	DDLFormParser* closure;
	int ctx;
	if (lua_getctx( ls, &ctx) != LUA_YIELD)
	{
		types::VariantStruct* substruct = form->get();
		serialize::Context::Flags flags = serialize::Context::None;
		int ii = 3, nn = lua_gettop( ls);
		if (nn > 4) throw std::runtime_error( "too many arguments");
		for (; ii <= nn; ++ii)
		{
			if (lua_istable( ls, ii))
			{
				substruct = get_substructure( ls, ii, substruct);
			}
			else if (lua_isstring( ls, ii))
			{
				const char* mode = lua_tostring( ls, ii);
				if (std::strcmp( mode, "strict") == 0)
				{
					flags = (serialize::Context::Flags)((int)serialize::Context::ValidateAttributes|(int)serialize::Context::ValidateInitialization);
				}
				else if (std::strcmp( mode, "complete") == 0)
				{
					flags = serialize::Context::ValidateInitialization;
				}
				else if (std::strcmp( mode, "relaxed") == 0)
				{
					flags = serialize::Context::None;
				}
				else
				{
					throw std::runtime_error( "2nd argument does not specify a mode of validating input (e.g. \"strict\")");
				}
			}
			else
			{
				static const char* ia[3] = {"2nd","3rd","4th"};
				throw std::runtime_error( std::string(ia[ii-3]) + " argument is not a string or table");
			}
		}
		TypedInputFilterR inp = get_operand_TypedInputFilter( ls, 2);
		LuaObject<DDLFormParser>::push_luastack( ls, DDLFormParser( *form, substruct));
		closure = LuaObject<DDLFormParser>::get( ls, -1);
		closure->init( inp, flags);
		lua_pushvalue( ls, 2);		//... iterator argument (table, generator function, etc.)
	}
	else
	{
		closure = (DDLFormParser*)lua_touserdata( ls, -1);
		lua_pop( ls, 1);
	}
	if (!closure->call())
	{
		lua_pushlightuserdata( ls, closure);
		lua_yieldk( ls, 0, 1, function_form_fill);
	}
	LuaObject<types::FormR>::push_luastack( ls, *form);
	return 1;
}


LUA_FUNCTION_THROWS( "form:table()", function_form_table)
{
	types::FormR* form;
	DDLFormSerializer* result;
	int ctx;
	if (lua_getctx( ls, &ctx) != LUA_YIELD)
	{
		form = LuaObject<types::FormR>::getSelf( ls, "form", "table");

		types::VariantStruct* substruct = form->get();
		int nn = lua_gettop( ls);
		if (nn > 1)
		{
			if (nn > 2) throw std::runtime_error( "too many arguments");
			substruct = get_substructure( ls, 2, substruct);
		}
		TypedOutputFilterR outp( new LuaTableOutputFilter( ls));
		LuaObject<DDLFormSerializer>::push_luastack( ls, DDLFormSerializer( *form, substruct));
		result = LuaObject<DDLFormSerializer>::get( ls, -1);
		result->init( outp, serialize::Context::SerializeWithIndices);
	}
	else
	{
		form = (types::FormR*)lua_touserdata( ls, -2);
		result = (DDLFormSerializer*)lua_touserdata( ls, -1);
		lua_pop( ls, 2);
	}
	if (!result->call())
	{
		lua_pushlightuserdata( ls, form);
		lua_pushlightuserdata( ls, result);
		lua_yieldk( ls, 0, 1, function_form_table);
	}
	if (lua_type( ls, -1) == LUA_TUSERDATA)
	{
		//... if LuaTableOutputFilter::print is never called then no object is left on the stack
		lua_newtable( ls);
	}
	return 1;
}


LUA_FUNCTION_THROWS( "form:get()", function_form_get)
{
	types::FormR* result = LuaObject<types::FormR>::getSelf( ls, "form", "get");
	types::VariantStruct* substruct = result->get();
	int nn = lua_gettop( ls);
	if (nn > 1)
	{
		if (nn > 2) throw std::runtime_error( "too many arguments");
		substruct = get_substructure( ls, 2, substruct);
	}
	TypedInputFilterR itr( new DDLFormSerializer( *result, substruct));
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
		const proc::ProcessorProvider* ctx = getProcessorProvider( ls);
		const types::FormDescription* st = ctx->formDescription( name);
		if (!st) throw std::runtime_error( std::string("form '") + name + "' not defined");
		types::FormR frm( new types::Form( st));
		LuaObject<types::FormR>::push_luastack( ls, frm);
		return 1;
	}
	throw std::runtime_error( "expected string as argument of form");
}


LUA_FUNCTION_THROWS( "<structure>:__tostring()", function_struct_tostring)
{
	serialize::StructSerializer* obj = LuaObject<serialize::StructSerializer>::getSelf( ls, "<structure>", "__tostring");
	check_parameters( ls, 1, 0);

	LuaObject<serialize::StructSerializer>::push_luastack( ls, *obj);
	obj = LuaObject<serialize::StructSerializer>::get( ls, -1);

	ToStringFilter* flt = new ToStringFilter();
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
	serialize::StructSerializer* obj;
	int ctx;
	if (lua_getctx( ls, &ctx) != LUA_YIELD)
	{
		obj = LuaObject<serialize::StructSerializer>::getSelf( ls, "<structure>", "table");
		check_parameters( ls, 1, 0);
		LuaObject<serialize::StructSerializer>::push_luastack( ls, *obj);
		obj = LuaObject<serialize::StructSerializer>::get( ls, -1);
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
	if (lua_type( ls, -1) == LUA_TUSERDATA)
	{
		//... if LuaTableOutputFilter::print is never called then no object is left on the stack
		lua_newtable( ls);
	}
	return 1;
}


LUA_FUNCTION_THROWS( "<structure>:get()", function_struct_get)
{
	serialize::StructSerializer* obj = LuaObject<serialize::StructSerializer>::getSelf( ls, "<structure>", "get");
	check_parameters( ls, 1, 0);
	obj = new serialize::StructSerializer( *obj);
	obj->reset();
	TypedInputFilterR itr( obj);
	LuaObject<TypedInputFilterClosure>::push_luastack( ls, TypedInputFilterClosure( itr));
	lua_pushcclosure( ls, function_typedinputfilterClosure_get, 1);
	return 1;
}


LUA_FUNCTION_THROWS( "<structure>:__tostring()", function_typedinputfilter_tostring)
{
	TypedInputFilterR* objref = LuaObject<TypedInputFilterR>::getSelf( ls, "<structure>", "__tostring");
	check_parameters( ls, 1, 0);
	if (!objref->get())
	{
		lua_pushnil( ls);
		return 1;
	}
	TypedInputFilterR obj = TypedInputFilterR( (*objref)->copy());
	obj->resetIterator();

	ToStringFilter* flt = new ToStringFilter();
	TypedOutputFilterR out( flt);
	RedirectFilterClosure exc( obj, out);
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
	RedirectFilterClosure* closure;
	int ctx;
	if (lua_getctx( ls, &ctx) != LUA_YIELD)
	{
		TypedInputFilterR* objref = LuaObject<TypedInputFilterR>::getSelf( ls, "<structure>", "table");
		if (!objref->get())
		{
			lua_pushnil( ls);
			return 1;
		}
		TypedInputFilterR obj = TypedInputFilterR( (*objref)->copy());
		obj->resetIterator();
		if (!obj->setFlags( TypedInputFilter::SerializeWithIndices))
		{
			throw std::runtime_error( "calling table() for object without input structure info");
		}
		TypedOutputFilterR outp( new LuaTableOutputFilter( ls));
		LuaObject<RedirectFilterClosure>::push_luastack( ls, RedirectFilterClosure( obj, outp));
		closure = LuaObject<RedirectFilterClosure>::get( ls, -1);
	}
	else
	{
		closure = (RedirectFilterClosure*)lua_touserdata( ls, -1);
		lua_pop( ls, 1);
	}
	if (!closure->call())
	{
		lua_pushlightuserdata( ls, closure);
		lua_yieldk( ls, 0, 1, function_typedinputfilter_table);
	}
	if (lua_type( ls, -1) == LUA_TUSERDATA)
	{
		//... if LuaTableOutputFilter::print is never called then no object is left on the stack
		lua_newtable( ls);
	}
	return 1;
}


LUA_FUNCTION_THROWS( "<structure>:get()", function_typedinputfilter_get)
{
	TypedInputFilterR* objref = LuaObject<TypedInputFilterR>::getSelf( ls, "<structure>", "get");
	if (!objref->get())
	{
		lua_pushnil( ls);
		return 1;
	}
	TypedInputFilterR obj = TypedInputFilterR( (*objref)->copy());
	obj->resetIterator();
	check_parameters( ls, 1, 0);

	LuaObject<TypedInputFilterClosure>::push_luastack( ls, TypedInputFilterClosure( obj));
	lua_pushcclosure( ls, function_typedinputfilterClosure_get, 1);
	return 1;
}


LUA_FUNCTION_THROWS( "<formfunction>(..)", function_formfunction_call)
{
	FormFunctionClosureR* closure = LuaObject<FormFunctionClosureR>::get( ls, lua_upvalueindex( 1));
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
			(*closure)->init( getProcessorProvider( ls), inp, serialize::Context::None);
		}
		lua_pushvalue( ls, 1);		//... iterator argument (table, generator function, etc.)
	}
	if (!(*closure)->call())
	{
		lua_yieldk( ls, 0, 1, function_formfunction_call);
	}
	LuaObject<TypedInputFilterR>::push_luastack( ls, (*closure)->result());
	return 1;
}

LUA_FUNCTION_THROWS( "transaction:begin(..)", function_transaction_begin)
{
	db::TransactionR* transaction = LuaObject<db::TransactionR>::getSelf( ls, "transaction", "begin");
	check_parameters( ls, 1, 1, LUA_TSTRING);
	const char* name = lua_tostring( ls, 2);
	if (transaction->get()) throw std::runtime_error( "subsequent transaction begins without commit or rollback");
	const proc::ProcessorProvider* ctx = getProcessorProvider( ls);
	transaction->reset( ctx->transaction( name));
	(*transaction)->begin();
	return 0;
}

LUA_FUNCTION_THROWS( "transaction:commit()", function_transaction_commit)
{
	db::TransactionR* transaction = LuaObject<db::TransactionR>::getSelf( ls, "transaction", "commit");
	check_parameters( ls, 1, 0);
	if (!transaction->get()) throw std::runtime_error( "commit called without calling transaction:begin() before");
	(*transaction)->commit();
	transaction->reset();
	return 0;
}

LUA_FUNCTION_THROWS( "transaction:rollback()", function_transaction_rollback)
{
	db::TransactionR* transaction = LuaObject<db::TransactionR>::getSelf( ls, "transaction", "commit");
	check_parameters( ls, 1, 0);
	if (!transaction->get()) throw std::runtime_error( "rollback called without calling transaction:begin() before");
	(*transaction)->rollback();
	transaction->reset();
	return 0;
}

LUA_FUNCTION_THROWS( "formfunction(..)", function_formfunction)
{
	check_parameters( ls, 0, 1, LUA_TSTRING);

	const char* name = lua_tostring( ls, 1);
	const proc::ProcessorProvider* ctx = getProcessorProvider( ls);
	const FormFunction* ff = ctx->formFunction( name);
	if (ff)
	{
		LuaObject<FormFunctionClosureR>::push_luastack( ls, FormFunctionClosureR( ff->createClosure()));
		lua_pushcclosure( ls, function_formfunction_call, 1);
		return 1;
	}
	throw std::runtime_error( std::string( "form function '") + name + "' not found");
}

static const char* userdata_tolstring( lua_State* ls, int index, std::size_t* len)
{
	lua_pushvalue( ls, index);		///...STK: udata
	lua_getmetatable( ls, -1);		///...STK: udata mt
	lua_pushliteral( ls, "__tostring");	///...STK: udata mt __tostring
	lua_rawget( ls, -2);			///...STK: udata mt mt[__tostring]
	if (lua_isnil( ls, -1)) return 0;
	lua_pushvalue( ls, index);		///... STK: udata mt mt[__tostring] udata
	lua_call( ls, 1, 1);			///... STK: udata mt str
	const char* rt = lua_tolstring( ls, -1, len);
	return rt;
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
				std::stringstream msg;
				msg << "Invalid type (" << (tn?tn:"unknown") << " of print argument " << (index-1);
				throw std::runtime_error( msg.str());
			}

		case LUA_TNUMBER:
		case LUA_TSTRING:
			elem = lua_tolstring( ls, index, &elemsize);
			return true;

		case LUA_TUSERDATA:
			elem = userdata_tolstring( ls, index, &elemsize);
			if (!elem)
			{
				std::stringstream msg;
				msg << "Value of print argument " << (index-1) << " is userdata and is not representable as string (has no '__tostring' method)";
				throw std::runtime_error( msg.str());
			}
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

LUA_FUNCTION_THROWS( "output:print(..)", function_output_print_object_metadata)
{
	RedirectFilterClosure* closure = (RedirectFilterClosure*)lua_touserdata( ls, -3);
	Input* input = (Input*)lua_touserdata( ls, -2);
	Output* output = (Output*)lua_touserdata( ls, -1);
	lua_pop( ls, 3);

	if (!output->called())
	{
		if (!input->inputfilter()->getMetadata())
		{
			lua_pushlightuserdata( ls, closure);
			lua_pushlightuserdata( ls, input);
			lua_pushlightuserdata( ls, output);
			lua_yieldk( ls, 0, 1, function_output_print_object_metadata);
		}
		output->called(true);
	}
	lua_pushlightuserdata( ls, closure);
	return function_output_print_object( ls);
}

LUA_FUNCTION_THROWS( "output:print(..)", function_output_print)
{
	const char* item[2] = {0,0};
	std::size_t itemsize[2] = {0,0};
	Output* output;
	int ctx;
	if (lua_getctx( ls, &ctx) != LUA_YIELD)
	{
		output = LuaObject<Output>::getSelf( ls, "output", "print");
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
					RedirectFilterClosure* closure;
					if (item[1] != 0) throw std::runtime_error( "invalid type of first argument");
					{
						TypedInputFilterR inp = get_operand_TypedInputFilter( ls, 2);
						TypedOutputFilterR outp( new TypingOutputFilter( output->outputfilter()));
						LuaObject<RedirectFilterClosure>::push_luastack( ls, RedirectFilterClosure( inp, outp));
						closure = LuaObject<RedirectFilterClosure>::get( ls, -1);
					}
					if (!output->called())
					{
						Input* input = LuaObject<Input>::getGlobal( ls, "input");
						bool md = input->inputfilter()->getMetadata();
						if (!md)
						{
							lua_pushvalue( ls, 2);			//... iterator argument
							lua_pushlightuserdata( ls, closure);	//... redirect closure object
							lua_pushlightuserdata( ls, input);
							lua_pushlightuserdata( ls, output);
							return function_output_print_object_metadata( ls);
						}
						output->called( true);
					}
					lua_pushvalue( ls, 2);			//... iterator argument
					lua_pushlightuserdata( ls, closure);	//... redirect closure object
					return function_output_print_object( ls);
				}
				break;
			default:
				throw std::runtime_error( "too many arguments");
		}
	}
	else
	{
		item[0] = (const char*)lua_touserdata( ls, -5);
		itemsize[0] = (std::size_t)lua_tointeger( ls, -4);
		item[1] = (const char*)lua_touserdata( ls, -3);
		itemsize[1] = (std::size_t)lua_tointeger( ls, -2);
		output = (Output*)lua_touserdata( ls, -1);
		lua_pop( ls, 5);
	}
	if (!output->called())
	{
		Input* input = LuaObject<Input>::getGlobal( ls, "input");
		if (!input->inputfilter()->getMetadata())
		{
			lua_pushlightuserdata( ls, const_cast<void*>( (const void*)item[0]));
			lua_pushinteger( ls, itemsize[0]);
			lua_pushlightuserdata( ls, const_cast<void*>( (const void*)item[1]));
			lua_pushinteger( ls, itemsize[1]);
			lua_pushlightuserdata( ls, output);
			lua_yieldk( ls, 0, 1, function_output_print);
		}
		output->called(true);
	}
	if (!output->print( item[1]/*tag*/, itemsize[1], item[0]/*val*/, itemsize[0]))
	{
		lua_pushlightuserdata( ls, const_cast<void*>( (const void*)item[0]));
		lua_pushinteger( ls, itemsize[0]);
		lua_pushlightuserdata( ls, const_cast<void*>( (const void*)item[1]));
		lua_pushinteger( ls, itemsize[1]);
		lua_pushlightuserdata( ls, output);
		lua_yieldk( ls, 0, 1, function_output_print);
	}
	return 0;
}


LUA_FUNCTION_THROWS( "output:opentag(..)", function_output_opentag)
{
	Output* output;
	const char* tag;
	std::size_t tagsize;
	int ctx;
	if (lua_getctx( ls, &ctx) != LUA_YIELD)
	{
		output = LuaObject<Output>::getSelf( ls, "output", "opentag");
		check_parameters( ls, 1, 1, LUA_TSTRING);
		tag = lua_tostring( ls, 2);
		tagsize = std::strlen( tag);
	}
	else
	{
		tag = (const char*)lua_touserdata( ls, -3);
		tagsize = (std::size_t)lua_tointeger( ls, -2);
		output = (Output*)lua_touserdata( ls, -1);
		lua_pop( ls, 3);
	}
	if (!output->called())
	{
		Input* input = LuaObject<Input>::getGlobal( ls, "input");
		if (!input->inputfilter()->getMetadata())
		{
			lua_pushlightuserdata( ls, const_cast<void*>( (const void*)tag));
			lua_pushinteger( ls, tagsize);
			lua_pushlightuserdata( ls, output);
			lua_yieldk( ls, 0, 1, function_output_opentag);
		}
		output->called(true);
	}
	if (!output->print( tag, tagsize, 0/*val*/, 0))
	{
		lua_pushlightuserdata( ls, const_cast<void*>( (const void*)tag));
		lua_pushinteger( ls, tagsize);
		lua_pushlightuserdata( ls, output);
		lua_yieldk( ls, 0, 1, function_output_opentag);
	}
	return 0;
}


LUA_FUNCTION_THROWS( "output:closetag(..)", function_output_closetag)
{
	Output* output;
	int ctx;
	if (lua_getctx( ls, &ctx) != LUA_YIELD)
	{
		output = LuaObject<Output>::getSelf( ls, "output", "closetag");
		check_parameters( ls, 1, 0);
	}
	else
	{
		output = (Output*)lua_touserdata( ls, -1);
		lua_pop( ls, 1);
	}
	if (!output->called())
	{
		Input* input = LuaObject<Input>::getGlobal( ls, "input");
		if (!input->inputfilter()->getMetadata())
		{
			lua_pushlightuserdata( ls, output);
			lua_yieldk( ls, 0, 1, function_output_closetag);
		}
		output->called(true);
	}
	if (!output->print( 0/*tag*/, 0, 0/*val*/, 0))
	{
		lua_pushlightuserdata( ls, output);
		lua_yieldk( ls, 0, 1, function_output_closetag);
	}
	return 0;
}


LUA_FUNCTION_THROWS( "filter(..)", function_filter)
{
	const char* name = "";
	const char* encoding = "";
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
		case 2:
			encoding = lua_tostring( ls, 2);
			if (!encoding) throw std::runtime_error( "encoding name is not a string");
		case 1:
		{
			name = lua_tostring( ls, 1);
			if (!name) throw std::runtime_error( "filter name is not a string");

			const proc::ProcessorProvider* ctx = getProcessorProvider( ls);
			types::CountedReference<Filter> flt( ctx->filter( name, encoding));
			if (!flt.get())
			{
				if (encoding[0])
				{
					throw std::runtime_error( std::string( "filter '") + name + "'" + " with encoding '" + encoding + "' is not defined");
				}
				else
				{
					throw std::runtime_error( std::string( "filter '") + name + "' is not defined");
				}
			}
			LuaObject<Filter>::push_luastack( ls, *flt);
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
	Input* input;
	int ctx;
	if (lua_getctx( ls, &ctx) != LUA_YIELD)
	{
		input = LuaObject<Input>::getSelf( ls, "input", "doctype");
		check_parameters( ls, 1, 0);
	}
	else
	{
		input = (Input*)lua_touserdata( ls, -1);
		lua_pop( ls, 1);
	}
	if (!input->inputfilter().get())
	{
		lua_pushnil( ls);
		return 1;
	}
	{
		LuaExceptionHandlerScope escope(ls);
		{
			std::string doctypestr;
			if (input->inputfilter()->getDocType( doctypestr))
			{
				if (doctypestr.size())
				{
					types::DocType doctype( doctypestr);
					lua_newtable( ls);
					if (doctype.rootid)
					{
						lua_pushstring( ls, doctype.rootid);
						lua_setfield( ls, -2, "root");
					}
					if (doctype.publicid)
					{
						lua_pushstring( ls, doctype.publicid);
						lua_setfield( ls, -2, "public");
					}
					if (doctype.systemid)
					{
						lua_pushstring( ls, doctype.systemid);
						lua_setfield( ls, -2, "system");
					}
					return 1;
				}
				else
				{
					lua_pushnil( ls);
					return 1;
				}
			}
			else if (input->inputfilter()->state() == InputFilter::Error)
			{
				const char* err = input->inputfilter()->getError();
				std::string msg( "error parsing DOCTYPE: ");
				msg.append( err?err:"unknown");
				throw std::runtime_error(msg);
			}
		}
	}
	lua_pushlightuserdata( ls, input);
	lua_yieldk( ls, 0, 1, function_input_doctype);
	return 0;
}


LUA_FUNCTION_THROWS( "input:doctypeid()", function_input_doctypeid)
{
	Input* input;
	int ctx;
	if (lua_getctx( ls, &ctx) != LUA_YIELD)
	{
		input = LuaObject<Input>::getSelf( ls, "input", "doctypeid");
		check_parameters( ls, 1, 0);
	}
	else
	{
		input = (Input*)lua_touserdata( ls, -1);
		lua_pop( ls, 1);
	}
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
				std::string doctypeid( types::getIdFromDoctype( doctype));
				if (doctypeid.size())
				{
					lua_pushlstring( ls, doctypeid.c_str(), doctypeid.size());
				}
				else
				{
					lua_pushnil( ls);
				}
				return 1;
			}
		}
		else if (input->inputfilter()->state() == InputFilter::Error)
		{
			const char* err = input->inputfilter()->getError();
			std::string msg( "error parsing DOCTYPE: ");
			msg.append( err?err:"unknown");
			throw std::runtime_error(msg);
		}
	}
	lua_pushlightuserdata( ls, input);
	lua_yieldk( ls, 0, 1, function_input_doctypeid);
	return 0;
}


LUA_FUNCTION_THROWS( "output:as(..)", function_output_as)
{
	Output* output = LuaObject<Output>::getSelf( ls, "output", "as");	//< self argument (mandatory)
	Filter* filter = 0;							//< 1st argument (mandatory)
	std::string doctype;							//< 2nd argument (optional)
	int ii=2,nn = lua_gettop( ls);
	if (nn <= 1)
	{
		throw std::runtime_error( "too few arguments");
	}
	else if (nn > 3)
	{
		throw std::runtime_error( "too many arguments");
	}
	for (; ii <= nn; ++ii)
	{
		if (lua_type( ls, ii) == LUA_TSTRING)
		{
			const proc::ProcessorProvider* gtc = getProcessorProvider( ls);
			const char* doctype_form = lua_tostring( ls, ii);
			const types::FormDescription* formdescr = gtc->formDescription( doctype_form);
			if (!formdescr) throw std::runtime_error( std::string("string argument is not referring to a form defined: '") + doctype_form + "'");
			const char* doctype_root = formdescr->xmlRoot();
			if (!doctype_root) throw std::runtime_error( "string argument is referring to a form without xml root element defined");
			doctype = gtc->xmlDoctypeString( formdescr->name(), formdescr->ddlname(), doctype_root);
		}
		else if (lua_type( ls, ii) == LUA_TTABLE)
		{
			const proc::ProcessorProvider* gtc = getProcessorProvider( ls);
			if (!doctype.empty()) throw std::runtime_error( "doctype specified twice");
			const char* doctype_system = 0;
			const char* doctype_public = 0;
			const char* doctype_root = 0;
			const char* doctype_form = 0;
			lua_getfield( ls, ii, "system");
			if (!lua_isnil( ls, -1))
			{
				doctype_system = lua_tostring( ls, -1);
				if (!doctype_system) throw std::runtime_error( "in table argument doctype: doctype['system'] is not convertible to a string");
			}
			lua_getfield( ls, ii, "public");
			if (!lua_isnil( ls, -1))
			{
				doctype_public = lua_tostring( ls, -1);
				if (!doctype_public) throw std::runtime_error( "in table argument doctype: doctype['public'] is not convertible to a string");
			}
			lua_getfield( ls, ii, "root");
			if (!lua_isnil( ls, -1))
			{
				doctype_root = lua_tostring( ls, -1);
				if (!doctype_root) throw std::runtime_error( "in table argument doctype: doctype['root'] is not convertible to a string");
			}
			lua_getfield( ls, ii, "form");
			if (!lua_isnil( ls, -1))
			{
				doctype_form = lua_tostring( ls, -1);
				if (!doctype_form) throw std::runtime_error( "in table argument doctype: doctype['form'] is not convertible to a string");
			}
			if (doctype_root)
			{
				doctype.append( types::DocType( doctype_root, doctype_public, doctype_system).tostring());
			}
			else if (doctype_form)
			{
				const types::FormDescription* formdescr = gtc->formDescription( doctype_form);
				if (!formdescr) throw std::runtime_error( std::string("doctype['form'] is not referring to a form defined: '") + doctype_form + "'");
				doctype_root = formdescr->xmlRoot();
				if (!doctype_root) throw std::runtime_error( "doctype['form'] is referring to a form without xml root element defined");
				doctype = gtc->xmlDoctypeString( formdescr->name(), formdescr->ddlname(), doctype_root);
			}
			else
			{
				std::runtime_error( "neither doctype['form'] nor doctype['root'] are defined in table passed");
			}
		}
		else if (lua_type( ls, ii) == LUA_TUSERDATA)
		{
			if (filter) throw std::runtime_error( "filter specified twice");
			filter = LuaObject<Filter>::get( ls, ii);
			if (!filter) throw std::runtime_error( "filter object expected as first argument");
		}
		else
		{
			std::runtime_error( "string (doctype) or filter expected as argument");
		}
	}
	OutputFilter* ff = 0;
	if (filter)
	{
		if (filter->outputfilter().get())
		{
			ff = filter->outputfilter()->copy();
			if (output->outputfilter().get())
			{
				ff->assignState( *output->outputfilter());
			}
			output->outputfilter().reset( ff);
		}
		else
		{
			throw std::runtime_error( "called with undefined output for the argument filter object");
		}
	}
	if (!doctype.empty())
	{
		output->outputfilter()->setDocType( doctype);
	}
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
	LuaObject<InputFilterClosure>::push_luastack( ls, InputFilterClosure(input->getIterator()));
	lua_pushcclosure( ls, function_inputfilterClosure_get, 1);
	return 1;
}


LUA_FUNCTION_THROWS( "input:table()", function_input_table_DDLFormSerializer)
{
	DDLFormSerializer* result = (DDLFormSerializer*)lua_touserdata( ls, -1);
	lua_pop( ls, 1);

	if (!result->call())
	{
		lua_pushlightuserdata( ls, result);
		lua_yieldk( ls, 0, 1, function_input_table_DDLFormSerializer);
	}
	return 1;
}

LUA_FUNCTION_THROWS( "input:table()", function_input_table_DDLFormParser)
{
	DDLFormParser* closure = (DDLFormParser*)lua_touserdata( ls, -1);
	lua_pop( ls, 1);

	if (!closure->call())
	{
		lua_pushlightuserdata( ls, closure);
		lua_yieldk( ls, 0, 1, function_input_table_DDLFormParser);
	}
	{
		TypedOutputFilterR outp( new LuaTableOutputFilter( ls));
		LuaObject<DDLFormSerializer>::push_luastack( ls, DDLFormSerializer( closure->form()));
		DDLFormSerializer* result = LuaObject<DDLFormSerializer>::get( ls, -1);
		result->init( outp, serialize::Context::SerializeWithIndices);
		lua_pushlightuserdata( ls, result);
	}
	function_input_table_DDLFormSerializer(ls);
	if (lua_type( ls, -1) == LUA_TUSERDATA)
	{
		//... if LuaTableOutputFilter::print is never called then no object is left on the stack
		lua_newtable( ls);
	}
	luaL_checktype( ls, -1, LUA_TTABLE);
	return 1;
}

LUA_FUNCTION_THROWS( "input:form()", function_input_form_DDLFormParser)
{
	DDLFormParser* closure = (DDLFormParser*)lua_touserdata( ls, -1);
	lua_pop( ls, 1);

	if (!closure->call())
	{
		lua_pushlightuserdata( ls, closure);
		lua_yieldk( ls, 0, 1, function_input_form_DDLFormParser);
	}
	LuaObject<types::FormR>::push_luastack( ls, closure->form());
	return 1;
}

LUA_FUNCTION_THROWS( "input:table()", function_input_table_RedirectFilterClosure)
{
	RedirectFilterClosure* obj = (RedirectFilterClosure*)lua_touserdata( ls, -1);
	lua_pop( ls, 1);

	if (!obj->call())
	{
		lua_pushlightuserdata( ls, obj);
		lua_yieldk( ls, 0, 1, function_input_table_RedirectFilterClosure);
	}
	if (lua_type( ls, -1) == LUA_TUSERDATA)
	{
		//... if LuaTableOutputFilter::print is never called then no object is left on the stack
		lua_newtable( ls);
	}
	return 1;
}

static int function_input_table_nil( lua_State* ls)
{
	lua_pushnil( ls);
	return 1;
}

static lua_CFunction get_input_struct_closure( lua_State* ls, Input* input, bool outputIsTable)
{
	if (input->inputfilter().get())
	{
		std::string doctype;
		// try to get 'form' for validation if the document is not standalone:
		if (input->inputfilter()->getDocType( doctype))
		{
			if (doctype.size())
			{
				std::string doctypeid( types::getIdFromDoctype( doctype));
				const proc::ProcessorProvider* gtc = getProcessorProvider( ls);
				const types::FormDescription* st = gtc->formDescription( doctypeid);
				if (!st) throw std::runtime_error( std::string("form not defined for document type '") + doctypeid + "'");
				types::FormR form( new types::Form( st));

				DDLFormParser* closure;
				serialize::Context::Flags flags = serialize::Context::ValidateAttributes;
				TypedInputFilterR inp( new TypingInputFilter( input->getIterator()));
				LuaObject<DDLFormParser>::push_luastack( ls, DDLFormParser( form));
				closure = LuaObject<DDLFormParser>::get( ls, -1);
				closure->init( inp, flags);
				lua_pushlightuserdata( ls, closure);
				if (outputIsTable)
				{
					return &function_input_table_DDLFormParser;
				}
				else
				{
					return &function_input_form_DDLFormParser;
				}
			}
			else if (outputIsTable)
			{
				// document is standalone
				TypedInputFilterR inp( new TypingInputFilter( input->getIterator()));
				TypedOutputFilterR outp( new LuaTableOutputFilter( ls));
				LuaObject<RedirectFilterClosure>::push_luastack( ls, RedirectFilterClosure( inp, outp));
				RedirectFilterClosure* obj = LuaObject<RedirectFilterClosure>::get( ls, -1);
				lua_pushlightuserdata( ls, obj);
				return &function_input_table_RedirectFilterClosure;
			}
			else
			{
				return &function_input_table_nil;
			}
		}
		else if (input->inputfilter()->state() == InputFilter::Error)
		{
			std::string msg( "error parsing DOCTYPE: ");
			const char* err = input->inputfilter()->getError();
			msg.append( err?err:"unknown");
			throw std::runtime_error(msg);
		}
	}
	else
	{
		throw std::runtime_error( "undefined input filter");
	}
	return 0;
}

LUA_FUNCTION_THROWS( "input:table()", function_input_table)
{
	Input* input;
	int ctx;
	if (lua_getctx( ls, &ctx) != LUA_YIELD)
	{
		input = LuaObject<Input>::getSelf( ls, "input", "table");
		check_parameters( ls, 1, 0);
	}
	else
	{
		input = (Input*)lua_touserdata( ls, -1);
		lua_pop( ls, 1);
	}
	lua_CFunction func = get_input_struct_closure( ls, input, true);
	if (!func)
	{
		lua_pushlightuserdata( ls, input);
		return lua_yieldk( ls, 0, 1, function_input_table);
	}
	return func( ls);
}


LUA_FUNCTION_THROWS( "input:form()", function_input_form)
{
	Input* input;
	int ctx;
	if (lua_getctx( ls, &ctx) != LUA_YIELD)
	{
		input = LuaObject<Input>::getSelf( ls, "input", "form");
		check_parameters( ls, 1, 0);
	}
	else
	{
		input = (Input*)lua_touserdata( ls, -1);
		lua_pop( ls, 1);
	}
	lua_CFunction func = get_input_struct_closure( ls, input, false);
	if (!func)
	{
		lua_pushlightuserdata( ls, input);
		return lua_yieldk( ls, 0, 1, function_input_form);
	}
	return func( ls);
}


LUA_FUNCTION_THROWS( "logger.print(..)", function_logger_print)
{
	/* first parameter maps to a log level, rest gets printed depending on
	 * whether it's a string or a number
	 */
	int ii,sofs=0,nn = lua_gettop(ls);
	if (nn <= 0)
	{
		throw std::runtime_error( "missing arguments");
	}
	if (luaL_testudata( ls, 1, metaTableName<Logger>()))
	{
		sofs = 1;
	}
	const char *logLevel = lua_tostring( ls, 1+sofs);
	if (!logLevel)
	{
		throw std::runtime_error( "first argument log level is not a string");
	}
	std::string logmsg;

	for (ii=2+sofs; ii<=nn; ii++)
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
			<< logmsg;
	}
	return 0;
}


LUA_FUNCTION_THROWS( "logger.printc(..)", function_logger_printc)
{
	/* first parameter maps to a log level, rest gets printed depending on
	 * whether it's a string or a number
	 */
	int ii,sofs=0,nn = lua_gettop(ls);
	if (nn && luaL_testudata( ls, 1, metaTableName<Logger>()))
	{
		sofs = 1;
	}
	std::string logmsg;
	for (ii=1+sofs; ii<=nn; ii++)
	{
		if (!getDescription( ls, ii, logmsg))
		{
			throw std::runtime_error( "failed to map arguments to strings");
		}
	}
	std::cerr << logmsg << "\n";
	return 0;
}


static const luaL_Reg logger_methodtable[ 3] =
{
	{"print",&function_logger_print},
	{"printc",&function_logger_printc},
	{0,0}
};

static const luaL_Reg input_methodtable[ 7] =
{
	{"as",&function_input_as},
	{"form",&function_input_form},
	{"table",&function_input_table},
	{"doctype",function_input_doctype},
	{"doctypeid",function_input_doctypeid},
	{"get",&function_input_get},
	{0,0}
};

static const luaL_Reg transaction_methodtable[ 3] =
{
	{"begin",function_transaction_begin},
	{"commit",function_transaction_commit},
	{"rollback",function_transaction_rollback},
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

static const luaL_Reg form_methodtable[ 6] =
{
	{"name",&function_form_name},
	{"table",&function_form_table},
	{"get",&function_form_get},
	{"fill",&function_form_fill},
	{"__tostring",&function_form_tostring},
	{0,0}
};

static const luaL_Reg provider_methodtable[ 5] =
{
	{"filter",&function_filter},
	{"form",&function_form},
	{"formfunction",&function_formfunction},
	{"normalizer",&function_normalizer},
	{0,0}
};

LuaScript::LuaScript( const std::string& path_)
	:m_path(path_)
{
	// Load the source of the script from file
	m_content = utils::readSourceFileContent( m_path);
	std::map<std::string,bool> sysfuncmap;

	// Fill the map of all system functions to exclude them from the list of exported functions:
	{
		LuaScriptInstance instance( this, 0/*modulemap*/);
		instance.initbase( 0, false);
		lua_State* ls = instance.ls();
		lua_rawgeti( ls, LUA_REGISTRYINDEX, LUA_RIDX_GLOBALS);
		lua_pushnil(ls);
		while (lua_next( ls, -2))
		{
			if (lua_isfunction( ls, -1) && lua_type( ls, -2) == LUA_TSTRING)
			{
				sysfuncmap[ lua_tostring( ls, -2)] = true;
			}
			lua_pop( ls, 1);
		}
	}
	// Check the script syntax and get the list of all global functions (that are not system or module functions)
	{
		LuaScriptInstance instance( this, 0/*modulemap*/);
		instance.initbase( 0, true);
		lua_State* ls = instance.ls();

		lua_rawgeti( ls, LUA_REGISTRYINDEX, LUA_RIDX_GLOBALS);
		lua_pushnil(ls);
		while (lua_next( ls, -2))
		{
			if (lua_isfunction( ls, -1) && lua_isstring( ls, -2))
			{
				std::string funcname( lua_tostring( ls, -2));
				std::map<std::string,bool>::const_iterator fi = sysfuncmap.find( funcname);
				if (fi == sysfuncmap.end() || fi->second == false)
				{
					m_functions.push_back( funcname);
				}
			}
			lua_pop( ls, 1);
		}
	}
}

LuaScriptInstance::LuaScriptInstance( const LuaScript* script_, const LuaModuleMap* modulemap_)
	:m_ls(0),m_thread(0),m_threadref(0),m_script(script_),m_modulemap(modulemap_)
{}

std::string LuaScriptInstance::luaErrorMessage( lua_State* ls_, int index)
{
	std::string rt;
	const char* msg = lua_tostring( ls_, index);
	if (!msg) msg = "";
	std::string scriptfilename( utils::getFileStem( script()->path()));
	const char* fp = std::strstr( msg, "[string \"");
	const char* ep = 0;
	if (fp) ep = std::strchr( fp, ']');

	if (fp && ep)
	{
		rt.append( msg, fp - msg);
		rt.push_back( '[');
		rt.append( scriptfilename);
		rt.append( ep);
	}
	else
	{
		rt.append( msg);
	}
	return rt;
}

std::string LuaScriptInstance::luaUserErrorMessage( lua_State* ls_, int index)
{
	std::string rt;
	const char* msg = lua_tostring( ls_, index);
	if (!msg) msg = "";

	const char* fp = std::strstr( msg, "[string \"");
	const char* ep = 0;
	if (fp) ep = std::strchr( fp, ']');

	if (fp && ep)
	{
		for (++ep; *ep == ':' || *ep == ' ' || (*ep >= '0' && *ep <= '9'); ++ep);
		rt.append( ep);
	}
	else
	{
		rt.append( msg);
	}
	return rt;
}

void LuaScriptInstance::init( const proc::ProcessorProvider* provider_)
{
	initbase( provider_, true);
}

void LuaScriptInstance::initbase( const proc::ProcessorProvider* provider_, bool callMain)
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
			buf << "Failed to load script '" << m_script->path() << "':" << luaErrorMessage( m_ls, -1);
			throw std::runtime_error( buf.str());
		}
		// open standard lua libraries (we load all of them):
		luaL_openlibs( m_ls);

		// register objects already here that may be used in the initilization part:
		Logger logger_;
		LuaObject<Logger>::createGlobal( m_ls, "logger", logger_, logger_methodtable);
		if (m_modulemap)
		{
			setGlobalModuleMap( m_ls, m_modulemap);
			lua_pushcfunction( m_ls, &function_module);
			lua_setglobal( m_ls, "module");
		}
		else
		{
			lua_pushcfunction( m_ls, &function_module_dummy);
			lua_setglobal( m_ls, "module");
		}
		// call main (initialization part):
		if (callMain)
		{
			if (lua_pcall( m_ls, 0, LUA_MULTRET, 0) != 0)
			{
				std::ostringstream buf;
				buf << "Unable to call main entry of script: " << luaErrorMessage( m_ls);
				throw std::runtime_error( buf.str());
			}
		}
		LuaObject<RedirectFilterClosure>::createMetatable( m_ls, 0, 0, 0);
		LuaObject<types::FormR>::createMetatable( m_ls, 0, 0, form_methodtable);
		LuaObject<DDLFormParser>::createMetatable( m_ls, 0, 0, 0);
		LuaObject<DDLFormSerializer>::createMetatable( m_ls, 0, 0, 0);
		LuaObject<serialize::StructSerializer>::createMetatable( m_ls, 0, 0, struct_methodtable);
		LuaObject<InputFilterClosure>::createMetatable( m_ls, 0, 0, 0);
		LuaObject<TypedInputFilterR>::createMetatable( m_ls, 0, 0, typedinputfilter_methodtable);
		LuaObject<TypedInputFilterClosure>::createMetatable( m_ls, 0, 0, 0);
		LuaObject<FormFunctionClosureR>::createMetatable( m_ls, 0, 0, 0);
		if (provider_) setProcessorProvider( m_ls, provider_);
		LuaObject<db::TransactionR>::createGlobal( m_ls, "transaction", db::TransactionR(), transaction_methodtable);
		LuaObject<Filter>::createMetatable( m_ls, &function__LuaObject__index<Filter>, &function__LuaObject__newindex<Filter>, 0);
		lua_pushcfunction( m_ls, &function_filter);
		lua_setglobal( m_ls, "filter");
		lua_pushcfunction( m_ls, &function_form);
		lua_setglobal( m_ls, "form");
		lua_pushcfunction( m_ls, &function_formfunction);
		lua_setglobal( m_ls, "formfunction");
		lua_pushcfunction( m_ls, &function_scope);
		lua_setglobal( m_ls, "scope");
		lua_pushcfunction( m_ls, &function_normalizer);
		lua_setglobal( m_ls, "normalizer");

		//Register provider context:
		lua_newtable( m_ls);
		luaL_setfuncs( m_ls, provider_methodtable, 0);
		lua_setglobal( m_ls, "provider");
	}
}

void LuaScriptInstance::init( const Input& input_, const Output& output_, const proc::ProcessorProvider* provider_)
{
	initbase( provider_, true);
	LuaExceptionHandlerScope luaThrows(m_ls);
	{
		LuaObject<Input>::createGlobal( m_ls, "input", input_, input_methodtable);
		LuaObject<Output>::createGlobal( m_ls, "output", output_, output_methodtable);
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

void LuaModule::load( lua_State* ls)
{
	LuaExceptionHandlerScope luaThrows(ls);
	{
		if (!m_initializer) throw std::runtime_error( "Module entry point not defined (null)");
		if (m_initializer( ls) != 0)
		{
			std::ostringstream buf;
			buf << "Unable to call main entry of lua module '" << m_name << "' (" << lua_tostring( ls, -1 ) << ")";
			throw std::runtime_error( buf.str());
		}
	}
}

void LuaModuleMap::defineLuaModule( const std::string& name, const LuaModule& mod)
{
	std::string nam( name);
	std::map<std::string,LuaModule>::const_iterator ii=m_map.find( nam),ee=m_map.end();
	if (ii != ee) throw std::runtime_error( (std::string("duplicate definition of lua module '") + nam + "'").c_str());
	m_map[ nam] = mod;
}

bool LuaModuleMap::getLuaModule( const std::string& name, LuaModule& rt) const
{
	std::string nam( name);
	std::map<std::string,LuaModule>::const_iterator ii=m_map.find( nam),ee=m_map.end();
	if (ii == ee) return false;
	rt = ii->second;
	return true;
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
	rt = LuaScriptInstanceR( new LuaScriptInstance( m_ar[ ii->second], m_modulemap));
	return true;
}

std::list<std::string> LuaFunctionMap::commands() const
{
	std::list<std::string> rt;
	std::map<std::string,std::size_t>::const_iterator ii = m_procmap.begin(), ee = m_procmap.end();
	for (; ii != ee; ++ii) rt.push_back( ii->first);
	return rt;
}

TypedInputFilterR LuaScriptInstance::getObject( int idx)
{
	TypedInputFilterR rt;
	int typ = lua_type( thread(), idx);
	if (typ == LUA_TTABLE || typ == LUA_TFUNCTION)
	{
		rt = get_operand_TypedInputFilter( thread(), idx);
	}
	else
	{
		types::FormR* form;
		if ((form=LuaObject<types::FormR>::get( thread(), idx)) != 0)
		{
			return TypedInputFilterR( new serialize::DDLStructSerializer( form->get()));
		}
		TypedInputFilterR* resultref;
		if ((resultref=LuaObject<TypedInputFilterR>::get( thread(), idx)) != 0)
		{
			return *resultref;
		}
		serialize::StructSerializer* obj;
		if ((obj=LuaObject<serialize::StructSerializer>::get( thread(), idx)) != 0)
		{
			return TypedInputFilterR( obj->copy());
		}
	}
	return rt;
}

void LuaScriptInstance::pushObject( const TypedInputFilterR& obj)
{
	LuaObject<TypedInputFilterR>::push_luastack( thread(), obj);
}




