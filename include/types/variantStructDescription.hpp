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
#include "types/countedReference.hpp"
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
	~VariantStructDescription();

	struct Element
	{
		char* name;
		VariantStruct* initvalue;
		VariantStructDescription* substruct;
		const NormalizeFunction* normalizer;

		enum Flags
		{
			NoFlags=0x0,		//< no flags set
			Optional=0x1,		//< element is an optional
			Mandatory=0x2,		//< element is an mandatory
			Attribute=0x4,		//< element is an attribute
			Array=0x8		//< element is an array
		};
		unsigned char flags;

		bool optional() const						{return (flags & (unsigned char)Optional) != 0;}
		bool mandatory() const						{return (flags & (unsigned char)Mandatory) != 0;}
		bool attribute() const						{return (flags & (unsigned char)Attribute) != 0;}
		bool array() const						{return (flags & (unsigned char)Array) != 0;}

		void setOptional( bool v=true)					{if (v) flags |= (unsigned char)Optional;  else flags -= (flags & (unsigned char)Optional);}
		void setMandatory( bool v=true)					{if (v) flags |= (unsigned char)Mandatory; else flags -= (flags & (unsigned char)Mandatory);}
		void setAttribute( bool v=true)					{if (v) flags |= (unsigned char)Attribute; else flags -= (flags & (unsigned char)Attribute);}

		Variant::Type type();
		void makeArray();
		void copy( const Element& o);
	};

	class const_iterator
	{
	public:
		const_iterator( Element const* itr_)				:m_itr(itr_){}
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
		iterator( Element* itr_)					:m_itr(itr_){}
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

	const Element& back() const						{if (m_size==0) throw std::logic_error("array bound read"); return m_ar[ m_size-1];}
	Element& back()								{if (m_size==0) throw std::logic_error("array bound write"); return m_ar[ m_size-1];}

	const_iterator begin() const						{return beginptr();}
	const_iterator end() const						{return endptr();}
	iterator begin()							{return beginptr();}
	iterator end()								{return endptr();}

	int addAttribute( const std::string& name, const Variant& initvalue, const NormalizeFunction* normalizer);
	int addAtom( const std::string& name, const Variant& initvalue, const NormalizeFunction* normalizer);
	int addStructure( const std::string& name, const VariantStructDescription& substruct);
	int addIndirection( const std::string& name_, const VariantStructDescription* descr);
	int addElement( const Element& elem);
	void inherit( const VariantStructDescription& parent);

	int findidx( const std::string& name) const;
	const_iterator find( const std::string& name) const;
	iterator find( const std::string& name);

	std::size_t size() const						{return m_size;}

	int compare( const VariantStructDescription& o) const;
	std::string names( const std::string& sep) const;

	void print( std::ostream& out, const std::string& indent, const std::string& newitem, std::size_t level) const;
	std::string tostring() const;

private:
	const Element* beginptr() const						{return m_ar;}
	Element* beginptr()							{return m_ar;}
	const Element* endptr() const						{return m_ar+m_size;}
	Element* endptr()							{return m_ar+m_size;}

private:
	std::size_t m_size;
	std::size_t m_nofattributes;
	Element* m_ar;
};

typedef types::CountedReference<VariantStructDescription> VariantStructDescriptionR;

}} //namespace
#endif


