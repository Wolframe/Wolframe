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
///\file datatypeBigint.hpp
///\brief Custom data type interface for arbitrary length bcd integer numbers
#ifndef _CUSTOM_DATA_TYPE_BCD_BIGINT_HPP_INCLUDED
#define _CUSTOM_DATA_TYPE_BCD_BIGINT_HPP_INCLUDED
#include "types/customDataType.hpp"
#include "types/variant.hpp"
#include "bcdArithmetic.hpp"

namespace _Wolframe {
namespace types {

class BigintDataInitializer
	:public CustomDataInitializer
{
public:
	explicit BigintDataInitializer( const std::vector<types::Variant>& arg);
	virtual ~BigintDataInitializer(){}

	unsigned int max_digits() const			{return m_max_digits;}

	static CustomDataInitializer* create( const std::vector<types::Variant>& arg)
	{
		if (arg.size() == 0) return 0;
		return new BigintDataInitializer( arg);
	}

private:
	unsigned int m_max_digits;
};

class BigintDataValue
	:public CustomDataValue
	,public types::BigBCD
{
public:
	///\brief Constructor
	explicit BigintDataValue( _WOLFRAME_INTEGER val)
		:types::BigBCD(val){}
	///\brief Constructor
	explicit BigintDataValue( const std::string& val)
		:types::BigBCD(val){}

	///\brief Copy constructor
	BigintDataValue( const BigintDataValue& o)
		:CustomDataValue(o),types::BigBCD(o)
		,m_max_digits(o.m_max_digits){}

	explicit BigintDataValue( const BigintDataInitializer* ini=0)
		:m_max_digits(ini?ini->max_digits():std::numeric_limits<unsigned int>::max()){}

	virtual ~BigintDataValue(){};

	virtual int compare( const CustomDataValue& o) const;

	virtual std::string tostring() const
	{
		return types::BigBCD::tostring();
	}

	virtual void assign( const Variant& o);
	virtual bool getBaseTypeValue( Variant& dest) const;

	virtual CustomDataValue* copy() const
	{
		return new BigintDataValue(*this);
	}

	static CustomDataValue* create( const CustomDataInitializer* ini_)
	{
		const BigintDataInitializer* ini = reinterpret_cast<const BigintDataInitializer*>(ini_);
		return new BigintDataValue( ini);
	}
private:
	unsigned int m_max_digits;
};


class BigintDataType
	:public CustomDataType
	,public types::BigBCD
{
public:
	BigintDataType( const std::string& name_)
		:CustomDataType(name_,&BigintDataValue::create,&BigintDataInitializer::create)
	{
		define( Add, &add);
		define( Subtract, &subtract);
		define( Multiply, &multiply);
		define( Divide, &divide);
		define( Negation, &negation);
		define( ToInt, &toInt);
		define( ToDouble, &toDouble);
	}

	static CustomDataType* create( const std::string& name)
	{
		return new BigintDataType( name);
	}

private:
	static types::Variant add( const CustomDataValue& operand, const Variant& arg);
	static types::Variant subtract( const CustomDataValue& operand, const Variant& arg);
	static types::Variant multiply( const CustomDataValue& operand, const Variant& arg);
	static types::Variant divide( const CustomDataValue& operand, const Variant& arg);
	static types::Variant negation( const CustomDataValue& operand);
	static types::Variant toInt( const CustomDataValue& operand);
	static types::Variant toDouble( const CustomDataValue& operand);
};

}}//namespace
#endif





