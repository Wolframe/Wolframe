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
///\file filter/null_filter.hpp
///\brief Filter that emits an error for beeing undefined instead of doing something when used

#ifndef _Wolframe_FILTER_NULL_HPP_INCLUDED
#define _Wolframe_FILTER_NULL_HPP_INCLUDED
#include "filter/filter.hpp"
#include <string>
#include <vector>

namespace _Wolframe {
namespace langbind {

/// \brief Create a new null filter
Filter createNullFilter();
/// \brief Create a new null filter (with new, to be destroyed with delete)
Filter* createNullFilterPtr( const std::string& name, const std::vector<FilterArgument>& arg);

class NullFilterType :public FilterType
{
public:
	NullFilterType()
		:FilterType("null"){}
	virtual ~NullFilterType(){}

	virtual Filter* create( const std::vector<FilterArgument>& arg) const
	{
		return createNullFilterPtr( "null", arg);
	}
};

}}//namespace
#endif


