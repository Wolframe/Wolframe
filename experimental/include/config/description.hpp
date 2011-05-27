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
/// \file config/description.hpp
/// \brief Defines the description of a struct for the configuration parser to assign the values to the mebers by name

#ifndef _Wolframe_CONFIG_DESCRIPTION_HPP_INCLUDED
#define _Wolframe_CONFIG_DESCRIPTION_HPP_INCLUDED
#include "config/structParser.hpp"
#include "config/descriptionBase.hpp"

namespace _Wolframe {
namespace config {

template <class Structure>
struct Description :public DescriptionBase
{
	template <typename Element>
	Description& operator()( const char* name, Element Structure::*eptr)
	{
		std::size_t pp = (std::size_t)&(((Structure*)0)->*eptr);
		Item e( pp, std::string(name), &_Wolframe::config::ElementParser<Element>::parse);
		m_ar.push_back( e);
		return *this;
	}
	Description(){}
};

}}// end namespace
#endif
