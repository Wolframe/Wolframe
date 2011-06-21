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
///\file config/descriptionBase.hpp
///\brief Defines the non intrusive description of an application processor object (the implementation is intrusive and defined in serialize/description.hpp)

#ifndef _Wolframe_SERIALIZE_DESCRIPTION_BASE_HPP_INCLUDED
#define _Wolframe_SERIALIZE_DESCRIPTION_BASE_HPP_INCLUDED
#include <boost/property_tree/ptree.hpp>
#include <vector>
#include <string>
#include <cstddef>

namespace _Wolframe {
namespace serialize {

///\struct AppProcessorObject
///\brief brief A forward declaration for the application processor object reference
struct AppProcessorObject;

///\brief A application processor object serialization description
struct DescriptionBase
{
	typedef void (*Serialize)( void* obj, std::size_t ofs, AppProcessorObject* appobj);
	typedef void (*Deserialize)( void* obj, std::size_t ofs, AppProcessorObject* appobj);

	///\brief A description of one item of the application processor object serialization POD structure
	struct Item
	{
		std::size_t m_ofs;		///< member position offset in bytes from the start of the structure
		std::string m_name;		///< member name
		Serialize m_serialize;		///< serialization member function
		Deserialize m_deserialize;	///< deserialization member function

		Item( std::size_t ofs, const std::string& name, Serialize s, Deserialize d)
			:m_ofs(ofs),m_name(name),m_serialize(s),m_deserialize(d){}
		Item( const Item& o)
			:m_ofs(o.m_ofs),m_name(o.m_name),m_serialize(o.m_serialize),m_deserialize(o.m_deserialize){}
	};
	std::vector<Item> m_ar;

	///\brief Fills the structure with values serialized from the application processor object
	///\param[in,out] obj structure to fill
	///\param[in] appobj application processor object to get as C/C++ POD structure
	///\param[out] errmsg the error message in case of failure (return false)
	///\return true if Ok, false if failed
	bool serialize( void* obj, AppProcessorObject* appobj, std::string& errmsg) const;

	///\brief Build the application processor object out of its serialized representation
	///\param[out] appobj application processor object reference to push deserialized element to
	///\param[in] obj serialized object representation
	void deserialize( AppProcessorObject* appobj, void* obj) const;
};


}}// end namespace
#endif
