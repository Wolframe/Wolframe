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
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
///\file types/dateTime.hpp
///\brief ADT Interface to date arithmetic functions
#ifndef _ADT_DATETIME_HPP_INCLUDED
#define _ADT_DATETIME_HPP_INCLUDED
#include "types/abstractDataType.hpp"
#include "types/dateArithmetic.hpp"
#include "types/variant.hpp"
#include <boost/algorithm/string.hpp>

namespace _Wolframe {
namespace types {

class DateDataInitializer
	:public AbstractDataInitializer
{
public:
	DateDataInitializer( const std::string& description_)
		:m_description( boost::trim_copy( description_))
	{
		m_format = m_description.c_str();
	}
	virtual ~DateDataInitializer(){}

	const char* format() const	{return m_format;}

	static AbstractDataInitializer* create( const std::string& description_)
	{
		return new DateDataInitializer( description_);
	}

private:
	std::string m_description;
	const char* m_format;
};


class DateDataValue
	:public AbstractDataValue
	,public types::Date
{
public:
	DateDataValue( unsigned short y, unsigned short m, unsigned short d)
		:Date(y,m,d){}

	DateDataValue( const std::string& dt)
		:Date(dt,format()){}

	///\brief Copy constructor
	DateDataValue( const DateDataValue& o)
		:Date(o){}

	DateDataValue()
		{}

	const char* format() const
	{
		return dynamic_cast<const DateDataInitializer*>(AbstractDataValue::initializer())->format();
	}

	virtual ~DateDataValue(){};

	virtual int compare( const AbstractDataValue& o) const
	{
		if (o.type() != type())
		{
			return ((uintptr_t)type() > (uintptr_t)o.type())?1:-1;
		}
		else
		{
			const DateDataValue* odt = dynamic_cast<const DateDataValue*>(&o);
			int rt = -1;
			rt += (int)(Date::operator==(*odt));
			rt += (int)(Date::operator>=(*odt));
			return rt;
		}
	}

	virtual std::string tostring() const
	{
		return Date::tostring( format());
	}

	virtual void assign( const Variant& o)
	{
		if (o.type() == Variant::String)
		{
			Date::operator=( Date( o.tostring()));
		}
		else
		{
			Date::operator=( Date(1970,1,1) + o.toint());
		}
	}

	static AbstractDataValue* create( const AbstractDataInitializer*)
	{
		return new DateDataValue();
	}

	static AbstractDataValue* copy( const AbstractDataValue* o)
	{
		const DateDataValue* odt = dynamic_cast<const DateDataValue*>(o);
		return new DateDataValue(*odt);
	}
};


class DateDataType
	:public AbstractDataType
	,public Date
{
public:
	DateDataType( const std::string& name_)
		:AbstractDataType(name_,&DateDataValue::create,&DateDataValue::copy,&DateDataInitializer::create)
	{
		define( Increment, &increment);
		define( Decrement, &decrement);
		define( Add, &add);
		define( Subtract, &subtract);
	}

	static AbstractDataType create( const std::string& name)
	{
		return DateDataType( name);
	}

private:
	static types::Variant increment( const AbstractDataValue& operand)
	{
		const DateDataValue* op = dynamic_cast<const DateDataValue*>(&operand);
		types::Variant rt( op->type(), op->initializer());
		DateDataValue* res = dynamic_cast<DateDataValue*>( rt.data().value.AdtRef);
		res->Date::operator=( dynamic_cast<const DateDataValue&>(operand).operator+( 1));
		return rt;
	}

	static types::Variant decrement( const AbstractDataValue& operand)
	{
		const DateDataValue* op = dynamic_cast<const DateDataValue*>(&operand);
		types::Variant rt( op->type(), op->initializer());
		DateDataValue* res = dynamic_cast<DateDataValue*>( rt.data().value.AdtRef);
		res->Date::operator=( dynamic_cast<const DateDataValue&>(operand).operator-( 1));
		return rt;
	}

	static types::Variant add( const AbstractDataValue& operand, const Variant& arg)
	{
		const DateDataValue* op = dynamic_cast<const DateDataValue*>(&operand);
		types::Variant rt( op->type(), op->initializer());
		DateDataValue* res = dynamic_cast<DateDataValue*>( rt.data().value.AdtRef);
		res->Date::operator=( dynamic_cast<const DateDataValue&>(operand).operator - ( arg.toint()));
		return rt;
	}

	static types::Variant subtract( const AbstractDataValue& operand, const Variant& arg)
	{
		if (arg.type() == types::Variant::ADT)
		{
			if (arg.data().value.AdtRef->type() == operand.type())
			{
				long daydiff = dynamic_cast<const DateDataValue&>(operand).operator-( *(const Date*)dynamic_cast<const DateDataValue*>(arg.data().value.AdtRef));
				return types::Variant( daydiff);
			}
			else
			{
				throw std::runtime_error("illegal argument for date subtraction");
			}
		}
		else 
		{
			const DateDataValue* op = dynamic_cast<const DateDataValue*>(&operand);
			types::Variant rt( op->type(), op->initializer());
			DateDataValue* res = dynamic_cast<DateDataValue*>( rt.data().value.AdtRef);
			res->Date::operator=( dynamic_cast<const DateDataValue&>(operand).operator - ( arg.toint()));
			return rt;
		}
	}
};

}}//namespace
#endif





