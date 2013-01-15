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
///\file ddl_structTypeBuild.cpp
///\brief Implementation of the building of structure definitions from a buffering filter
#include "ddl/structTypeBuild.hpp"
#include <boost/algorithm/string.hpp>

using namespace _Wolframe;
using namespace _Wolframe::ddl;

struct Name
{
	std::string identifier;
	bool isArray;
	bool isAttribute;

	void set( const std::string& tag)
	{
		isArray = false;
		isAttribute = false;
		const char* start = tag.c_str();
		std::size_t size = tag.size();
		if (tag.size() && tag[0] == '@')
		{
			isAttribute = true;
			start = tag.c_str()+1;
			size -= 1;
		}
		if (size >= 2 && std::strcmp( start+size-2, "[]") == 0)
		{
			isArray = true;
			size -= 2;
		}
		identifier = std::string( start, size);
	}
};

StructTypeBuild::StructTypeBuild( langbind::TypedInputFilter& src, const TypeMap* typemap)
{
	langbind::FilterBase::ElementType type,lasttype = langbind::FilterBase::OpenTag;
	Name attribute;
	langbind::TypedFilterBase::Element val;

	while (src.getNext( type, val))
	{
		switch (type)
		{
			case langbind::FilterBase::OpenTag:
			{
				Name tag;
				tag.set( val.tostring());
				StructTypeBuild rtElem( src, typemap);
				if (tag.isAttribute && rtElem.contentType() != StructType::Atomic)
				{
					throw std::runtime_error( "Attribute type declared with @ has a non atomic value");
				}
				if (tag.isArray)
				{
					StructType rtElemAr;
					rtElemAr.defineAsVector( rtElem);
					defineContent( tag.identifier, rtElemAr);
				}
				else
				{
					defineContent( tag.identifier, rtElem);
				}
				lasttype = type;
				break;
			}
			case langbind::FilterBase::CloseTag:
				lasttype = type;
				return;

			case langbind::FilterBase::Attribute:
				lasttype = type;
				attribute.set( val.tostring());
				break;

			case langbind::FilterBase::Value:
			{
				std::string typenam = val.tostring();
				const NormalizeFunction* tp = 0;
				if (!boost::algorithm::iequals( typenam, "string"))
				{
					tp = typemap->getType( typenam);
					if (!tp)
					{
						std::ostringstream msg;
						msg << "unknown type in structure (" << typenam << ")";
						throw std::runtime_error( msg.str());
					}
				}
				AtomicType at( tp);
				StructType vv;
				if (lasttype == langbind::FilterBase::Attribute)
				{
					if (attribute.isArray)
					{
						vv.defineAsVector( at);
					}
					else
					{
						vv = at;
					}
					if (attribute.isAttribute)
					{
						defineAttribute( attribute.identifier, vv);
					}
					else
					{
						defineContent( attribute.identifier, vv);
					}
					lasttype = langbind::FilterBase::OpenTag;
					break;
				}
				else
				{
					if (nof_elements() > 0) throw std::runtime_error( "values without tag in structure definition");
					lasttype = langbind::FilterBase::Value;
					defineAsAtomic( at);
				}
			}
		}
	}
	throw std::runtime_error( "input filter not buffering input");
}


