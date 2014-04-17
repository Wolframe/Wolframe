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
///\file pythonStructure.hpp
///\brief Interface to python data structure representing input/output of a python form function
#ifndef _Wolframe_python_STRUCTURE_HPP_INCLUDED
#define _Wolframe_python_STRUCTURE_HPP_INCLUDED
#include "pythonObject.hpp"
#include "pythonObjectIterator.hpp"
#include "types/variant.hpp"
#include "utils/printFormats.hpp"
#include <vector>
#include <string>
#include <ostream>
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace langbind {
namespace python {

///\class Structure
///\brief Data structure for input and output of a 'python' function call
class Structure
	:public Object
{
public:
	///\brief Constructor
	Structure(){}
	///\brief Copy constructor
	Structure( const Structure& o)			:Object(o){}
	///\brief Copy constructor
	Structure( const Object& o)			:Object(o){}
	///\brief Constructor
	Structure( PyObject* o, bool isborrowed=true)	:Object(o,isborrowed){}

	typedef ObjectIterator const_iterator;

	///\brief Get the start iterator on structure or array elements
	const_iterator begin() const	{return const_iterator( *this);}
	///\brief Get the end marker for a structure or and array
	const_iterator end() const	{return const_iterator();}

	///\brief Print the structure serialized as string to out
	void print( std::ostream& out, const utils::PrintFormat* pformat, std::size_t level) const;
	///\brief Get the structure serialized as string for output
	std::string tostring( const utils::PrintFormat* pformat=utils::logPrintFormat()) const;
};

///\brief Reference with ownership to a structure
typedef boost::shared_ptr<Structure> StructureR;

}}}//namespace
#endif

