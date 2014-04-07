/************************************************************************

 Copyright (C) 2011 - 2014 Project Wolframe.
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
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
//\file database/vm/valueTupleSet.hpp
//\brief Defines the instruction set of the virtual machine defining database transactions
#ifndef _DATABASE_VIRTUAL_MACHINE_VALUE_TYPLE_SET_HPP_INCLUDED
#define _DATABASE_VIRTUAL_MACHINE_VALUE_TYPLE_SET_HPP_INCLUDED
#include "types/variant.hpp"
#include <string>
#include <vector>
#include <cstdlib>
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace db {
namespace vm {

class ValueTupleSet
{
public:
	ValueTupleSet(){}
	explicit ValueTupleSet( const std::vector<std::string>& colnames_)
		:m_colnames(colnames_){}
	ValueTupleSet( const ValueTupleSet& o)
		:m_colnames(o.m_colnames),m_ar(o.m_ar){}

	friend class const_iterator;
	class const_iterator
	{
	public:
		const_iterator( const ValueTupleSet* ref_=0)
			:m_idx(0),m_ref(ref_){}
		const_iterator( const const_iterator& o)
			:m_idx(o.m_idx),m_ref(o.m_ref){}
		const_iterator& operator++()
		{
			if (m_ref)
			{
				m_idx += m_ref->m_colnames.size();
				if (m_idx >= m_ref->m_ar.size())
				{
					m_idx=0;
					m_ref=0;
				}
			}
			return *this;
		}
		const_iterator operator++(int)
		{
			const_iterator rt(*this);
			operator++();
			return rt;
		}

		std::size_t index() const
		{
			return m_idx;
		}

		bool operator==( const const_iterator& o) const
		{
			if (!m_ref && !o.m_ref) return true;
			return (m_ref == o.m_ref && m_idx == o.m_idx);
		}
		bool operator!=( const const_iterator& o) const
		{
			if (!m_ref && !o.m_ref) return false;
			return (m_ref != o.m_ref || m_idx != o.m_idx);
		}

		const const_iterator* operator->() const		{return this;}
		const types::Variant& column( std::size_t i) const	{if (!m_ref || i == 0 || i > m_ref->m_colnames.size() || m_idx + i - 1 >= m_ref->m_ar.size()) throw std::runtime_error("value tuple set column index out of range"); return m_ref->m_ar.at( m_idx + i - 1);}

	private:
		std::size_t m_idx;
		const ValueTupleSet* m_ref;
	};

	const_iterator begin() const
	{
		return const_iterator(this);
	}
	const_iterator end() const
	{
		return const_iterator();
	}

	std::size_t nofColumns() const
	{
		return m_colnames.size();
	}
	const std::string& columnName( std::size_t i) const
	{
		if (i == 0 || i > m_colnames.size()) throw std::runtime_error( "column index out of range");
		return m_colnames.at(i-1);
	}

	std::size_t columnIndex( const std::string& name) const
	{
		std::size_t ii=0;
		for (; ii<m_colnames.size(); ++ii)
		{
			if (boost::algorithm::iequals( name, m_colnames.at(ii)))
			{
				return ii+1;
			}
		}
		throw std::runtime_error("unknown value set column name");
	}

	void push( const std::vector<types::VariantConst>& c)
	{
		if (c.size() != m_colnames.size()) throw std::runtime_error("pushed result with non matching number of columns");
		for (std::vector<types::VariantConst>::const_iterator ci=c.begin(), ce=c.end(); ci != ce; ++ci)
		{
			m_ar.push_back( *ci);
		}
	}

	void push( const std::vector<types::Variant>& c)
	{
		if (c.size() != m_colnames.size()) throw std::runtime_error("pushed result with non matching number of columns");
		for (std::vector<types::Variant>::const_iterator ci=c.begin(), ce=c.end(); ci != ce; ++ci)
		{
			m_ar.push_back( *ci);
		}
	}

	std::size_t size() const	
	{
		return m_ar.size() / m_colnames.size();
	}

	bool empty() const
	{
		return m_ar.empty();
	}

	void checkConstraintNonEmpty() const
	{
		if (size() == 0) throw std::runtime_error( "constraint 'set non empty' failed");
	}

	void checkConstraintUnique() const
	{
		if (size() > 1) throw std::runtime_error( "constraint 'set unique' failed");
	}

	void append( const ValueTupleSet& ts)
	{
		if (ts.m_colnames.size() != m_colnames.size()) throw std::runtime_error("joining incompatible lists");
		m_ar.insert( m_ar.end(), ts.m_ar.begin(), ts.m_ar.end());
	}

private:
	std::vector<std::string> m_colnames;
	std::vector<types::Variant> m_ar;
};

typedef boost::shared_ptr<ValueTupleSet> ValueTupleSetR;

}}}//namespace
#endif

