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
/// \file config/descriptionBase.hpp
/// \brief Defines the non intrusive description of a struct (the implementation is intrusive and defined in config/description.hpp)

#ifndef _Wolframe_CONFIG_DESCRIPTION_BASE_HPP_INCLUDED
#define _Wolframe_CONFIG_DESCRIPTION_BASE_HPP_INCLUDED
#include <boost/property_tree/ptree.hpp>
#include <vector>
#include <string>
#include <cstddef>

namespace _Wolframe {
namespace config {

struct DescriptionBase
{
	typedef void (*Parse)( const char* name, void* ref, std::size_t ofs, const boost::property_tree::ptree& pt);
	struct Item
	{
		std::size_t m_ofs;
		std::string m_name;
		Parse m_parse;

	Item( std::size_t ofs, const std::string& name, Parse parse)
		:m_ofs(ofs),m_name(name),m_parse(parse){}
	Item( const Item& o)
		:m_ofs(o.m_ofs),m_name(o.m_name),m_parse(o.m_parse){}
	};
	std::vector<Item> m_ar;

	/// \brief Fills the configuration structure with values from the configuration
	/// \param[in,out] configStruct structure to fill
	/// \param[in] pt property tree with configurarion in its original form
	/// \param[out] error the error message in case of failure (return false)
	/// \return true if Ok, false if failed
	bool parse( void* configStruct, const boost::property_tree::ptree& pt, std::string& errmsg) const;
};


}}// end namespace
#endif
