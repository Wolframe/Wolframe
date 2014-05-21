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
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
///\brief Implementation of directmap command descriptions
///\file directmapCommandDescription.cpp
#include "directmapCommandDescription.hpp"
#include "utils/fileUtils.hpp"
#include "utils/parseUtils.hpp"
#include "logger-v1.hpp"
#include "config/programBase.hpp"
#include <stdexcept>
#include <boost/algorithm/string.hpp>

using namespace _Wolframe;
using namespace _Wolframe::langbind;

static std::string filterargAsString( const std::vector<langbind::FilterArgument>& arg)
{
	std::ostringstream out;
	std::vector<langbind::FilterArgument>::const_iterator ai = arg.begin(), ae = arg.end();
	for (; ai != ae; ++ai)
	{
		if (ai != arg.begin()) out << ", ";
		out << ai->first << "='" << ai->second << "'";
	}
	return out.str();
}

std::string DirectmapCommandDescription::tostring() const
{
	std::ostringstream rt;
	rt << "name='" << name << "'";
	rt << ", call='" << call << "'";
	rt << ", input filter='" << inputfilter;
	if (inputfilterarg.size())
	{
		rt << "( " << filterargAsString( inputfilterarg) << " )'";
	}
	else
	{
		rt << "'";
	}
	rt << ", output filter='" << outputfilter;
	if (outputfilterarg.size())
	{
		rt << "( " << filterargAsString( outputfilterarg) << " )'";
	}
	else
	{
		rt << "'";
	}
	rt << ", input form='" << inputform << "'";
	rt << ", output form='" << outputform << "'";
	return rt.str();
}

