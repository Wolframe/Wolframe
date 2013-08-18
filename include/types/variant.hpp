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
#include "types/countedReference.hpp"
#include <string>
#include <cstring>
#include <ostream>
#include <stdexcept>
#include <boost/cstdint.hpp>

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
		null_,					//< value undefined (NULL)
		int_,					//< C++ int
		uint_,					//< C++ unsigned int
		bool_,					//< C++ bool
		double_,				//< C++ double
		string_					//< null terminated UTF-8 string
	};
	enum Flags
	{
		Initialized	=0x1,			//< value is initialized (meaning depends on the user of this value)
		Constant	=0x2			//< value is a reference that is not owned and freed by this)
	};

	///\brief Get the type name as string constant for logging
	static const char* typeName( Type i)
	{
		static const char* ar[] = {"null","int","uint","bool","double","string","array","struct","indirection","unresolved"};
		return ar[ (int)i];
	}

	///\class Data
	///\brief Internal representation of this value
	struct Data
	{
		union
		{
			const void* metadata;		//< unused by base variant type
			std::size_t size;		//< size of a string
		} dim;
		typedef boost::int64_t Int_;
		typedef boost::uint64_t UInt_;
		union
		{
			bool bool_;
			double double_;
			Int_ int_;
			UInt_ uint_;
			char* string_;
			void* ref_;
		} value;
	};
	///\brief Constructor
	Variant( Type type_)				{init(type_);}
	Variant()					{init();}
	Variant( bool o)				{init(bool_); m_data.value.bool_ = o;}
	Variant( double o)				{init(double_); m_data.value.double_ = o;}
	Variant( float o)				{init(double_); m_data.value.double_ = o;}
	Variant( int o)					{init(int_); m_data.value.int_ = o;}
	Variant( Data::Int_ o)				{init(int_); m_data.value.int_ = o;}
	Variant( unsigned int o)			{init(uint_); m_data.value.uint_ = o;}
	Variant( Data::UInt_ o)				{init(uint_); m_data.value.uint_ = o;}
	Variant( const char* o)				{initString( o, std::strlen(o));}
	Variant( const char* o, std::size_t n)		{initString( o, n);}
	Variant( const std::string& o)			{initString( o.c_str(), o.size());}
	Variant( const Variant& o)			{initCopy( o);}
	///\brief Destructor
	~Variant()					{release();}

	///\brief Assignment operator, keeping the initialization flag of this
	Variant& operator=( const Variant& o)		{bool init_=initialized(); release(); initCopy( o); setInitialized(init_); return *this;}
	Variant& operator=( bool o)			{bool init_=initialized(); release(); init(bool_); m_data.value.bool_ = o; setInitialized(init_); return *this;}
	Variant& operator=( double o)			{bool init_=initialized(); release(); init(double_); m_data.value.double_ = o; setInitialized(init_); return *this;}
	Variant& operator=( float o)			{bool init_=initialized(); release(); init(double_); m_data.value.double_ = o; setInitialized(init_); return *this;}
	Variant& operator=( int o)			{bool init_=initialized(); release(); init(int_); m_data.value.int_ = o; setInitialized(init_); return *this;}
	Variant& operator=( Data::Int_ o)		{bool init_=initialized(); release(); init(int_); m_data.value.int_ = o; setInitialized(init_); return *this;}
	Variant& operator=( unsigned int o)		{bool init_=initialized(); release(); init(uint_); m_data.value.uint_ = o; setInitialized(init_); return *this;}
	Variant& operator=( Data::UInt_ o)		{bool init_=initialized(); release(); init(uint_); m_data.value.uint_ = o; setInitialized(init_); return *this;}
	Variant& operator=( const char* o)		{bool init_=initialized(); release(); initString( o, std::strlen(o)); setInitialized(init_); return *this;}
	Variant& operator=( const std::string& o)	{bool init_=initialized(); release(); initString( o.c_str(), o.size()); setInitialized(init_); return *this;}

	///\brief Initialization as string constant (Constant flag set, data not owned by this)
	void initConstant( const char* o, std::size_t l);
	void initConstant( const std::string& o)	{initConstant( o.c_str(),o.size());}
	void initConstant( const char* o)		{initConstant( o, std::strlen(o));}

	///\brief Compare data with conversion (value with different types is converted first)
	bool operator==( const Variant& o) const	{return compare( o) == 0;}
	bool operator!=( const Variant& o) const	{int cv = compare( o); return cv != 0 && cv != -2;}
	bool operator>( const Variant& o) const		{int cv = compare( o); return cv > 0;}
	bool operator>=( const Variant& o) const	{int cv = compare( o); return cv >= 0;}
	bool operator<=( const Variant& o) const	{int cv = compare( o); return cv <= 0 && cv != -2;}
	bool operator<( const Variant& o) const		{int cv = compare( o); return cv == -1;}

	///\brief Get the type of this
	Type type() const				{return (Type)m_type;}
	///\brief Get the internal representation of the data of this
	const Data& data() const			{return m_data;}

	///\brief Get the pointer to the C representation of a string (throws for non string)
	char* charptr() const				{if (type() != string_) throw std::logic_error("illegal access (type mismatch)"); return m_data.value.string_;}
	///\brief Get the size of a string (throws for non string)
	std::size_t charsize() const			{if (type() != string_) throw std::logic_error("illegal access (type mismatch)"); return m_data.dim.size;}

	///\brief Getter with value conversion
	std::string tostring() const;
	double tonumber() const;
	double todouble() const;
	bool tobool() const;
	Data::Int_ toint() const;
	Data::UInt_ touint() const;

	///\brief Get the size of a string
	std::size_t size() const			{return (type() == string_)?m_data.dim.size:1;}

	///\brief Test if this value is initialized
	bool initialized() const			{return flags( Initialized);}
	///\brief Set the value to be initialized
	void setInitialized( bool v=true)		{setFlags( Initialized, v);}

	///\brief Test if this value is constant (owned by this)
	bool constant() const				{return flags( Constant);}
	///\brief Set the value to be constant
	///\remark Setting this flag for a value owned by this can cause memory leaks
	void setConstant( bool v=true)			{setFlags( Constant, v);}

	///\brief Test if this value is atomic (not VariantStruct or VariantIndirection)
	bool atomic() const				{return m_type <= (unsigned char)string_;}

	///\brief Test if this value is defined (not null)
	bool defined() const				{return m_type != (unsigned char)null_;}

	///\brief Reseting the content of this
	void clear()					{release(); init();}

	///\brief Converting the value of this to a defined type
	void convert( Type type_);

protected:
	bool flags( Flags flags_) const			{return ((unsigned char)m_flags & (unsigned char)flags_) == (unsigned char)flags_;}
	void setFlags( Flags flags_, bool v=true)	{if (v) m_flags = (Flags)((unsigned char)m_flags | (unsigned char)flags_); else m_flags = (Flags)((unsigned char)m_flags - ((unsigned char)m_flags & (unsigned char)flags_));}

	void init( Type type_);
	void init();
	void release();
	void initString( const char* str_, std::size_t strsize_);
	void initCopy( const Variant& o);
	void initConstCopy( const Variant& o);

	///\brief Compares two variants (implicit type conversion to the higher priority type (order of declaration in enum Type, higher priority first))
	///\return -1: this less than 0, 0: this equals o, 1: this bigger than o, -2: values not comparable
	int compare( const Variant& o) const;

protected:
	Data m_data;
	unsigned char m_type;
	unsigned char m_flags;
};


///\class VariantConst
///\brief Variant value type that represents a variant copy without content ownership
///\remark The livetime of the variant type this structure is initialized from must must cover the livetime of this structure
struct VariantConst :public Variant
{
	///\brief Constructor
	VariantConst()					{init();}
	VariantConst( const Variant& o)			:Variant(){initConstCopy( o);}
	VariantConst( const VariantConst& o)		:Variant(){initConstCopy( o);}
	VariantConst( bool o)				:Variant(bool_){m_data.value.bool_ = o; setConstant();}
	VariantConst( double o)				:Variant(double_){m_data.value.double_ = o; setConstant();}
	VariantConst( float o)				:Variant(double_){m_data.value.double_ = o; setConstant();}
	VariantConst( int o)				:Variant(int_){m_data.value.int_ = o; setConstant();}
	VariantConst( Data::Int_ o)			:Variant(int_){m_data.value.int_ = o; setConstant();}
	VariantConst( unsigned int o)			:Variant(uint_){m_data.value.uint_ = o; setConstant();}
	VariantConst( Data::UInt_ o)			:Variant(uint_){m_data.value.uint_ = o; setConstant();}
	VariantConst( const char* o)			:Variant(){initConstant( o, std::strlen(o));}
	VariantConst( const char* o, std::size_t n)	:Variant(){initConstant( o, n);}
	VariantConst( const std::string& o)		:Variant(){initConstant( o.c_str(), o.size());}
	///\brief Destructor
	~VariantConst(){}

	///\brief Assignment operator, keeping the initialization flag of this
	VariantConst& operator=( const Variant& o)	{bool init_=initialized(); initConstCopy( o); setInitialized(init_); return *this;}
	VariantConst& operator=( const VariantConst& o)	{bool init_=initialized(); initConstCopy( o); setInitialized(init_); return *this;}
	Variant& operator=( bool o)			{bool init_=initialized(); Variant::init(Variant::bool_); m_data.value.bool_ = o; setInitialized(init_); setConstant(); return *this;}
	Variant& operator=( double o)			{bool init_=initialized(); Variant::init(Variant::double_); m_data.value.double_ = o; setInitialized(init_); setConstant(); return *this;}
	Variant& operator=( float o)			{bool init_=initialized(); Variant::init(Variant::double_); m_data.value.double_ = o; setInitialized(init_); setConstant(); return *this;}
	Variant& operator=( int o)			{bool init_=initialized(); Variant::init(Variant::int_); m_data.value.int_ = o; setInitialized(init_); setConstant(); return *this;}
	Variant& operator=( Data::Int_ o)		{bool init_=initialized(); Variant::init(Variant::int_); m_data.value.int_ = o; setInitialized(init_); setConstant(); return *this;}
	Variant& operator=( unsigned int o)		{bool init_=initialized(); Variant::init(Variant::uint_); m_data.value.uint_ = o; setInitialized(init_); setConstant(); return *this;}
	Variant& operator=( Data::UInt_ o)		{bool init_=initialized(); Variant::init(Variant::uint_); m_data.value.uint_ = o; setInitialized(init_); setConstant(); return *this;}
	VariantConst& operator=( const char* o)		{bool init_=initialized(); initConstant( o, std::strlen(o)); setInitialized(init_); return *this;}
	VariantConst& operator=( const std::string& o)	{bool init_=initialized(); initConstant( o.c_str(), o.size()); setInitialized(init_); return *this;}

	///\brief Initialization as string constant
	void init( const char* o, std::size_t len)	{bool init_=initialized(); initConstant( o, len); setInitialized(init_);}
	void init( const char* o)			{bool init_=initialized(); initConstant( o, o?std::strlen(o):0); setInitialized(init_);}
	void init( const std::string& o)		{bool init_=initialized(); initConstant( o.c_str(), o.size()); setInitialized(init_);}

	///\brief Empty initialization
	void init()					{Variant::init(); setConstant();}

	///\brief Reseting the content of this
	void clear()					{Variant::init();}
};

}} //namespace

namespace std
{
///\brief Output stream operators for logging etc.
ostream& operator << (ostream &os, const _Wolframe::types::Variant& o);
} //namespace

#endif

