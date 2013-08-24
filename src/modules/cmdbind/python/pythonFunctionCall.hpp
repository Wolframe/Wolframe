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
///\file pythonFunctionProgramType.hpp
///\brief Function call for Python scripts
#ifndef _Wolframe_PYTHON_FUNCTION_CALL_HPP_INCLUDED
#define _Wolframe_PYTHON_FUNCTION_CALL_HPP_INCLUDED
#include <utility>
#include <vector>
#include "types/variant.hpp"
#include "types/countedReference.hpp"
#include "processor/procProvider.hpp"

namespace _Wolframe {
namespace langbind {
namespace python {

///\class Instance
///\brief Interpreter instance for executing a function
class Instance
{
public:
	///\brief Constructor
	Instance(){}
	///\brief Destructor
	virtual ~Instance(){}

	///\brief Find out if the language binding needs indices (starting with '1') for array elements in the input to distinguish between single elements and arrays with one element
	///\return true, if yes
	bool needsArrayIndices() const		{return true;}
};

typedef types::CountedReference<Instance> InstanceR;

///\class Context
///\brief Global interpreter context with all data structures needed to create interpreter instances addressed by function names
class Context
{
public:
	///\brief Constructor
	Context();
	///\brief Destructor
	virtual ~Context(){}

	std::vector<std::string> loadProgram( const std::string& name);
	InstanceR getInstance( const std::string& name) const;
};

///\class Structure
///\brief Data structure for input and output of a Python function call
class Structure
{
public:
	///\brief Constructor
	explicit Structure( const InstanceR& instance_)
		:m_instance(instance_){}
	virtual ~Structure(){}

	///\brief Create a substructure and get a reference pointer to it
	///\param[in] elemid_ Id of the created element
	///\remark Throws on error
	///\remark Only a reference is returned; the disposal of the structure (ownership) is up to 'this'
	Structure* addSubstruct( const types::Variant& elemid_);

	///\brief Setter for element value in case of an 'atomic' element or setter for content element in case of a structure
	///\param[in] value value or content element of 'this'
	void setValue( const types::Variant& value);
	///\brief Getter for element value in case of an 'atomic' element or getter for content element in case of a structure
	const types::Variant& getValue() const;

	///\brief Find out if 'this' represents an atomic value
	///\return true, if 'this' represents an atomic value
	bool atomic() const;
	///\brief Find out if 'this' represents an array of 'Structure'
	///\return true, if 'this' represents an array of 'Structure'
	bool array() const;

	///\brief Iterator on structure or array elements
	typedef std::vector<std::pair<types::Variant,Structure*> >::const_iterator const_iterator;
	///\brief Get the start iterator on structure or array elements
	const_iterator begin() const;
	///\brief Get the end marker for a structure or and array
	const_iterator end() const;

private:
	InstanceR m_instance;		//< interpreter instance
};

///\brief Reference with ownership to a structure
typedef types::CountedReference<Structure> StructureR;

///\brief Call a function written in Python
StructureR call( const proc::ProcessorProvider* provider, Instance* instance, const StructureR& arg);

}//namespace python


}} //namespace
#endif


