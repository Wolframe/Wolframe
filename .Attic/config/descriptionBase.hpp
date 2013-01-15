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
///\file config/descriptionBase.hpp
///\brief Defines the non intrusive description of a struct (the implementation is intrusive and defined in config/description.hpp)

#ifndef _Wolframe_CONFIG_DESCRIPTION_BASE_HPP_INCLUDED
#define _Wolframe_CONFIG_DESCRIPTION_BASE_HPP_INCLUDED
#error DEPRECATED
#include <boost/property_tree/ptree.hpp>
#include <vector>
#include <string>
#include <cstddef>

namespace _Wolframe {
namespace config {

///\brief A configuration structure description
struct DescriptionBase
{
	typedef void (*Parse)( const char* name, void* ref, std::size_t ofs, const boost::property_tree::ptree& pt);
	typedef void (*Print)( std::ostream& out, const char* name, const void* ref, std::size_t ofs, unsigned int indent);

	///\brief A description of one item of the configuration POD structure
	struct Item
	{
		std::size_t m_ofs;		//< member position offset in bytes from the start of the structure
		std::string m_name;		//< member name
		const char* m_type;		//< member type name
		Parse m_parse;			//< parse member function
		Print m_print;			//< print member function

		///\brief Constructor
		Item( std::size_t ofs, const std::string& name, const char* type, Parse parse, Print print)
			:m_ofs(ofs),m_name(name),m_type(type),m_parse(parse),m_print(print){}
		///\brief Copy constructor
		Item( const Item& o)
			:m_ofs(o.m_ofs),m_name(o.m_name),m_type(o.m_type),m_parse(o.m_parse),m_print(o.m_print){}
	};
	std::vector<Item> m_ar;

	///\brief Fills the configuration structure with values from the configuration
	///\param[in,out] configStruct structure to fill
	///\param[in] pt property tree with configurarion in its original form
	///\param[out] errmsg the error message in case of failure (return false)
	///\return true if Ok, false if failed
	bool parse( void* configStruct, const boost::property_tree::ptree& pt, std::string& errmsg) const;

	///\brief Print the configuration structure with values from the configuration
	///\param[out] stream to print to
	///\param[in] configStruct structure to print
	void print( std::ostream& out, const void* configStruct, unsigned int indent=0) const;
};


}}// end namespace
#endif
