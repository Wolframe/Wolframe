/************************************************************************
Copyright (C) 2011 Project Wolframe.
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
///\file types/bcdArithmetic.hpp
///\brief Defines some operations on arbitrary sized packed bcd numbers
///\note for the Addition,Subtraction and Verification we thank Douglas W. Jones for http://www.divms.uiowa.edu/~jones/bcd/bcd.html
#ifndef _Wolframe_TYPES_BCD_ARITHMETIC_HPP_INCLUDED
#define _Wolframe_TYPES_BCD_ARITHMETIC_HPP_INCLUDED
#include <string>
#include <boost/cstdint.hpp>

namespace _Wolframe {
namespace types {

class BigBCD
{
public:
	BigBCD();
	BigBCD( const std::string& numstr);
	BigBCD( const BigBCD& o);
	~BigBCD();

	std::string tostring() const;

	BigBCD& operator=( const BigBCD& o);

	BigBCD operator /( const BigBCD& opr) const	{return div( opr);}
	BigBCD operator *( const BigBCD& opr) const	{return mul( opr);}
	BigBCD operator *( unsigned int opr) const	{return mul( opr);}
	BigBCD operator +( const BigBCD& opr) const	{return add( opr);}
	BigBCD operator -( const BigBCD& opr) const	{return sub( opr);}
	BigBCD operator -() const			{return neg();}

	bool operator==( const BigBCD& o) const		{return isequal(o);}
	bool operator!=( const BigBCD& o) const		{return !isequal(o);}
	bool operator<=( const BigBCD& o) const		{return isle(o);}
	bool operator<( const BigBCD& o) const		{return islt(o);}
	bool operator>=( const BigBCD& o) const		{return !islt(o);}
	bool operator>( const BigBCD& o) const		{return !isle(o);}

	bool isValid() const;

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
		const boost::uint32_t* m_ar;
	};

	const_iterator begin() const				{return const_iterator(*this);}
	const_iterator end() const				{return const_iterator();}

private:
	void init( std::size_t size_, bool neg_=false);
	void expand( std::size_t addsize);

	explicit BigBCD( std::size_t n, bool sgn=true);

	static void digits_addition( BigBCD& dest, const BigBCD& this_, const BigBCD& opr);
	static void digits_subtraction( BigBCD& dest, const BigBCD& this_, const BigBCD& opr);
	static void digits_shift( BigBCD& rt, const BigBCD& this_, int nof_digits);
	static void digits_nibble_multiplication( BigBCD& rt, const BigBCD& this_, unsigned char factor);
	static void digits_16_multiplication( BigBCD& rt, const BigBCD& this_);
	static void digits_multiplication( BigBCD& rt, const BigBCD& this_, unsigned int factor);
	static void xchg( BigBCD& a, BigBCD& b);
	static unsigned int division_estimate( const BigBCD& this_, const BigBCD& opr);
	static BigBCD estimate_as_bcd( unsigned int estimate, int estshift);

	BigBCD shift( int digits) const;
	BigBCD add( const BigBCD& opr) const;
	BigBCD sub( const BigBCD& opr) const;
	BigBCD mul( unsigned int opr) const;
	BigBCD mul( const BigBCD& opr) const;
	BigBCD div( const BigBCD& opr) const;
	BigBCD mul16() const;
	BigBCD neg() const				{BigBCD rt(*this); rt.m_neg = !rt.m_neg; return rt;}

	char sign() const				{return m_neg?'-':'+';}
	bool isequal( const BigBCD& other) const;
	bool islt( const BigBCD& other) const;
	bool isle( const BigBCD& other) const;
	bool isabslt( const BigBCD& other) const;
	bool isabsle( const BigBCD& other) const;

private:
	std::size_t m_size;
	boost::uint32_t* m_ar;
	bool m_neg;
};

}}//namespace
#endif
