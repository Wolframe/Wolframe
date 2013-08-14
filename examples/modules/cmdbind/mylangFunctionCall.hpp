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
///\file mylangFunctionProgramType.hpp
///\brief Function call for mylang scripts
#ifndef _Wolframe_Mylang_FUNCTION_CALL_HPP_INCLUDED
#define _Wolframe_Mylang_FUNCTION_CALL_HPP_INCLUDED
#include <utility>
#include <vector>
#include "types/variant.hpp"
#include "types/countedReference.hpp"
#include "processor/procProvider.hpp"

namespace _Wolframe {
namespace langbind {
namespace mylang {

///\class Instance
///\brief Interpreter instance for executing a function
struct Instance
{
	Instance(){}
};

typedef types::CountedReference<Instance> InstanceR;

///\class Context
///\brief Global interpreter context with all data structures needed to create interpreter instances addressed by function names
struct Context
{
	Context();

	std::vector<std::string> loadProgram( const std::string& name);
	InstanceR getInstance( const std::string& name) const;
};

struct Structure
{
	explicit Structure( const InstanceR& instance_)
		:m_instance(instance_){}

	///\brief Create a substructure and get a pointer to it
	///\remark throws on error
	Structure* addSubstruct( const types::Variant& elemid_);

	void setValue( const types::Variant& value);
	const types::Variant& getValue() const;

	bool atomic() const;

	typedef std::vector<std::pair<types::Variant,Structure*> >::const_iterator const_iterator;
	const_iterator begin() const;
	const_iterator end() const;

private:
	InstanceR m_instance;
};

typedef types::CountedReference<Structure> StructureR;

///\brief Mylang function call
StructureR call( const proc::ProcessorProvider* provider, const StructureR& arg);

}//namespace mylang



}} //namespace
#endif


