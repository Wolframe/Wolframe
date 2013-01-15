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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
///\file config/structParser.hpp
///\brief Parser for the configuration structure

#ifndef _Wolframe_CONFIG_STRUCTURE_PARSER_HPP_INCLUDED
#define _Wolframe_CONFIG_STRUCTURE_PARSER_HPP_INCLUDED
#error DEPRECATED
#include <boost/property_tree/ptree.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <string>
#include <vector>
#include <cstring>
#include "config/valueParser.hpp"
#include "config/traits.hpp"

namespace _Wolframe {
namespace config {

template <typename T>
static void parseElement( const char* name, T& value, const boost::property_tree::ptree& pt);

template <typename T>
static void parseElement1( const char* name, T& value, const boost::property_tree::ptree& pt);

template <typename Element>
struct ElementParser
{
	static void parse( const char* name, void* st, std::size_t ofs, const boost::property_tree::ptree& pt)
	{
		parseElement( name, *reinterpret_cast<Element*>((void*)((char*)st + ofs)), pt);
	}
};

struct ParseError
{
	std::string m_location;
	std::string m_message;

	ParseError( const std::string& location, const std::string& message)
		:m_location(location),m_message(message){}
	ParseError( const std::string& location, const ParseError& prev)
		:m_location(location),m_message(prev.m_message)
	{
		m_location.append( ".");
		m_location.append( prev.m_location);
	}
};

///\brief parses a structure according its description
template <typename T>
static void parseElement_( const char* name, T& value, const traits::struct_&, const boost::property_tree::ptree& pt)
{
	if (name)
	{
		parseElement1( name, value, pt);
	}
	else
	{
		static const DescriptionBase* descr = T::description();
		std::vector<DescriptionBase::Item>::const_iterator itr,end;

		for (itr=descr->m_ar.begin(),end=descr->m_ar.end(); itr != end; ++itr)
		{
			itr->m_parse( itr->m_name.c_str(), &value, itr->m_ofs, pt);
		}
		//check if all configuration elements are defined in the description
		boost::property_tree::ptree::const_iterator pi,pe;
		for (pi=pt.begin(),pe=pt.end(); pi != pe; pi++)
		{
			for (itr=descr->m_ar.begin(),end=descr->m_ar.end(); itr != end; ++itr)
			{
				if (boost::iequals( pi->first, itr->m_name.c_str())) break;
			}
			if (itr == end)
			{
				throw ParseError( pi->first,"undefined element");
			}
		}
	}
}

///\brief parses a configuration base class by calling its parse function
template <typename T>
static void parseElement_( const char* name, T& value, const traits::cfgbase_&, const boost::property_tree::ptree& pt)
{
	if (!parse( value, pt, name))
	{
		throw ParseError( name, "parse error");
	}
}


///\brief parses a all elements of a vector (n elements with same name in pt)
template <typename T>
static void parseElement_( const char* name, T& value, const traits::vector_&, const boost::property_tree::ptree& pt)
{
	if (!name)
	{
		throw ParseError( "?", "invalid indirection in configuration description (set of set)");
	}

	boost::property_tree::ptree::const_iterator end = pt.end();
	for (boost::property_tree::ptree::const_iterator it = pt.begin(); it != end; ++it)
	{
		if (boost::iequals( it->first, name))
		{
			typename T::value_type elem;
			parseElement( (const char*)0, elem, it->second);
			value.push_back( elem);
		}
	}
}


///\brief parses an atomic element restricted by a value domain
template <typename T, typename Domain>
static void parseElement_( const char* name, T& value, const Domain& domain, const boost::property_tree::ptree& pt)
{
	std::string explanation;
	if (name)
	{
		parseElement1( name, value, pt);
	}
	else if (pt.begin() != pt.end())
	{
		throw ParseError( "", "configuration element not atomic");
	}
	else
	{
		if (!domain.parse( value, pt.data(), explanation))
		{
			throw ParseError( name, explanation);
		}
	}
}


///\brief parses an atomic element
template <typename T>
static void parseElement_( const char* name, T& value, const traits::arithmetic_&, const boost::property_tree::ptree& pt)
{
	static Parser::BaseTypeDomain domain;
	parseElement_( name, value, domain, pt);
}


///\brief parses a bool element
template <typename T>
static void parseElement_( const char* name, T& value, const traits::bool_&, const boost::property_tree::ptree& pt)
{
	static Parser::BoolDomain domain;
	parseElement_( name, value, domain, pt);
}

///\brief parses an element with indirection (searches it in pt)
template <typename T>
static void parseElement1( const char* name, T& value, const boost::property_tree::ptree& pt)
{
	boost::property_tree::ptree::const_iterator it,end = pt.end();
	for (it = pt.begin(); it != end; ++it)
	{
		if (boost::iequals( it->first, name))
		{
			try
			{
				parseElement( 0, value, it->second);
			}
			catch (ParseError e)
			{
				throw ParseError( name, e);
			}
			catch (boost::bad_lexical_cast e)
			{
				throw ParseError( name, "illegal token type");
			}
			catch (const std::exception& e)
			{
				throw ParseError( name, e.what());
			}
			++it;
			break;
		}
	}
	for (; it != end; ++it)
	{
		if (boost::iequals( it->first, name))
		{
			throw ParseError( name, "duplicate definition");
		}
	}
}

///\brief the unified parse function
template <typename T>
static void parseElement( const char* name, T& value, const boost::property_tree::ptree& pt)
{
	parseElement_( name, value, traits::getCategory(value), pt);
}

}}// end namespace
#endif
