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
///\file mylangStructure.hpp
///\brief Interface to mylang data structure representing input/output of a mylang form function
#ifndef _Wolframe_Mylang_STRUCTURE_HPP_INCLUDED
#define _Wolframe_Mylang_STRUCTURE_HPP_INCLUDED
#include "types/variant.hpp"
#include "types/countedReference.hpp"
#include "utils/printFormats.hpp"
#include <vector>
#include <string>
#include <ostream>

namespace _Wolframe {
namespace langbind {
namespace mylang {

///\brief Reference with ownership to a structure
class Structure;
typedef types::CountedReference<Structure> StructureR;

///\class Structure
///\brief Data structure for input and output of a 'Mylang' function call
class Structure
{
public:
	///\brief Constructor
	explicit Structure()
		:m_array(false){}
	virtual ~Structure(){}

	///\brief Getter for element value in case of this representing an atom
	types::Variant getValue() const;

	///\brief Find out if 'this' represents an atomic value
	///\return true, if yes
	bool atomic() const;
	///\brief Find out if 'this' represents an array of 'Structure'
	///\return true, if yes
	bool array() const;
	///\brief Find out how many elements are defined in this structure
	///\return the count

	typedef std::pair<types::Variant,StructureR> KeyValuePair;

	///\brief Iterator on structure or array elements
	class const_iterator
	{
	public:
		const_iterator();
		const_iterator( const Structure* st_, std::size_t pos_);
		explicit const_iterator( const Structure* st_);
		const_iterator( const const_iterator& o);
		~const_iterator(){}

		int compare( const const_iterator& o) const;

		bool operator==( const const_iterator& o) const			{return compare(o) == 0;}
		bool operator!=( const const_iterator& o) const			{return compare(o) != 0;}
		bool operator<( const const_iterator& o) const			{return compare(o) < 0;}
		bool operator<=( const const_iterator& o) const			{return compare(o) <= 0;}
		bool operator>( const const_iterator& o) const			{return compare(o) > 0;}
		bool operator>=( const const_iterator& o) const			{return compare(o) >= 0;}

		const_iterator& operator++()					{fetch_next(); return *this;}
		const_iterator operator++(int)					{const_iterator rt(*this); fetch_next(); return rt;}

		struct Element
		{
			types::VariantConst key;
			Structure* val;

			bool atomic() const					{return val->atomic();}
			bool array() const					{return val->array();}
			types::Variant getValue() const				{return val->getValue();}
		};
		const Element* operator->() const				{return &m_elem;}
		const Element& operator*() const				{return m_elem;}

	private:
		void fetch_next();

		const Structure* m_st;
		std::vector<KeyValuePair>::const_iterator m_itr;
		std::vector<KeyValuePair>::const_iterator m_end;
		Element m_elem;
	};

	///\brief Get the start iterator on structure or array elements
	const_iterator begin() const						{return const_iterator(this);}
	///\brief Get the end marker for a structure or and array
	const_iterator end() const						{return const_iterator(this, m_struct.size());}

	///\brief Print the structure serialized as string to out
	void print( std::ostream& out, const utils::PrintFormat* pformat, std::size_t level) const;
	///\brief Get the structure serialized as string for output
	std::string tostring( const utils::PrintFormat* pformat=utils::logPrintFormat()) const;

private:
	friend class Structure::const_iterator;
	friend class StructureBuilder;
	std::vector<KeyValuePair> m_struct;	//< mimic language structure
	types::Variant m_value;			//< value for atomic element
	bool m_array;				//< true, if this represents an array
};

}}}//namespace
#endif

