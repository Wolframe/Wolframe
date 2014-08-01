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
//\file luaObjects.cpp
#include "luaObjects.hpp"
#include "luaObjectTemplate.hpp"
#include "luaDebug.hpp"
#include "luafilter.hpp"
#include "luaException.hpp"
#include "luaCppCall.hpp"
#include "langbind/auditFunction.hpp"
#include "types/normalizeFunction.hpp"
#include "types/docmetadata.hpp"
#include "filter/typingfilter.hpp"
#include "filter/typedfilterScope.hpp"
#include "filter/inputfilterScope.hpp"
#include "filter/tostringfilter.hpp"
#include "utils/fileUtils.hpp"
#include "cmdbind/doctypeDetector.hpp"
#include <limits>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <cstddef>
#include <cstdarg>
#include <boost/type_traits/remove_cv.hpp>
extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#if LUA_VERSION_NUM < 502
#error Wolframe needs Lua version >= 5.2.1
#endif
}

using namespace _Wolframe;
using namespace _Wolframe::langbind;

template <class ObjectType>
static void setGlobalSingletonPointer( lua_State* ls, ObjectType* obj)
{
	typedef typename boost::remove_cv<ObjectType>::type ObjectType_ncv;
	ObjectType_ncv* obj_ncv = const_cast<ObjectType_ncv*>( obj);
	lua_pushlightuserdata( ls, obj_ncv);
	lua_setglobal( ls, MetaTable<ObjectType_ncv>::name());
}

template <class ObjectType>
static ObjectType* getGlobalSingletonPointer( lua_State* ls)
{
	typedef typename boost::remove_cv<ObjectType>::type ObjectType_ncv;
	lua_getglobal( ls, MetaTable<ObjectType_ncv>::name());
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

static void setExecContext( lua_State* ls, proc::ExecContext* ctx_)
{
	return setGlobalSingletonPointer<proc::ExecContext>( ls, ctx_);
}

static proc::ExecContext* getExecContext( lua_State* ls)
{
	proc::ExecContext* rt = getGlobalSingletonPointer<proc::ExecContext>( ls);
	if (!rt) throw std::runtime_error( "execution context undefined");
	return rt;
}

class DDLTypeMap :public types::NormalizeFunctionMap
{
public:
	DDLTypeMap( lua_State* ls)
		:m_provider( getExecContext( ls)->provider()){}

	virtual const types::NormalizeFunction* get( const std::string& name) const
	{
		return m_provider->normalizeFunction( name);
	}

private:
	const proc::ProcessorProviderInterface* m_provider;
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
	for (int ii = 0; ii < nn; ++ii)
	{
		int expect = va_arg( aa, int);
		int typ = lua_type( ls, si+ii+1);
		if (typ != expect)
		{
			const char* expectname = lua_typename( ls, expect);
			const char* typname = lua_typename( ls, typ);
			std::ostringstream msg;
			msg << "expected " << (expectname?expectname:"?") << " instead of " << (typname?typname:"?") << " as argument " << (ii+1);
			throw std::runtime_error( msg.str());
		}
	}
	va_end( aa);
}


template <class Object>
static int function__LuaObject__index( lua_State* ls)
{
	LuaErrorMessage luaerr;
	Object* obj = LuaObject<Object>::getSelf( ls, MetaTable<Object>::name(), "__index");
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
				lua_tostring( ls, -1); //PF:BUGFIX lua 5.1.4 needs this one
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
	Object* obj = LuaObject<Object>::getSelf( ls, MetaTable<Object>::name(), "__newindex");
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
		case LUA_TBOOLEAN:
		case LUA_TNUMBER:
		case LUA_TSTRING:
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
			msg << "expected atomic value, table, form or generator function and got '" << typnam << "' as argument";
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

static void getVariantValue( lua_State* ls, types::VariantConst& val, int idx)
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
			val.init( val_str, val_len);
			break;

		case LUA_TUSERDATA:
		{
			types::CustomDataValueR* custom = LuaObject<types::CustomDataValueR>::get( ls, idx);
			if (!custom)
			{
				throw std::runtime_error( "custom data type expected in case of user defined type for atomic 'variant type' argument");
			}
			val.init( *custom->get());
			break;
		}
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

		case types::Variant::Custom:
			LuaObject<types::CustomDataValueR>::push_luastack( ls, types::CustomDataValueR( val.data().value.Custom->copy()));
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

		case types::Variant::Timestamp:
		{
			LuaExceptionHandlerScope escope(ls);
			{
				std::string strval = val.tostring();
				lua_pushlstring( ls, strval.c_str(), strval.size());
				lua_tostring( ls, -1); //PF:BUGFIX lua 5.1.4 needs this one
			}
			break;
		}

		case types::Variant::BigNumber:
		{
			LuaExceptionHandlerScope escope(ls);
			{
				std::string strval = val.tostring();
				lua_pushlstring( ls, strval.c_str(), strval.size());
				lua_tostring( ls, -1); //PF:BUGFIX lua 5.1.4 needs this one
			}
			break;
		}

		case types::Variant::String:
		{
			LuaExceptionHandlerScope escope(ls);
			{
				lua_pushlstring( ls, val.charptr(), val.charsize());
				lua_tostring( ls, -1); //PF:BUGFIX lua 5.1.4 needs this one
			}
			break;
		}
		default:
			throw std::runtime_error( "atomic value expected for 'variant type' argument");
	}
}

LUA_FUNCTION_THROWS( "<type>(..)", function_normalizer_call)
{
	types::NormalizeFunctionR* func = LuaObject<types::NormalizeFunctionR>::get( ls, lua_upvalueindex( 1));
	if (lua_gettop( ls) != 1)
	{
		throw std::runtime_error( "atomic value expected for 'variant type' argument");

	}
	types::VariantConst param;
	getVariantValue( ls, param, 1);
	types::Variant result = (*func)->execute( param);
	pushVariantValue( ls, result);
	return 1;
}

LUA_FUNCTION_THROWS( "iterator.scope(..)", function_scope)
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

	serialize::DDLFormSerializer ser( *form);
	ser.init( out, serialize::Flags::SerializeWithIndices);
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
		lua_tostring( ls, -1); //PF:BUGFIX lua 5.1.4 needs this one
		return 1;
	}
}


LUA_FUNCTION_THROWS( "form:name()", function_form_name)
{
	types::FormR* form = LuaObject<types::FormR>::getSelf( ls, "form", "name");
	check_parameters( ls, 1, 0);

	LuaExceptionHandlerScope escope(ls);
	{
		const types::FormDescription* descr = (*form)->description();
		lua_pushlstring( ls, descr->name().c_str(), descr->name().size());
		lua_tostring( ls, -1); //PF:BUGFIX lua 5.1.4 needs this one
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
	serialize::DDLFormParser* closure;
	int ctx;
	if (lua_getctx( ls, &ctx) != LUA_YIELD)
	{
		types::VariantStruct* substruct = form->get();
		serialize::Flags::Enum flags = serialize::Flags::None;
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
					flags = (serialize::Flags::Enum)((int)serialize::Flags::ValidateAttributes|(int)serialize::Flags::ValidateInitialization);
				}
				else if (std::strcmp( mode, "complete") == 0)
				{
					flags = serialize::Flags::ValidateInitialization;
				}
				else if (std::strcmp( mode, "relaxed") == 0)
				{
					flags = serialize::Flags::None;
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
		LuaObject<serialize::DDLFormParser>::push_luastack( ls, serialize::DDLFormParser( *form, substruct));
		closure = LuaObject<serialize::DDLFormParser>::get( ls, -1);
		closure->init( inp, flags);
		lua_pushvalue( ls, 2);		//... iterator argument (table, generator function, etc.)
	}
	else
	{
		closure = (serialize::DDLFormParser*)lua_touserdata( ls, -1);
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
	serialize::DDLFormSerializer* result;
	int ctx;
	if (lua_getctx( ls, &ctx) != LUA_YIELD)
	{
		form = LuaObject<types::FormR>::getSelf( ls, "form", "value");

		types::VariantStruct* substruct = form->get();
		int nn = lua_gettop( ls);
		if (nn > 1)
		{
			if (nn > 2) throw std::runtime_error( "too many arguments");
			substruct = get_substructure( ls, 2, substruct);
		}
		TypedOutputFilterR outp( new LuaTableOutputFilter( ls));
		LuaObject<serialize::DDLFormSerializer>::push_luastack( ls, serialize::DDLFormSerializer( *form, substruct));
		result = LuaObject<serialize::DDLFormSerializer>::get( ls, -1);
		result->init( outp, serialize::Flags::SerializeWithIndices);
	}
	else
	{
		form = (types::FormR*)lua_touserdata( ls, -2);
		result = (serialize::DDLFormSerializer*)lua_touserdata( ls, -1);
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
	TypedInputFilterR itr( new serialize::DDLFormSerializer( *result, substruct));
	LuaObject<TypedInputFilterClosure>::push_luastack( ls, TypedInputFilterClosure(itr));
	lua_pushcclosure( ls, function_typedinputfilterClosure_get, 1);
	return 1;
}


LUA_FUNCTION_THROWS( "form:metadata()", function_form_metadata)
{
	types::FormR* form = LuaObject<types::FormR>::getSelf( ls, "form", "get");

	lua_newtable( ls);
	std::vector<types::DocMetaData::Attribute>::const_iterator ai = (*form)->description()->metadata().attributes().begin(), ae = (*form)->description()->metadata().attributes().end();
	for (; ai != ae; ++ai)
	{
		lua_pushlstring( ls, ai->value.c_str(), ai->value.size());
		lua_tostring( ls, -1); //PF:BUGFIX lua 5.1.4 needs this one
		lua_setfield( ls, -2, ai->name.c_str());
	}
	return 1;
}


LUA_FUNCTION_THROWS( "provider.form()", function_form)
{
	check_parameters( ls, 0, 1, LUA_TSTRING);

	const char* name = lua_tostring( ls, 1);
	proc::ExecContext* ctx = getExecContext( ls);
	const types::FormDescription* st = ctx->provider()->formDescription( name);
	if (!st) throw std::runtime_error( std::string("form '") + name + "' not defined");
	types::FormR frm( new types::Form( st));
	LuaObject<types::FormR>::push_luastack( ls, frm);
	return 1;
}


LUA_FUNCTION_THROWS( "<type>()", function_type_value_constructor)
{
	const types::CustomDataType* typ = (types::CustomDataType*)lua_touserdata( ls, lua_upvalueindex( 1));
	types::CustomDataInitializerR* ini = LuaObject<types::CustomDataInitializerR>::get( ls, lua_upvalueindex( 2));
	if (!typ || !ini) throw std::runtime_error( "invalid call of type constructor");

	int nn = lua_gettop( ls);
	if (nn > 1) throw std::runtime_error( "too many arguments");
	types::CustomDataValueR value( typ->createValue( ini->get()));
	if (nn == 1)
	{
		types::VariantConst arg;
		getVariantValue( ls, arg, 1);
		value->assign( arg);
	}
	LuaObject<types::CustomDataValueR>::push_luastack( ls, value);
	return 1;
}

LUA_FUNCTION_THROWS( "<type>()", function_bignumber_constructor)
{
	int nn = lua_gettop( ls);
	if (nn > 1) throw std::runtime_error( "too many arguments");
	if (nn == 1)
	{
		if (lua_type( ls, 1) == LUA_TSTRING)
		{
			if (nn > 1) throw std::runtime_error( "too many arguments");
			const char* str = lua_tostring( ls, 1);
			LuaObject<types::BigNumber>::push_luastack( ls, types::BigNumber( str, std::strlen(str)));
			return 1;
		}
		else if (lua_type( ls, 1) == LUA_TNUMBER)
		{
			double val = lua_tonumber( ls, 1);
			LuaObject<types::BigNumber>::push_luastack( ls, types::BigNumber( val));
			return 1;
		}
		else
		{
			throw std::runtime_error( "string or number expected as argument");
		}
	}
	else
	{
		LuaObject<types::BigNumber>::push_luastack( ls, types::BigNumber());
		return 1;
	}
}

LUA_FUNCTION_THROWS( "<type>()", function_datetime_constructor)
{
	int nn = lua_gettop( ls);
	if (nn > 1) throw std::runtime_error( "too many arguments");
	if (nn >= 1)
	{
		if (lua_type( ls, 1) == LUA_TSTRING)
		{
			if (nn > 1) throw std::runtime_error( "too many arguments");
			const char* str = lua_tostring( ls, 1);
			LuaObject<types::DateTime>::push_luastack( ls, types::DateTime( str, std::strlen(str)));
			return 1;
		}
		else if (lua_type( ls, 1) == LUA_TNUMBER)
		{
			for (int ii=2; ii<=nn; ++ii)
			{
				if (lua_type( ls, ii) != LUA_TNUMBER)
				{
					throw std::runtime_error( "string or sequence of integers expected as arguments");
				}
			}
			int YY = lua_tointeger( ls, 1);
			if (nn < 3) throw std::runtime_error( "number of integer argument must be 3,6,7 or 8");
			int MM = lua_tointeger( ls, 2);
			int DD = lua_tointeger( ls, 3);
			if (nn == 3)
			{
				LuaObject<types::DateTime>::push_luastack( ls, types::DateTime( YY, MM, DD));
				return 1;
			}
			if (nn < 6) throw std::runtime_error( "number of integer argument must be 3,6,7 or 8");
			int hh = lua_tointeger( ls, 4);
			int mm = lua_tointeger( ls, 5);
			int ss = lua_tointeger( ls, 6);
			if (nn == 6)
			{
				LuaObject<types::DateTime>::push_luastack( ls, types::DateTime( YY, MM, DD, hh, mm, ss));
				return 1;
			}
			int ll = lua_tointeger( ls, 7);
			if (nn == 7)
			{
				LuaObject<types::DateTime>::push_luastack( ls, types::DateTime( YY, MM, DD, hh, mm, ss, ll));
				return 1;
			}
			int cc = lua_tointeger( ls, 8);
			if (nn == 8)
			{
				LuaObject<types::DateTime>::push_luastack( ls, types::DateTime( YY, MM, DD, hh, mm, ss, ll, cc));
				return 1;
			}
			throw std::runtime_error( "too many arguments");
		}
		else
		{
			throw std::runtime_error( "string or sequence of numbers expected as arguments");
		}
	}
	else
	{
		throw std::runtime_error( "too few arguments");
	}
}

LUA_FUNCTION_THROWS( "provider.type()", function_type)
{
	int nn = lua_gettop( ls);
	std::vector<types::Variant> initializerList;
	std::string typeName;

	if (nn < 1)
	{
		throw std::runtime_error( "too few arguments");
	}
	if (lua_type( ls, 1) != LUA_TSTRING)
	{
		throw std::runtime_error( "expected typename (string) as first argument");
	}
	typeName.append( lua_tostring( ls, 1));

	types::VariantConst arg;
	for (int ii=2; ii<nn; ++ii)
	{
		getVariantValue( ls, arg, ii);
		initializerList.push_back( arg);
	}
	proc::ExecContext* ctx = getExecContext( ls);
	const types::CustomDataType* typ = ctx->provider()->customDataType( typeName);
	if (typ)
	{
		types::CustomDataInitializerR ini;
		if (typ->hasInitializer())
		{
			ini.reset( typ->createInitializer( initializerList));
		}
		lua_pushlightuserdata( ls, const_cast<types::CustomDataType*>(typ));
		LuaObject<types::CustomDataInitializerR>::push_luastack( ls, ini);
		lua_pushcclosure( ls, function_type_value_constructor, 2);
		return 1;
	}
	const types::NormalizeFunction* func = ctx->provider()->normalizeFunction( typeName);
	if (func)
	{
		if (!initializerList.empty())
		{
			throw std::runtime_error( "unexpected initializer argument for type (normalizer)");
		}
		LuaObject<types::NormalizeFunctionR>::push_luastack( ls, types::NormalizeFunctionR( func->copy()));
		lua_pushcclosure( ls, function_normalizer_call, 1);
		return 1;
	}
	const types::NormalizeFunctionType* functype = ctx->provider()->normalizeFunctionType( typeName);
	if (functype)
	{
		LuaObject<types::NormalizeFunctionR>::push_luastack( ls, types::NormalizeFunctionR( functype->createFunction( initializerList)));
		lua_pushcclosure( ls, function_normalizer_call, 1);
		return 1;
	}
	if (typeName == "bignumber")
	{
		if (!initializerList.empty())
		{
			throw std::runtime_error( "unexpected initializer argument for type 'bignumber'");
		}
		lua_pushcfunction( ls, function_bignumber_constructor);
		return 1;
	}
	if (typeName == "datetime")
	{
		if (!initializerList.empty())
		{
			throw std::runtime_error( "unexpected initializer argument for type 'datetime'");
		}
		lua_pushcfunction( ls, function_datetime_constructor);
		return 1;
	}
	throw std::runtime_error( std::string( "type name '") + typeName + "' not defined"); 
}


LUA_FUNCTION_THROWS( "<structure>:__tostring()", function_struct_tostring)
{
	serialize::StructSerializer* obj = LuaObject<serialize::StructSerializer>::getSelf( ls, "<structure>", "__tostring");
	check_parameters( ls, 1, 0);

	LuaObject<serialize::StructSerializer>::push_luastack( ls, *obj);
	obj = LuaObject<serialize::StructSerializer>::get( ls, -1);

	ToStringFilter* flt = new ToStringFilter();
	TypedOutputFilterR out( flt);
	obj->init( out, serialize::Flags::SerializeWithIndices);
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
		lua_tostring( ls, -1); //PF:BUGFIX lua 5.1.4 needs this one
		return 1;
	}
}


LUA_FUNCTION_THROWS( "<structure>:table()", function_struct_table)
{
	serialize::StructSerializer* obj;
	int ctx;
	if (lua_getctx( ls, &ctx) != LUA_YIELD)
	{
		obj = LuaObject<serialize::StructSerializer>::getSelf( ls, "<structure>", "value");
		check_parameters( ls, 1, 0);
		LuaObject<serialize::StructSerializer>::push_luastack( ls, *obj);
		obj = LuaObject<serialize::StructSerializer>::get( ls, -1);
		obj->init( TypedOutputFilterR( new LuaTableOutputFilter( ls)), serialize::Flags::SerializeWithIndices);
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
	obj->setFlags( TypedInputFilter::SerializeWithIndices);

	ToStringFilter* flt = new ToStringFilter();
	TypedOutputFilterR out( flt);
	RedirectFilterClosure exc( obj, out, false);
	if (!exc.call())
	{
		throw std::logic_error( "internal: tostring serialization with yield");
	}
	std::string content = flt->content();
	LuaExceptionHandlerScope escope(ls);
	{
		lua_pushlstring( ls, content.c_str(), content.size());
		lua_tostring( ls, -1); //PF:BUGFIX lua 5.1.4 needs this one
		return 1;
	}
}


LUA_FUNCTION_THROWS( "<structure>:table()", function_typedinputfilter_table)
{
	RedirectFilterClosure* closure;
	int ctx;
	if (lua_getctx( ls, &ctx) != LUA_YIELD)
	{
		TypedInputFilterR* objref = LuaObject<TypedInputFilterR>::getSelf( ls, "<structure>", "value");
		if (!objref->get())
		{
			lua_pushnil( ls);
			return 1;
		}
		TypedInputFilterR obj = TypedInputFilterR( (*objref)->copy());
		obj->resetIterator();
		if (!obj->setFlags( TypedInputFilter::SerializeWithIndices))
		{
			LOG_WARNING << "calling table() for object without input structure info";
		}
		TypedOutputFilterR outp( new LuaTableOutputFilter( ls));
		LuaObject<RedirectFilterClosure>::push_luastack( ls, RedirectFilterClosure( obj, outp, false));
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
			(*closure)->init( getExecContext( ls), inp, serialize::Flags::None);
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

LUA_FUNCTION_THROWS( "provider.formfunction(..)", function_formfunction)
{
	check_parameters( ls, 0, 1, LUA_TSTRING);

	const char* name = lua_tostring( ls, 1);
	proc::ExecContext* ctx = getExecContext( ls);
	const FormFunction* ff = ctx->provider()->formFunction( name);
	if (ff)
	{
		LuaObject<FormFunctionClosureR>::push_luastack( ls, FormFunctionClosureR( ff->createClosure()));
		lua_pushcclosure( ls, function_formfunction_call, 1);
		return 1;
	}
	throw std::runtime_error( std::string( "form function '") + name + "' not found");
}

LUA_FUNCTION_THROWS( "provider.audit(..)", function_audit)
{
	FormFunctionClosureR closure;
	int ctx;
	if (lua_getctx( ls, &ctx) != LUA_YIELD)
	{
		int nn = lua_gettop( ls);
		if (nn > 2) throw std::runtime_error( "too many arguments");
		if (nn < 2) throw std::runtime_error( "too few arguments");

		if (lua_type( ls, 1) != LUA_TSTRING)
		{
			throw std::runtime_error("string expected as first argument (authorization function)");
		}
		const char* name = lua_tostring( ls, 1);

		proc::ExecContext* exc = getExecContext( ls);
		const AuditFunction* af = exc->provider()->auditFunction( name);
		if (af)
		{
			closure.reset( af->createClosure());
		}
		else
		{
			const FormFunction* ff = exc->provider()->formFunction( name);
			if (!ff) throw std::runtime_error( std::string( "audit function '") + name + "' not found");
			closure.reset( ff->createClosure());
		}
		TypedInputFilterR inp = get_operand_TypedInputFilter( ls, 2);
		closure->init( exc, inp, serialize::Flags::None);
	}
	else
	{
		FormFunctionClosureR* closureptr = LuaObject<FormFunctionClosureR>::get( ls, -1);
		closure = *closureptr;
		lua_pop( ls, 1);
	}
	if (!closure->call())
	{
		LuaObject<FormFunctionClosureR>::push_luastack( ls, closure);
		lua_yieldk( ls, 0, 1, function_audit);
	}
	return 0;
}

LUA_FUNCTION_THROWS( "provider.authorize(..)", function_authorize)
{
	int nn = lua_gettop( ls);
	if (nn > 2) throw std::runtime_error( "too many arguments");
	if (nn < 1) throw std::runtime_error( "too few arguments");

	if (lua_type( ls, 1) != LUA_TSTRING)
	{
		throw std::runtime_error("string expected as first argument (authorization function)");
	}
	const char* authorizationFunction = lua_tostring( ls, 1);
	std::string authorizationResource;

	if (nn >= 2)
	{
		if (lua_type( ls, 2) != LUA_TSTRING)
		{
			throw std::runtime_error("string expected as second argument (authorization resource)");
		}
		authorizationResource.append( lua_tostring( ls, 2));
	}
	proc::ExecContext* ctx = getExecContext( ls);
	std::string errmsg;
	lua_pushboolean( ls, ctx->checkAuthorization( authorizationFunction, authorizationResource, errmsg));
	return 1;
}

LUA_FUNCTION_THROWS( "provider.document(..)", function_document)
{
	check_parameters( ls, 0, 1, LUA_TSTRING);
	std::size_t contentlen;
	const char* content = lua_tolstring( ls, 1, &contentlen);

	proc::ExecContext* ctx = getExecContext( ls);
	std::string docformat;
	cmdbind::DoctypeDetectorR dt( ctx->provider()->doctypeDetector());

	dt->putInput( content, contentlen);
	if (dt->run())
	{
		if (dt->info().get())
		{
			//... if we cannot decide we define docformat as empty
			docformat = dt->info()->docformat();
		}
	}
	else
	{
		std::string msg( "document format not recognized");
		if (dt->lastError())
		{
			msg.append(": ");
			msg.append( dt->lastError());
		}
		throw std::runtime_error( msg);
	}
	LuaObject<Input>::push_luastack( ls, Input( docformat, std::string( content, contentlen)));
	return 1;
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
		if (!input->inputfilter()->getMetaData())
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
						LuaObject<RedirectFilterClosure>::push_luastack( ls, RedirectFilterClosure( inp, outp, false));
						closure = LuaObject<RedirectFilterClosure>::get( ls, -1);
					}
					if (!output->called())
					{
						Input* input = LuaObject<Input>::getGlobal( ls, "input");
						if (input && !input->inputfilter()->getMetaData())
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
		if (input && !input->inputfilter()->getMetaData())
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
	bool newEnter = (lua_getctx( ls, &ctx) != LUA_YIELD);
	if (newEnter)
	{
		output = LuaObject<Output>::getSelf( ls, "output", "opentag");
		if (lua_gettop( ls) != 2) throw std::runtime_error( "expected tag to open in ouput as argument");
		int tp = lua_type( ls, 2);
		if (tp != LUA_TSTRING && tp != LUA_TNUMBER) throw std::runtime_error( "expected string or number as first argument");
		tag = lua_tostring( ls, 2);
		tagsize = std::strlen( tag);
	}
	else
	{
		tag = (const char*)lua_tolstring( ls, -2, &tagsize);
		output = (Output*)lua_touserdata( ls, -1);
	}
	if (!output->called())
	{
		Input* input = LuaObject<Input>::getGlobal( ls, "input");
		if (input && !input->inputfilter()->getMetaData())
		{
			if (newEnter)
			{
				lua_pushlstring( ls, tag, tagsize);
				lua_tostring( ls, -1); //PF:BUGFIX lua 5.1.4 needs this one
				lua_pushlightuserdata( ls, output);
			}
			lua_yieldk( ls, 0, 1, function_output_opentag);
		}
		output->called(true);
	}
	if (!output->print( tag, tagsize, 0/*val*/, 0))
	{
		if (newEnter)
		{
			lua_pushlstring( ls, tag, tagsize);
			lua_tostring( ls, -1); //PF:BUGFIX lua 5.1.4 needs this one
			lua_pushlightuserdata( ls, output);
		}
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
		if (input && !input->inputfilter()->getMetaData())
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


LUA_FUNCTION_THROWS( "provider.filter(..)", function_filter)
{
	const char* name = "";
	std::vector<FilterArgument> arg;
	switch (lua_gettop( ls))
	{
		case 0:
		{
			LuaExceptionHandlerScope escope(ls);
			{
				Input* input = LuaObject<Input>::getGlobal( ls, "input");
				Output* output = LuaObject<Output>::getGlobal( ls, "output");
				if (!input || !output)
				{
					lua_pushnil( ls);
					//... cannot call filter with empty arguments because 
					//	the global input/output objects are not defined.
					//	they are only defined in lua command handler mode.
				}
				else
				{
					langbind::InputFilterR ifl;
					langbind::OutputFilterR ofl;
					if (input->inputfilter().get())
					{
						ifl.reset( input->inputfilter()->copy());
						ofl.reset( output->outputfilter()->copy());
					}
					LuaObject<Filter>::push_luastack( ls, Filter( ifl, ofl));
				}
				return 1;
			}
		}
		case 2:
			if (lua_type( ls, 2) == LUA_TTABLE)
			{
				lua_pushvalue( ls, 2);
				lua_pushnil( ls);
				while (lua_next( ls, -2))
				{
					if (lua_type( ls, -2) == LUA_TSTRING)
					{
						const char* val = lua_tostring( ls, -1);
						if (!val) throw std::runtime_error( "illegal filter argument value");
						arg.push_back( FilterArgument( lua_tostring( ls, -2), val));
					}
					else if (lua_type( ls, -2) == LUA_TNUMBER)
					{
						const char* val = lua_tostring( ls, -1);
						if (!val) throw std::runtime_error("illegal filter argument value");
						arg.push_back( FilterArgument( "", val));
					}
					else
					{
						throw std::runtime_error("illegal filter argument");
					}
					lua_pop( ls, 1);
				}
				lua_pop( ls, 1);
			}
			else
			{
				const char* val = lua_tostring( ls, -1);
				if (!val) throw std::runtime_error("illegal filter argument value");
				arg.push_back( FilterArgument( "", val));
			}
		case 1:
		{
			name = lua_tostring( ls, 1);
			if (!name) throw std::runtime_error( "filter name is not a string");

			proc::ExecContext* ctx = getExecContext( ls);
			const FilterType* filtertype = ctx->provider()->filterType( name);
			if (!filtertype)
			{
				throw std::runtime_error( std::string( "filter type '") + name + "' is not defined");
			}
			types::CountedReference<Filter> flt( filtertype->create( arg));

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
	if (filter->inputfilter().get())
	{
		input->setInputFilter( filter->inputfilter());

		if (input == LuaObject<Input>::getGlobal( ls, "input"))
		{
			//... the global input and output share the attributes
			Output* output = LuaObject<Output>::getGlobal( ls, "output");
			if (output && output->outputfilter().get())
			{
				output->outputfilter()->inheritMetaData( input->inputfilter()->getMetaDataRef());
			}
		}
	}
	else
	{
		throw std::runtime_error( "called with undefined input for the argument filter object");
	}
	return 0;
}

LUA_FUNCTION_THROWS( "input:docformat()", function_input_docformat)
{
	Input* input;
	int ctx;
	if (lua_getctx( ls, &ctx) != LUA_YIELD)
	{
		input = LuaObject<Input>::getSelf( ls, "input", "docformat");
		check_parameters( ls, 1, 0);
	}
	else
	{
		input = (Input*)lua_touserdata( ls, -1);
		lua_pop( ls, 1);
	}
	if (input->docformat().empty())
	{
		lua_pushnil( ls);
		return 1;
	}
	else
	{
		lua_pushlstring( ls, input->docformat().c_str(), input->docformat().size());
		lua_tostring( ls, -1); //PF:BUGFIX lua 5.1.4 needs this one
		return 1;
	}
}

LUA_FUNCTION_THROWS( "input:metadata()", function_input_metadata)
{
	Input* input;
	int ctx;
	if (lua_getctx( ls, &ctx) != LUA_YIELD)
	{
		input = LuaObject<Input>::getSelf( ls, "input", "metadata");
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
			const types::DocMetaData* metadata = input->inputfilter()->getMetaData();
			if (metadata)
			{
				lua_newtable( ls);
				std::vector<types::DocMetaData::Attribute>::const_iterator ai = metadata->attributes().begin(), ae = metadata->attributes().end();
				for (; ai != ae; ++ai)
				{
					lua_pushlstring( ls, ai->value.c_str(), ai->value.size());
					lua_tostring( ls, -1); //PF:BUGFIX lua 5.1.4 needs this one
					lua_setfield( ls, -2, ai->name.c_str());
				}
				return 1;
			}
			else if (input->inputfilter()->state() == InputFilter::Error)
			{
				const char* err = input->inputfilter()->getError();
				std::string msg( "error parsing DOCTYPE: ");
				msg.append( err?err:"unknown");
				lua_pop( ls, 1);
				throw std::runtime_error(msg);
			}
		}
	}
	lua_pushlightuserdata( ls, input);
	lua_yieldk( ls, 0, 1, function_input_metadata);
	return 0;
}


LUA_FUNCTION_THROWS( "input:doctype()", function_input_doctypeid)
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
		const types::DocMetaData* metadata = input->inputfilter()->getMetaData();
		if (metadata)
		{
			std::string doctype = metadata->doctype();
			LuaExceptionHandlerScope escope(ls);
			{
				if (!doctype.empty())
				{
					lua_pushlstring( ls, doctype.c_str(), doctype.size());
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
	Output* output = LuaObject<Output>::getSelf( ls, "output", "as"); //< self argument (mandatory)
	Filter* filter = 0;
	types::DocMetaData docmetadata;
	bool docmetadata_defined = false;
	std::string doctype;
	bool doctype_defined = false;
	int ii=2,nn = lua_gettop( ls);
	if (nn <= 1)
	{
		throw std::runtime_error( "too few arguments");
	}
	else if (nn > 4)
	{
		throw std::runtime_error( "too many arguments");
	}
	for (; ii <= nn; ++ii)
	{
		if (lua_type( ls, ii) == LUA_TSTRING)
		{
			if (doctype_defined) throw std::runtime_error( "argument string specifying the document type or document metadata table element with name 'doctype' specified twice");
			doctype_defined = true;
			doctype = lua_tostring( ls, ii);
		}
		else if (lua_type( ls, ii) == LUA_TTABLE)
		{
			if (docmetadata_defined) throw std::runtime_error( "argument document meta data table specified twice");
			docmetadata_defined = true;
			lua_pushnil( ls);
			while (lua_next( ls, ii))
			{
				if (lua_type( ls, -2) != LUA_TSTRING || lua_type( ls, -1) != LUA_TSTRING)
				{
					lua_pop( ls, 1);
					throw std::runtime_error( "only strings expected as keys and values in document meta data table");
				}
				const char* idstr = lua_tostring( ls, -2);
				docmetadata.setAttribute( idstr, lua_tostring( ls, -1));
				lua_pop( ls, 1);
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
			std::runtime_error( "string (doctype id), table (doctype) or filter expected as argument");
		}
	}
	if (filter)
	{
		if (filter->outputfilter().get())
		{
			output->setOutputFilter( filter->outputfilter());
			if (output == LuaObject<Output>::getGlobal( ls, "output"))
			{
				//... the global input and output share the attributes
				Input* input = LuaObject<Input>::getGlobal( ls, "input");
				if (input && input->inputfilter().get())
				{
					output->outputfilter()->inheritMetaData( input->inputfilter()->getMetaDataRef());
				}
			}
		}
		else
		{
			throw std::runtime_error( "called with undefined filter argument");
		}
	}
	types::DocMetaData allmetadata;
	if (doctype_defined)
	{
		proc::ExecContext* gtc = getExecContext( ls);
		const types::FormDescription* formdescr = gtc->provider()->formDescription( doctype);
		if (formdescr)
		{
			allmetadata.join( formdescr->metadata().attributes());
		}
	}
	if (docmetadata_defined)
	{
		allmetadata.join( docmetadata.attributes());
	}
	if (doctype_defined)
	{
		allmetadata.setDoctype( doctype);
	}
	if (doctype_defined || docmetadata_defined)
	{
		output->outputfilter()->setMetaData( allmetadata);
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
	serialize::DDLFormSerializer* result = (serialize::DDLFormSerializer*)lua_touserdata( ls, -1);
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
	serialize::DDLFormParser* closure = (serialize::DDLFormParser*)lua_touserdata( ls, -1);
	lua_pop( ls, 1);

	if (!closure->call())
	{
		lua_pushlightuserdata( ls, closure);
		lua_yieldk( ls, 0, 1, function_input_table_DDLFormParser);
	}
	{
		TypedOutputFilterR outp( new LuaTableOutputFilter( ls));
		LuaObject<serialize::DDLFormSerializer>::push_luastack( ls, serialize::DDLFormSerializer( closure->form()));
		serialize::DDLFormSerializer* result = LuaObject<serialize::DDLFormSerializer>::get( ls, -1);
		result->init( outp, serialize::Flags::SerializeWithIndices);
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
	serialize::DDLFormParser* closure = (serialize::DDLFormParser*)lua_touserdata( ls, -1);
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

static lua_CFunction get_input_struct_table_closure( lua_State* ls, Input* input)
{
	if (input->inputfilter().get())
	{
		//... the filter provides no structure info. try to get the 'form' from the document type for it
		const types::DocMetaData* docmetadata;
		if (0!=(docmetadata = input->inputfilter()->getMetaData()))
		{
			std::string doctype = docmetadata->doctype();
			if (!doctype.empty())
			{
				//... document with !DOCTYPE declaration -> lookup for form
				proc::ExecContext* gtc = getExecContext( ls);
				const types::FormDescription* st = gtc->provider()->formDescription( doctype);
				if (!st)
				{
					LOG_DEBUG << "no form defined for document type '" << doctype << "'";

					//... no form defined -> map it without structure info but issue a warning
					TypedInputFilterR inp( new TypingInputFilter( input->getIterator()));
					if (!inp->setFlags( TypedInputFilter::SerializeWithIndices))
					{
						LOG_WARNING << "calling :table() on document type '" << doctype << "' without form defined for filter without input structure info";
					}
					TypedOutputFilterR outp( new LuaTableOutputFilter( ls));
					LuaObject<RedirectFilterClosure>::push_luastack( ls, RedirectFilterClosure( inp, outp, false));
					RedirectFilterClosure* obj = LuaObject<RedirectFilterClosure>::get( ls, -1);
					lua_pushlightuserdata( ls, obj);
					return &function_input_table_RedirectFilterClosure;
				}
				else
				{
					//... form defined. pass filter input through form to get the structure info
					types::FormR form( new types::Form( st));
	
					serialize::DDLFormParser* closure;
					serialize::Flags::Enum flags = serialize::Flags::ValidateAttributes;
					TypedInputFilterR inp( new TypingInputFilter( input->getIterator()));
					LuaObject<serialize::DDLFormParser>::push_luastack( ls, serialize::DDLFormParser( form));
					closure = LuaObject<serialize::DDLFormParser>::get( ls, -1);
					closure->init( inp, flags);
					lua_pushlightuserdata( ls, closure);
					return &function_input_table_DDLFormParser;
				}
			}
			else
			{
				//... document is standalone -> map it without structure info but issue a warning
				TypedInputFilterR inp( new TypingInputFilter( input->getIterator()));
				if (!inp->setFlags( TypedInputFilter::SerializeWithIndices))
				{
					LOG_WARNING << "calling table() on standalone document for filter without input structure info";
				}
				TypedOutputFilterR outp( new LuaTableOutputFilter( ls));
				LuaObject<RedirectFilterClosure>::push_luastack( ls, RedirectFilterClosure( inp, outp, false));
				RedirectFilterClosure* obj = LuaObject<RedirectFilterClosure>::get( ls, -1);
				lua_pushlightuserdata( ls, obj);
				return &function_input_table_RedirectFilterClosure;
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

static lua_CFunction get_input_struct_form_closure( lua_State* ls, Input* input)
{
	if (input->inputfilter().get())
	{
		const types::DocMetaData* docmetadata;
		// try to get 'form' for validation if the document is not standalone:
		if (0!=(docmetadata = input->inputfilter()->getMetaData()))
		{
			std::string doctype = docmetadata->doctype();
			if (!doctype.empty())
			{
				proc::ExecContext* gtc = getExecContext( ls);
				const types::FormDescription* st = gtc->provider()->formDescription( doctype);
				if (!st)
				{
					return &function_input_table_nil;
				}
				else
				{
					types::FormR form( new types::Form( st));
	
					serialize::DDLFormParser* closure;
					serialize::Flags::Enum flags = serialize::Flags::ValidateAttributes;
					TypedInputFilterR inp( new TypingInputFilter( input->getIterator()));
					LuaObject<serialize::DDLFormParser>::push_luastack( ls, serialize::DDLFormParser( form));
					closure = LuaObject<serialize::DDLFormParser>::get( ls, -1);
					closure->init( inp, flags);
					lua_pushlightuserdata( ls, closure);
					return &function_input_form_DDLFormParser;
				}
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
		input = LuaObject<Input>::getSelf( ls, "input", "value");
		check_parameters( ls, 1, 0);
	}
	else
	{
		input = (Input*)lua_touserdata( ls, -1);
		lua_pop( ls, 1);
	}
	lua_CFunction func = get_input_struct_table_closure( ls, input);
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
	lua_CFunction func = get_input_struct_form_closure( ls, input);
	if (!func)
	{
		lua_pushlightuserdata( ls, input);
		return lua_yieldk( ls, 0, 1, function_input_form);
	}
	return func( ls);
}


static int callUnaryOperator( lua_State* ls, types::CustomDataType::UnaryOperatorType optype, const types::CustomDataValue* operand)
{
	check_parameters( ls, 1, 0);
	types::CustomDataType::UnaryOperator opfunc = operand->type()->getOperator( optype);
	if (!opfunc) throw std::runtime_error( std::string( "undefined unary operator '") + types::CustomDataType::unaryOperatorTypeName( optype) + "' for custom data type '" + operand->type()->name() + "'");
	pushVariantValue( ls, opfunc( *operand));
	return 1;
}

static int callBinaryOperator( lua_State* ls, types::CustomDataType::BinaryOperatorType optype, const types::CustomDataValue* operand)
{
	int nofarg = lua_gettop( ls);
	if (nofarg < 2)
	{
		throw std::runtime_error( "too few arguments");
	}
	else if (nofarg > 2)
	{
		throw std::runtime_error( "too many arguments");
	}
	types::VariantConst arg;
	getVariantValue( ls, arg, 2);
	types::CustomDataType::BinaryOperator opfunc = operand->type()->getOperator( optype);
	if (!opfunc) throw std::runtime_error( std::string( "undefined binary operator '") + types::CustomDataType::binaryOperatorTypeName( optype) + "' for custom data type '" + operand->type()->name() + "'");
	pushVariantValue( ls, (*opfunc)( *operand, arg));
	return 1;
}

static int callConversionOperator( lua_State* ls, types::CustomDataType::ConversionOperatorType optype, const types::CustomDataValue* operand)
{
	types::CustomDataType::ConversionOperator opfunc = operand->type()->getOperator( optype);
	if (!opfunc) throw std::runtime_error( std::string( "undefined conversion operator '") + types::CustomDataType::conversionOperatorTypeName( optype) + "' for custom data type '" + operand->type()->name() + "'");
	pushVariantValue( ls, (*opfunc)( *operand));
	return 1;
}

static int callDimensionOperator( lua_State* ls, types::CustomDataType::DimensionOperatorType optype, const types::CustomDataValue* operand)
{
	types::CustomDataType::DimensionOperator opfunc = operand->type()->getOperator( optype);
	if (!opfunc) throw std::runtime_error( std::string( "undefined dimension operator '") + types::CustomDataType::dimensionOperatorTypeName( optype) + "' for custom data type '" + operand->type()->name() + "'");
	lua_pushinteger( ls, (*opfunc)( *operand));
	return 1;
}

static int callCompare( lua_State* ls, const types::CustomDataValue* operand)
{
	int nofarg = lua_gettop( ls);
	if (nofarg < 2)
	{
		throw std::runtime_error( "too few arguments");
	}
	else if (nofarg > 2)
	{
		throw std::runtime_error( "too many arguments");
	}
	types::VariantConst arg;
	getVariantValue( ls, arg, 2);
	if (arg.type() == types::Variant::Custom)
	{
		if (arg.data().value.Custom->type() == operand->type())
		{
			return operand->compare( *arg.data().value.Custom);
		}
		else
		{
			throw  std::runtime_error("different custom data type values are uncomparable");
		}
	}
	else
	{
		types::CustomDataValueR cstarg( operand->type()->createValue( operand->initializer()));
		cstarg->assign( arg);
		return operand->compare( *cstarg.get());
	}
}

struct CustomDataValueMethodDef
{
	const char* name;
	types::CustomDataType::CustomDataValueMethod call;
};

LUA_FUNCTION_THROWS( "<custom>:<method>()", function_customtype_methodcall)
{
	types::CustomDataValueR* operand = LuaObject<types::CustomDataValueR>::get( ls, lua_upvalueindex( 1));
	const CustomDataValueMethodDef* methoddef = (const CustomDataValueMethodDef*)lua_touserdata( ls, lua_upvalueindex( 2));
	if (!operand || !methoddef) throw std::runtime_error( "invalid call of method");
	int ii=1,nn=lua_gettop(ls); 
	std::vector<types::Variant> args;
	for (; ii<=nn; ++ii)
	{
		types::VariantConst arg;
		getVariantValue( ls, arg, ii);
		args.push_back( arg);
	}
	try
	{
		types::Variant res = methoddef->call( **operand, args);
		if (res.defined())
		{
			pushVariantValue( ls, res);
			return 1;
		}
		else
		{
			return 0;
		}
	}
	catch (const std::bad_alloc& e)
	{
		throw e;
	}
	catch (const std::runtime_error& e)
	{
		throw std::runtime_error( std::string( "error calling custom data type method '") + methoddef->name + "':" + e.what());
	}
}

LUA_FUNCTION_THROWS( "custom:__index()", function_customtype_index)
{
	types::CustomDataValueR* operand = LuaObject<types::CustomDataValueR>::getSelf( ls, "custom", "__unm");
	check_parameters( ls, 1, 1, LUA_TSTRING);
	const char* methodname = lua_tostring( ls, 2);
	types::CustomDataType::CustomDataValueMethod method = (*operand)->type()->getMethod( methodname);
	if (!method)
	{
		return 0; //... return NIL
	}
	LuaObject<types::CustomDataValueR>::push_luastack( ls, *operand);
	CustomDataValueMethodDef* methoddef = (CustomDataValueMethodDef*)lua_newuserdata( ls, sizeof(CustomDataValueMethodDef));
	if (!methoddef) throw std::bad_alloc();
	methoddef->name = methodname;
	methoddef->call = method;
	lua_pushcclosure( ls, function_customtype_methodcall, 2);
	return 1;
}

LUA_FUNCTION_THROWS( "custom:__unm()", function_customtype_unm)
{
	types::CustomDataValueR* operand = LuaObject<types::CustomDataValueR>::getSelf( ls, "custom", "__unm");
	return callUnaryOperator( ls, types::CustomDataType::Negation, operand->get());
}

LUA_FUNCTION_THROWS( "custom:__add()", function_customtype_add)
{
	types::CustomDataValueR* operand = LuaObject<types::CustomDataValueR>::getSelf( ls, "custom", "__add");
	return callBinaryOperator( ls, types::CustomDataType::Add, operand->get());
}

LUA_FUNCTION_THROWS( "custom:__sub()", function_customtype_sub)
{
	types::CustomDataValueR* operand = LuaObject<types::CustomDataValueR>::getSelf( ls, "custom", "__sub");
	return callBinaryOperator( ls, types::CustomDataType::Subtract, operand->get());
}

LUA_FUNCTION_THROWS( "custom:__mul()", function_customtype_mul)
{
	types::CustomDataValueR* operand = LuaObject<types::CustomDataValueR>::getSelf( ls, "custom", "__mul");
	return callBinaryOperator( ls, types::CustomDataType::Multiply, operand->get());
}

LUA_FUNCTION_THROWS( "custom:__div()", function_customtype_div)
{
	types::CustomDataValueR* operand = LuaObject<types::CustomDataValueR>::getSelf( ls, "custom", "__div");
	return callBinaryOperator( ls, types::CustomDataType::Divide, operand->get());
}

LUA_FUNCTION_THROWS( "custom:__pow()", function_customtype_pow)
{
	types::CustomDataValueR* operand = LuaObject<types::CustomDataValueR>::getSelf( ls, "custom", "__pow");
	return callBinaryOperator( ls, types::CustomDataType::Power, operand->get());
}

LUA_FUNCTION_THROWS( "custom:__concat()", function_customtype_concat)
{
	types::CustomDataValueR* operand = LuaObject<types::CustomDataValueR>::getSelf( ls, "custom", "__concat");
	return callBinaryOperator( ls, types::CustomDataType::Concat, operand->get());
}

LUA_FUNCTION_THROWS( "custom:__tostring()", function_customtype_tostring)
{
	types::CustomDataValueR* operand = LuaObject<types::CustomDataValueR>::getSelf( ls, "custom", "__tostring");
	std::string val( (*operand)->tostring());
	lua_pushlstring( ls, val.c_str(), val.size());
	lua_tostring( ls, -1); //PF:BUGFIX lua 5.1.4 needs this one
	return 1;
}

LUA_FUNCTION_THROWS( "custom:tonumber()", function_customtype_tonumber)
{
	types::CustomDataValueR* operand = LuaObject<types::CustomDataValueR>::getSelf( ls, "custom", "tonumber");
	return callConversionOperator( ls, types::CustomDataType::ToDouble, operand->get());
}

LUA_FUNCTION_THROWS( "custom:typename()", function_customtype_typename)
{
	types::CustomDataValueR* operand = LuaObject<types::CustomDataValueR>::getSelf( ls, "custom", "typename");
	const std::string& val( (*operand)->type()->name());
	lua_pushlstring( ls, val.c_str(), val.size());
	lua_tostring( ls, -1); //PF:BUGFIX lua 5.1.4 needs this one
	return 1;
}

LUA_FUNCTION_THROWS( "custom:__len()", function_customtype_len)
{
	types::CustomDataValueR* operand = LuaObject<types::CustomDataValueR>::getSelf( ls, "custom", "__len");
	return callDimensionOperator( ls, types::CustomDataType::Length, operand->get());
}

LUA_FUNCTION_THROWS( "custom:__eq()", function_customtype_eq)
{
	types::CustomDataValueR* operand = LuaObject<types::CustomDataValueR>::getSelf( ls, "custom", "__eq");
	return (0==callCompare( ls, operand->get()));
}

LUA_FUNCTION_THROWS( "custom:__lt()", function_customtype_lt)
{
	types::CustomDataValueR* operand = LuaObject<types::CustomDataValueR>::getSelf( ls, "custom", "__lt");
	return (0>callCompare( ls, operand->get()));
}

LUA_FUNCTION_THROWS( "custom:__le()", function_customtype_le)
{
	types::CustomDataValueR* operand = LuaObject<types::CustomDataValueR>::getSelf( ls, "custom", "__le");
	return (0>=callCompare( ls, operand->get()));
}

LUA_FUNCTION_THROWS( "datetime:__tostring()", function_datetime_tostring)
{
	types::DateTime* operand = LuaObject<types::DateTime>::getSelf( ls, "datetime", "__tostring");
	std::string val( operand->tostring());
	lua_pushlstring( ls, val.c_str(), val.size());
	lua_tostring( ls, -1); //PF:BUGFIX lua 5.1.4 needs this one
	return 1;
}

LUA_FUNCTION_THROWS( "datetime:year()", function_datetime_year)
{
	types::DateTime* operand = LuaObject<types::DateTime>::getSelf( ls, "datetime", "__tostring");
	lua_pushinteger( ls, operand->year());
	return 1;
}

LUA_FUNCTION_THROWS( "datetime:month()", function_datetime_month)
{
	types::DateTime* operand = LuaObject<types::DateTime>::getSelf( ls, "datetime", "__tostring");
	lua_pushinteger( ls, operand->month());
	return 1;
}

LUA_FUNCTION_THROWS( "datetime:day()", function_datetime_day)
{
	types::DateTime* operand = LuaObject<types::DateTime>::getSelf( ls, "datetime", "__tostring");
	lua_pushinteger( ls, operand->day());
	return 1;
}

LUA_FUNCTION_THROWS( "datetime:hour()", function_datetime_hour)
{
	types::DateTime* operand = LuaObject<types::DateTime>::getSelf( ls, "datetime", "__tostring");
	lua_pushinteger( ls, operand->hour());
	return 1;
}

LUA_FUNCTION_THROWS( "datetime:minute()", function_datetime_minute)
{
	types::DateTime* operand = LuaObject<types::DateTime>::getSelf( ls, "datetime", "__tostring");
	lua_pushinteger( ls, operand->minute());
	return 1;
}

LUA_FUNCTION_THROWS( "datetime:second()", function_datetime_second)
{
	types::DateTime* operand = LuaObject<types::DateTime>::getSelf( ls, "datetime", "__tostring");
	lua_pushinteger( ls, operand->second());
	return 1;
}

LUA_FUNCTION_THROWS( "datetime:millisecond()", function_datetime_millisecond)
{
	types::DateTime* operand = LuaObject<types::DateTime>::getSelf( ls, "datetime", "__tostring");
	lua_pushinteger( ls, operand->millisecond());
	return 1;
}

LUA_FUNCTION_THROWS( "datetime:microsecond()", function_datetime_microsecond)
{
	types::DateTime* operand = LuaObject<types::DateTime>::getSelf( ls, "datetime", "__tostring");
	lua_pushinteger( ls, operand->microsecond());
	return 1;
}

LUA_FUNCTION_THROWS( "bignumber:__tostring()", function_bignumber_tostring)
{
	types::BigNumber* operand = LuaObject<types::BigNumber>::getSelf( ls, "bignumber", "__tostring");
	std::string val( operand->tostring());
	lua_pushlstring( ls, val.c_str(), val.size());
	lua_tostring( ls, -1); //PF:BUGFIX lua 5.1.4 needs this one
	return 1;
}

LUA_FUNCTION_THROWS( "bignumber:precision()", function_bignumber_precision)
{
	types::BigNumber* operand = LuaObject<types::BigNumber>::getSelf( ls, "bignumber", "precision");
	lua_pushinteger( ls, operand->precision());
	return 1;
}

LUA_FUNCTION_THROWS( "bignumber:scale()", function_bignumber_scale)
{
	types::BigNumber* operand = LuaObject<types::BigNumber>::getSelf( ls, "bignumber", "scale");
	lua_pushinteger( ls, operand->scale());
	return 1;
}

LUA_FUNCTION_THROWS( "bignumber:digits()", function_bignumber_digits)
{
	types::BigNumber* operand = LuaObject<types::BigNumber>::getSelf( ls, "bignumber", "digits");
	const unsigned char* ar = operand->digits();
	unsigned int ii=0, nn=operand->size();
	std::string val;
	for (; ii<nn; ++ii)
	{
		val.push_back( ar[ii]+'0');
	}
	lua_pushlstring( ls, val.c_str(), val.size());
	lua_tostring( ls, -1); //PF:BUGFIX lua 5.1.4 needs this one
	return 1;
}

LUA_FUNCTION_THROWS( "bignumber:tonumber()", function_bignumber_tonumber)
{
	types::BigNumber* operand = LuaObject<types::BigNumber>::getSelf( ls, "bignumber", "tonumber");
	lua_pushnumber( ls, operand->todouble());
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
		_Wolframe::log::Logger( _Wolframe::log::LogBackend::instance() ).Get( lv ) << logmsg;
	}
	return 0;
}

LUA_FUNCTION_THROWS( "logger.printc(..)", function_logger_printc)
{
	/* first parameter maps to a log level, rest gets printed depending on
	 * whether it's a string or a number
	 */
	int ii,nn = lua_gettop(ls);
	std::string logmsg;
	for (ii=1; ii<=nn; ii++)
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

static const luaL_Reg input_methodtable[ 9] =
{
	{"as",&function_input_as},
	{"form",&function_input_form},
	{"value",&function_input_table},
	{"table",&function_input_table},
	{"metadata",function_input_metadata},
	{"doctype",function_input_doctypeid},
	{"docformat",function_input_docformat},
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

static const luaL_Reg typedinputfilter_methodtable[ 5] =
{
	{"value",&function_typedinputfilter_table},
	{"table",&function_typedinputfilter_table},
	{"get", &function_typedinputfilter_get},
	{"__tostring",&function_typedinputfilter_tostring},
	{0,0}
};

static const luaL_Reg struct_methodtable[ 5] =
{
	{"value",&function_struct_table},
	{"table",&function_struct_table},
	{"get", &function_struct_get},
	{"__tostring",&function_struct_tostring},
	{0,0}
};

static const luaL_Reg form_methodtable[ 8] =
{
	{"name",&function_form_name},
	{"value",&function_form_table},
	{"table",&function_form_table},
	{"get",&function_form_get},
	{"fill",&function_form_fill},
	{"metadata",&function_form_metadata},
	{"__tostring",&function_form_tostring},
	{0,0}
};

static const luaL_Reg provider_methodtable[ 8] =
{
	{"filter",&function_filter},
	{"form",&function_form},
	{"type",&function_type},
	{"formfunction",&function_formfunction},
	{"authorize",&function_authorize},
	{"audit",&function_audit},
	{"document",&function_document},
	{0,0}
};

static const luaL_Reg iterator_methodtable[ 2] =
{
	{"scope",&function_scope},
	{0,0}
};

static const luaL_Reg customvalue_methodtable[ 16] =
{
	{"__index", &function_customtype_index},
	{"__unm", &function_customtype_unm},
	{"__add", &function_customtype_add},
	{"__sub", &function_customtype_sub},
	{"__mul", &function_customtype_mul},
	{"__div", &function_customtype_div},
	{"__pow", &function_customtype_pow},
	{"__concat", &function_customtype_concat},
	{"__tostring", &function_customtype_tostring},
	{"__len", &function_customtype_len},
	{"__eq", &function_customtype_eq},
	{"__lt", &function_customtype_lt},
	{"__le", &function_customtype_le},
	{"tonumber", &function_customtype_tonumber},
	{"typename", &function_customtype_typename},
	{0,0}
};

static const luaL_Reg datetime_methodtable[ 10] =
{
	{"year", &function_datetime_year},
	{"month", &function_datetime_month},
	{"day", &function_datetime_day},
	{"hour", &function_datetime_hour},
	{"minute", &function_datetime_minute},
	{"second", &function_datetime_second},
	{"millisecond", &function_datetime_millisecond},
	{"microsecond", &function_datetime_microsecond},
	{"__tostring", &function_datetime_tostring},
	{0,0}
};

static const luaL_Reg bignumber_methodtable[ 6] =
{
	{"precision", &function_bignumber_precision},
	{"scale", &function_bignumber_scale},
	{"digits", &function_bignumber_digits},
	{"tonumber", &function_bignumber_tonumber},
	{"__tostring", &function_bignumber_tostring},
	{0,0}
};

static std::string getLuaErrorMessage( lua_State* ls, int index, const std::string& path)
{
	std::string rt;
	const char* msg = lua_tostring( ls, index);
	if (!msg) msg = "";
	std::string scriptfilename( utils::getFileStem( path));
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

static LuaDump* createLuaScriptDump( const std::string& path)
{
	lua_State* ls = luaL_newstate();
	if (!ls) throw std::runtime_error( "failed to create lua state");
	std::string content = utils::readSourceFileContent( path);

	if (luaL_loadbuffer( ls, content.c_str(), content.size(), path.c_str()))
	{
		std::ostringstream buf;
		buf << "Failed to load script '" << path << "':" << getLuaErrorMessage( ls, -1, path);
		lua_close( ls);
		throw std::runtime_error( buf.str());
	}
	LuaDump* rt = luaCreateDump( ls);
	lua_close( ls);
	return rt;
}

LuaScript::LuaScript( const std::string& path_)
	:m_path(path_)
{
	// Load the source of the script from file
	m_content = boost::shared_ptr<LuaDump>( createLuaScriptDump( m_path), freeLuaDump);
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
	:m_ls(0)
	,m_thread(0)
	,m_threadref(0)
	,m_script(script_)
	,m_modulemap(modulemap_)
{}

std::string LuaScriptInstance::luaErrorMessage( lua_State* ls_, int index)
{
	return getLuaErrorMessage( ls_, index, script()->path());
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

void LuaScriptInstance::init( proc::ExecContext* ctx_)
{
	initbase( ctx_, true);
}

void LuaScriptInstance::initbase( proc::ExecContext* ctx_, bool callMain)
{
	m_ls = luaL_newstate();
	if (!m_ls) throw std::runtime_error( "failed to create lua state");

	LuaExceptionHandlerScope luaThrows(m_ls);
	{
		// create thread and prevent garbage collecting of it (http://permalink.gmane.org/gmane.comp.lang.lua.general/22680)
		m_thread = lua_newthread( m_ls);
		lua_pushvalue( m_ls, -1);
		m_threadref = luaL_ref( m_ls, LUA_REGISTRYINDEX);

		// load script content from dump:
		luaLoadDump( m_ls, m_script->content());

		// open standard lua libraries (we load all of them):
		luaL_openlibs( m_ls);

		// register objects already here that may be used in the initilization part:
		lua_newtable( m_ls);
		luaL_setfuncs( m_ls, logger_methodtable, 0);
		lua_setglobal( m_ls, "logger");

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
		LuaObject<RedirectFilterClosure>::createMetatable( m_ls, 0, 0, 0/*mt*/, 0/*typename*/);
		LuaObject<types::FormR>::createMetatable( m_ls, 0, 0, form_methodtable, "form");
		LuaObject<types::CustomDataInitializerR>::createMetatable( m_ls, 0, 0, 0/*mt*/, 0/*typename*/);
		LuaObject<types::CustomDataValueR>::createMetatable( m_ls, 0, 0, customvalue_methodtable, "custom");
		LuaObject<types::DateTime>::createMetatable( m_ls, 0, 0, datetime_methodtable, "datetime");
		LuaObject<types::BigNumber>::createMetatable( m_ls, 0, 0, bignumber_methodtable, "bignumber");
		LuaObject<serialize::DDLFormParser>::createMetatable( m_ls, 0, 0, 0/*mt*/, 0/*typename*/);
		LuaObject<serialize::DDLFormSerializer>::createMetatable( m_ls, 0, 0, 0/*mt*/, 0/*typename*/);
		LuaObject<serialize::StructSerializer>::createMetatable( m_ls, 0, 0, struct_methodtable, 0/*typename*/);
		LuaObject<InputFilterClosure>::createMetatable( m_ls, 0, 0, 0/*mt*/, 0/*typename*/);
		LuaObject<TypedInputFilterR>::createMetatable( m_ls, 0, 0, typedinputfilter_methodtable, 0/*typename*/);
		LuaObject<TypedInputFilterClosure>::createMetatable( m_ls, 0, 0, 0/*mt*/, 0/*typename*/);
		LuaObject<FormFunctionClosureR>::createMetatable( m_ls, 0, 0, 0/*mt*/, 0/*typename*/);
		LuaObject<types::NormalizeFunctionR>::createMetatable( m_ls, 0, 0, 0/*mt*/, 0/*typename*/);
		LuaObject<Input>::createMetatable( m_ls, 0, 0, input_methodtable, 0/*typename*/);
		LuaObject<Output>::createMetatable( m_ls, 0, 0, output_methodtable, 0/*typename*/);

		if (ctx_) setExecContext( m_ls, ctx_);
		LuaObject<Filter>::createMetatable( m_ls, &function__LuaObject__index<Filter>, &function__LuaObject__newindex<Filter>, 0/*mt*/, "filter");

		//Register iterator context:
		lua_newtable( m_ls);
		luaL_setfuncs( m_ls, iterator_methodtable, 0);
		lua_setglobal( m_ls, "iterator");

		//Register provider context:
		lua_newtable( m_ls);
		luaL_setfuncs( m_ls, provider_methodtable, 0);
		lua_setglobal( m_ls, "provider");
	}
}

void LuaScriptInstance::init( const Input& input_, const Output& output_, proc::ExecContext* ctx_)
{
	initbase( ctx_, true);
	LuaExceptionHandlerScope luaThrows(m_ls);
	{
		LuaObject<Input>::createGlobal( m_ls, "input", input_);
		LuaObject<Output>::createGlobal( m_ls, "output", output_);
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
		m_ar.push_back( LuaScriptR( new LuaScript( script)));
		m_pathmap[ script.path()] = scriptId;
	}
	m_procmap[ nam] = scriptId;
}

LuaScriptInstance* LuaFunctionMap::createLuaScriptInstance( const std::string& procname) const
{
	std::string nam( procname);
	std::transform( nam.begin(), nam.end(), nam.begin(), ::tolower);

	std::map<std::string,std::size_t>::const_iterator ii=m_procmap.find( nam),ee=m_procmap.end();
	if (ii == ee)
	{
		throw std::runtime_error( std::string("function '") + nam + "' is not defined in script");
	}
	return new LuaScriptInstance( m_ar[ ii->second].get(), m_modulemap);
}

std::vector<std::string> LuaFunctionMap::commands() const
{
	std::vector<std::string> rt;
	std::map<std::string,std::size_t>::const_iterator ii = m_procmap.begin(), ee = m_procmap.end();
	for (; ii != ee; ++ii) rt.push_back( ii->first);
	return rt;
}

TypedInputFilterR LuaScriptInstance::getObject( int idx)
{
	TypedInputFilterR rt;
	int typ = lua_type( thread(), idx);
	if (typ != LUA_TUSERDATA && typ != LUA_TLIGHTUSERDATA)
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
		throw std::runtime_error( "cannot handle this object type");
	}
	return rt;
}

void LuaScriptInstance::pushObject( const TypedInputFilterR& obj)
{
	LuaObject<TypedInputFilterR>::push_luastack( thread(), obj);
}




