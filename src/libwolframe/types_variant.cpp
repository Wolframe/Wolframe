#include "types/variant.hpp"
#include "types/malloc.hpp"
#include "utils/conversions.hpp"
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
	static char strinit[1] = "";
	std::memset( this, 0, sizeof( *this));
	m_type = type_;
	switch (m_type)
	{
		case Null:
		case Int:
		case UInt:
		case Bool: return;
		case Double: m_data.value.Double = 0.0; return;
		case String: m_data.value.String = strinit; setConstant(); return;
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
			wolframe_free( m_data.value.String);
			std::memset( this, 0, sizeof( *this));
		}
		else if (!atomic())
		{
			throw std::logic_error( "invalid free of variant (structure)");
		}
	}
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
	m_data.value.String = (char*)wolframe_malloc( strsize_+1);
	if (!m_data.value.String) throw std::bad_alloc();
	std::memcpy( m_data.value.String, str_, strsize_);
	m_data.value.String[ strsize_] = 0;
}

void Variant::initCopy( const Variant& o)
{
	if (o.m_type == String)
	{
		initString( o.m_data.value.String, o.m_data.dim.size);
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
	}
	return -2;
}

static Variant::Data::UInt variant2uint_cast( const Variant& o)
{
	switch (o.type())
	{
		case Variant::Null:
			throw boost::bad_lexical_cast();
		case Variant::Bool:
			return o.data().value.Bool?1:0;
		case Variant::Double:
			return boost::numeric_cast<double>( o.data().value.Double);
		case Variant::Int:
			if (o.data().value.Int < 0) throw boost::bad_lexical_cast();
			return o.data().value.Int;
		case Variant::UInt:
			return o.data().value.UInt;
		case Variant::String:
			return utils::touint_cast( std::string( o.data().value.String));
	}
	throw boost::bad_lexical_cast();
}

static Variant::Data::Int variant2int_cast( const Variant& o)
{
	switch (o.type())
	{
		case Variant::Null:
			throw boost::bad_lexical_cast();
		case Variant::Bool:
			return o.data().value.Bool?1:0;
		case Variant::Double:
			return boost::numeric_cast<double>( o.data().value.Double);
		case Variant::Int:
			return o.data().value.Int;
		case Variant::UInt:
			if (o.data().value.UInt > (Variant::Data::UInt)std::numeric_limits<Variant::Data::Int>::max()) throw boost::bad_lexical_cast();
			return o.data().value.UInt;
		case Variant::String:
			return utils::toint_cast( std::string( o.data().value.String));
	}
	throw boost::bad_lexical_cast();
}

template <typename TYPE>
static typename boost::enable_if_c<boost::is_arithmetic<TYPE>::value,TYPE>::type variant_cast( const Variant& o)
{
	switch (o.type())
	{
		case Variant::Null:
			throw boost::bad_lexical_cast();
		case Variant::Bool:
			return boost::numeric_cast<TYPE>( o.data().value.Bool);
		case Variant::Double:
			return boost::numeric_cast<TYPE>( o.data().value.Double);
		case Variant::Int:
			return boost::numeric_cast<TYPE>( o.data().value.Int);
		case Variant::UInt:
			return boost::numeric_cast<TYPE>( o.data().value.UInt);
		case Variant::String:
			return boost::lexical_cast<TYPE>( std::string( o.data().value.String));
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
			return utils::tostring_cast( o.data().value.Int);
		case Variant::UInt:
			return utils::tostring_cast( o.data().value.UInt);
		case Variant::String:
			return std::string( o.data().value.String, o.data().dim.size);
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
			case Variant::Double:
				return compare_double( variant_cast<double>( o), m_data.value.Double);
			case Variant::Int:
				if (o.type() == UInt && o.m_data.value.UInt > (Data::UInt)std::numeric_limits<Data::Int>::max()) return -1;
				return compare_int( variant_cast<Data::Int>( o), m_data.value.Int);
			case Variant::UInt:
				return compare_int( variant_cast<Data::UInt>( o), m_data.value.UInt);
			case Variant::Bool:
				return compare_bool( variant_cast<bool>( o), m_data.value.Bool);
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

double Variant::tonumber() const
{
	return variant_cast<double>( *this);
}

double Variant::todouble() const
{
	return variant_cast<double>( *this);
}

bool Variant::tobool() const
{
	return variant_cast<bool>( *this);
}

Variant::Data::Int Variant::toint() const
{
	return variant2int_cast( *this);
}

Variant::Data::UInt Variant::touint() const
{
	return variant2uint_cast( *this);
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
	switch (type_)
	{
		case Null: release(); init(); return;
		case Bool: *this = tobool(); return;
		case Int: *this = toint(); return;
		case UInt: *this = touint(); return;
		case Double: *this = todouble(); return;
		case String: {Variant rt = tostring(); move(rt); return;}
	}
	throw std::runtime_error( "illegal conversion of atomic type");
}


std::ostream& std::operator << (std::ostream &os, const _Wolframe::types::Variant& o)
{
	return os << o.tostring();
}

