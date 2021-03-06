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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
///\file serialize/configSerialize.hpp
///\brief Defines functions for serialization/deserialization of configuration
#ifndef _Wolframe_SERIALIZE_CONFIG_SERIALIZE_HPP_INCLUDED
#define _Wolframe_SERIALIZE_CONFIG_SERIALIZE_HPP_INCLUDED
#include "filter/ptreefilter.hpp"
#include "filter/typedfilter.hpp"
#include "serialize/struct/structSerializer.hpp"
#include "serialize/struct/structParser.hpp"
#include "types/variantStructDescription.hpp"
#include "types/variantStruct.hpp"
#include "types/propertyTree.hpp"
#include "config/configurationTree.hpp"

namespace _Wolframe {
namespace serialize {

void parseConfigStructureVP( void* stptr, const serialize::StructDescriptionBase* descr, const config::ConfigurationNode& pt);

template <class Structure>
void parseConfigStructure( Structure& st, const config::ConfigurationNode& pt)
{
	parseConfigStructureVP( (void*)&st, st.getStructDescription(), pt);
}

template <class Structure>
types::PropertyTree::Node structureToPropertyTree( const Structure& st)
{
	langbind::PropertyTreeOutputFilter* res;
	langbind::TypedOutputFilterR out( res = new langbind::PropertyTreeOutputFilter());
	serialize::StructSerializer serializer( (const void*)&st, st.getStructDescription());
	serializer.init( out);
	if (!serializer.call()) throw std::runtime_error( "illegal state in structure serializer");
	return res->content();
}

std::string structureToStringVP( const void* stptr, const serialize::StructDescriptionBase* descr);

template <class Structure>
std::string structureToString( const Structure& st)
{
	return structureToStringVP( (const void*)&st, st.getStructDescription());
}

}}//namespace
#endif
