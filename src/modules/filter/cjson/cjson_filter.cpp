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
///\file cjson_filter.cpp
///\brief Implementation of a filter using the cjson library for input and output
#include "cjson_filter.hpp"
#include "inputfilterImpl.hpp"
#include "outputfilterImpl.hpp"
#include <cstddef>
#include <cstring>
#include <vector>
#include <string>
#include <algorithm>
#include <stdexcept>
#include <boost/shared_ptr.hpp>
#include <boost/algorithm/string.hpp>

using namespace _Wolframe;
using namespace _Wolframe::langbind;

struct CJsonFilter :public Filter
{
	CJsonFilter( const char* encoding=0)
	{
		m_inputfilter.reset( new InputFilterImpl());
		if (encoding)
		{
			m_inputfilter->setAttribute( "encoding", encoding);
		}
		OutputFilterImpl* oo = new OutputFilterImpl( m_inputfilter->getMetaDataRef());
		m_outputfilter.reset( oo);
	}
};

static const char* getArgumentEncoding( const std::vector<FilterArgument>& arg)
{
	const char* encoding = 0;
	std::vector<FilterArgument>::const_iterator ai = arg.begin(), ae = arg.end();
	for (; ai != ae; ++ai)
	{
		if (ai->first.empty() || boost::algorithm::iequals( ai->first, "encoding"))
		{
			if (encoding)
			{
				if (ai->first.empty())
				{
					throw std::runtime_error( "too many filter arguments");
				}
				else
				{
					throw std::runtime_error( "duplicate filter argument 'encoding'");
				}
			}
			encoding = ai->second.c_str();
			break;
		}
		else
		{
			throw std::runtime_error( std::string( "unknown filter argument '") + ai->first + "'");
		}
	}
	return encoding;
}

Filter* CJsonFilterType::create( const std::vector<FilterArgument>& arg) const
{
	return new CJsonFilter( getArgumentEncoding( arg));
}


