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
///\file types/docmetadata.cpp
///\brief Implementation of structure representing document meta data

#include "types/docmetadata.hpp"
#include <string>
#include <boost/shared_ptr.hpp>

using namespace _Wolframe;
using namespace _Wolframe::types;

DocMetaData::DocMetaData( const DocMetaData& o)
	:m_attributes(o.m_attributes){}
DocMetaData::DocMetaData( const Type& type_, const std::vector<Attribute>& attributes_)
	:m_attributes(attributes_){}
DocMetaData::DocMetaData(){}

void DocMetaData::clear()
{
	attributes.clear();
}

void DocMetaData::init( const std::vector<Attribute>& attributes_)
{
	m_attributes = attributes_;
}

void DocMetaData::join( const std::vector<Attribute>& attributes_)
{
	std::vector<Attribute>::const_iterator ai = attributes_.begin(), ae = attributes_.end();
	for (; ai != ae; ++ai)
	{
		setAttribute( *ai);
	}
}

static const char* findLastChar( const char* cc, char ch)
{
	const char* ee = std::strchr( cc, ch);
	for (; ee != 0; cc = ee, ee = std::strchr( cc+1, ch));
	return (cc[0]==ch)?cc:0;
}

static std::pair<const char*, const char*> locateStem( const std::string& src)
{
	const char* lsp = findLastChar( src.c_str(), ' ');
	if (!lsp)
	{
		lsp = src.c_str();
	}
	else
	{
		lsp++;
	}
	const char* fnm = findLastChar( lsp, '/');
	if (!fnm)
	{
		fnm = lsp;
	}
	else
	{
		fnm++;
	}
	const char* ext = findLastChar( fnm, '.');
	if (fnm > ext)
	{
		ext = 0;
	}
	return std::pair<const char*, const char*>( src.c_str(), src.c_str() + fnm - src.c_str());
}

static std::string replaceStem( const std::string& src, const std::string& id)
{
	std::pair<const char*, const char*> pos = locateStem( src);
	std::string rt;
	rt.append( src.c_str(), pos.first - src.c_str());
	rt.append( id);
	if (pos.second) rt.append( pos.second);
	return rt;
}

static std::string getStem( const std::string& src)
{
	std::pair<const char*, const char*> pos = locateStem( src);
	if (!pos.second) pos.second = src.c_str() + src.size();
	return std::string( pos.first, pos.second - pos.first);
}

void DocMetaData::deleteAttribute( Attribute::Id id)
{
	std::vector<Attribute>::iterator ai = m_attributes.begin(), ae = m_attributes.end();
	for (;ai != ae; ++ai)
	{
		if (ai->id == attr.id)
		{
			m_attributes.erase( ai);
			break;
		}
	}
}

const char* DocMetaData::getAttribute( Attribute::Id id) const
{
	std::vector<Attribute>::const_iterator ai = m_attributes.begin(), ae = m_attributes.end();
	for (;ai != ae; ++ai)
	{
		if (ai->id == attr.id)
		{
			return ai->value.c_str();
		}
	}
	return 0;
}

void DocMetaData::setAttribute( const Attribute& attr)
{
	bool attr_set = false;
	switch (attr.id)
	{
		case Attribute::RootElement: break;
		case Attribute::XmlNamespace: break;
		case Attribute::Xsi: break;
		case Attribute::SchemaLocation:
			deleteAttribute( Attribute::DOCTYPE_SYSTEM);
			deleteAttribute( Attribute::DOCTYPE_PUBLIC);
			break;
		case Attribute::DOCTYPE_SYSTEM: break;
		case Attribute::DOCTYPE_PUBLIC: break;
		case Attribute::Identifier: break;
	}

	std::vector<Attribute>::iterator ai = m_attributes.begin(), ae = m_attributes.end();
	for (;ai != ae; ++ai)
	{
		if (ai->id == attr.id)
		{
			ai->value = attr.value;
			attr_set = true;
		}
	}
	if (!attr_set)
	{
		m_attributes.push_back( attr);
	}
}

void DocMetaData::setAttribute( Attribute::Id id, const std::string& value)
{
	setAttribute( Attribute( id, value));
}

void DocMetaData::setDoctype( const std::string& id_, const std::string& root_)
{
	bool root_set = false;
	bool id_set = false;
	std::vector<Attribute>::iterator ai = m_attributes.begin(), ae = m_attributes.end();
	for (;ai != ae; ++ai)
	{
		if (ai->id == Attribute::DOCTYPE_SYSTEM)
		{
			ai->value = replaceStem( ai->value, id_);
			id_set = true;
		}
		if (ai->id == Attribute::SchemaLocation)
		{
			ai->value = replaceStem( ai->value, id_);
			id_set = true;
		}
		if (ai->id == Attribute::RootElement)
		{
			ai->value = root_;
			root_set = true;
		}
		if (ai->id == Attribute::Identifier)
		{
			ai->value = id_;
			id_set = true;
		}
	}
	if (!root_set)
	{
		m_attributes.push_back( Attribute( Attribute::RootElement, root_));
	}
	if (!id_set)
	{
		m_attributes.push_back( Attribute( Attribute::Identifier, id_));
	}
}

const char* DocMetaData::root() const
{
	std::vector<Attribute>::const_iterator ai = m_attributes.begin(), ae = m_attributes.end();
	for (;ai != ae; ++ai)
	{
		if (ai->id == Attribute::RootElement)
		{
			return ai->value;
		}
	}
	return 0;
}

const char* DocMetaData::doctype() const
{
	std::vector<Attribute>::const_iterator ai, ae = m_attributes.end();
	for (ai = m_attributes.begin(); ai != ae; ++ai)
	{
		if (ai->id == Attribute::DOCTYPE_SYSTEM)
		{
			return getStem( ai->value);
		}
	}
	for (ai = m_attributes.begin(); ai != ae; ++ai)
	{
		if (ai->id == Attribute::SchemaLocation)
		{
			return getStem( ai->value);
		}
	}
	for (ai = m_attributes.begin(); ai != ae; ++ai)
	{
		if (ai->id == Attribute::Identifier)
		{
			return getStem( ai->value);
		}
	}
	return 0;
}


