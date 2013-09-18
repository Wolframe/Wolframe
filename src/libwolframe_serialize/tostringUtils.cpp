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
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
///\brief Implementation of tostring utility functions
///\file tostringUtils.cpp
#include "serialize/tostringUtils.hpp"
#include "serialize/ddl/filtermapDDLSerialize.hpp"
#include "filter/tostringfilter.hpp"

using namespace _Wolframe;
using namespace _Wolframe::serialize;

std::string serialize::ddlStructureToString( const types::VariantStruct& st)
{
	langbind::ToStringFilter* res;
	langbind::TypedOutputFilterR out( res = new langbind::ToStringFilter( "\t"));
	serialize::DDLStructSerializer serializer( &st);
	serializer.init( out);
	if (!serializer.call()) throw std::runtime_error( "illegal state in DDL structure serializer");
	return res->content();
}

std::string serialize::typedInputFilterToString( const langbind::TypedInputFilterR& inp)
{
	langbind::ToStringFilter* res;
	langbind::TypedOutputFilterR out( res = new langbind::ToStringFilter( "\t"));
	langbind::TypedInputFilter::ElementType type;
	types::VariantConst element;
	int taglevel = 0;
	while (taglevel >= 0 && inp->getNext( type, element))
	{
		if (type == langbind::TypedInputFilter::OpenTag) ++taglevel;
		else if (type == langbind::TypedInputFilter::CloseTag) --taglevel;
		if (taglevel >= 0) out->print( type, element);
	}
	switch (inp->state())
	{
		case langbind::InputFilter::Open: break;
		case langbind::InputFilter::Error: throw std::runtime_error( inp->getError());
		case langbind::InputFilter::EndOfMessage: throw std::runtime_error( "input not complete and cannot yield execution here. tostring failed");
	}
	return res->content();
}

