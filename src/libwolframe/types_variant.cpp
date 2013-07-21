#include "types/variant.hpp"
#include <limits>
#include <stdexcept>
#include <cstdlib>
#include <boost/lexical_cast.hpp>
#include <boost/utility/enable_if.hpp>

using namespace _Wolframe;
using namespace _Wolframe::types;

void Variant::init()
{
	std::memset( this, 0, sizeof( *this));
}

void Variant::release()
{
	if (type() == string_ && m_data.value.string_ && !constant())
	{
		std::free( m_data.value.string_);
	}
}

void Variant::initString( const char* str_, std::size_t strsize_, bool constant_)
{
	std::memset( this, 0, sizeof( *this));
	m_type = string_;
	m_data.dim.size = strsize_;
	if (constant_)
	{
		m_data.value.string_ = const_cast<char*>(str_);
		setConstant();
	}
	else
	{
		m_data.value.string_ = (char*)std::malloc( strsize_+1);
		if (!m_data.value.string_) throw std::bad_alloc();
		std::memcpy( m_data.value.string_, str_, strsize_);
		m_data.value.string_[ strsize_] = 0;
	}
}

void Variant::initCopy( Variant& dest, const Variant& orig)
{
	if (orig.constant())
	{
		std::memcpy( &dest, &orig, sizeof(dest));
	}
	else
	{
		if (orig.type() == string_)
		{
			dest.initString( orig.m_data.value.string_, orig.m_data.dim.size);
			dest.setInitialized( orig.initialized());
		}
		else
		{
			std::memcpy( &dest, &orig, sizeof( dest));
		}
	}
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
		case Variant::bool_:	return compare_int( (int)d1.value.bool_, (int)d2.value.bool_);
		case Variant::double_:	return compare_double( d1.value.double_, d2.value.double_);
		case Variant::int_:	return compare_int( d1.value.int_, d2.value.int_);
		case Variant::uint_:	return compare_int( d1.value.uint_, d2.value.uint_);
		case Variant::string_:	return std::strcmp( d1.value.string_, d2.value.string_);
	}
	return -2;
}

template <typename TYPE>
static typename boost::enable_if_c<boost::is_arithmetic<TYPE>::value,TYPE>::type variant_cast( const Variant& o)
{
	switch (o.type())
	{
		case Variant::bool_:
			return boost::numeric_cast<TYPE>( o.data().value.bool_);
		case Variant::double_:
			return boost::numeric_cast<TYPE>( o.data().value.double_);
		case Variant::int_:
			return boost::numeric_cast<TYPE>( o.data().value.int_);
		case Variant::uint_:
			return boost::numeric_cast<TYPE>( o.data().value.uint_);
		case Variant::string_:
			return boost::lexical_cast<TYPE>( std::string( o.data().value.string_));
	}
	throw boost::bad_lexical_cast();
}

template <typename TYPE>
static typename boost::enable_if_c<boost::is_same<TYPE,std::string>::value,TYPE>::type variant_cast( const Variant& o)
{
	switch (o.type())
	{
		case Variant::bool_:
			return boost::lexical_cast<std::string>( o.data().value.bool_);
		case Variant::double_:
			return boost::lexical_cast<std::string>( o.data().value.double_);
		case Variant::int_:
			return boost::lexical_cast<std::string>( o.data().value.int_);
		case Variant::uint_:
			return boost::lexical_cast<std::string>( o.data().value.uint_);
		case Variant::string_:
			return std::string( o.data().value.string_);
	}
	throw boost::bad_lexical_cast();
}

int Variant::compare( const Variant& o) const
{
	Type tt = type();
	Type ot = o.type();
	if (tt == ot)
	{
		return compare_type( tt, m_data, o.m_data);
	}
	if ((int)tt > (int)ot)
	{
		return -o.compare( *this);
	}
	try
	{
		switch (tt)
		{
			case Variant::double_:
				return compare_double( variant_cast<double>( o), m_data.value.double_);
			case Variant::int_:
				if (o.type() == uint_ && o.m_data.value.uint_ > (unsigned int)std::numeric_limits<int>::max()) return -1;
				return compare_int( variant_cast<int>( o), m_data.value.int_);
			case Variant::uint_:
				return compare_int( variant_cast<unsigned int>( o), m_data.value.uint_);
			case Variant::bool_:
				return compare_bool( variant_cast<bool>( o), m_data.value.bool_);
			case Variant::string_:
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

int Variant::toint() const
{
	return variant_cast<int>( *this);
}

unsigned int Variant::touint() const
{
	return variant_cast<unsigned int>( *this);
}







