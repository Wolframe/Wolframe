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
#include <iostream>
#include "types/variant.hpp"

namespace _Wolframe {
namespace types {

///\brief Forward declaration for VariantStruct
class VariantStructDescription;
///\brief Forward declaration for VariantStruct
class IndirectionDescription;

///\brief Structure of Variant type
class VariantStruct :public Variant
{
public:
	///\enum Type
	///\brief Typed filter element type
	enum Type
	{
		bool_=Variant::bool_,
		double_=Variant::double_,
		int_=Variant::int_,
		uint_=Variant::uint_,
		string_=Variant::string_,
		array_,
		struct_,
		indirection_
	};

	VariantStruct()						:Variant(){}
	VariantStruct( const Variant& e)			{Variant::initcopy( *this, e);}
	VariantStruct( const VariantStructDescription* d)	{initstruct( d);}
	VariantStruct( const VariantStruct& o)			:Variant(){initcopy( *this, o);}
	VariantStruct array() const;
	~VariantStruct()					{release();}

	VariantStruct& operator=( const VariantStruct& o)	{release(); initcopy( *this, o); return *this;}

public:
	bool operator==( const VariantStruct& o) const		{return compare( o) == 0;}
	bool operator!=( const VariantStruct& o) const		{int cv = compare( o); return cv != 0 && cv != -2;}
	bool operator>( const VariantStruct& o) const		{int cv = compare( o); return cv > 0;}
	bool operator>=( const VariantStruct& o) const		{int cv = compare( o); return cv >= 0;}
	bool operator<=( const VariantStruct& o) const		{int cv = compare( o); return cv <= 0 && cv != -2;}
	bool operator<( const VariantStruct& o) const		{int cv = compare( o); return cv == -1;}
	int compare( const VariantStruct& o) const;

	Type type() const					{return (Type)type();}
	const Data& data() const				{return m_data;}
	std::size_t nof_elements() const;

	const VariantStructDescription* description() const	{return ((Type)type()==struct_ || (Type)type()==indirection_)?(const VariantStructDescription*)m_data.dim.metadata:0;}
	const VariantStruct* prototype() const			{return ((Type)type()==array_)?(const VariantStruct*)m_data.value.ref_:0;}

	void expandIndirection();
	void push();

	std::string tostring() const;

	const VariantStruct& operator[]( std::size_t idx) const;
	VariantStruct& operator[]( std::size_t idx);

	const VariantStruct* at( std::size_t idx) const;
	VariantStruct* at( std::size_t idx);

	const VariantStruct& back() const;
	VariantStruct& back();

public:
	class const_iterator
	{
	public:
		const_iterator( VariantStruct const* itr_=0)				:m_itr(itr_){}
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

	class iterator
	{
	public:
		iterator( VariantStruct* itr_=0)					:m_itr(itr_){}
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
	void print( std::ostream& out, const std::string& indent, const std::string& newitem, std::size_t level=0) const;

	const_iterator find( const std::string& name_) const;
	iterator find( const std::string& name_);

	const_iterator begin() const						{return at(0);}
	const_iterator end() const						{return const_iterator();}
	iterator begin()							{return at(0);}
	iterator end()								{return iterator();}

private:
	void setType( Type type_)						{Variant::setType((Variant::Type)type_);}

	static int compare_array( std::size_t size, const VariantStruct* a1, const VariantStruct* a2);
	void initstruct( const VariantStructDescription* descr);
	void initindirection( const VariantStructDescription* descr);
	static void initcopy( VariantStruct& dest, const VariantStruct& orig);

	void release();
};

}} //namespace
#endif



