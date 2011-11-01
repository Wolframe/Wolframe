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
///\file directmapCompile.hpp
///\brief interface for a compiler of a self defined direct map DDL
///
#ifndef _Wolframe_DIRECTMAP_COMPILE_HPP_INCLUDED
#define _Wolframe_DIRECTMAP_COMPILE_HPP_INCLUDED
#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <sstream>
#include <cstddef>
#include <cstring>

namespace _Wolframe {
namespace directmap {

class Definition
{
public:
	class Element
	{
	public:
		enum Type
		{
			string_,float_,long_,ulong_,int_,uint_,short_,ushort_,char_,uchar_,struct_
		};
		static const char* typeName( Type tp)
		{
			static const char* ar[] = {"float","long","ulong","int","uint","short","ushort","char","uchar","string","struct",0};
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
		Type type() const				{return m_type;}
		std::string name() const			{return m_name;}
		std::string defaultValue() const		{return m_default;}
		int ref() const					{return m_ref;}
		std::size_t size() const			{return m_size;}
		bool isArray() const				{return m_isArray;}

		Element()
			:m_type(string_),m_ref(-1),m_size(0),m_isArray(false){}

		Element( const std::string& name_)
			:m_type(string_),m_name(name_),m_ref(-1),m_size(0),m_isArray(false){}

		Element( const std::string& type_, const std::string& name_)
			:m_type(string_),m_name(name_),m_ref(-1),m_size(0),m_isArray(false)
		{
			initType( type_);
		}
		Element( const std::string& type_, const std::string& name_, const std::string& default_)
			:m_type(string_),m_name(name_),m_default(default_),m_ref(-1),m_size(0),m_isArray(false)
		{
			initType( type_);
		}
		Element( const std::string& type_, const std::string& name_, const char&)
			:m_type(string_),m_name(name_),m_ref(-1),m_size(0),m_isArray(true)
		{
			initType( type_);
		}
	private:
		friend class Definition;
		Type m_type;
		std::string m_name;
		std::string m_default;
		int m_ref;
		std::size_t m_size;
		bool m_isArray;

		void initType( const std::string& type_)
		{
			unsigned int ii=0;
			while (typeName( (Type)ii) && std::strcmp( typeName( (Type)ii), type_.c_str()) != 0) ii++;
			if (!typeName( (Type)ii)) throw std::runtime_error( "illegal type ");
			m_type = (Type)ii;
		}
	};

	class Struct
	{
	public:
		std::size_t size() const				{return m_size;}
		const std::string& name() const				{return m_name;}
		const std::vector<Element>& elements() const		{return m_elements;}

		Struct() :m_size(0){}
		Struct( const std::string& name_)						:m_name(name_),m_size(0){}
		Struct( const std::string& name_, const std::vector<Element>& elements_)	:m_name(name_),m_elements(elements_),m_size(0){}

	private:
		friend class Definition;
		std::string m_name;
		std::vector<Element> m_elements;
		std::size_t m_size;
	};

	const std::vector<Struct> ar() const
	{
		return m_ar;
	}

	const Struct* get( const std::string& name) const
	{
		std::map<std::string,std::size_t>::const_iterator itr = m_linkmap.find( name), end = m_linkmap.end();
		if (itr == end) return 0;
		return &m_ar[ itr->second];
	}

	const std::vector<std::string>& errors() const {return m_errors;}

	bool compile( const char* filename, std::string& error);

private:
	std::map<std::string,std::size_t> m_linkmap;
	std::vector<Struct> m_ar;
	std::vector<std::string> m_errors;

	std::size_t calcElementSize( std::size_t idx, std::size_t depht);
};

}}
#endif
