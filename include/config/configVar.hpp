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
#ifndef _Wolframe_CONFIGURATION_VARIABLE_HPP_INCLUDED
#define _Wolframe_CONFIGURATION_VARIABLE_HPP_INCLUDED
/// \file config/configVar.hpp
/// \brief Representation of configuration variables
#include "logger.hpp"
#include "valueParser.hpp"
#include <boost/type_traits.hpp>
#include <boost/array.hpp>
#include <vector>
#include <list>

namespace _Wolframe {
namespace config {

template <typename ValueType, class Domain=Parser::BaseTypeDomain>
class Variable
{
public:
	typedef std::string string;

	Variable( const char* name_, ValueType* value_)				:m_name(name_),m_value(value_),m_initialized(false){}
	Variable( const char* name_, const Domain& domain_, ValueType* value_)	:m_name(name_),m_value(value_),m_domain(domain_),m_initialized(false){}

private:
	/// \brief setter function for an single value
	template <typename T> boost::enable_if< boost::is_arithmetic<T>, bool>

	set_( const string& value)
	{
		if (m_initialized)
		{
			LOG_ERROR << "duplicate definition of <" << m_name << ">";
			return false;
		}
		else if (Parser::getValue( m_name, value, *m_value, m_domain))
		{
			m_initialized = true;
			return false;
		}
		return true;
	}

	/// \brief setter function for a container
	template <typename T> boost::enable_if< boost::is_same< ValueType, std::vector< typename T::value_type> >, bool>

	set_( const string& value)
	{
		try
		{
			typename T::value_type element;
			if (Parser::getValue( m_name, value, element, m_domain))
			{
				m_value->push_back( element);
				return true;
			}
		}
		catch (std::bad_alloc)
		{
			LOG_ERROR << "out of memory when parsing configuration element <" << m_name << ">";
		}
		catch (...)
		{
			LOG_ERROR << "cannot define more than " << m_value->size() << "elements to <" << m_name << ">";
		}
		return false;
	}

public:
	bool set( const string& value)
	{
		return (bool)set_<ValueType>( value);
	};

private:
	const char* m_name;
	ValueType* m_value;
	Domain m_domain;
	bool m_initialized;
};

}}//namespace
#endif

