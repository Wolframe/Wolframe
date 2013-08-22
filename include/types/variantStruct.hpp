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
///\file types/variantStruct.hpp
///\brief Structures of variant types

#ifndef _Wolframe_TYPES_VARIANT_STRUCT_HPP_INCLUDED
#define _Wolframe_TYPES_VARIANT_STRUCT_HPP_INCLUDED
#include <string>
#include <map>
#include <iostream>
#include <utility>
#include "types/variant.hpp"

namespace _Wolframe {
namespace types {

///\brief Forward declaration for VariantStruct
class VariantStructDescription;
///\brief Forward declaration for VariantStruct
class IndirectionDescription;

///\brief Structure of variant type atoms or substructures
class VariantStruct :public Variant
{
public:
	typedef std::map<std::string,const VariantStructDescription*> ResolveMap;

	///\enum Type
	///\brief Typed filter element type
	enum Type
	{
		null_=Variant::null_,		//< undefined value
		int_=Variant::int_,		//< atomic signed int
		uint_=Variant::uint_,		//< atomic unsigned int
		bool_=Variant::bool_,		//< atomic boolean
		double_=Variant::double_,	//< atomic double
		string_=Variant::string_,	//< atomic string
		array_,				//< array of VariantStruct
		struct_,			//< content of structure
		indirection_,			//< pointer to structure
		unresolved_			//< unresolved indirection
	};
	///\brief Get the type name as string constant for logging
	static const char* typeName( Type i)				{return Variant::typeName( (Variant::Type)i);}

	///\brief Default constructor
	VariantStruct()							:Variant(){}
	///\brief Constructor from structure description
	explicit VariantStruct( const VariantStructDescription* d)	:Variant(){initStruct( d);}
	///\brief Copy constructor
	VariantStruct( const Variant& o)				:Variant(){VariantStruct::initCopy( *(const VariantStruct*)&o);}
	///\brief Copy constructor
	VariantStruct( const VariantStruct& o)				:Variant(){VariantStruct::initCopy( o);}
	///\brief Destructor
	~VariantStruct()						{VariantStruct::release();}

	///\brief Make the structure to be an array with the currently defined value as prototype element
	void makeArray();

	///\brief Assignment operator, keeping the initialization flag of this
	VariantStruct& operator=( const VariantStruct& o)		{bool init_=initialized(); VariantStruct::release(); VariantStruct::initCopy( o); setInitialized(init_); return *this;}
	VariantStruct& operator=( const Variant& o)			{bool init_=initialized(); VariantStruct::release(); Variant::initCopy( o); setInitialized(init_); return *this;}

	///\brief Reseting the content of this
	void clear()							{release(); init();}

public:
	///\brief Compare structures
	bool operator==( const VariantStruct& o) const			{return compare( o) == 0;}
	bool operator!=( const VariantStruct& o) const			{int cv = compare( o); return cv != 0;}
	bool operator>( const VariantStruct& o) const			{int cv = compare( o); return cv > 0;}
	bool operator>=( const VariantStruct& o) const			{int cv = compare( o); return cv >= 0;}
	bool operator<=( const VariantStruct& o) const			{int cv = compare( o); return cv <= 0;}
	bool operator<( const VariantStruct& o) const			{int cv = compare( o); return cv < 0;}
	int compare( const VariantStruct& o) const;

	///\brief Get the type of the structure
	Type type() const						{return (Type)m_type;}

	///\brief Get the number of elements defined (throws for other types than struct_ or array_)
	std::size_t nof_elements() const;

	///\brief Get the description of a structure (throws for other types than struct_)
	const VariantStructDescription* description() const		{return ((Type)m_type == struct_ || (Type)m_type == indirection_) ? (const VariantStructDescription*)m_data.dim.metadata:0;}
	void setDescription( const VariantStructDescription* descr);

	typedef std::map< const VariantStructDescription*, const VariantStructDescription*> DescriptionAssignmentMap;
	void setIndirectionDescription( const DescriptionAssignmentMap& assignmentmap);

	///\brief Return the prototype element (initialization of new element) of an array (throws for other types than array_)
	const VariantStruct* prototype() const				{return ((Type)m_type == array_)?(const VariantStruct*)m_data.value.ref_:0;}
	VariantStruct* prototype()					{return ((Type)m_type == array_)?(VariantStruct*)m_data.value.ref_:0;}

	///\brief Return the referenced name in case of an unresolved external
	const std::string unresolvedName() const			{if ((Type)m_type != unresolved_) throw std::logic_error("undefined access of unresolved name"); return std::string( (const char*)m_data.value.ref_, m_data.dim.size);}
	///\brief Resolve all unresolved externals according to the given map. Throws, if not all unresolved symbols could be resolved !
	void resolve( const ResolveMap& rmap);

	///\brief Expands an Indirection (throws for other types than indirection_)
	void expandIndirection();

	///\brief Add a new element to an array (throws for other types than array_)
	void push();

	///\brief Gets the value of the structure as string
	std::string tostring() const;

	///\brief Random access (throws logic error on ABR/ABW)
	const VariantStruct& operator[]( std::size_t idx) const;
	VariantStruct& operator[]( std::size_t idx);

	///\brief Random access or 0 if no random access defined (throws logic error on ABR/ABW)
	const VariantStruct* at( std::size_t idx) const;
	VariantStruct* at( std::size_t idx);

	///\brief Get the last element (throws logic error on ABR/ABW)
	const VariantStruct& back() const;
	VariantStruct& back();

	///\brief Get a substructure reference
	const VariantStruct* select( const std::string& name) const;
	VariantStruct* select( const std::string& name);

public:
	///\brief Const iterator on struct or array elements
	class const_iterator
	{
	public:
		const_iterator( VariantStruct const* itr_)			:m_itr(itr_){}
		const_iterator( const const_iterator& o)			:m_itr(o.m_itr){}

		int compare( const const_iterator& o) const;

		bool operator==( const const_iterator& o) const			{return compare(o) == 0;}
		bool operator!=( const const_iterator& o) const			{return compare(o) != 0;}
		bool operator<( const const_iterator& o) const			{return compare(o) < 0;}
		bool operator<=( const const_iterator& o) const			{return compare(o) <= 0;}
		bool operator>( const const_iterator& o) const			{return compare(o) > 0;}
		bool operator>=( const const_iterator& o) const			{return compare(o) >= 0;}

		const_iterator& operator++()					{++m_itr; return *this;}
		const_iterator operator++(int)					{const_iterator rt(*this); ++m_itr; return rt;}

		const VariantStruct* operator->() const				{return m_itr;}
		const VariantStruct& operator*() const				{return *m_itr;}

		int operator - (const const_iterator& o) const			{if (!o.m_itr || !m_itr) throw std::logic_error("illegal operation"); return m_itr - o.m_itr;}
		const_iterator operator + (int i) const				{if (!m_itr) throw std::logic_error("illegal operation"); return m_itr + i;}

	private:
		VariantStruct const* m_itr;
	};

	///\brief Iterator on struct or array elements
	class iterator
	{
	public:
		iterator( VariantStruct* itr_)					:m_itr(itr_){}
		iterator( const iterator& o)					:m_itr(o.m_itr){}

		int compare( const iterator& o) const;

		bool operator==( const iterator& o) const			{return compare(o) == 0;}
		bool operator!=( const iterator& o) const			{return compare(o) != 0;}
		bool operator<( const iterator& o) const			{return compare(o) < 0;}
		bool operator<=( const iterator& o) const			{return compare(o) <= 0;}
		bool operator>( const iterator& o) const			{return compare(o) > 0;}
		bool operator>=( const iterator& o) const			{return compare(o) >= 0;}

		iterator& operator++()						{++m_itr; return *this;}
		iterator operator++(int)					{iterator rt(*this); ++m_itr; return rt;}

		VariantStruct* operator->()					{return m_itr;}
		VariantStruct& operator*()					{return *m_itr;}

		int operator - (const iterator& o) const			{if (!o.m_itr || !m_itr) throw std::logic_error("illegal operation"); return m_itr - o.m_itr;}
		iterator operator + (int i) const				{if (!m_itr) throw std::logic_error("illegal operation"); return m_itr + i;}

	private:
		VariantStruct* m_itr;
	};

public:
	///\brief Print the value of a structure as string (curly bracket syntax)
	void print( std::ostream& out, const std::string& indent, const std::string& newitem, std::size_t level=0) const;

	///\brief Find an element (direct child)
	const_iterator find( const std::string& name_) const;
	iterator find( const std::string& name_);

	///\brief Find an element (direct child) with case insensitive comparison (of ascii A-Z)
	const_iterator find_cis( const std::string& name_) const;
	iterator find_cis( const std::string& name_);

	///\brief Get the an iterator on the first element (direct child)
	const_iterator begin() const						{return elementptr(0);}
	iterator begin()							{return elementptr(0);}
	///\brief Get the an iterator on the end of the list of elements
	const_iterator end() const						{return elementptr( nof_elements());}
	iterator end()								{return elementptr( nof_elements());}

private:
	///\brief Random access or 0 if no random access defined or invalid access
	const VariantStruct* elementptr( std::size_t idx) const;
	VariantStruct* elementptr( std::size_t idx);

	friend class VariantUnresolved;
	friend class VariantStructConst;
	void setType( Type type_)						{m_type = (unsigned char)type_;}

	static int compareArray( std::size_t size, const VariantStruct* a1, const VariantStruct* a2);
	void initStruct( const VariantStructDescription* descr);
	void initCopy( const VariantStruct& orig);
	void initConstCopy( const VariantStruct& o);
	void initConstCopy( const Variant& o);
	void initUnresolved( const std::string& name_);
	void initIndirection( const VariantStructDescription* descr);

	void release();
	void check() const;
};


///\class VariantStructIndirection
///\brief Structure of Variant type
class VariantIndirection :public Variant
{
public:
	///\brief Constructor
	explicit VariantIndirection( const VariantStructDescription* descr);
	~VariantIndirection()	{m_type=0;}
};


///\class VariantUnresolved
///\brief Unresolved indirection
class VariantUnresolved :public VariantStruct
{
public:
	///\brief Constructor
	explicit VariantUnresolved( const std::string& name_);
};


///\class VariantStructConst
///\brief Variant structure that represents a variant structure copy without content ownership
///\remark The livetime of the variant type this structure is initialized from must must cover the livetime of this structure
class VariantStructConst :public VariantStruct
{
	public:
	///\brief Constructor
	VariantStructConst( const VariantConst& o)		:VariantStruct(){Variant::initConstCopy( o);}
	VariantStructConst( const Variant& o)			:VariantStruct(){Variant::initConstCopy( o);}
	VariantStructConst( const VariantStruct& o)		:VariantStruct(){initConstCopy( o);}
	VariantStructConst( const VariantStructConst& o)	:VariantStruct(){initConstCopy( o);}
	VariantStructConst( bool o)				:VariantStruct(bool_){m_data.value.bool_ = o; setConstant(); }
	VariantStructConst( double o)				:VariantStruct(double_){m_data.value.double_ = o; setConstant(); }
	VariantStructConst( float o)				:VariantStruct(double_){m_data.value.double_ = (double)o; setConstant();}
	VariantStructConst( int o)				:VariantStruct(int_){m_data.value.int_ = o; setConstant();}
	VariantStructConst( unsigned int o)			:VariantStruct(uint_){m_data.value.uint_ = o; setConstant();}
	VariantStructConst( const char* o)			{initConstant( o, std::strlen(o));}
	VariantStructConst( const char* o, std::size_t n)	{initConstant( o, n);}
	VariantStructConst( const std::string& o)		{initConstant( o.c_str(), o.size());}
	///\brief Destructor
	~VariantStructConst(){}

	///\brief Assignment operator, keeping the initialization flag of this
	VariantStructConst& operator=( const Variant& o)	{bool init_=initialized(); initConstCopy( o); setInitialized(init_); return *this;}
	VariantStructConst& operator=( const VariantConst& o)	{bool init_=initialized(); initConstCopy( o); setInitialized(init_); return *this;}
	VariantStructConst& operator=( bool o)			{bool init_=initialized(); Variant::init(Variant::bool_); m_data.value.bool_ = o; setInitialized(init_); setConstant(); return *this;}
	VariantStructConst& operator=( double o)		{bool init_=initialized(); Variant::init(Variant::double_); m_data.value.double_ = o; setInitialized(init_); setConstant(); return *this;}
	VariantStructConst& operator=( float o)			{bool init_=initialized(); Variant::init(Variant::double_); m_data.value.double_ = (double)o; setInitialized(init_); setConstant(); return *this;}
	VariantStructConst& operator=( int o)			{bool init_=initialized(); Variant::init(Variant::int_); m_data.value.int_ = o; setInitialized(init_); setConstant(); return *this;}
	VariantStructConst& operator=( unsigned int o)		{bool init_=initialized(); Variant::init(Variant::uint_); m_data.value.uint_ = o; setInitialized(init_); setConstant(); return *this;}
	VariantStructConst& operator=( const char* o)		{bool init_=initialized(); initConstant( o, std::strlen(o)); setInitialized(init_); return *this;}
	VariantStructConst& operator=( const std::string& o)	{bool init_=initialized(); initConstant( o.c_str(), o.size()); setInitialized(init_); return *this;}

	///\brief Initialization as string constant
	void init( const char* o, std::size_t len)		{bool init_=initialized(); initConstant( o, len); setInitialized(init_);}
	void init( const char* o)				{bool init_=initialized(); initConstant( o, o?std::strlen(o):0); setInitialized(init_);}
	void init( const std::string& o)			{bool init_=initialized(); initConstant( o.c_str(), o.size()); setInitialized(init_);}

	///\brief Reseting the content of this
	void clear()						{Variant::init();}
};

}} //namespace

namespace std
{
///\brief Output stream operators for logging etc.
ostream& operator << (ostream &os, const _Wolframe::types::VariantStruct& o);
} //namespace

#endif



