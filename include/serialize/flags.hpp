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
/// \file serialize/flags.hpp
/// \brief Defines the flags for serialization behaviour

#ifndef _Wolframe_SERIALIZE_FLAGS_HPP_INCLUDED
#define _Wolframe_SERIALIZE_FLAGS_HPP_INCLUDED

namespace _Wolframe {
namespace serialize {

/// \struct Flags
/// \brief Set of flags to negotiate source behaviour in serialization and sink behaviour in validation
struct Flags
{
	/// \enum Enum
	/// \brief the enumeration that can also be interpreted as bit set
	enum Enum
	{
		None=0x00,				///< Empty flag set (no flags set)
		ValidateAttributes=0x01,		///< Do validate if attribute value pairs are serialized as such in the input filter serialization (and not as open content close) and the input filter does not declare itself as not knowing about attributes (PropagateNoAttr)
		ValidateInitialization=0x02,		///< Do validate if all declared elements in the structure were initilized by the input stream (if not explicitely declared as optional)
		SerializeWithIndices=0x04,		///< Do produce array elements with indices to help the receiver to distinguish between arrays with one element and single elements 
		CaseInsensitiveCompare=0x08		///< Do validate with case insensitive compare
	};
};

}}//namespace
#endif
