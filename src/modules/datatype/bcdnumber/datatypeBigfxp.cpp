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
///\file datatypeBigfxp.cpp
#include "datatypeBigfxp.hpp"
#include <limits>

using namespace _Wolframe;
using namespace _Wolframe::types;

BigfxpDataInitializer::BigfxpDataInitializer( const std::vector<types::Variant>& arg)
	:m_max_integer_digits(std::numeric_limits<unsigned int>::max())
	,m_max_fractional_digits(std::numeric_limits<unsigned int>::max())
{
	if (arg.size() > 2) throw std::runtime_error("too many arguments for big fixed point number initializer");
	if (arg.size() == 0) return;
	if (arg.size() == 1)
	{
		types::Variant::Data::Int a1 = arg.at(0).touint();
		if (a1 >= std::numeric_limits<unsigned int>::max()) throw std::runtime_error("max fractional digits argument out of range for big fixed point number initializer");
		m_max_fractional_digits = (unsigned int)a1;
	}
	else
	{
		types::Variant::Data::Int a1 = arg.at(0).touint();
		if (a1 >= std::numeric_limits<unsigned int>::max()) throw std::runtime_error("max integer digits argument out of range for big fixed point number initializer");
		m_max_integer_digits = (unsigned int)a1;

		types::Variant::Data::Int a2 = arg.at(1).touint();
		if (a2 >= std::numeric_limits<unsigned int>::max()) throw std::runtime_error("max fractional digits argument out of range for big fixed point number initializer");
		m_max_fractional_digits = (unsigned int)a2;
	}
}

types::Variant BigfxpDataType::add( const CustomDataValue& operand, const Variant& arg)
{
	const BigfxpDataValue* op = reinterpret_cast<const BigfxpDataValue*>(&operand);
	types::Variant rt( op->type(), op->initializer());
	BigfxpDataValue* res = reinterpret_cast<BigfxpDataValue*>( rt.data().value.Custom);
	if (arg.type() == types::Variant::Custom && arg.data().value.Custom->type() == op->type())
	{
		const BigfxpDataValue* bcdarg = reinterpret_cast<const BigfxpDataValue*>(arg.data().value.Custom);
		res->BigFxpBCD::operator=( op->operator+( *bcdarg));
	}
	else
	{
		BigFxpBCD argnum( arg.tostring());
		res->BigFxpBCD::operator=( op->BigFxpBCD::operator+( argnum));
	}
	return rt;
}

types::Variant BigfxpDataType::subtract( const CustomDataValue& operand, const Variant& arg)
{
	const BigfxpDataValue* op = reinterpret_cast<const BigfxpDataValue*>(&operand);
	types::Variant rt( op->type(), op->initializer());
	BigfxpDataValue* res = reinterpret_cast<BigfxpDataValue*>( rt.data().value.Custom);
	if (arg.type() == types::Variant::Custom && arg.data().value.Custom->type() == op->type())
	{
		const BigfxpDataValue* bcdarg = reinterpret_cast<const BigfxpDataValue*>(arg.data().value.Custom);
		res->BigFxpBCD::operator=( op->operator-( *bcdarg));
	}
	else
	{
		BigFxpBCD argnum( arg.tostring());
		res->BigFxpBCD::operator=( op->types::BigFxpBCD::operator-( argnum));
	}
	return rt;
}

types::Variant BigfxpDataType::multiply( const CustomDataValue& operand, const Variant& arg)
{
	const BigfxpDataValue* op = reinterpret_cast<const BigfxpDataValue*>(&operand);
	types::Variant rt( op->type(), op->initializer());
	BigfxpDataValue* res = reinterpret_cast<BigfxpDataValue*>( rt.data().value.Custom);
	if (arg.type() == types::Variant::Custom && arg.data().value.Custom->type() == op->type())
	{
		const BigfxpDataValue* bcdarg = reinterpret_cast<const BigfxpDataValue*>(arg.data().value.Custom);
		res->BigFxpBCD::operator=( op->operator*( *bcdarg));
	}
	else
	{
		BigFxpBCD argnum( arg.tostring());
		res->BigFxpBCD::operator=( op->types::BigFxpBCD::operator*( argnum));
	}
	return rt;
}

types::Variant BigfxpDataType::negation( const CustomDataValue& operand)
{
	const BigfxpDataValue* op = reinterpret_cast<const BigfxpDataValue*>(&operand);
	types::Variant rt( op->type(), op->initializer());
	BigfxpDataValue* res = reinterpret_cast<BigfxpDataValue*>( rt.data().value.Custom);
	res->BigFxpBCD::operator=( op->operator-());
	return rt;
}

types::Variant BigfxpDataType::toDouble( const CustomDataValue& operand)
{
	const BigfxpDataValue* op = reinterpret_cast<const BigfxpDataValue*>(&operand);
	return types::Variant( op->todouble());
}

types::Variant BigfxpDataType::divide( const CustomDataValue& operand, const std::vector<types::Variant>& arg)
{
	if (arg.size() < 2) throw std::runtime_error( "two arguments expected for divide (operand, additional scale)");
	if (arg.size() > 2) throw std::runtime_error( "too many arguments for format (two arguments: operand, additional scale)");

	const BigfxpDataValue* op = reinterpret_cast<const BigfxpDataValue*>(&operand);
	types::Variant rt( op->type(), op->initializer());
	BigfxpDataValue* res = reinterpret_cast<BigfxpDataValue*>( rt.data().value.Custom);
	if (arg[0].type() == types::Variant::Custom && arg[0].data().value.Custom->type() == op->type())
	{
		const BigfxpDataValue* bcdarg = reinterpret_cast<const BigfxpDataValue*>(arg[0].data().value.Custom);
		res->BigFxpBCD::operator=( op->divide( *bcdarg, arg[1].touint()));
	}
	else
	{
		BigFxpBCD argnum( arg[0].tostring());
		res->BigFxpBCD::operator=( op->types::BigFxpBCD::divide( argnum, arg[1].touint()));
	}
	return rt;
}

types::Variant BigfxpDataType::format( const CustomDataValue& operand, const std::vector<types::Variant>& arg)
{
	if (arg.size() < 1) throw std::runtime_error( "argument expected for format (precision)");
	if (arg.size() > 1) throw std::runtime_error( "too many arguments for format (one argument: precision)");
	const BigfxpDataValue* op = reinterpret_cast<const BigfxpDataValue*>(&operand);
	types::Variant rt( op->type(), op->initializer());
	BigfxpDataValue* res = reinterpret_cast<BigfxpDataValue*>( rt.data().value.Custom);
	res->BigFxpBCD::format( arg[0].touint());
	return rt;
}

types::Variant BigfxpDataType::round( const CustomDataValue& operand, const std::vector<types::Variant>& arg)
{
	if (arg.size() < 1) throw std::runtime_error( "argument expected for round (bcd fixed point number)");
	if (arg.size() > 1) throw std::runtime_error( "too many arguments for round (one argument: bcd fixed point number)");
	const BigfxpDataValue* op = reinterpret_cast<const BigfxpDataValue*>(&operand);
	types::Variant rt( op->type(), op->initializer());
	BigfxpDataValue* res = reinterpret_cast<BigfxpDataValue*>( rt.data().value.Custom);

	if (arg[0].type() == types::Variant::Custom && arg[0].data().value.Custom->type() == op->type())
	{
		const BigfxpDataValue* bcdarg = reinterpret_cast<const BigfxpDataValue*>(arg[0].data().value.Custom);
		res->BigFxpBCD::round( *bcdarg);
	}
	else
	{
		BigFxpBCD argnum( arg[0].tostring());
		res->BigFxpBCD::round( argnum);
	}
	return rt;
}

int BigfxpDataValue::compare( const CustomDataValue& o) const
{
	if (o.type() != type())
	{
		return ((uintptr_t)type() > (uintptr_t)o.type())?1:-1;
	}
	else
	{
		const BigfxpDataValue* odt = reinterpret_cast<const BigfxpDataValue*>(&o);
		return types::BigFxpBCD::compare(*odt);
	}
}

void BigfxpDataValue::assign( const Variant& o)
{
	switch (o.type())
	{
		case Variant::Null:
		case Variant::Timestamp:
		case Variant::Bool:
			throw std::runtime_error( std::string("cannot convert '") + o.typeName() + "' to big bcd fixed point number");

		case Variant::Custom:
		{
			const CustomDataValue* ref = o.customref();
			if (ref->type() != type())
			{
				BigFxpBCD argnum( o.tostring());
				types::BigFxpBCD::operator=( argnum);
			}
			else
			{
				const BigfxpDataValue* val = reinterpret_cast<const BigfxpDataValue*>(ref);
				types::BigFxpBCD::operator=( *val);
			}
			break;
		}
		case Variant::BigNumber:
			types::BigFxpBCD::operator = ( *o.bignumref());
			break;

		case Variant::Double:
			types::BigFxpBCD::operator=( o.todouble());
			break;

		case Variant::Int:
			types::BigFxpBCD::operator=( o.toint());
			break;

		case Variant::UInt:
			types::BigFxpBCD::operator=( o.touint());
			break;

		case Variant::String:
			types::BigFxpBCD::operator=( o.tostring());
			break;
	}
	unsigned int prec = (unsigned int)types::BigFxpBCD::nof_digits();
	unsigned int scal = (unsigned int)types::BigFxpBCD::scale();
	if (prec - scal > m_max_integer_digits)
	{
		throw std::runtime_error( "to many integer digits in bcd fixed point number");
	}
	if (scal > m_max_fractional_digits)
	{
		throw std::runtime_error( "to many fractional digits in bcd fixed point number");
	}
	if (m_max_fractional_digits != std::numeric_limits<unsigned int>::max())
	{
		BigFxpBCD::setScale( m_max_fractional_digits);
	}
}

bool BigfxpDataValue::getBaseTypeValue( Variant& dest) const
{
	try
	{
		dest = types::BigNumber( types::BigFxpBCD::tostring());
		return true;
	}
	catch (const std::runtime_error&)
	{
		return false;
	}
}





