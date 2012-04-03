/************************************************************************
Copyright (C) 2011 Project Wolframe.
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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
///\file ddl/bignumType.hpp
///\brief Defines a bignum type for the DDLs used for forms
#ifndef _Wolframe_DDL_BIGNUMTYPE_HPP_INCLUDED
#define _Wolframe_DDL_BIGNUMTYPE_HPP_INCLUDED
#include <string>
#include <boost/lexical_cast.hpp>
#include <boost/numeric/conversion/cast.hpp>


#if WITH_LIBGMP
extern "C"
{
#include <gmp.h>
}
#endif

namespace _Wolframe {
namespace ddl {

#if WITH_LIBGMP
struct Bigint
{
	Bigint()
	{
		mpz_init( m_value);
	}
	~Bigint()
	{
		mpz_clear( m_value);
	}

	bool set( const std::string& o)
	{
		if (0>mpz_set_str( m_value, o.c_str()))
		{
			return false;
		}
		return true;
	}

	void set( const int& o)			{mpz_set_ui( m_value, o);}
	void set( const unsigned int& o)	{mpz_set_ui( m_value, o);}

	void set( const int& o)			{mpz_set_si( m_value, o);}
	void set( const unsigned short& o)	{mpz_set_ui( m_value, o);}
	void set( const short& o)		{mpz_set_si( m_value, o);}

	void set( const unsigned char& o)	{mpz_set_ui( m_value, o);}
	void set( const char& o)		{mpz_set_si( m_value, o);}

	void set( const double& o)		{mpz_set_d( m_value, o);}
	void set( const float& o)		{mpz_set_f( m_value, o);}

	bool get( std::string& x)
	{
		try
		{
			char *buffer = (char*)malloc( mpz_sizeinbase( m_value,10)+3);
			mpz_get_str( buffer, 10, m_value);
			o.clear();
			o.append( buffer);
			return true;
		}
		catch (std::exception&)
		{
			return false;
		}
	}

	bool get( int& x)			{x = mpz_get_ui( m_value);}
	bool get( unsigned int& x)		{x = mpz_get_ui( m_value);}

	bool get( int& x)			{x = mpz_get_si( m_value);}
	bool get( unsigned short& x)		{x = mpz_get_ui( m_value);}
	bool get( short& x)			{x = mpz_get_si( m_value);}

	bool get( unsigned char& x)		{x = mpz_get_ui( m_value);}
	bool get( char& x)			{x = mpz_get_si( m_value);}

	bool get( double& x)			{x = mpz_get_d( m_value);}
	bool get( float& x)			{x = mpz_get_f( m_value);}
};

#else

struct Bigint
{
	Bigint() {}
	~Bigint() {}

	template <typename Type>
	bool set( const Type& o)
	{
		try
		{
			m_value = boost::lexical_cast<std::string>(o);
			std::string::const_iterator itr = m_value.begin();
			if (*itr == '-') ++itr;
			for (; itr!=m_value.end(); ++itr)
			{
				if (*itr < '0' || *itr > '9')
				{
					m_value.clear();
					throw std::logic_error("cannot convert to bigint");
				}
			}
			return true;
		}
		catch (const std::exception&)
		{
			return false;
		}
	}

	template <typename Type>
	bool get( Type& o)
	{
		try
		{
			o = boost::lexical_cast<Type>( m_value);
			return true;
		}
		catch (const std::exception&)
		{
			return false;
		}
	}
private:
	std::string m_value;
};
#endif
}} //namespace
#endif
