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
//\file types/numberBaseConversion.cpp
//\brief Implementation of number conversion from decimal to binary represention and back
#include "types/numberBaseConversion.hpp"
#include "types/integer.hpp"
#include <string>
#include <cstring>
#include <stdexcept>

using namespace _Wolframe;
using namespace _Wolframe::types;

void types::convertBCDtoBinary( const unsigned char* digits, unsigned int nofdigits, unsigned char* buf, unsigned int bufsize)
{
	// Include of generated conversion table:
	#include "types/bcdToBinaryConversionTable.cpp"

	if (nofdigits > MaxDecDigits)
	{
		throw std::runtime_error("number out of decimal to binary integer conversion range");
	}
	unsigned int sum[ HexNumFactor::TabSize];
	std::memset( sum, 0, sizeof(sum));

	unsigned int ii = 0;
	for (; digits[ii] == 0; ++ii){}
	for (; ii<nofdigits; ++ii)
	{
		unsigned int tabidx = nofdigits - ii -1;
		unsigned int ofs = g_decnum_hexnum_factor_table[ tabidx].ofs;
		unsigned int digit = digits[ii];
		for (; ofs < HexNumFactor::TabSize; ++ofs)
		{
			unsigned int fac = g_decnum_hexnum_factor_table[ tabidx].tab[ ofs];
			sum[ ofs] += digit * fac;
		}
	}
	for (ii=HexNumFactor::TabSize; ii>1; --ii)
	{
		if (sum[ ii-1] > 255)
		{
			sum[ ii-2] += sum[ ii-1] / 256;
			sum[ ii-1] = sum[ ii-1] % 256;
		}
		else if (sum[ ii-1] == 0)
		{
			if (ii <= g_decnum_hexnum_factor_table[ nofdigits-1].ofs) break;
		}
	}
	unsigned int hexnumsize = 0;
	unsigned int sumstart = ii;
	for (ii=sumstart; ii<HexNumFactor::TabSize; ++ii)
	{
		if (sum[ii])
		{
			hexnumsize = HexNumFactor::TabSize - ii;
			break;
		}
	}
	if (sum[0] > 255) throw std::logic_error( "decimal to binary integer conversion table dimension is too small");
	if (hexnumsize > bufsize)
	{
		throw std::runtime_error("destination buffer too small for decimal to binary integer conversion");
	}
	for (ii=0; ii<bufsize; ii++)
	{
		buf[ ii] = (unsigned char)sum[ HexNumFactor::TabSize-ii-1];
	}
}

unsigned int types::convertBinaryToBCD( const unsigned char* uintptr, unsigned int uintsize, unsigned char* digitsbuf, unsigned int digitsbufsize)
{
	// Include of generated conversion table:
	#include "types/binaryToBcdConversionTable.cpp"

	unsigned int uinthexdigits = uintsize*2;
	if (uinthexdigits > MaxHexDigits)
	{
		throw std::runtime_error("number out of binary integer to BCD conversion range");
	}
	unsigned int sum[ DecNumFactor::TabSize];
	std::memset( sum, 0, sizeof(sum));

	unsigned int ii = 0;
	for (; uintptr[ii] == 0; ++ii){}
	ii *= 2;

	for (; ii<uinthexdigits; ii+=2)
	{
		unsigned int tabidx_hi = uinthexdigits - ii -1;
		unsigned int tabidx_lo = uinthexdigits - ii -2;

		unsigned int ofs_hi = g_hexnum_decnum_factor_table[ tabidx_hi].ofs;
		unsigned int ofs_lo = g_hexnum_decnum_factor_table[ tabidx_lo].ofs;

		unsigned int hexdigit_lo = uintptr[ii>>1] & 0x0F;
		unsigned int hexdigit_hi = uintptr[ii>>1] >> 4;

		for (; ofs_lo < DecNumFactor::TabSize; ++ofs_lo)
		{
			unsigned int fac = g_hexnum_decnum_factor_table[ tabidx_lo].tab[ ofs_lo];
			sum[ ofs_lo] += hexdigit_lo * fac;
		}
		for (; ofs_hi < DecNumFactor::TabSize; ++ofs_hi)
		{
			unsigned int fac = g_hexnum_decnum_factor_table[ tabidx_hi].tab[ ofs_hi];
			sum[ ofs_hi] += hexdigit_hi * fac;
		}
	}
	for (ii=DecNumFactor::TabSize; ii>1; --ii)
	{
		if (sum[ ii-1] > 10)
		{
			sum[ ii-2] += sum[ ii-1] / 10;
			sum[ ii-1] = sum[ ii-1] % 10;
		}
	}

	unsigned int decnumstart = 0;
	for (ii=0; ii<DecNumFactor::TabSize; ++ii)
	{
		if (sum[ii])
		{
			decnumstart = ii;
			break;
		}
	}
	if (sum[0] > 10) throw std::logic_error( "decimal to binary integer conversion table dimension is too small");
	if (DecNumFactor::TabSize - decnumstart > digitsbufsize)
	{
		throw std::runtime_error("destination buffer too small for binary integer to BCD conversion");
	}
	unsigned int nn = DecNumFactor::TabSize - decnumstart -1;
	for (ii=0; ii<nn; ii++)
	{
		digitsbuf[ ii] = (unsigned char)sum[ decnumstart + ii];
	}
	return nn;
}

