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
		int_,					//< C++ int
		uint_,					//< C++ unsigned int
		bool_,					//< C++ bool
		double_,				//< C++ double
		string_					//< null terminated UTF-8 string
	};
	enum Flags
	{
		Initialized	=0x1,
		Constant	=0x2
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
	};
	Variant( Type type_)				{init(type_);}
	Variant()					{init();}
	Variant( bool o)				{init(bool_); m_data.value.bool_ = o;}
	Variant( double o)				{init(double_); m_data.value.double_ = o;}
	Variant( float o)				{init(double_); m_data.value.double_ = (double)o;}
	Variant( int o)					{init(int_); m_data.value.int_ = o;}
	Variant( unsigned int o)			{init(uint_); m_data.value.uint_ = o;}
	Variant( const char* o)				{initString( o, std::strlen(o));}
	Variant( const char* o, std::size_t n)		{initString( o, n);}
	Variant( const std::string& o)			{initString( o.c_str(), o.size());}
	Variant( const Variant& o)			{initCopy( o);}
	~Variant()					{release();}

	Variant& operator=( const Variant& o)		{release(); initCopy( o); return *this;}
	Variant& operator=( bool o)			{unsigned char flags_=m_flags; release(); init(bool_); m_data.value.bool_ = o; m_flags=flags_; return *this;}
	Variant& operator=( double o)			{unsigned char flags_=m_flags; release(); init(double_); m_data.value.double_ = o; m_flags=flags_; return *this;}
	Variant& operator=( float o)			{unsigned char flags_=m_flags; release(); init(double_); m_data.value.double_ = (double)o; m_flags=flags_; return *this;}
	Variant& operator=( int o)			{unsigned char flags_=m_flags; release(); init(int_); m_data.value.int_ = o; m_flags=flags_; return *this;}
	Variant& operator=( unsigned int o)		{unsigned char flags_=m_flags; release(); init(uint_); m_data.value.uint_ = o; m_flags=flags_; return *this;}
	Variant& operator=( const char* o)		{bool init_=initialized(); release(); initString( o, std::strlen(o)); setInitialized(init_); return *this;}
	Variant& operator=( const std::string& o)	{bool init_=initialized(); release(); initString( o.c_str(), o.size()); setInitialized(init_); return *this;}

	void initConstant( const char* o)		{bool init_=initialized(); release(); initString( o, std::strlen(o), true); setInitialized(init_); setConstant();}
	void initConstant( const char* o, std::size_t l){bool init_=initialized(); release(); initString( o, l, true); setInitialized(init_); setConstant();}
	void initConstant( const std::string& o)	{bool init_=initialized(); release(); initString( o.c_str(), o.size(), true); setInitialized(init_); setConstant();}

	bool operator==( const Variant& o) const	{return compare( o) == 0;}
	bool operator!=( const Variant& o) const	{int cv = compare( o); return cv != 0 && cv != -2;}
	bool operator>( const Variant& o) const		{int cv = compare( o); return cv > 0;}
	bool operator>=( const Variant& o) const	{int cv = compare( o); return cv >= 0;}
	bool operator<=( const Variant& o) const	{int cv = compare( o); return cv <= 0 && cv != -2;}
	bool operator<( const Variant& o) const		{int cv = compare( o); return cv == -1;}

	Type type() const				{return (Type)m_type;}
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

	bool initialized() const			{return flags( Initialized);}
	void setInitialized( bool v=true)		{setFlags( Initialized, v);}

	bool constant() const				{return flags( Constant);}
	void setConstant( bool v=true)			{setFlags( Constant, v);}

	bool atomic() const				{return m_type <= (unsigned char)string_;}

protected:
	bool flags( Flags flags_) const			{return ((unsigned char)m_flags & (unsigned char)flags_) == (unsigned char)flags_;}
	void setFlags( Flags flags_, bool v=true)	{if (v) m_flags = (Flags)((unsigned char)m_flags | (unsigned char)flags_); else m_flags = (Flags)((unsigned char)m_flags - ((unsigned char)m_flags & (unsigned char)flags_));}

	void init( Type type_);
	void init();
	void release();
	void initString( const char* str_, std::size_t strsize_, bool constant_=false);
	void initCopy( const Variant& o);

	///\brief Compares two variants (implicit type conversion to the higher priority type (order of declaration in enum Type, higher priority first))
	///\return -1: this less than 0, 0: this equals o, 1: this bigger than o, -2: values not comparable
	int compare( const Variant& o) const;

protected:
	Data m_data;
	unsigned char m_type;
	unsigned char m_flags;
};


struct NormalizeFunction
{
	virtual ~NormalizeFunction(){}
	virtual const char* name() const=0;
	virtual Variant execute( const Variant& i) const=0;
};

typedef types::CountedReference<NormalizeFunction> NormalizeFunctionR;

struct NormalizeFunctionMap
{
	virtual ~NormalizeFunctionMap(){}
	virtual const NormalizeFunction* get( const std::string& name) const=0;
};

typedef types::CountedReference<NormalizeFunctionMap> NormalizeFunctionMapR;


}} //namespace
#endif

