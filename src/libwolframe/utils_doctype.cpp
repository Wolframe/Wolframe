/************************************************************************
Copyright (C) 2011, 2012 Project Wolframe.
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
///\file utils_doctype.cpp
///\brief Implementation of document type utilities and conversion functions
#include "utils/doctype.hpp"
#include "utils/miscUtils.hpp"
#include <stdexcept>

using namespace _Wolframe;
using namespace _Wolframe::utils;

std::string _Wolframe::utils::getIdFromDoctype( const std::string& doctype)
{
	std::string rootid;
	std::string publicid;
	std::string systemid;
	std::string::const_iterator itr=doctype.begin(), end=doctype.end();

	if (utils::parseNextToken( rootid, itr, end))
	{
		if (utils::parseNextToken( publicid, itr, end))
		{
			if (publicid == "PUBLIC")
			{
				if (!utils::parseNextToken( publicid, itr, end)
				||  !utils::parseNextToken( systemid, itr, end))
				{
					throw std::runtime_error( "illegal doctype definition (public id)");
				}
				return utils::getFileStem( systemid);
			}
			else if (publicid == "SYSTEM")
			{
				if (!utils::parseNextToken( systemid, itr, end))
				{
					throw std::runtime_error( "illegal doctype definition (system id)");
				}
				return utils::getFileStem( systemid);
			}
			else if (utils::parseNextToken( systemid, itr, end))
			{
				if (itr != end)
				{
					throw std::runtime_error( "illegal doctype definition (not terminated)");
				}
				return utils::getFileStem( systemid);
			}
			else
			{
				return utils::getFileStem( publicid);
			}
		}
		else
		{
			return rootid;
		}
	}
	else
	{
		return "";
	}
}

std::string _Wolframe::utils::getDoctypeFromIds( const char* d_root, const char* d_public, const char* d_system)
{
	std::string rt;
	if (!d_root) throw std::runtime_error( "no XML doctype root defined");
	rt.append( d_root);
	if (d_public)
	{
		rt.append( " PUBLIC \"");
		rt.append( d_public);
		rt.append( "\" ");
		if (!d_system) std::runtime_error( "doctype public is defined but no doctype system");
		rt.append( " SYSTEM \"");
		rt.append( d_system);
		rt.append( "\" ");
	} else if (d_system)
	{
		rt.append( " SYSTEM \"");
		rt.append( d_system);
		rt.append( "\" ");
	}
	return rt;
}
