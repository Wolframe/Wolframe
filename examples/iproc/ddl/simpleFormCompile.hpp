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
///
///\file ddl/simpleformCompile.hpp
///\brief interface for a compiler of a self defined DDL for forms
///
#ifndef _Wolframe_DDL_SIMPLEFORM_COMPILE_HPP_INCLUDED
#define _Wolframe_DDL_SIMPLEFORM_COMPILE_HPP_INCLUDED
#include <string>
#include <vector>
#include <map>
#include <cstddef>
#include <cstring>

namespace _Wolframe {
namespace ddl {
namespace simpleform {

struct SimpleformDDLParser
{
	struct Element
	{
	public:
		enum Type
		{
			float_,long_,ulong_,int_,uint_,short_,ushort_,char_,uchar_,string_,form_
		};

		static const char* typeName( Type tp)
		{
			static const char* ar[] = {"float","long","ulong","int","uint","short","ushort","char","uchar","string","form",0};
			return ar[ (int)tp];
		}

		static bool getType( const char* name, Type& tp)
		{
			const char* rt;
			unsigned int ii;
			for (ii=0,rt=typeName((Type)(ii)); rt!=0; ii++,rt=typeName((Type)(ii)))
			{
				if (std::strcmp( rt, name) == 0)
				{
					tp = (Type)ii;
					return true;
				}
			}
			return false;
		}

		Type type;
		std::string name;
		std::string defaultValue;
		int ref;
		std::size_t size;
		bool isArray;

		Element() :ref(-1),size(0U),isArray(false){}
	};

	struct Struct
	{
		std::string name;
		std::vector<Element> elements;
		std::size_t size;
		Struct() :size(0){}
	};

	const Struct* get( const std::string& name) const
	{
		std::map<std::string,std::size_t>::const_iterator itr = linkmap.find( name), end = linkmap.end();
		if (itr == end) return 0;
		return &ar[ itr->second];
	}

	bool define( const std::string& name, const Struct& ee)
	{
		std::map<std::string,std::size_t>::const_iterator itr = linkmap.find( name);
		if (itr == linkmap.end()) return false;
		linkmap[ name] = ar.size();
		ar.push_back( ee);
		return true;
	}

	bool compile( const char* filename, std::string& error);

	std::map<std::string,std::size_t> linkmap;
	std::vector<Struct> ar;
	std::vector<std::string> errors;

private:
	void error( const std::string& msg) {errors.push_back( msg);}
	std::size_t calcElementSize( std::size_t idx, std::size_t depht=0);
};

}}}
#endif
