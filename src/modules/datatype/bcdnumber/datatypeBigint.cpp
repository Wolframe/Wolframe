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
///\file datatypeBigint.cpp
#include "datatypeBigint.hpp"

using namespace _Wolframe;
using namespace _Wolframe::types;

types::Variant BigintDataType::add( const CustomDataValue& operand_, const Variant& arg)
{
	const BigintDataValue* op = reinterpret_cast<const BigintDataValue*>(&operand_);
	types::Variant rt( op->type(), op->initializer());
	BigintDataValue* res = reinterpret_cast<BigintDataValue*>( rt.data().value.Custom);
	if (arg.type() == types::Variant::Custom && arg.data().value.Custom->type() == op->type())
	{
		const BigintDataValue* bcdarg = reinterpret_cast<const BigintDataValue*>(arg.data().value.Custom);
		res->BigBCD::init( op->operator+( *bcdarg));
	}
	else
	{
		res->BigBCD::init( op->operator+( arg.toint()));
	}
	return rt;
}

types::Variant BigintDataType::subtract( const CustomDataValue& operand, const Variant& arg)
{
	const BigintDataValue* op = reinterpret_cast<const BigintDataValue*>(&operand);
	types::Variant rt( op->type(), op->initializer());
	BigintDataValue* res = reinterpret_cast<BigintDataValue*>( rt.data().value.Custom);
	if (arg.type() == types::Variant::Custom && arg.data().value.Custom->type() == op->type())
	{
		const BigintDataValue* bcdarg = reinterpret_cast<const BigintDataValue*>(arg.data().value.Custom);
		res->BigBCD::init( op->operator-( *bcdarg));
	}
	else
	{
		res->BigBCD::init( op->operator-( arg.toint()));
	}
	return rt;
}

types::Variant BigintDataType::multiply( const CustomDataValue& operand, const Variant& arg)
{
	const BigintDataValue* op = reinterpret_cast<const BigintDataValue*>(&operand);
	types::Variant rt( op->type(), op->initializer());
	BigintDataValue* res = reinterpret_cast<BigintDataValue*>( rt.data().value.Custom);
	if (arg.type() == types::Variant::Custom && arg.data().value.Custom->type() == op->type())
	{
		const BigintDataValue* bcdarg = reinterpret_cast<const BigintDataValue*>(arg.data().value.Custom);
		res->BigBCD::init( op->operator*( *bcdarg));
	}
	else
	{
		res->BigBCD::init( op->operator*( arg.toint()));
	}
	return rt;
}

types::Variant BigintDataType::divide( const CustomDataValue& operand, const Variant& arg)
{
	const BigintDataValue* op = reinterpret_cast<const BigintDataValue*>(&operand);
	types::Variant rt( op->type(), op->initializer());
	BigintDataValue* res = reinterpret_cast<BigintDataValue*>( rt.data().value.Custom);
	if (arg.type() == types::Variant::Custom && arg.data().value.Custom->type() == op->type())
	{
		const BigintDataValue* bcdarg = reinterpret_cast<const BigintDataValue*>(arg.data().value.Custom);
		res->BigBCD::init( op->operator/( *bcdarg));
	}
	else
	{
		res->BigBCD::init( op->operator/( arg.toint()));
	}
	return rt;
}


types::Variant BigintDataType::negation( const CustomDataValue& operand)
{
	const BigintDataValue* op = reinterpret_cast<const BigintDataValue*>(&operand);
	types::Variant rt( op->type(), op->initializer());
	BigintDataValue* res = reinterpret_cast<BigintDataValue*>( rt.data().value.Custom);
	res->BigBCD::operator=( op->operator-());
	return rt;
}

types::Variant BigintDataType::toInt( const CustomDataValue& operand)
{
	const BigintDataValue* op = reinterpret_cast<const BigintDataValue*>(&operand);
	return types::Variant( op->toint());
}

types::Variant BigintDataType::toDouble( const CustomDataValue& operand)
{
	const BigintDataValue* op = reinterpret_cast<const BigintDataValue*>(&operand);
	return types::Variant( op->todouble());
}

int BigintDataValue::compare( const CustomDataValue& o) const
{
	if (o.type() != type())
	{
		return ((uintptr_t)type() > (uintptr_t)o.type())?1:-1;
	}
	else
	{
		const BigintDataValue* odt = reinterpret_cast<const BigintDataValue*>(&o);
		return types::BigBCD::compare(*odt);
	}
}

void BigintDataValue::getBaseTypeValue( Variant& dest) const
{
	dest = types::BigNumber( types::BigBCD::tostring());
}

void BigintDataValue::assign( const Variant& o)
{
	switch (o.type())
	{
		case Variant::Null:
		case Variant::Timestamp:
		case Variant::Bool:
			throw std::runtime_error( std::string("cannot convert '") + o.typeName() + "' to big bcd integer");

		case Variant::Custom:
		{
			const CustomDataValue* ref = o.customref();
			if (ref->type() != type())
			{
				throw std::runtime_error( std::string("cannot convert '") + o.typeName() + "' to big bcd integer");
			}
			else
			{
				const BigintDataValue* val = reinterpret_cast<const BigintDataValue*>(ref);
				types::BigBCD::init( *val);
			}
			break;
		}
		case Variant::BigNumber:
			types::BigBCD::init( *o.bignumref());
			break;

		case Variant::Double:
		case Variant::Int:
			types::BigBCD::init( o.toint());
			break;

		case Variant::UInt:
			types::BigBCD::init( o.touint());
			break;

		case Variant::String:
			types::BigBCD::init( o.charptr(), o.charsize());
			break;
	}
}
