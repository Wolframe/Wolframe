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
///\file structOptionParser.cpp
///\brief Implements uniform handling of structures in program command line options
#include "serialize/structOptionParser.hpp"
#include "utils/parseUtils.hpp"
#include <boost/algorithm/string.hpp>
#include <stdexcept>

using namespace _Wolframe;
using namespace _Wolframe::serialize;

static void defineOptionByIndex( const serialize::StructDescriptionBase* descr, void* ptr, std::size_t idx, const std::string& value)
{
	if (!descr->setAtomicValue( ptr, idx, value)) throw std::runtime_error( "failed to initialize structure element in option");
}

static void defineOptionByName( const serialize::StructDescriptionBase* descr, void* ptr, const std::string& id, const std::string& value)
{
	serialize::StructDescriptionBase::Map::const_iterator ii=descr->begin(), ee=descr->end();
	std::size_t idx = 0;
	bool found = false;
	for (; ii != ee; ++ii,++idx)
	{
		if (boost::algorithm::istarts_with( ii->first, id))
		{
			if (found) throw std::runtime_error( std::string("ambiguus element in option '") + id + "'");
			found = true;
			if (!descr->setAtomicValue( ptr, idx, value)) throw std::runtime_error( "failed to initialize structure element '" + id + "' in option");
		}
	}
}

static bool nextItem( std::string::const_iterator& ii, std::string::const_iterator ee, std::string& tok, std::string& val)
{
	static const utils::CharTable operatorTable(",=");
	static const utils::CharTable valueTable(",=", true);
	char ch;
	std::string ignore;
	tok.clear();
	val.clear();

	ch = utils::parseNextToken( tok, ii, ee, operatorTable, valueTable);
	switch (ch)
	{
		case '\0':
			return false;

		case ',':
			return true;

		case '=':
			throw std::runtime_error("identifier expected before '=' in option");

		default:
			ch = utils::parseNextToken( val, ii, ee, operatorTable);
			switch (ch)
			{
				case '=':
					ch = utils::parseNextToken( val, ii, ee, operatorTable, valueTable);
					if (ch == '=') std::runtime_error("unexpected '=' after '=' in option");
					if (ch == '\0')
					{
						return true;
					}
					if (ch == ',')
					{
						return true;
					}
					ch = utils::parseNextToken( ignore, ii, ee, operatorTable);
					if (ch != ',' && ch != '\0')
					{
						throw std::runtime_error("expected ',' or end of option");
					}
					return true;

				case ',':
				case '\0':
					val = tok;
					tok.clear();
					return true;

				default:
					throw std::runtime_error("expected '=' or ',' or end of option");
			}
	}
}

void serialize::parseStructOptionStringImpl( const serialize::StructDescriptionBase* descr, void* ptr, const::std::string& opt)
{
	std::string id;
	std::string val;
	std::string::const_iterator ii = opt.begin(), ee = opt.end();
	std::size_t idx = 0;
	enum OptionType
	{
		Undetermined,
		ByIndex,
		ByName

	};
	OptionType type = Undetermined;
	while (nextItem( ii, ee, id, val))
	{
		if (id.empty())
		{
			if (type == ByName)
			{
				throw std::runtime_error("option element identification by index and by name mixed");
			}
			type = ByIndex;
			defineOptionByIndex( descr, ptr, idx, val);
		}
		else
		{
			if (type == ByIndex)
			{
				throw std::runtime_error("option element identification by index and by name mixed");
			}
			type = ByName;
			defineOptionByName( descr, ptr, id, val);
		}
		++idx;
	}
}

types::PropertyTree::Node serialize::structOptionTree( const::std::string& opt)
{
	boost::property_tree::ptree pt;
	std::string id;
	std::string val;
	std::string::const_iterator ii = opt.begin(), ee = opt.end();

	while (nextItem( ii, ee, id, val))
	{
		if (id.empty())
		{
			throw std::runtime_error("property tree needs program option elements addressed by name");
		}
		else
		{
			pt.add_child( id, boost::property_tree::ptree(val));
		}
	}
	return types::PropertyTree::Node( pt);
}



