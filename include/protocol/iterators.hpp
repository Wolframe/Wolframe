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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
#ifndef _Wolframe_PROTOCOL_ITERATORS_HPP_INCLUDED
#define _Wolframe_PROTOCOL_ITERATORS_HPP_INCLUDED
#include <iterator>

///\file protocol/iterators.hpp
///\brief Iterator types of the protocol input blocks

namespace _Wolframe {
namespace protocol {

///\class iterator_t
///\brief template for STL conform random access iterators for arrays as used by the protocol
///\tparam array_type_ type of array this iterator iterates on
///\tparam size_type_ random access iterator size_type
///\tparam value_type_ random access iterator value_type
///\tparam access_type_ random access iterator type of value dereferenced with '*' access operator on iterator_t
///\tparam deref_type_ random access iterator deref_type
template <class array_type_, typename size_type_, typename value_type_, typename access_type_, typename deref_type_>
class iterator_t
{
public:
	typedef array_type_ array_type;
	typedef access_type_ access_type;
	typedef deref_type_ deref_type;
	typedef std::random_access_iterator_tag iterator_category;
	typedef size_type_ size_type;
	typedef value_type_ value_type;
	typedef size_type difference_type;
	typedef size_type distance_type;
	typedef value_type* pointer;
	typedef value_type& reference;

	//constructors
	iterator_t()								:m_this(0),m_idx(0){}
	iterator_t( array_type* t)						:m_this(t),m_idx(0){}
	iterator_t( const iterator_t& o)					:m_this(o.m_this),m_idx(o.m_idx){}

	iterator_t& operator=( const iterator_t& o)				{m_this = o.m_this; m_idx = o.m_idx; return *this;}
	access_type operator*()							{return (*m_this)[ m_idx];}
	access_type operator*() const						{return (*m_this)[ m_idx];}
	deref_type operator->()							{return &m_this->at( m_idx);}
	deref_type operator->() const						{return &m_this->at( m_idx);}
	iterator_t& operator++()						{++m_idx; return *this;}
	iterator_t operator++(int)						{iterator_t rt=*this; ++m_idx; return rt;}
	iterator_t& operator--()						{--m_idx; return *this;}
	iterator_t operator--(int)						{iterator_t rt=*this; --m_idx; return rt;}
	iterator_t operator+( difference_type d)				{iterator_t rt=*this; rt.m_idx+=d; return rt;}
	iterator_t operator+( difference_type d) const				{iterator_t rt=*this; rt.m_idx+=d; return rt;}
	iterator_t operator-( difference_type d)				{iterator_t rt=*this; rt.m_idx-=d; return rt;}
	iterator_t operator-( difference_type d) const				{iterator_t rt=*this; rt.m_idx-=d; return rt;}
	iterator_t& operator+=( difference_type d)				{m_idx+=d; return *this;}
	iterator_t& operator-=( difference_type d)				{m_idx-=d; return *this;}
	difference_type operator-( const iterator_t& b) const			{return m_idx-b.m_idx;}
	bool operator == (const iterator_t& o) const				{return (o.m_this == m_this && o.m_idx == m_idx);}
	bool operator != (const iterator_t& o) const				{return (o.m_this != m_this || o.m_idx != m_idx);}
	bool operator > (const iterator_t& o) const				{return (o.m_this == m_this && m_idx > o.m_idx);}
	bool operator < (const iterator_t& o) const				{return (o.m_this == m_this && m_idx < o.m_idx);}
	bool operator >= (const iterator_t& o) const				{return (o.m_this == m_this && m_idx >= o.m_idx);}
	bool operator <= (const iterator_t& o) const				{return (o.m_this == m_this && m_idx <= o.m_idx);}
	const value_type& operator[]( size_type& i) const			{return (*m_this)[m_idx+i];}
	value_type& operator[]( size_type& i)					{return (*m_this)[m_idx+i];}
	deref_type ptr() const							{return (deref_type)(m_this->ptr()) + m_idx;}
	deref_type ptr()							{return (deref_type)(m_this->ptr()) + m_idx;}

private:
	array_type* m_this;	///< reference to the array iterated. The array is accessed over the structure (*m_this)[ m_idx] to catch ABR/ABW (Array Bounds Read/Write)
	size_type m_idx;        ///< iterator position index
};
}}//namespace
#endif
