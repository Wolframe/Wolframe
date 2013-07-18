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
///\brief Variant value type

#ifndef _Wolframe_TYPES_VARIANT_HPP_INCLUDED
#define _Wolframe_TYPES_VARIANT_HPP_INCLUDED
#include <string>
#include <cstring>
#include <stdexcept>
#include "types/countedReference.hpp"

namespace _Wolframe {
namespace types {

///\class Variant
///\brief Variant value type
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

	///\brief Get the name of a type as string
	static const char* typeName( Type i)
	{
		static const char* ar[] = {"double","int","uint","bool","string","array","struct","indirection"};
		return ar[ (int)i];
	}

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
			void* ref_;
		} value;
		enum {Initialized=0x80,FlagMask=0x7f};
	};
	Variant()					{init(); m_type = bool_;}
	Variant( bool o)				{init(); m_type = bool_; m_data.value.bool_ = o; setInitialized();}
	Variant( double o)				{init(); m_type = double_; m_data.value.double_ = o; setInitialized();}
	Variant( float o)				{init(); m_type = double_; m_data.value.double_ = (double)o; setInitialized();}
	Variant( int o)					{init(); m_type = int_; m_data.value.int_ = o; setInitialized();}
	Variant( unsigned int o)			{init(); m_type = uint_; m_data.value.uint_ = o; setInitialized();}
	Variant( const char* o)				{initstring( o, std::strlen(o)); setInitialized();}
	Variant( const char* o, std::size_t n)		{initstring( o, n); setInitialized();}
	Variant( const std::string& o)			{initstring( o.c_str(), o.size()); setInitialized();}
	Variant( const Variant& o)			{initcopy( *this, o);}
	~Variant()					{release();}

	Variant& operator=( const Variant& o)		{release(); initcopy( *this, o); return *this;}
	Variant& operator=( bool o)			{release(); init(); m_type = bool_; m_data.value.bool_ = o; setInitialized(); return *this;}
	Variant& operator=( double o)			{release(); init(); m_type = double_; m_data.value.double_ = o; setInitialized(); return *this;}
	Variant& operator=( float o)			{release(); init(); m_type = double_; m_data.value.double_ = (double)o; setInitialized(); return *this;}
	Variant& operator=( int o)			{release(); init(); m_type = int_; m_data.value.int_ = o; setInitialized(); return *this;}
	Variant& operator=( unsigned int o)		{release(); init(); m_type = uint_; m_data.value.uint_ = o; setInitialized(); return *this;}
	Variant& operator=( const char* o)		{release(); initstring( o, std::strlen(o)); setInitialized(); return *this;}
	Variant& operator=( const std::string& o)	{release(); initstring( o.c_str(), o.size()); setInitialized(); return *this;}

	bool operator==( const Variant& o) const	{return compare( o) == 0;}
	bool operator!=( const Variant& o) const	{int cv = compare( o); return cv != 0 && cv != -2;}
	bool operator>( const Variant& o) const		{int cv = compare( o); return cv > 0;}
	bool operator>=( const Variant& o) const	{int cv = compare( o); return cv >= 0;}
	bool operator<=( const Variant& o) const	{int cv = compare( o); return cv <= 0 && cv != -2;}
	bool operator<( const Variant& o) const		{int cv = compare( o); return cv == -1;}

	Type type() const				{return (Type)((unsigned char)m_type&(unsigned char)Data::FlagMask);}
	const Data& data() const			{return m_data;}

	char* charptr() const				{if (type() != string_) throw std::logic_error("illegal access (type mismatch)"); return m_data.value.string_;}
	std::size_t charsize() const			{if (type() != string_) throw std::logic_error("illegal access (type mismatch)"); return m_data.dim.size;}

	std::string tostring() const;
	double tonumber() const;
	double todouble() const;
	bool tobool() const;
	int toint() const;
	unsigned int touint() const;

	std::size_t size() const			{return (type() == string_)?m_data.dim.size:1;}
	bool initialized() const			{return ((unsigned char)m_type&(unsigned char)Data::Initialized) == (unsigned char)Data::Initialized;}
	void setInitialized( bool v=true)		{if (v) m_type = (Type)((unsigned int)m_type | (unsigned int)(Data::Initialized)); else m_type = (Type)((unsigned int)m_type & ~(unsigned int)(Data::Initialized));}
	bool atomic() const				{return (int)type() <= (int)string_;}

protected:
	void setType( Type type_)			{m_type = (Type)((int)type_ | ((int)m_type&(int)Data::Initialized));}
	void init();
	void release();
	void initstring( const char* str_, std::size_t strsize_);
	static void initcopy( Variant& dest, const Variant& orig);

	///\brief Compares two variants (implicit type conversion to the higher priority type (order of declaration in enum Type, higher priority first))
	///\return -1: this less than 0, 0: this equals o, 1: this bigger than o, -2: values not comparable
	int compare( const Variant& o) const;

private:
	Type m_type;
protected:
	Data m_data;
};


struct NormalizeFunction
{
	virtual ~NormalizeFunction(){}
	virtual const char* name() const=0;
	virtual Variant execute( const Variant& i) const=0;
};

struct NormalizeFunctionMap
{
	virtual ~NormalizeFunctionMap(){}
	virtual const NormalizeFunction* get( const std::string& name) const=0;
};

typedef types::CountedReference<NormalizeFunctionMap> NormalizeFunctionMapR;


}} //namespace
#endif

