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
//\file types/bignumber.hpp
//\brief Type for representing arbitrary precision fixed point numbers and big integers as binary coded decimal (BCD) numbers

#ifndef _Wolframe_TYPES_BIG_NUMBER_HPP_INCLUDED
#define _Wolframe_TYPES_BIG_NUMBER_HPP_INCLUDED
#include "types/integer.hpp"
#include <string>
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace types {

//\class BigNumber
//\brief Type for representing big numbers as binary coded decimal (BCD) numbers
struct BigNumber
{
	//\brief Default constructor
	BigNumber();
	//\brief Copy constructor
	//\param[in] o number to copy
	BigNumber( const BigNumber& o);
	//\brief Constructor from a double
	//\param[in] val number value as double precision floating point number
	BigNumber( double val);
	//\brief Constructor from a signed integer
	//\param[in] val number value as signed integer
	BigNumber( _WOLFRAME_INTEGER val);
	//\brief Constructor from an unsigned integer
	//\param[in] val number value as unsigned integer
	BigNumber( _WOLFRAME_UINTEGER val);
	//\brief Constructor from a string
	//\param[in] val number value as printable string
	BigNumber( const std::string& val);
	//\brief Constructor
	//\param[in] sign_ true, if the number is negative, false if positive or 0
	//\param[in] precision_ number of significant digits (also the number of elements in digits_)
	//\param[in] scale_ number of digits right of the comma
	//\param[in] digits_ decimal digits of the number [0x00..0x09]
	BigNumber( bool sign_, unsigned short precision_, signed short scale_, const unsigned char* digits_);
	//\brief Destructor
	~BigNumber();

	//\brief Get the number of significant digits (also the number of elements in digits())
	//\return the precision
	unsigned short precision() const	{return m_size;}
	//\brief Get the number of digits right of the comma
	//\return the scale
	signed short scale() const		{return m_scale;}
	//\brief Get the size of the number (number of significant digits)
	//\return the size
	unsigned short size() const		{return m_size;}
	//\brief Get the sign of the number (true -> negative number)
	//\return the sign
	bool sign() const			{return m_sign;}
	//\brief Get the significant digits of the number
	//\return the digits [0x00..0x09]
	const unsigned char* digits() const	{return m_ar;}

	//\brief Get the number value as printable string
	//\return the number as string
	std::string tostring() const;
	//\brief Get the number value as printable string in normalized form ([0..1] + exponent if scale < 0 or scale > precision)
	//\return the number as string
	std::string tostringNormalized() const;
	//\brief Get the number value as integer if a conversion is possible
	//\return the number as integer
	_WOLFRAME_INTEGER toint() const;
	//\brief Get the number value as unsigned integer if a conversion is possible
	//\return the number as unsigned integer
	_WOLFRAME_UINTEGER touint() const;
	//\brief Get the number value as double precision floating point number if a conversion is possible
	//\return the number as double precision floating point number
	double todouble() const;
	//\brief Check if the number is valid
	//\return true, if yes
	bool isvalid() const;

private:
	//\brief Constructor 
	void constructor( const std::string& val);

	friend class BigNumberConst;
	enum ConstQualifier {ConstC};
	//\brief Helper constructor for types::BigNumberConst
	//\param[in] sign_ true, if the number is negative, false if positive or 0
	//\param[in] precision_ number of significant digits (also the number of elements in digits_)
	//\param[in] scale_ number of digits right of the comma
	//\param[in] digits_ decimal digits of the number [0x00..0x09]
	BigNumber( const ConstQualifier&, bool sign_, unsigned short precision_, signed short scale_, const unsigned char* digits_);

private:
	signed short m_scale;		//< scale number of digits right of the comma
	bool m_sign;			//< sign (true, if the number is negative, false if positive or 0)
	bool m_const;			//< true if the value is not allocated (no ownership)
	unsigned short m_size;		//< size of 'm_ar' in bytes
	unsigned char* m_ar;		//< decimal digits of the number [0x00..0x09]
};

//\class BigNumberConst
//\brief Constant big number that is not owned by the structure (caller has ownership)
//\remark This object holds only reference. No copy is made. The life time of this has to be covered by the scope of the parameters used to construct the big number
class BigNumberConst
	:public BigNumber
{
public:
	//\brief Default constructor
	BigNumberConst()
		:BigNumber(ConstC,false,0,0,0){}
	//\brief Constructor
	//\param[in] sign_ true, if the number is negative, false if positive or 0
	//\param[in] precision_ number of significant digits (also the number of elements in digits_)
	//\param[in] scale_ number of digits right of the comma
	//\param[in] digits_ decimal digits of the number [0x00..0x09]
	BigNumberConst( bool sign_, unsigned short precision_, signed short scale_, const unsigned char* digits_)
		:BigNumber(ConstC,sign_,precision_,scale_,digits_){}
	//\brief Copy constructor
	//\brief o big number reference to copy
	BigNumberConst( const BigNumberConst& o)
		:BigNumber(ConstC,o.sign(),o.precision(),o.scale(),o.digits()){}
};


}}//namespace
#endif


