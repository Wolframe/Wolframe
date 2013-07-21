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
///\file modules/normalize/locale/localeNormalize.hpp
///\brief Interface for normalization functions based on boost locale (ICU)
#ifndef _LANGBIND_LOCALE_NORMALIZE_HPP_INCLUDED
#define _LANGBIND_LOCALE_NORMALIZE_HPP_INCLUDED
#include "langbind/normalizeFunction.hpp"
#include <vector>
#include <boost/locale/generator.hpp>

namespace _Wolframe {
namespace langbind {

class ResourceHandle
{
public:
	ResourceHandle()
	{
		m_gen.locale_cache_enabled( true);
	}

	std::locale getLocale( const std::string& name)
	{
		return m_gen( name);
	}
private:
	boost::locale::generator m_gen;
};

types::NormalizeFunction* createLocaleNormalizeFunction( ResourceHandle& reshnd, const std::string& name, const std::string& arg);
const std::vector<std::string>& normalizeFunctions();

}}//namespace
#endif

