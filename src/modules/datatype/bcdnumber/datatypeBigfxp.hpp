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
///\file datatypeBigfxp.hpp
///\brief Custom data type interface for arbitrary length bcd fixed point numbers
#ifndef _CUSTOM_DATA_TYPE_BCD_BIGFXP_HPP_INCLUDED
#define _CUSTOM_DATA_TYPE_BCD_BIGFXP_HPP_INCLUDED
#include "types/customDataType.hpp"
#include "types/variant.hpp"
#include "fxpArithmetic.hpp"
#include <limits>

namespace _Wolframe {
namespace types {

class BigfxpDataInitializer
	:public CustomDataInitializer
{
public:
	explicit BigfxpDataInitializer( const std::vector<types::Variant>& arg);
	virtual ~BigfxpDataInitializer(){}

	unsigned int max_integer_digits() const		{return m_max_integer_digits;}
	unsigned int max_fractional_digits() const	{return m_max_fractional_digits;}

	static CustomDataInitializer* create( const std::vector<types::Variant>& arg)
	{
		if (arg.size() == 0) return 0;
		return new BigfxpDataInitializer( arg);
	}

private:
	unsigned int m_max_integer_digits;
	unsigned int m_max_fractional_digits;
};

class BigfxpDataValue
	:public CustomDataValue
	,public types::BigFxpBCD
{
public:
	///\brief Copy constructor
	BigfxpDataValue( const BigfxpDataValue& o)
		:CustomDataValue(o)
		,types::BigFxpBCD(o)
		,m_max_integer_digits(o.m_max_integer_digits)
		,m_max_fractional_digits(o.m_max_fractional_digits){}

	BigfxpDataValue( const BigfxpDataInitializer* ini)
		:types::BigFxpBCD()
		,m_max_integer_digits(ini?ini->max_integer_digits():std::numeric_limits<unsigned int>::max())
		,m_max_fractional_digits(ini?ini->max_fractional_digits():std::numeric_limits<unsigned int>::max()){}

	virtual ~BigfxpDataValue(){};

	virtual int compare( const CustomDataValue& o) const;

	virtual std::string tostring() const
	{
		return types::BigFxpBCD::tostring();
	}

	virtual void assign( const Variant& o);
	virtual bool getBaseTypeValue( Variant& dest) const;

	virtual CustomDataValue* copy() const
	{
		return new BigfxpDataValue( *this);
	}

	static CustomDataValue* create( const CustomDataInitializer* ini_)
	{
		const BigfxpDataInitializer* ini = reinterpret_cast<const BigfxpDataInitializer*>(ini_);
		return new BigfxpDataValue( ini);
	}

private:
	unsigned int m_max_integer_digits;
	unsigned int m_max_fractional_digits;
};


class BigfxpDataType
	:public CustomDataType
{
public:
	BigfxpDataType( const std::string& name_)
		:CustomDataType(name_,&BigfxpDataValue::create,&BigfxpDataInitializer::create)
	{
		define( Add, &add);
		define( Subtract, &subtract);
		define( Multiply, &multiply);
		define( Negation, &negation);
		define( ToDouble, &toDouble);
		define( "round", &round);
		define( "format", &format);
		define( "divide", &divide);
	}

	static CustomDataType* create( const std::string& name)
	{
		return new BigfxpDataType( name);
	}

private:
	static types::Variant add( const CustomDataValue& operand, const Variant& arg);
	static types::Variant subtract( const CustomDataValue& operand, const Variant& arg);
	static types::Variant multiply( const CustomDataValue& operand, const Variant& arg);
	static types::Variant negation( const CustomDataValue& operand);
	static types::Variant toDouble( const CustomDataValue& operand);
	static types::Variant format( const CustomDataValue& operand, const std::vector<types::Variant>& arg);
	static types::Variant round( const CustomDataValue& operand, const std::vector<types::Variant>& arg);
	static types::Variant divide( const CustomDataValue& operand, const std::vector<types::Variant>& arg);
};

}}//namespace
#endif

