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
///\file config/structSerialize.hpp
///\brief Defines a configuration structure parser
#ifndef _Wolframe_CONFIG_STRUCT_SERIALIZE_HPP_INCLUDED
#define _Wolframe_CONFIG_STRUCT_SERIALIZE_HPP_INCLUDED
#include "filter/ptreefilter.hpp"
#include "filter/tostringfilter.hpp"
#include "filter/typedfilter.hpp"
#include "serialize/struct/filtermapBase.hpp"
#include "types/variantStructDescription.hpp"
#include "types/variantStruct.hpp"

namespace _Wolframe {
namespace config {

template <class Structure>
void parseConfigStructure( Structure& st, const boost::property_tree::ptree& pt)
{
	langbind::TypedInputFilterR inp( new langbind::PropertyTreeInputFilter( pt));
	serialize::StructParser parser( (void*)&st, st.getStructDescription());
	parser.init( inp);
	if (!parser.call()) throw std::runtime_error( "illegal state in structure parser");
}

template <class Structure>
boost::property_tree::ptree structureToPropertyTree( const Structure& st)
{
	langbind::PropertyTreeOutputFilter* res;
	langbind::TypedOutputFilterR out( res = new langbind::PropertyTreeOutputFilter());
	serialize::StructSerializer serializer( (const void*)&st, st.getStructDescription());
	serializer.init( out);
	if (!serializer.call()) throw std::runtime_error( "illegal state in structure serializer");
	return res->content();
}

template <class Structure>
std::string structureToString( const Structure& st)
{
	langbind::ToStringFilter* res;
	langbind::TypedOutputFilterR out( res = new langbind::ToStringFilter( "\t"));
	serialize::StructSerializer serializer( (const void*)&st, st.getStructDescription());
	serializer.init( out);
	if (!serializer.call()) throw std::runtime_error( "illegal state in structure serializer");
	return res->content();
}

}}//namespace
#endif


