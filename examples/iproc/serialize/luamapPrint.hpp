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
///\file serialize/luamapPrint.hpp
///\brief Defines the intrusive implementation of the printing part of serialization for the lua map

#ifndef _Wolframe_LUAMAP_PRINT_HPP_INCLUDED
#define _Wolframe_LUAMAP_PRINT_HPP_INCLUDED
#include "serialize/mapContext.hpp"
#include "serialize/luamapTraits.hpp"
#include <boost/utility/value_init.hpp> 
#include <vector> 

namespace _Wolframe {
namespace serialize {

template <typename T>
struct IntrusivePrinter;

template <typename T>
bool pushAtom_( const void* obj, const luanumeric_&, lua_State* ls, Context*)
{
	lua_pushnumber( ls, *((T*)obj));
	return true;
}

template <typename T>
bool pushAtom_( const void* obj, const luabool_&, lua_State* ls, Context*)
{
	lua_pushboolean( ls, *((T*)obj));
	return true;
}

template <typename T>
bool pushAtom_( const void* obj, const luastring_&, lua_State* ls, Context*)
{
	lua_pushstring( ls, ((T*)obj)->c_str());
	return true;
}

template <typename T>
bool pushAtom_( const void* obj, const luastruct_&, lua_State* ls, Context* ctx)
{
	ctx->setError( 0, "atomic value expected");
	return false;
}

template <typename T>
bool push_( const void* obj, lua_State* ls, Context* ctx)
{
	return pushAtom_<T>( obj, getLuaCategory(T()), ls, ctx);
}


template <typename T>
bool printObject_( const void* obj, const struct_&, lua_State* ls, Context* ctx)
{
	static const DescriptionBase* descr = T::getDescription();
	
	lua_newtable( ls);
	DescriptionBase::Map::const_iterator itr = descr->begin();
	while (itr != descr->end())
	{
		lua_pushstring( ls, itr->first);
		if (!itr->second.print()( (char*)obj+itr->second.ofs(), ls, ctx))
		{
			ctx->setError( itr->first);
			return false;
		}
		lua_settable( ls, -2);
		++itr;
	}
	return true;
}

template <typename T>
bool printObject_( const void* obj, const arithmetic_&, lua_State* ls, Context* ctx)
{
	return push_<T>( obj, ls, ctx);
}

template <typename T>
bool printObject_( const void* obj, const vector_&, lua_State* ls, Context* ctx)
{
	lua_newtable( ls);
	std::size_t index = 0;

	typename T::const_iterator itr = ((T*)obj)->begin();
	while (itr != ((T*)obj)->end())
	{
		lua_pushnumber( ls, ++index);
		if (!IntrusivePrinter<typename T::value_type>::print( (const void*)&(*itr), ls, ctx))
		{
			return false;
		}
		lua_settable( ls, -2);
		++itr;
	}
	return true;
}

template <typename T>
struct IntrusivePrinter
{
	static bool print( const void* obj, lua_State* ls, Context* ctx)
	{
		return printObject_<T>( obj, getCategory(*(T*)obj), ls, ctx);
	}
};

/* Aba, Windows: This gives:

	cl.exe /MD /D_SCL_SECURE_NO_WARNINGS=1 /D_CRT_SECURE_NO_WARNINGS=1 /W2 /WX /nologo /O2 /EHsc /c /I.  /I..\..\include  /I"C:\Cygwin\home\Andreas\boost_1_46_0-win64\Include\boost-1_46"  /I"C:\Program Files\Microsoft SDKs\Windows\v7.0\Include"  /I..\..\gtest\include /I..\..\lua\src  /I. /I"C:\cygwin\home\Andreas\libxml2-2.7.8.win64\include"  /I"C:\cygwin\home\Andreas\libxml2-2.7.8.win64\include\libxml" /EHsc  /D_WIN32_WINNT=0x504  /DWITH_LIBXML2  /Fotests\luamapParser.obj tests\luamapParser.cpp
luamapParser.cpp
.\serialize/luamapPrint.hpp(119) : error C2220: warning treated as error - no 'object' file generated
        .\serialize/luamapPrint.hpp(135) : see reference to function template instantiation 'bool _Wolframe::serialize::printObject_<T>(const void *,const _Wolframe::serialize::vector_ &,lua_State *,_Wolframe::serialize::Context *)' being compiled
        with
        [
            T=std::vector<testing::internal::string>
        ]
        .\serialize/luamapPrint.hpp(134) : while compiling class template member function 'bool _Wolframe::serialize::IntrusivePrinter<T>::print(const void *,lua_State *,_Wolframe::serialize::Context *)'
        with
        [
            T=std::vector<testing::internal::string>
        ]
        .\serialize/luamapDescription.hpp(81) : see reference to class template instantiation '_Wolframe::serialize::IntrusivePrinter<T>' being compiled
        with
        [
            T=std::vector<testing::internal::string>
        ]
        tests\luamapParser.cpp(121) : see reference to function template instantiation '_Wolframe::serialize::Description<Structure> &_Wolframe::serialize::Description<Structure>::operator ()<std::vector<_Ty>>(const char *,Element Plant::* )' being compiled
        with
        [
            Structure=Plant,
            _Ty=testing::internal::string,
            Element=std::vector<testing::internal::string>
        ]
.\serialize/luamapPrint.hpp(119) : warning C4244: 'argument' : conversion from 'size_t' to 'lua_Number', possible loss of data
.\serialize/luamapPrint.hpp(119) : warning C4244: 'argument' : conversion from 'size_t' to 'lua_Number', possible loss of data
        .\serialize/luamapPrint.hpp(135) : see reference to function template instantiation 'bool _Wolframe::serialize::printObject_<T>(const void *,const _Wolframe::serialize::vector_ &,lua_State *,_Wolframe::serialize::Context *)' being compiled
        with
        [
            T=std::vector<Plant>
        ]
        .\serialize/luamapPrint.hpp(134) : while compiling class template member function 'bool _Wolframe::serialize::IntrusivePrinter<T>::print(const void *,lua_State *,_Wolframe::serialize::Context *)'
        with
        [
            T=std::vector<Plant>
        ]
        .\serialize/luamapDescription.hpp(81) : see reference to class template instantiation '_Wolframe::serialize::IntrusivePrinter<T>' being compiled
        with
        [
            T=std::vector<Plant>
        ]
        tests\luamapParser.cpp(178) : see reference to function template instantiation '_Wolframe::serialize::Description<Structure> &_Wolframe::serialize::Description<Structure>::operator ()<std::vector<_Ty>>(const char *,Element Garden::* )' being compiled
        with
        [
            Structure=Garden,
            _Ty=Plant,
            Element=std::vector<Plant>
        ]
.\serialize/luamapPrint.hpp(119) : warning C4244: 'argument' : conversion from 'size_t' to 'lua_Number', possible loss of data
        .\serialize/luamapPrint.hpp(135) : see reference to function template instantiation 'bool _Wolframe::serialize::printObject_<T>(const void *,const _Wolframe::serialize::vector_ &,lua_State *,_Wolframe::serialize::Context *)' being compiled
        with
        [
            T=std::vector<Garden>
        ]
        .\serialize/luamapPrint.hpp(134) : while compiling class template member function 'bool _Wolframe::serialize::IntrusivePrinter<T>::print(const void *,lua_State *,_Wolframe::serialize::Context *)'
        with
        [
            T=std::vector<Garden>
        ]
        .\serialize/luamapDescription.hpp(81) : see reference to class template instantiation '_Wolframe::serialize::IntrusivePrinter<T>' being compiled
        with
        [
            T=std::vector<Garden>
        ]
        tests\luamapParser.cpp(193) : see reference to function template instantiation '_Wolframe::serialize::Description<Structure> &_Wolframe::serialize::Description<Structure>::operator ()<std::vector<_Ty>>(const char *,Element Places::* )' being compiled
        with
        [
            Structure=Places,
            _Ty=Garden,
            Element=std::vector<Garden>
        ]

*/
		

}}//namespace
#endif

