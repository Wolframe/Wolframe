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
///\file types/variant.hpp
///\brief template for map with case insensitive key strings

#ifndef _Wolframe_TYPES_VARIANT_HPP_INCLUDED
#define _Wolframe_TYPES_VARIANT_HPP_INCLUDED
#include <string>
#include <cstring>

namespace _Wolframe {
namespace types {

///\brief Forward declaration for Variant
class StructDescription;

///\class Variant
///\brief Variant for holding values with their type
class Variant
{
public:
	///\enum Type
	///\brief Type of the variant
	enum Type
	{
		double_,				//< C++ double
		int_,					//< C++ int
		uint_,					//< C++ unsigned int
		bool_,					//< C++ bool
		string_					//< null terminated UTF-8 string
	};
	struct Data
	{
		union
		{
			const void* metadata;		//< unused by base variant type
			std::size_t size;		//< size of a string
		} dim;
		union
		{
			bool bool_;
			double double_;
			int int_;
			unsigned int uint_;
			char* string_;
			void* data_;
		} value;
	};
	Variant()					{init();}
	Variant( bool o)				{init(); m_type = bool_; m_data.value.bool_ = o;}
	Variant( double o)				{init(); m_type = double_; m_data.value.double_ = o;}
	Variant( int o)					{init(); m_type = int_; m_data.value.int_ = o;}
	Variant( unsigned int o)			{init(); m_type = uint_; m_data.value.uint_ = o;}
	Variant( const char* o)				{initstring( o, std::strlen(o));}
	Variant( const char* o, std::size_t n)		{initstring( o, n);}
	Variant( const std::string& o)			{initstring( o.c_str(), o.size());}
	Variant( const Variant& o)			{initcopy( *this, o);}
	~Variant()					{release();}

	Variant& operator=( const Variant& o)		{release(); initcopy( *this, o); return *this;}

	bool operator==( const Variant& o) const	{return compare( o) == 0;}
	bool operator!=( const Variant& o) const	{int cv = compare( o); return cv != 0 && cv != -2;}
	bool operator>( const Variant& o) const		{int cv = compare( o); return cv > 0;}
	bool operator>=( const Variant& o) const	{int cv = compare( o); return cv >= 0;}
	bool operator<=( const Variant& o) const	{int cv = compare( o); return cv <= 0 && cv != -2;}
	bool operator<( const Variant& o) const		{int cv = compare( o); return cv == -1;}

	Type type() const				{return m_type;}
	const Data& data() const			{return m_data;}

	std::string tostring() const;
	double tonumber() const;

private:
	void init();
	void release();
	void initstring( const char* str_, std::size_t strsize_);
	static void initcopy( Variant& dest, const Variant& orig);

	///\brief Compares two variants (implicit type conversion to the higher priority type (order of declaration in enum Type, higher priority first))
	///\return -1: this less than 0, 0: this equals o, 1: this bigger than o, -2: values not comparable
	int compare( const Variant& o) const;

private:
	Type m_type;
	Data m_data;
};

}} //namespace
#endif

