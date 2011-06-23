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
///\file serialize/appProcessorSystemCall.hpp
///\brief Defines the building of the non intrusive system call interface from an intrusive system call description

#ifndef _Wolframe_APPLICATION_PROCESSOR_SYSTEM_CALL_HPP_INCLUDED
#define _Wolframe_APPLICATION_PROCESSOR_SYSTEM_CALL_HPP_INCLUDED
#include <string>
#include <map>

namespace _Wolframe {
namespace syscall {

///\class DataMapDef
///\brief Defines the non intrusive interface to data mapping from the application processor to the system
struct DataMapDef
{
	void* (*create)();						///< data constructor
	void (*destroy)( void* e);					///< data destructor
	static const serialize::DescriptionBase* (*description)();	///< data serialization/deserialization description
};

///\class DataMap
///\brief Defines a mapping definition of a POD (plain old datatype) struct description
///\tparam POD struct type
template <class POD>
class DataMap :public POD
{
	static void* create()
	{
		return (void*)new e();
	}
	static void destroy( void* e)
	{
		delete (POD*)e;
	}
	static const serialize::DescriptionBase* description();
	static const DataMapDef& getDef()
	{
		static const DataMapDef rt = {&create, &destroy, &description};
		return rt;
	}
};

///\class SystemCallDef
///\brief Uniform definition of a system call from an application processor program
struct SystemCallDef
{
	typedef bool (*Function)( void* engine, void* in, void* out);	///< non intrusive function definition
	DataMapDef inDef;						///< input data description
	DataMapDef outDef;						///< return value data description
	Function function;						///< non intrusive interface of function to call
};

///\class SystemCall
///\brief Definition of a system call from an application processor program
template <class ENGINE, class IN, class OUT>
struct SystemCall
{
	///\brief Execution call interface for the function
	///\param [in,out] engine pointer to the instance executing the command
	///\param [in] in pointer to parameter structure passed to the command as input
	///\param [out] out pointer to structure for the return value of the command
	static bool execute( void* engine, const void* in, void* out)
	{
		return ((ENGINE*)engine)->call(*(const IN*)in, *(OUT*)out);
	}
	///\brief Description of the execute call with all structures used for it
	static const SystemCallDef& getDef()
	{
		static const SystemCallDef rt = {DataMap<IN>::getDef(),DataMap<OUT>::getDef(),&execute};
		return rt;
	}
};

///\class SystemCallTable
///\brief Struture defining all system calls with parameter mapping for the application processor program
struct SystemCallTable :public std::map<std::string,SystemCallDef>
{
	typedef std::map<std::string,SystemCallDef> This;	///< this class type

	///\brief intrusive definition  of a system call
	template <class ENGINE, class IN, class OUT>
	void define( const std::string& name, ENGINE* engine)
	{
		(*This)[ name] = SystemCall<ENGINE,IN,OUT>::getDef();
	}
}

#endif

