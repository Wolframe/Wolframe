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
///\file pythonStructureBuilder.hpp
///\brief Interface to python data structure building
#ifndef _Wolframe_python_STRUCTURE_BUILDER_HPP_INCLUDED
#define _Wolframe_python_STRUCTURE_BUILDER_HPP_INCLUDED
#include "pythonStructure.hpp"
#include "types/variant.hpp"
#include "types/countedReference.hpp"
#include "utils/printFormats.hpp"
#include <utility>
#include <vector>
#include <string>

namespace _Wolframe {
namespace langbind {
namespace python {

///\class StructureBuilder
///\brief Data structure builder
class StructureBuilder
{
public:
	///\brief Constructor
	StructureBuilder();
	///\brief Copy constructor
	StructureBuilder( const StructureBuilder& o)
		:m_stk(o.m_stk){}
	///\brief Destructor
	virtual ~StructureBuilder(){}

	///\brief Create an element in a structure and puts the focus on it
	///\param[in] elemid_ Id of the created element
	///\remark Throws on error
	void openElement( const std::string& elemid_);

	///\brief Ends the construction of the current element and puts the focus on the parent
	void closeElement();

	///\brief Create an element in an array and puts the focus on it
	///\remark Throws on error
	void openArrayElement();

	///\brief Setter for element value in case of this representing an atom
	///\param[in] value value or content element of 'this'
	void setValue( const types::Variant& value_);

	///\brief Get the index of the last element in case of an array
	std::size_t lastArrayIndex() const;

	///\brief Get a reference to the structure build
	StructureR get() const;

	///\brief Get the hierarchy level of the currently visited element (0 for top level)
	int taglevel() const;

	///\brief Find out if the currently visited element is defined
	bool defined() const;
	///\brief Find out if the currently visited element is atomic
	bool atomic() const;

	///\brief Get the path of the currently visited element as string
	std::string currentElementPath() const;

	///\brief Clear the current builder
	void clear();

private:
	struct StackElement
	{
		bool m_arrayelem;
		std::string m_key;
		Object m_value;

		StackElement( const StackElement& o)
			:m_arrayelem(o.m_arrayelem),m_key(o.m_key),m_value(o.m_value){}
		StackElement()
			:m_arrayelem(true){}
		StackElement( const std::string& key_)
			:m_arrayelem(false),m_key(key_){}
	};
	std::vector<StackElement> m_stk;	//< builder stack
};

}}}//namespace
#endif

