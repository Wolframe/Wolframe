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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
///\file mylangStructure.hpp
///\brief Interface to mylang data structure representing input/output of a mylang form function
#ifndef _Wolframe_Mylang_STRUCTURE_HPP_INCLUDED
#define _Wolframe_Mylang_STRUCTURE_HPP_INCLUDED
#include "types/variant.hpp"
#include "types/countedReference.hpp"
#include <vector>
#include <string>

namespace _Wolframe {
namespace langbind {
namespace mylang {

///\brief Formward declaration
class InterpreterInstance;

///\class Structure
///\brief Data structure for input and output of a 'Mylang' function call
class Structure
{
public:
	///\brief Constructor
	explicit Structure( InterpreterInstance* instance_)
		:m_instance(instance_),m_array(false){}
	virtual ~Structure(){}

	///\brief Create an element in a structure and get a reference pointer to it
	///\param[in] elemid_ Id of the created element
	///\remark Throws on error
	///\remark Only a reference is returned; the disposal of the structure (ownership) is up to 'this'
	Structure* addStructElement( const std::string& elemid_);

	///\brief Create an element in an array and get a reference pointer to it
	///\param[in] elemid_ Id of the array
	///\remark Throws on error
	///\remark Only a reference is returned; the disposal of the structure (ownership) is up to 'this'
	Structure* addArrayElement();

	///\brief Setter for element value in case of this representing an atom
	///\param[in] value value or content element of 'this'
	void setValue( const types::Variant& value_);
	///\brief Getter for element value in case of this representing an atom
	const types::Variant& getValue() const;

	///\brief Find out if 'this' represents an atomic value
	///\return true, if 'this' represents an atomic value
	bool atomic() const;
	///\brief Find out if 'this' represents an array of 'Structure'
	///\return true, if 'this' represents an array of 'Structure'
	bool array() const;

	///\brief Iterator on structure or array elements
	typedef std::pair<types::Variant,Structure*> KeyValuePair;
	typedef std::vector<KeyValuePair>::const_iterator const_iterator;
	///\brief Get the start iterator on structure or array elements
	const_iterator begin() const;
	///\brief Get the end marker for a structure or and array
	const_iterator end() const;

private:
	InterpreterInstance* m_instance;	//< interpreter instance
	std::vector<KeyValuePair> m_struct;	//< mimic language structure
	types::Variant m_value;			//< value for atomic element
	bool m_array;				//< true, if this represents an array
};

///\brief Reference with ownership to a structure
typedef types::CountedReference<Structure> StructureR;

}}}//namespace
#endif

