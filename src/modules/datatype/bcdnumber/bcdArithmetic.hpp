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
///\file bcdArithmetic.hpp
///\brief Defines some operations on arbitrary sized packed bcd numbers
///\note for the Addition,Subtraction and Verification we thank Douglas W. Jones for http://www.divms.uiowa.edu/~jones/bcd/bcd.html
#ifndef _Wolframe_DATATYPE_BCD_ARITHMETIC_HPP_INCLUDED
#define _Wolframe_DATATYPE_BCD_ARITHMETIC_HPP_INCLUDED
#include <string>
#include <boost/cstdint.hpp>
#include "utils/allocators.hpp"
#include "types/integer.hpp"
#include "types/bignumber.hpp"

namespace _Wolframe {
namespace types {

#define _Wolframe_TYPES_BCD_USE_64BIT
#ifdef _Wolframe_TYPES_BCD_USE_64BIT
typedef boost::uint64_t BCDElement;
typedef boost::uint64_t FactorType;
#else
typedef boost::uint32_t BCDElement;
typedef boost::uint32_t FactorType;
#endif

///\class BigBCD
///\brief Arbitrary size BCD number type with basic arithmetic operations
class BigBCD
{
public:
	BigBCD();
	BigBCD( const std::string& numstr);
	BigBCD( _WOLFRAME_INTEGER num);
	BigBCD( _WOLFRAME_UINTEGER num);
	BigBCD( const BigNumber& num);
	BigBCD( const BigBCD& o);
	~BigBCD();

	void init( const BigBCD& o)			{copy( o, 0);}
	void init( const std::string& str);
	void init( const char* str, std::size_t strsize);
	void init( _WOLFRAME_INTEGER num);
	void init( _WOLFRAME_UINTEGER num);
	void init( const BigNumber& num);

	std::string tostring() const;
	_WOLFRAME_INTEGER toint() const;
	double todouble() const;

	BigBCD operator /( const BigBCD& opr) const	{return div( opr);}
	BigBCD operator *( const BigBCD& opr) const	{return mul( opr);}
	BigBCD operator *( _WOLFRAME_INTEGER opr) const	{return mul( opr);}
	BigBCD operator +( const BigBCD& opr) const	{return add( opr);}
	BigBCD operator -( const BigBCD& opr) const	{return sub( opr);}
	BigBCD operator -() const			{return neg();}

	BigBCD shift( int digits) const;
	BigBCD cut( unsigned int digits) const;
	BigBCD round( const BigBCD& gran) const;

	void invert_sign()				{m_sign = !m_sign; normalize();}
	char sign() const				{return m_sign?'-':'+';}

	bool operator==( const BigBCD& o) const		{return compare(o)==0;}
	bool operator!=( const BigBCD& o) const		{return compare(o)!=0;}
	bool operator<=( const BigBCD& o) const		{return compare(o)<=0;}
	bool operator<( const BigBCD& o) const		{return compare(o)<0;}
	bool operator>=( const BigBCD& o) const		{return compare(o)>=0;}
	bool operator>( const BigBCD& o) const		{return compare(o)>0;}
	int compare( const BigBCD& o) const;

	bool isValid() const;
	bool isNull() const;
	std::size_t nof_digits() const			{return begin().size();}

	friend class const_iterator;
	class const_iterator
	{
	public:
		const_iterator();
		const_iterator( const BigBCD& bcd);
		const_iterator( const const_iterator& o)	:m_idx(o.m_idx),m_shf(o.m_shf),m_ar(o.m_ar){}
		const_iterator& operator++()			{increment(); return *this;}
		const_iterator operator++(int)			{const_iterator rt(*this); increment(); return rt;}
		unsigned char operator*() const;
		const const_iterator* operator->() const	{return this;}

		char ascii() const				{return operator*() + '0';}
		std::size_t size() const;

		bool operator==( const const_iterator& o) const	{return isequal(o);}
		bool operator!=( const const_iterator& o) const	{return !isequal(o);}
		bool operator<=( const const_iterator& o) const	{return isle(o);}
		bool operator<( const const_iterator& o) const	{return islt(o);}
		bool operator>=( const const_iterator& o) const	{return !islt(o);}
		bool operator>( const const_iterator& o) const	{return !isle(o);}

	private:
		void increment();
		bool isequal( const const_iterator& other) const;
		bool islt( const const_iterator& other) const;
		bool isle( const const_iterator& other) const;

		std::size_t m_idx;
		unsigned char m_shf;
		const BCDElement* m_ar;
	};

	const_iterator begin() const				{return const_iterator(*this);}
	const_iterator end() const				{return const_iterator();}

private:
	friend class BigFxpBCD;
	typedef utils::GreedySmallChunkAllocator Allocator;
	void allocate( std::size_t size_, Allocator* allocator);
	void copy( const BigBCD& o, Allocator* allocator);
	void normalize();

	static void digits_addition( BigBCD& dest, const BigBCD& this_, const BigBCD& opr, Allocator* allocator);
	static void digits_subtraction( BigBCD& dest, const BigBCD& this_, const BigBCD& opr, Allocator* allocator);
	static void digits_shift( BigBCD& dest, const BigBCD& this_, int nof_digits, Allocator* allocator);
	static void digits_cut( BigBCD& dest, const BigBCD& this_, unsigned int nof_digits, Allocator* allocator);
	static void digits_nibble_multiplication( BigBCD& dest, const BigBCD& this_, unsigned char factor, Allocator* allocator);
	static void digits_16_multiplication( BigBCD& dest, const BigBCD& this_, Allocator* allocator);
	static void digits_multiplication( BigBCD& dest, const BigBCD& this_, FactorType factor, Allocator* allocator);
	static void digits_multiplication( BigBCD& dest, const BigBCD& this_, const BigBCD& factor, Allocator* allocator);
	static void digits_division( BigBCD& dest, const BigBCD& this_, const BigBCD& factor, Allocator* allocator);
	static void xchg( BigBCD& a, BigBCD& b);
	static FactorType division_estimate( const BigBCD& this_, const BigBCD& opr);
	static BigBCD estimate_as_bcd( FactorType estimate, int estshift, Allocator* allocator);

	BigBCD add( const BigBCD& opr) const;
	BigBCD sub( const BigBCD& opr) const;
	BigBCD mul( FactorType opr) const;
	BigBCD mul( _WOLFRAME_INTEGER opr) const;
	BigBCD mul( const BigBCD& opr) const;
	BigBCD div( const BigBCD& opr) const;
	BigBCD neg() const;

private:
	std::size_t m_size;
	BCDElement* m_ar;
	bool m_sign;
	bool m_allocated;
};

}}//namespace
#endif
