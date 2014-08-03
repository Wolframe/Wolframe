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
///\file pythonStructure.cpp
#include "pythonStructure.hpp"
#include <sstream>
#include <Python.h>

using namespace _Wolframe;
using namespace _Wolframe::langbind;
using namespace _Wolframe::langbind::python;

static void print_newitem( std::ostream& out, const utils::PrintFormat* pformat, std::size_t level)
{
	out << pformat->newitem;
	for (std::size_t ll=0; ll<level; ++ll) out << pformat->indent;
}

void Structure::print( std::ostream& out, const utils::PrintFormat* pformat, std::size_t level) const
{
	if (array())
	{
		Structure::const_iterator si = begin(), se = end();
		out << pformat->openstruct;
		int idx = 0;
		for (; si != se; ++si,++idx)
		{
			if (idx) out << pformat->decldelimiter;
			print_newitem( out, pformat, level);
			Structure elem( si->val);
			elem.print( out, pformat, level+1);
		}
		if (idx>0)
		{
			print_newitem( out, pformat, level);
		}
		out << pformat->closestruct;
	}
	else if (atomic())
	{
		out << pformat->startvalue;
		if (pformat->maxitemsize)
		{
			out << utils::getLogString( value(), pformat->maxitemsize);
		}
		else
		{
			out << value().tostring();
		}
		out << pformat->endvalue;
	}
	else
	{
		Structure::const_iterator si = begin(), se = end();
		out << pformat->openstruct;

		int idx = 0;
		for (; si != se; ++si,++idx)
		{
			if (idx) out << pformat->decldelimiter;
			print_newitem( out, pformat, level);

			out << Object::tostring(si->key) << pformat->assign;

			Structure val( si->val);
			val.print( out, pformat, level+1);
		}
		out << pformat->closestruct;
	}
}

std::string Structure::tostring( const utils::PrintFormat* pformat) const
{
	std::ostringstream buf;
	print( buf, pformat, 0);
	return buf.str();
}

