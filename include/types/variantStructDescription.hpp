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
///\file types/variantStructDescription.hpp
///\brief Description of structures of variant types

#ifndef _Wolframe_TYPES_VARIANT_STRUCT_DESCRIPTION_HPP_INCLUDED
#define _Wolframe_TYPES_VARIANT_STRUCT_DESCRIPTION_HPP_INCLUDED
#include <string>
#include <iostream>
#include <stdexcept>
#include "types/variant.hpp"

namespace _Wolframe {
namespace types {

///\brief Forward declaration for VariantStructDescription
class VariantStruct;

class VariantStructDescription
{
public:
	VariantStructDescription();
	VariantStructDescription( const VariantStructDescription& o);

	struct Element
	{
		char* name;
		VariantStruct* initvalue;
		VariantStructDescription* substruct;
		const NormalizeFunction* normalizer;

		enum Flags
		{
			NoFlags=0x0,
			Optional=0x1,
			Mandatory=0x2,
			Attribute=0x4
		};
		char flags;

		bool optional() const						{return (flags & (unsigned char)Optional) != 0;}
		bool mandatory() const						{return (flags & (unsigned char)Mandatory) != 0;}
		bool attribute() const						{return (flags & (unsigned char)Attribute) != 0;}

		void setOptional( bool v=true)					{if (v) flags |= (unsigned char)Optional; else flags &= ~(unsigned char)Optional;}
		void setMandatory( bool v=true)					{if (v) flags |= (unsigned char)Mandatory; else flags &= ~(unsigned char)Mandatory;}
		void setAttribute( bool v=true)					{if (v) flags |= (unsigned char)Attribute; else flags &= ~(unsigned char)Attribute;}

		Variant::Type type();
		void makeArray();
	};

	class const_iterator
	{
	public:
		const_iterator( Element const* itr_=0)				:m_itr(itr_){}
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

		const Element* operator->() const				{return m_itr;}
		const Element& operator*() const				{return *m_itr;}

		int operator - (const const_iterator& o) const			{if (!o.m_itr || !m_itr) throw std::logic_error("illegal operation"); return m_itr - o.m_itr;}
		const_iterator operator + (int i) const				{if (!m_itr) throw std::logic_error("illegal operation"); return m_itr + i;}

	private:
		Element const* m_itr;
	};

	class iterator
	{
	public:
		iterator( Element* itr_=0)					:m_itr(itr_){}
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

		Element* operator->()						{return m_itr;}
		Element& operator*()						{return *m_itr;}

		int operator - (const iterator& o) const			{if (!o.m_itr || !m_itr) throw std::logic_error("illegal operation"); return m_itr - o.m_itr;}
		iterator operator + (int i) const				{if (!m_itr) throw std::logic_error("illegal operation"); return m_itr + i;}

	private:
		Element* m_itr;
	};

public:
	const Element* at( std::size_t idx) const				{if (idx>=m_size) return 0; else return m_ar+idx;}
	Element* at( std::size_t idx)						{if (idx>=m_size) return 0; else return m_ar+idx;}

	const_iterator begin() const						{return at(0);}
	const_iterator end() const						{return const_iterator();}
	iterator begin()							{return at(0);}
	iterator end()								{return iterator();}

	int addAttribute( const std::string& name, const VariantStruct& initvalue, const NormalizeFunction* normalizer);
	int add( const std::string& name, const VariantStruct& initvalue, const NormalizeFunction* normalizer);
	int add( const std::string& name, const VariantStructDescription& substruct);

	int findidx( const std::string& name) const;
	const_iterator find( const std::string& name) const;
	iterator find( const std::string& name);

	std::size_t size() const						{return m_size;}

	int compare( const VariantStructDescription& o) const;
	std::string names( const std::string& sep) const;

private:
	std::size_t m_size;
	std::size_t m_nofattributes;
	Element* m_ar;
};


}} //namespace
#endif



