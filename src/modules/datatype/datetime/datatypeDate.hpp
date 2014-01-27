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
///\file datatypeDate.hpp
///\brief Custom data type interface to date arithmetic functions
#ifndef _CUSTOM_DATA_TYPE_DATETIME_HPP_INCLUDED
#define _CUSTOM_DATA_TYPE_DATETIME_HPP_INCLUDED
#include "types/customDataType.hpp"
#include "types/variant.hpp"
#include "dateArithmetic.hpp"

namespace _Wolframe {
namespace types {

class DateDataInitializer
	:public CustomDataInitializer
{
public:
	DateDataInitializer( const std::string& description_)
		:m_description( description_)
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

	explicit DateDataValue( const std::string& dt, const DateDataInitializer* ini=0)
		:Date(dt,ini?ini->format():0){}

	///\brief Copy constructor
	DateDataValue( const DateDataValue& o)
		:CustomDataValue(o),Date(o){}

	DateDataValue(){}

	const char* format() const
	{
		if (initializer())
		{
			return reinterpret_cast<const DateDataInitializer*>(CustomDataValue::initializer())->format();
		}
		else
		{
			return 0;
		}
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
			const DateDataValue* odt = reinterpret_cast<const DateDataValue*>(&o);
			int rt = -1;
			rt += (int)(Date::operator>=(*odt));
			rt += (int)(Date::operator>(*odt));
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

	virtual CustomDataValue* copy() const
	{
		return new DateDataValue(*this);
	}

	static CustomDataValue* create( const CustomDataInitializer*)
	{
		return new DateDataValue();
	}
};


class DateDataType
	:public CustomDataType
	,public Date
{
public:
	DateDataType( const std::string& name_)
		:CustomDataType(name_,&DateDataValue::create,&DateDataInitializer::create)
	{
		define( Increment, &increment);
		define( Decrement, &decrement);
		define( Add, &add);
		define( Subtract, &subtract);
		define( ToTimestamp, &toTimestamp);
		define( ToInt, &toInt);
	}

	static CustomDataType* create( const std::string& name)
	{
		return new DateDataType( name);
	}

private:
	static types::Variant increment( const CustomDataValue& operand);
	static types::Variant decrement( const CustomDataValue& operand);
	static types::Variant add( const CustomDataValue& operand, const Variant& arg);
	static types::Variant subtract( const CustomDataValue& operand, const Variant& arg);
	static types::Variant toTimestamp( const CustomDataValue& operand);
	static types::Variant toInt( const CustomDataValue& operand);
};

}}//namespace
#endif





