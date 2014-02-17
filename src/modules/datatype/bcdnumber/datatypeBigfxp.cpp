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

using namespace _Wolframe;
using namespace _Wolframe::types;


BigfxpDataInitializer::BigfxpDataInitializer( const std::string& description)
	:m_show_precision(0),m_calc_precision(0)
{
	std::string::const_iterator di = description.begin(), de = description.end();
	for (; di != de && *di >= '0' && *di <= '9'; ++di)
	{
		m_show_precision = (m_show_precision * 10) + *di - '0';
	}
	if (di == de) return;
	if (*di != '+')
	{
		throw std::runtime_error("illegal syntax of initializer description (<show-precision>+<calc-precision>)");
	}
	m_calc_precision = m_show_precision;
	for (; di != de && *di >= '0' && *di <= '9'; ++di)
	{
		m_calc_precision = (m_calc_precision * 10) + *di - '0';
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
		res->BigNumber::init( op->operator+( *bcdarg));
	}
	else
	{
		res->BigNumber::init( op->types::BigNumber::operator+( arg.toint()));
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
		res->BigNumber::init( op->operator-( *bcdarg));
	}
	else
	{
		res->BigNumber::init( op->operator-( arg.toint()));
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
		res->BigNumber::init( op->operator*( *bcdarg));
	}
	else
	{
		res->BigNumber::init( op->operator*( arg.toint()));
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
		res->BigNumber::init( op->operator/( *bcdarg));
	}
	else
	{
		res->BigNumber::init( op->operator/( arg.toint()));
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


