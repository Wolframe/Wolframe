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
///\file stringNormalize.cpp
///\brief Implementation of normalization functions for names without ICU
#include "stringNormalize.hpp"
#include "trimNormalizeFunction.hpp"
#include "asciiNormalizeFunction.hpp"
#include <boost/algorithm/string.hpp>
#include <limits>

using namespace _Wolframe;
using namespace _Wolframe::langbind;

types::NormalizeFunction* _Wolframe::langbind::createTrimNormalizeFunction( ResourceHandle&, const std::string& arg)
{
	try
	{
		if (!arg.empty()) std::runtime_error( "no arguments expected for normalizer 'string:trim'");
		return new TrimNormalizeFunction();
	}
	catch (const std::runtime_error& e)
	{
		throw std::runtime_error( std::string( "error in description of 'string:trim' normalize function: ") + e.what());
	}
}

types::NormalizeFunction* _Wolframe::langbind::createConvdiaNormalizeFunction( ResourceHandle&, const std::string& arg)
{
	try
	{
		if (!arg.empty()) std::runtime_error( "no arguments expected for normalizer 'string:convdia'");
		return new ConvDiaNormalizeFunction();
	}
	catch (const std::runtime_error& e)
	{
		throw std::runtime_error( std::string( "error in description of 'string:convdia' normalize function: ") + e.what());
	}
}

types::NormalizeFunction* _Wolframe::langbind::createUcnameNormalizeFunction( ResourceHandle&, const std::string& arg)
{
	try
	{
		if (!arg.empty()) std::runtime_error( "no arguments expected for normalizer 'string:ucname'");
		return new UppercaseNameNormalizeFunction();
	}
	catch (const std::runtime_error& e)
	{
		throw std::runtime_error( std::string( "error in description of 'string:ucname' normalize function: ") + e.what());
	}
}

types::NormalizeFunction* _Wolframe::langbind::createLcnameNormalizeFunction( ResourceHandle&, const std::string& arg)
{
	try
	{
		if (!arg.empty()) std::runtime_error( "no arguments expected for normalizer 'string:lcname'");
		return new LowercaseNameNormalizeFunction();
	}
	catch (const std::runtime_error& e)
	{
		throw std::runtime_error( std::string( "error in description of 'string:lcname' normalize function: ") + e.what());
	}
}


