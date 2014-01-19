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
///\brief Custom data type interface to date arithmetic functions
#ifndef _CUSTOM_DATA_TYPE_DATETIME_HPP_INCLUDED
#define _CUSTOM_DATA_TYPE_DATETIME_HPP_INCLUDED
#include "types/customDataType.hpp"
#include "types/dateArithmetic.hpp"
#include "types/variant.hpp"
#include <boost/algorithm/string.hpp>

namespace _Wolframe {
namespace types {

class DateDataInitializer
	:public CustomDataInitializer
{
public:
	DateDataInitializer( const std::string& description_)
		:m_description( boost::trim_copy( description_))
	{
		m_format = m_description.c_str();
	}
	virtual ~DateDataInitializer(){}

	const char* format() const	{return m_format;}

	static CustomDataInitializer* create( const std::string& description_)
	{
		return new DateDataInitializer( description_);
	}

private:
	std::string m_description;
	const char* m_format;
};


class DateDataValue
	:public CustomDataValue
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
		return dynamic_cast<const DateDataInitializer*>(CustomDataValue::initializer())->format();
	}

	virtual ~DateDataValue(){};

	virtual int compare( const CustomDataValue& o) const
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

	static CustomDataValue* create( const CustomDataInitializer*)
	{
		return new DateDataValue();
	}

	static CustomDataValue* copy( const CustomDataValue* o)
	{
		const DateDataValue* odt = dynamic_cast<const DateDataValue*>(o);
		return new DateDataValue(*odt);
	}
};


class DateDataType
	:public CustomDataType
	,public Date
{
public:
	DateDataType( const std::string& name_)
		:CustomDataType(name_,&DateDataValue::create,&DateDataValue::copy,&DateDataInitializer::create)
	{
		define( Increment, &increment);
		define( Decrement, &decrement);
		define( Add, &add);
		define( Subtract, &subtract);
	}

	static CustomDataType create( const std::string& name)
	{
		return DateDataType( name);
	}

private:
	static types::Variant increment( const CustomDataValue& operand)
	{
		const DateDataValue* op = dynamic_cast<const DateDataValue*>(&operand);
		types::Variant rt( op->type(), op->initializer());
		DateDataValue* res = dynamic_cast<DateDataValue*>( rt.data().value.CustomRef);
		res->Date::operator=( dynamic_cast<const DateDataValue&>(operand).operator+( 1));
		return rt;
	}

	static types::Variant decrement( const CustomDataValue& operand)
	{
		const DateDataValue* op = dynamic_cast<const DateDataValue*>(&operand);
		types::Variant rt( op->type(), op->initializer());
		DateDataValue* res = dynamic_cast<DateDataValue*>( rt.data().value.CustomRef);
		res->Date::operator=( dynamic_cast<const DateDataValue&>(operand).operator-( 1));
		return rt;
	}

	static types::Variant add( const CustomDataValue& operand, const Variant& arg)
	{
		const DateDataValue* op = dynamic_cast<const DateDataValue*>(&operand);
		types::Variant rt( op->type(), op->initializer());
		DateDataValue* res = dynamic_cast<DateDataValue*>( rt.data().value.CustomRef);
		res->Date::operator=( dynamic_cast<const DateDataValue&>(operand).operator - ( arg.toint()));
		return rt;
	}

	static types::Variant subtract( const CustomDataValue& operand, const Variant& arg)
	{
		if (arg.type() == types::Variant::Custom)
		{
			if (arg.data().value.CustomRef->type() == operand.type())
			{
				types::Variant::Data::Int daydiff = dynamic_cast<const DateDataValue&>(operand).operator-( *(const Date*)dynamic_cast<const DateDataValue*>(arg.data().value.CustomRef));
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
			DateDataValue* res = dynamic_cast<DateDataValue*>( rt.data().value.CustomRef);
			res->Date::operator=( dynamic_cast<const DateDataValue&>(operand).operator - ( arg.toint()));
			return rt;
		}
	}
};

}}//namespace
#endif





