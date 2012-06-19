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
///\note for the Addition,Subtraction and Verification we refer to http://www.divms.uiowa.edu/~jones/bcd/bcd.html.
#ifndef _Wolframe_TYPES_BCD_ARITHMETIC_HPP_INCLUDED
#define _Wolframe_TYPES_BCD_ARITHMETIC_HPP_INCLUDED
#include <string>
#include <boost/cstdint.hpp>

namespace _Wolframe {
namespace types {

class BigBCD
{
public:
	BigBCD( const std::string& numstr);
	BigBCD( const BigBCD& o);
	~BigBCD();

	std::string tostring() const;

	BigBCD operator +( const BigBCD& opr);
	BigBCD operator -( const BigBCD& opr);

	bool isValid() const;

private:
	void init( std::size_t size_, bool sign_);
	void expand( std::size_t addsize);

	BigBCD( std::size_t n, bool sgn);
	static void digits_addition( BigBCD& dest, BigBCD& this_, const BigBCD& opr, boost::uint32_t& carrybit);
	static void digits_subtraction( BigBCD& dest, BigBCD& this_, const BigBCD& opr);

	std::size_t m_size;
	boost::uint32_t* m_ar;
	bool m_sign;
};

}}//namespace
#endif
