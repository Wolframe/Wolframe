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
#include "types/variant.hpp"
#include "types/customDataType.hpp"
#include "types/string.hpp"
#include "types/datetime.hpp"
#include "types/bignumber.hpp"
#include "types/conversions.hpp"
#include <cstdlib>
#include <limits>
#include <stdexcept>
#include <boost/lexical_cast.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/utility/enable_if.hpp>

using namespace _Wolframe;
using namespace _Wolframe::types;

void Variant::init( Type type_)
{
	static const types::BigNumber bignuminit;
	static const char strinit[1] = "";
	std::memset( this, 0, sizeof( *this));
	m_type = (unsigned char)type_;
	switch (m_type)
	{
		case Null:
		case Int:
		case UInt:
		case Bool: return;
		case Double: m_data.value.Double = 0.0; return;
		case String: m_data.value.String = const_cast<char*>(strinit); setConstant(); return;
		case Timestamp: m_data.value.Timestamp = 0; return;
		case BigNumber: m_data.value.BigNumberRef = const_cast<types::BigNumber*>(&bignuminit); setConstant(); return;
		case Custom: throw std::runtime_error("cannot initialize custom data type without instance defined");
	}
	throw std::logic_error( "invalid initialzation of atomic type (from structure)");
}

void Variant::init()
{
	std::memset( this, 0, sizeof( *this));
}

void Variant::release()
{
	if (!constant())
	{
		if (m_type == String && m_data.value.String)
		{
			std::free( m_data.value.String);
			std::memset( this, 0, sizeof( *this));
		}
		else if (m_type == Custom && m_data.value.Custom)
		{
			delete m_data.value.Custom;
			std::memset( this, 0, sizeof( *this));
		}
		else if (m_type == BigNumber && m_data.value.BigNumberRef)
		{
			delete m_data.value.BigNumberRef;
			std::memset( this, 0, sizeof( *this));
		}
		else if (!atomic())
		{
			throw std::logic_error( "invalid free of variant (structure)");
		}
	}
}

void Variant::initConstant( const types::DateTime& o)
{
	bool init_ = initialized();
	release();
	std::memset( this, 0, sizeof( *this));
	m_type = (unsigned char)Timestamp;
	m_data.value.Timestamp = o.timestamp();
	setInitialized(init_);
	setConstant();
}

void Variant::initConstant( const types::CustomDataValue& o)
{
	bool init_ = initialized();
	release();
	std::memset( this, 0, sizeof( *this));
	m_type = (unsigned char)Custom;
	m_data.value.Custom = const_cast<types::CustomDataValue*>(&o);
	setInitialized(init_);
	setConstant();
}

void Variant::initConstant( const types::BigNumber& o)
{
	bool init_ = initialized();
	release();
	std::memset( this, 0, sizeof( *this));
	m_type = (unsigned char)BigNumber;
	m_data.value.BigNumberRef = const_cast<types::BigNumber*>(&o);
	setInitialized(init_);
	setConstant();
}

void Variant::initConstant( const char* o, std::size_t l)
{
	bool init_ = initialized();
	release();
	init( String);
	m_data.dim.size = l;
	m_data.value.String = const_cast<char*>(o);
	setInitialized(init_);
	setConstant();
}

void Variant::initString( const char* str_, std::size_t strsize_)
{
	std::memset( this, 0, sizeof( *this));
	m_type = String;
	m_data.dim.size = strsize_;
	m_data.value.String = (char*)std::malloc( strsize_+1);
	if (!m_data.value.String) throw std::bad_alloc();
	std::memcpy( m_data.value.String, str_, strsize_);
	m_data.value.String[ strsize_] = 0;
}

void Variant::initCustom( const types::CustomDataType* typ, const types::CustomDataInitializer* dsc)
{
	std::memset( this, 0, sizeof( *this));
	m_type = Custom;
	m_data.value.Custom = typ->createValue( dsc);
}

void Variant::initBigNumber( const types::BigNumber& num)
{
	std::memset( this, 0, sizeof( *this));
	m_type = BigNumber;
	m_data.value.BigNumberRef = new types::BigNumber( num);
}

void Variant::initDateTime( const DateTime& dt)
{
	std::memset( this, 0, sizeof( *this));
	m_type = Timestamp;
	m_data.value.Timestamp = dt.timestamp();
}

void Variant::initCustom( const types::CustomDataValue& o)
{
	std::memset( this, 0, sizeof( *this));
	m_type = Custom;
	m_data.value.Custom = o.copy();
}

void Variant::initCopy( const Variant& o)
{
	if (o.m_type == String)
	{
		initString( o.m_data.value.String, o.m_data.dim.size);
		setInitialized( o.initialized());
	}
	else if (o.m_type == Custom)
	{
		initCustom( *o.m_data.value.Custom);
		setInitialized( o.initialized());
	}
	else if (!o.atomic())
	{
		throw std::logic_error( "illegal copy operation of non atomic type (source)");
	}
	else
	{
		std::memcpy( this, &o, sizeof( *this));
		setConstant( false);
	}
}

void Variant::initConstCopy( const Variant& o)
{
	std::memcpy( this, &o, sizeof(*this));
	setConstant();
}

static int compare_double( double d1, double d2)
{
	if (d1 > d2 + std::numeric_limits<double>::epsilon()) return 1;
	if ((d1 - d2) <= std::numeric_limits<double>::epsilon() && (d1 - d2) >= -std::numeric_limits<double>::epsilon()) return 0;
	return - 1;
}

template <typename TYPE>
static int compare_int( TYPE d1, TYPE d2)
{
	return (int)(d1 >= d2) + (int)(d1 > d2) -1;
}

static int compare_bool( bool d1, bool d2)
{
	return compare_int( (int)d1, (int)d2);
}

static int compare_type( Variant::Type type, const Variant::Data& d1, const Variant::Data& d2)
{
	switch (type)
	{
		case Variant::Null:
		{
			int cmp = std::memcmp( &d1, &d2, sizeof(d1));
			return (cmp < 0)?-1:((cmp==0)?0:+1);
		}
		case Variant::Bool:	return compare_int( (int)d1.value.Bool, (int)d2.value.Bool);
		case Variant::Double:	return compare_double( d1.value.Double, d2.value.Double);
		case Variant::Int:	return compare_int( d1.value.Int, d2.value.Int);
		case Variant::UInt:	return compare_int( d1.value.UInt, d2.value.UInt);
		case Variant::String:
			if (d1.dim.size != d2.dim.size)
			{
				return (d1.dim.size < d2.dim.size)?-1:+1;
			}
			else
			{
				return std::memcmp( d1.value.String, d2.value.String, d2.dim.size);
			}
		case Variant::Custom:
			return d1.value.Custom->compare( *d2.value.Custom);
		case Variant::Timestamp:
			return compare_int( d1.value.Timestamp, d2.value.Timestamp);
		case Variant::BigNumber:
			return d1.value.BigNumberRef->compare( *d2.value.BigNumberRef);
	}
	return -2;
}

static Variant::Data::Timestamp variant2timestamp_cast( const Variant& o)
{
	switch (o.type())
	{
		case Variant::Null:
			throw std::logic_error( "cannot cast NULL type to timestamp type");
		case Variant::Bool:
			throw std::logic_error( "cannot cast Bool type to timestamp type");
		case Variant::Double:
			throw std::logic_error( "cannot cast double precision floating point number type to timestamp type");
		case Variant::Int:
			throw std::logic_error( "cannot cast signed integer type to timestamp type");
		case Variant::UInt:
			throw std::logic_error( "cannot cast unsigned integer type to timestamp type");
		case Variant::BigNumber:
			throw std::logic_error( "cannot cast big number type to timestamp type");
		case Variant::String:
			return DateTime( o.charptr(), o.charsize()).timestamp();
		case Variant::Timestamp:
			return o.data().value.Timestamp;
		case Variant::Custom:
		{
			Variant val;
			try
			{
				o.customref()->getBaseTypeValue( val);
				if (val.type() != Variant::Custom)
				{
					return variant2timestamp_cast( val);
				}
			}
			catch (const std::runtime_error& e)
			{
				throw std::logic_error( std::string("cannot cast custom data type to timestamp type: ") + e.what());
			}
			throw std::logic_error( "cannot cast custom data type to timestamp type");
		}
	}
	throw boost::bad_lexical_cast();
}

static BigNumber variant2bignumber_cast( const Variant& o)
{
	switch (o.type())
	{
		case Variant::Null:
			throw std::logic_error( "cannot cast NULL type to big number type");
		case Variant::Bool:
			return BigNumber( (Variant::Data::UInt)(o.data().value.Bool?1:0));
		case Variant::Double:
			return BigNumber( o.data().value.Double);
		case Variant::Int:
			return BigNumber( o.data().value.Int);
		case Variant::UInt:
			return BigNumber( o.data().value.UInt);
		case Variant::BigNumber:
			return *o.data().value.BigNumberRef;
		case Variant::Timestamp:
			throw std::logic_error( "cannot cast timestamp type to big number type");
		case Variant::String:
			return BigNumber( o.data().value.String, o.data().dim.size);
		case Variant::Custom:
		{
			Variant val;
			try
			{
				o.customref()->getBaseTypeValue( val);
				if (val.type() != Variant::Custom)
				{
					return variant2bignumber_cast( val);
				}
			}
			catch (const std::runtime_error& e)
			{
				throw std::logic_error( std::string("cannot cast custom data type to big number type: ") + e.what());
			}
			throw std::logic_error( "cannot cast custom data type to big number type");
		}
	}
	throw boost::bad_lexical_cast();
}

static Variant::Data::UInt variant2uint_cast( const Variant& o)
{
	switch (o.type())
	{
		case Variant::Null:
			throw std::logic_error( "cannot cast NULL type to unsigned integer type");
		case Variant::Bool:
			return o.data().value.Bool?1:0;
		case Variant::Double:
			return boost::numeric_cast<Variant::Data::UInt>( o.data().value.Double);
		case Variant::Int:
			if (o.data().value.Int < 0) throw boost::bad_lexical_cast();
			return o.data().value.Int;
		case Variant::UInt:
			return o.data().value.UInt;
		case Variant::BigNumber:
			return o.data().value.BigNumberRef->touint();
		case Variant::Timestamp:
			throw std::logic_error( "cannot cast timestamp type to unsigned integer type");
		case Variant::String:
			return touint_cast( std::string( o.data().value.String));
		case Variant::Custom:
		{
			Variant val;
			try
			{
				o.customref()->getBaseTypeValue( val);
				if (val.type() != Variant::Custom)
				{
					return variant2uint_cast( val);
				}
			}
			catch (const std::runtime_error& e)
			{
				throw std::logic_error( std::string("cannot cast custom data type to unsigned integer type: ") + e.what());
			}
			throw std::logic_error( "cannot cast custom data type to unsigned integer type");
		}
	}
	throw boost::bad_lexical_cast();
}

static Variant::Data::Int variant2int_cast( const Variant& o)
{
	switch (o.type())
	{
		case Variant::Null:
			throw std::logic_error( "cannot cast NULL type to signed integer type");
		case Variant::Bool:
			return o.data().value.Bool?1:0;
		case Variant::Double:
			return boost::numeric_cast<Variant::Data::Int>( o.data().value.Double);
		case Variant::Int:
			return o.data().value.Int;
		case Variant::UInt:
			if (o.data().value.UInt > (Variant::Data::UInt)std::numeric_limits<Variant::Data::Int>::max()) throw boost::bad_lexical_cast();
			return o.data().value.UInt;
		case Variant::BigNumber:
			return o.data().value.BigNumberRef->toint();
		case Variant::Timestamp:
			throw std::logic_error( "cannot cast timestamp type to signed integer type");
		case Variant::String:
			return toint_cast( std::string( o.data().value.String));
		case Variant::Custom:
		{
			Variant val;
			try
			{
				o.customref()->getBaseTypeValue( val);
				if (val.type() != Variant::Custom)
				{
					return variant2int_cast( val);
				}
			}
			catch (const std::runtime_error& e)
			{
				throw std::logic_error( std::string("cannot cast custom data type to signed integer type: ") + e.what());
			}
			throw std::logic_error( "cannot cast custom data type to signed integer type");
		}
	}
	throw boost::bad_lexical_cast();
}

template <typename TYPE>
static typename boost::enable_if_c<boost::is_same<TYPE,double>::value,TYPE>::type variant_cast( const Variant& o)
{
	switch (o.type())
	{
		case Variant::Null:
			throw boost::bad_lexical_cast();
		case Variant::Bool:
			return boost::numeric_cast<double>( o.data().value.Bool);
		case Variant::Double:
			return boost::numeric_cast<double>( o.data().value.Double);
		case Variant::Int:
			return boost::numeric_cast<double>( o.data().value.Int);
		case Variant::UInt:
			return boost::numeric_cast<double>( o.data().value.UInt);
		case Variant::String:
			return boost::lexical_cast<double>( std::string( o.data().value.String));
		case Variant::BigNumber:
			return o.data().value.BigNumberRef->todouble();
		case Variant::Timestamp:
			throw std::logic_error( "cannot cast timestamp type to double precision number type");
		case Variant::Custom:
		{
			Variant alt;
			try
			{
				o.customref()->getBaseTypeValue( alt);
				if (alt.type() != Variant::Custom)
				{
					return variant_cast<double>( alt);
				}
			}
			catch (const std::runtime_error& e)
			{
				throw std::logic_error( std::string("cannot cast custom data type to timestamp type: ") + e.what());
			}
			throw std::logic_error( "cannot cast custom data type to double precision number type");
		}
	}
	throw boost::bad_lexical_cast();
}

template <typename TYPE>
static typename boost::enable_if_c<boost::is_same<TYPE,std::string>::value,TYPE>::type variant_cast( const Variant& o)
{
	switch (o.type())
	{
		case Variant::Null:
			return std::string();
		case Variant::Bool:
			return o.data().value.Bool?"true":"false";
		case Variant::Double:
			return boost::lexical_cast<std::string>( o.data().value.Double);
		case Variant::Int:
			return tostring_cast( o.data().value.Int);
		case Variant::UInt:
			return tostring_cast( o.data().value.UInt);
		case Variant::String:
			return std::string( o.data().value.String, o.data().dim.size);
		case Variant::Custom:
			return o.data().value.Custom->tostring();
		case Variant::BigNumber:
			return o.data().value.BigNumberRef->tostring();
		case Variant::Timestamp:
			return DateTime( o.data().value.Timestamp).tostring();
	}
	throw boost::bad_lexical_cast();
}

int Variant::compare( const Variant& o) const
{
	if (m_type == o.m_type)
	{
		return compare_type( (Type)m_type, m_data, o.m_data);
	}
	if (m_type > o.m_type)
	{
		return -o.compare( *this);
	}
	try
	{
		switch (m_type)
		{
			case Variant::Null:
				return -1;
			case Variant::Custom:
			{
				const CustomDataInitializer* ini = m_data.value.Custom->initializer();
				CustomDataValueR val( m_data.value.Custom->type()->createValue( ini));
				val->assign( o);
				return m_data.value.Custom->compare( *val);
			}
			case Variant::Timestamp:
				return compare_int( m_data.value.Timestamp, o.totimestamp());
			case Variant::BigNumber:
				return m_data.value.BigNumberRef->compare( variant2bignumber_cast(o));
			case Variant::Double:
				return compare_double( variant_cast<double>( o), m_data.value.Double);
			case Variant::Int:
				if (o.type() == UInt && o.m_data.value.UInt > (Data::UInt)std::numeric_limits<Data::Int>::max()) return -1;
				return compare_int( variant2int_cast( o), m_data.value.Int);
			case Variant::UInt:
				return compare_int( variant2uint_cast( o), m_data.value.UInt);
			case Variant::Bool:
				return compare_bool( variant2int_cast( o) != 0, m_data.value.Bool);
			case Variant::String:
				throw std::logic_error("illegal state in Variant::compare (string has lowest order)");
		}
	}
	catch (std::bad_alloc& e)
	{
		throw e;
	}
	catch (...)
	{
	}
	return -2;
}

std::string Variant::tostring() const
{
	return variant_cast<std::string>( *this);
}

std::wstring Variant::towstring() const
{
	if (m_type == Variant::String)
	{
		return StringConst( m_data.value.String, m_data.dim.size).towstring();
	}
	else
	{
		std::string str = tostring();
		return StringConst( str).towstring();
	}
}

double Variant::todouble() const
{
	return variant_cast<double>( *this);
}

bool Variant::tobool() const
{
	return (variant2int_cast( *this) != 0);
}

Variant::Data::Int Variant::toint() const
{
	return variant2int_cast( *this);
}

Variant::Data::UInt Variant::touint() const
{
	return variant2uint_cast( *this);
}

Variant::Data::Timestamp Variant::totimestamp() const
{
	return variant2timestamp_cast( *this);
}

void Variant::move( Variant& o)
{
	bool init_ = initialized();
	release();
	std::memcpy( this, &o, sizeof(o));
	setInitialized( init_);
	o.init();
}

void Variant::convert( Type type_)
{
	if (m_type == type_) return;
	assign( type_, *this);
}

void Variant::assign( Type type_, const Variant& o)
{
	switch (type_)
	{
		case Null: release(); init(); return;
		case Custom: throw std::runtime_error( "cannot convert to unspecified custom data type");
		case Bool: *this = o.tobool(); return;
		case Int: *this = o.toint(); return;
		case UInt: *this = o.touint(); return;
		case Double: *this = o.todouble(); return;
		case String: {Variant rt = o.tostring(); move(rt); return;}
		case BigNumber: {*this = variant2bignumber_cast(o);}
		case Timestamp: {*this = variant2timestamp_cast(o);}
			
	}
	throw std::runtime_error( "illegal conversion of atomic type");
}

std::ostream& std::operator << (std::ostream &os, const _Wolframe::types::Variant& o)
{
	return os << o.tostring();
}

