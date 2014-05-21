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
#include <cstring>
#include <boost/shared_ptr.hpp>

using namespace _Wolframe;
using namespace _Wolframe::types;

bool DocMetaData::Attribute::getid( Id& res, const char* id)
{
	Id ai = firstid(), ae = lastid();
	do
	{
		if (0==std::strcmp( id, name(ai)))
		{
			res = ai;
			return true;
		}
		ai = nextid(ai);
	}
	while (ai != ae);
	return false;
}

DocMetaData::DocMetaData( const DocMetaData& o)
	:m_attributes(o.m_attributes){}
DocMetaData::DocMetaData( const std::vector<Attribute>& attributes_)
	:m_attributes(attributes_){}
DocMetaData::DocMetaData(){}

void DocMetaData::clear()
{
	m_attributes.clear();
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

typedef std::pair<std::size_t, std::size_t> PositionRange;
static PositionRange locateStem( const std::string& src)
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
	std::size_t beginidx = fnm - src.c_str();
	std::size_t endidx = ext ? (ext - src.c_str()):src.size();
	return PositionRange( beginidx, endidx);
}

static std::string replaceStem( const std::string& src, const std::string& id)
{
	PositionRange pos = locateStem( src);
	std::string rt;
	rt.append( src.c_str(), pos.first);
	rt.append( id);
	rt.append( src.c_str() + pos.second, src.size() - pos.second);
	return rt;
}

static std::string getStem( const std::string& src)
{
	PositionRange pos = locateStem( src);
	return std::string( src.c_str() + pos.first, pos.second - pos.first);
}

void DocMetaData::deleteAttribute( Attribute::Id id)
{
	std::vector<Attribute>::iterator ai = m_attributes.begin(), ae = m_attributes.end();
	for (;ai != ae; ++ai)
	{
		if (ai->id == id)
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
		if (ai->id == id)
		{
			return ai->value.c_str();
		}
	}
	return 0;
}

const char* DocMetaData::getAttribute( const char* name) const
{
	DocMetaData::Attribute::Id id;
	if (!DocMetaData::Attribute::getid( id, name)) return 0;
	return getAttribute( id);
}

void DocMetaData::setAttribute( const Attribute& attr)
{
	bool attr_set = false;
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
		if (ai->id == Attribute::DoctypeId)
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
		m_attributes.push_back( Attribute( Attribute::DoctypeId, id_));
	}
}

const char* DocMetaData::root() const
{
	std::vector<Attribute>::const_iterator ai = m_attributes.begin(), ae = m_attributes.end();
	for (;ai != ae; ++ai)
	{
		if (ai->id == Attribute::RootElement)
		{
			return ai->value.c_str();
		}
	}
	return 0;
}

std::string DocMetaData::doctype() const
{
	std::vector<Attribute>::const_iterator ai, ae = m_attributes.end();
	for (ai = m_attributes.begin(); ai != ae; ++ai)
	{
		if (ai->id == Attribute::DoctypeId)
		{
			return getStem( ai->value);
		}
	}
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
	return std::string();
}


