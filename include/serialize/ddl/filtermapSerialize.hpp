/************************************************************************
Copyright (C) 2011 Project Wolframe.
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
///\file serialize/struct/filtermapSerialize.hpp
///\brief Defines the DDL structure serialization for filters

#ifndef _Wolframe_SERIALIZE_DDL_FILTERMAP_HPP_INCLUDED
#define _Wolframe_SERIALIZE_DDL_FILTERMAP_HPP_INCLUDED
#include "protocol/inputfilter.hpp"
#include "protocol/outputfilter.hpp"
#include "serialize/mapContext.hpp"
#include "ddl/structType.hpp"
#include <cstddef>

namespace _Wolframe {
namespace serialize {

bool parse( ddl::StructType& st, protocol::InputFilter& flt, Context& ctx);
bool print( const ddl::StructType& st, protocol::OutputFilter*& out, Context& ctx);

}}//namespace
#endif

