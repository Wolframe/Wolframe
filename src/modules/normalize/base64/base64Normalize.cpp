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
///\file base64Normalize.cpp
///\brief Implementation of base64 based normalization functions
#include "base64Normalize.hpp"
#include "base64NormalizeFunction.hpp"
#include <boost/algorithm/string.hpp>
#include <limits>

using namespace _Wolframe;
using namespace _Wolframe::langbind;

types::NormalizeFunction* _Wolframe::langbind::createBase64NormalizeFunction( ResourceHandle&, const std::string& name, const std::string& arg)
{
	try
	{
		std::string type = boost::algorithm::to_lower_copy( name);
		if (boost::algorithm::iequals( type, "encode"))
		{
			if (!arg.empty()) std::runtime_error( std::string( "no arguments expected for normalizer '") + name + "'");
			return new Base64EncodeFunction();
		}
		else if (boost::algorithm::iequals( type, "decode"))
		{
			if (!arg.empty()) std::runtime_error( std::string( "no arguments expected for normalizer '") + name + "'");
			return new Base64DecodeFunction();
		}
		else
		{
			throw std::runtime_error( std::string( "unknown number type '") + name + "'");
		}
	}
	catch (const std::runtime_error& e)
	{
		throw std::runtime_error( std::string( "error in base64 normalize function description: ") + e.what());
	}
}


const std::vector<std::string>& _Wolframe::langbind::normalizeFunctions()
{
	struct NormalizeFunctions :public std::vector<std::string>
	{
		NormalizeFunctions()
		{
			push_back( "encode");
			push_back( "decode");
		}
	};
	static NormalizeFunctions rt;
	return rt;
}

