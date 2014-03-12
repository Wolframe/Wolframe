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
	:m_show_precision(0),m_calc_precision(0)
{
	if (arg.size() > 2) throw std::runtime_error("too many arguments for big fixed point number initializer");
	if (arg.size() == 0) return;

	types::Variant::Data::Int pr = arg.at(0).toint();
	if (pr >= std::numeric_limits<unsigned int>::max()) throw std::runtime_error("precision argument out of range for big fixed point number initializer");
	m_show_precision = (unsigned int)pr;
	if (arg.size() == 1)
	{
		m_calc_precision = m_show_precision;
	}
	else
	{
		pr = arg.at(1).toint();
		if (pr >= std::numeric_limits<unsigned int>::max()) throw std::runtime_error("precision argument out of range for big fixed point number initializer");
		if (pr + m_show_precision  >= std::numeric_limits<unsigned int>::max()) throw std::runtime_error("precision argument out of range for big fixed point number initializer");
		m_calc_precision = (unsigned int)(pr + m_show_precision);
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
		res->BigFxpBCD::init( op->operator+( *bcdarg));
	}
	else
	{
		res->BigFxpBCD::init( op->types::BigFxpBCD::operator+( arg.toint()));
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
		res->BigFxpBCD::init( op->operator-( *bcdarg));
	}
	else
	{
		res->BigFxpBCD::init( op->operator-( arg.toint()));
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
		res->BigFxpBCD::init( op->operator*( *bcdarg));
	}
	else
	{
		res->BigFxpBCD::init( op->operator*( arg.toint()));
	}
	return rt;
}

types::Variant BigfxpDataType::divide( const CustomDataValue& operand, const Variant& arg)
{
	const BigfxpDataValue* op = reinterpret_cast<const BigfxpDataValue*>(&operand);
	types::Variant rt( op->type(), op->initializer());
	BigfxpDataValue* res = reinterpret_cast<BigfxpDataValue*>( rt.data().value.Custom);
	if (arg.type() == types::Variant::Custom && arg.data().value.Custom->type() == op->type())
	{
		const BigfxpDataValue* bcdarg = reinterpret_cast<const BigfxpDataValue*>(arg.data().value.Custom);
		res->BigFxpBCD::init( op->operator/( *bcdarg));
	}
	else
	{
		res->BigFxpBCD::init( op->operator/( arg.toint()));
	}
	return rt;
}


types::Variant BigfxpDataType::negation( const CustomDataValue& operand)
{
	const BigfxpDataValue* op = reinterpret_cast<const BigfxpDataValue*>(&operand);
	types::Variant rt( op->type(), op->initializer());
	BigfxpDataValue* res = reinterpret_cast<BigfxpDataValue*>( rt.data().value.Custom);
	res->BigBCD::operator=( op->operator-());
	return rt;
}

types::Variant BigfxpDataType::toDouble( const CustomDataValue& operand)
{
	const BigfxpDataValue* op = reinterpret_cast<const BigfxpDataValue*>(&operand);
	return types::Variant( op->todouble());
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
				throw std::runtime_error( std::string("cannot convert '") + o.typeName() + "' to big bcd fixed point number");
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





