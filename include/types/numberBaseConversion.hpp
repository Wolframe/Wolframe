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
///\file types/numberBaseConversion.hpp
///\brief Private interface for big number conversions from decimal to binary represention and back

#ifndef _Wolframe_TYPES_NUMBER_BASE_CONVERSION_HPP_INCLUDED
#define _Wolframe_TYPES_NUMBER_BASE_CONVERSION_HPP_INCLUDED
#include "types/integer.hpp"
#include <string>

namespace _Wolframe {
namespace types {

///\brief Convert a large but limited size big endian integer value to a BCD number
///\param[in] uintptr pointer to integer number (big endian) 
///\param[in] uintsize size of 'uintptr' in bytes
///\param[out] digitsbuf buffer for digits to fill with the result (integers from 0..9 and NOT ascii '0'..'9')
///\param[in] digitsbufsize allocation size of 'digitsbuf' in bytes
///\return number of digits in the result (might be 0 for "0")
unsigned int convertBinaryToBCD( const unsigned char* uintptr, unsigned int uintsize, unsigned char* digitsbuf, unsigned int digitsbufsize);

///\brief Convert a large but limited size BCD number to a big endian integer value
///\param[in] digitsbuf buffer with BCD digits to convert (integers from 0..9 and NOT ascii '0'..'9')
///\param[in] digitsbufsize size of 'digitsbuf' in bytes
///\param[out] buf integer number (big endian) = conversion result
///\param[in] bufsize size of 'buf' in bytes
///\return number of digits in the result (might be 0 for "0")
void convertBCDtoBinary( const unsigned char* digits, unsigned int nofdigits, unsigned char* buf, unsigned int bufsize);

///\brief Convert a large but limited size big endian integer value to a BCD number
///\tparam UINTTYPE big endian integer type
///\param[in] val big integer number
///\param[out] digitsbuf buffer for digits to fill with the result (integers from 0..9 and NOT ascii '0'..'9')
///\param[in] digitsbufsize allocation size of 'digitsbuf' in bytes
///\return number of digits in the result (might be 0 for "0")
template <typename UINTTYPE>
unsigned int convertBigEndianUintToBCD( const UINTTYPE& val, unsigned char* digitsbuf, unsigned int digitsbufsize)
{
	return convertBinaryToBCD( (const unsigned char*)&val, sizeof(val), digitsbuf, digitsbufsize);
}

///\brief Convert a large but limited size BCD number to a big endian integer value
///\tparam UINTTYPE big endian integer type
///\param[in] digitsbuf buffer with BCD digits to convert (integers from 0..9 and NOT ascii '0'..'9')
///\param[in] digitsbufsize size of 'digitsbuf' in bytes
///\param[out] val big integer number
///\return number of digits in the result (might be 0 for "0")
template <typename UINTTYPE>
void convertBCDtoBigEndianUint( const unsigned char* digits, unsigned int nofdigits, UINTTYPE& val)
{
	convertBCDtoBinary( digits, nofdigits, (unsigned char*)&val, sizeof(val));
}

struct Endian
{
	template <typename UINTTYPE>
	static void reorder( UINTTYPE& num)
	{
		unsigned char* ref = (unsigned char*)&num;
		unsigned int ii=0, nn=sizeof(UINTTYPE)/2;
		for (; ii<nn; ++ii)
		{
			unsigned char tmp = ref[ii];
			ref[ii] = ref[ sizeof(UINTTYPE)-ii-1];
			ref[ sizeof(UINTTYPE)-ii-1] = tmp;
		}
	}
};

}}//namespace
#endif

