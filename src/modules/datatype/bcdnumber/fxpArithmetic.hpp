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
///\file fxpArithmetic.hpp
///\brief Defines some operations on arbitrary sized packed bcd fixed point numbers
#ifndef _Wolframe_DATATYPE_FXP_ARITHMETIC_HPP_INCLUDED
#define _Wolframe_DATATYPE_FXP_ARITHMETIC_HPP_INCLUDED
#include "bcdArithmetic.hpp"

namespace _Wolframe {
namespace types {

///\class BigFxpBCD
///\brief Arbitrary size fixed point number represented as BCD plus comma position with basic arithmetic operations.
class BigFxpBCD
{
public:
	BigFxpBCD()
		:m_scale(0){}

	BigFxpBCD( const BigFxpBCD& o)
		:m_bcd(o.m_bcd)
		,m_scale(o.m_scale){}

	BigFxpBCD( const BigBCD& o, int scale_=0);
	BigFxpBCD( const BigNumber& num);
	BigFxpBCD( const std::string& numstr);

	std::string format( unsigned int scale_);

	BigFxpBCD round( const BigFxpBCD& gran);

	std::string tostring() const;
	double todouble() const;

	BigFxpBCD& operator=( const BigFxpBCD& o);
	BigFxpBCD& operator=( const std::string& o);
	BigFxpBCD& operator=( double o);
	BigFxpBCD& operator=( _WOLFRAME_INTEGER o);
	BigFxpBCD& operator=( _WOLFRAME_UINTEGER o);
	BigFxpBCD& operator=( const BigNumber& num);

	BigFxpBCD divide( const BigFxpBCD& opr, unsigned int addscale) const;
	BigFxpBCD divide( _WOLFRAME_INTEGER opr, unsigned int addscale) const;
	BigFxpBCD operator *( const BigFxpBCD& opr) const;
	BigFxpBCD operator *( _WOLFRAME_INTEGER opr) const;
	BigFxpBCD operator +( const BigFxpBCD& opr) const;
	BigFxpBCD operator +( _WOLFRAME_INTEGER opr) const;
	BigFxpBCD operator -( const BigFxpBCD& opr) const;
	BigFxpBCD operator -( _WOLFRAME_INTEGER opr) const;
	BigFxpBCD operator -() const;

	bool operator==( const BigFxpBCD& o) const		{return compare(o)==0;}
	bool operator!=( const BigFxpBCD& o) const		{return compare(o)!=0;}
	bool operator<=( const BigFxpBCD& o) const		{return compare(o)<=0;}
	bool operator<( const BigFxpBCD& o) const		{return compare(o)<0;}
	bool operator>=( const BigFxpBCD& o) const		{return compare(o)>=0;}
	bool operator>( const BigFxpBCD& o) const		{return compare(o)>0;}
	int compare( const BigFxpBCD& o) const;

	bool sign() const					{return m_bcd.sign();}
	void invert_sign()					{m_bcd.invert_sign();}

private:
	void initFromString( const std::string& numstr);
	void initFromNumber( const BigNumber& num);

	BigBCD m_bcd;
	unsigned int m_scale;
};

}}//namespace
#endif

