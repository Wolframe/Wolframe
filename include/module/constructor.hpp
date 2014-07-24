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
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
/// \file module/constructor.hpp
/// \brief Base classes for virtual constructors to build objects loaded from modules

#include "config/configurationBase.hpp"

#ifndef _CONSTRUCTOR_HPP_INCLUDED
#define _CONSTRUCTOR_HPP_INCLUDED

namespace _Wolframe	{

/// Constructor base class
class ObjectConstructorBase
{
public:
	enum ObjectType	{
		AUTHENTICATION_OBJECT		=0x0010,
		AUTHORIZATION_OBJECT		=0x0020,
		AUDIT_OBJECT			=0x0110,
		DATABASE_OBJECT			=0x0210,
		JOB_SCHEDULE_OBJECT		=0x0220,
		FILTER_OBJECT			=0x0310,
		FORM_FUNCTION_OBJECT		=0x0410,
		NORMALIZE_FUNCTION_OBJECT	=0x0420,
		CUSTOM_DATA_TYPE_OBJECT		=0x0430,
		DOCTYPE_DETECTOR_OBJECT		=0x0510,
		CMD_HANDLER_OBJECT		=0x0610,
		PROGRAM_TYPE_OBJECT		=0x0710,
		DDL_COMPILER_OBJECT		=0x0720,
		RUNTIME_ENVIRONMENT_OBJECT	=0x0730,
		TEST_OBJECT			=0x9990
	};

	static const char* objectTypeName( ObjectType tp)
	{
		switch (tp)
		{
			case AUTHENTICATION_OBJECT: return "Authentication";
			case AUTHORIZATION_OBJECT: return "Authorization";
			case AUDIT_OBJECT: return "Audit";
			case DATABASE_OBJECT: return "Database";
			case JOB_SCHEDULE_OBJECT: return "Job Schedule Object";
			case FILTER_OBJECT: return "Filter";
			case FORM_FUNCTION_OBJECT: return "Form Function";
			case NORMALIZE_FUNCTION_OBJECT: return "Normalize Function";
			case CUSTOM_DATA_TYPE_OBJECT: return "Custom Data Type";
			case DOCTYPE_DETECTOR_OBJECT: return "Document Type/Format Detector";
			case CMD_HANDLER_OBJECT: return "Command Handler";
			case PROGRAM_TYPE_OBJECT: return "Program Type";
			case DDL_COMPILER_OBJECT: return "DLL Compiler";
			case RUNTIME_ENVIRONMENT_OBJECT: return "Runtime Environment";
			case TEST_OBJECT: return "#Test#";
		}
		return "Unknown module type";
	}

	const char* objectTypeName()
	{
		return objectTypeName( objectType());
	}

	virtual ~ObjectConstructorBase()	{}

	///
	virtual const char* objectClassName() const = 0;

	/// The type of the object. Filter, DDL compiler, authentication etc.
	virtual ObjectType objectType() const = 0;
};

// Templates of specialized constructors
/// Constructor of a configured object
template < class T >
class ConfiguredObjectConstructor : public ObjectConstructorBase
{
public:
	virtual ~ConfiguredObjectConstructor()	{}
	virtual T* object( const config::NamedConfiguration& conf ) = 0;
};

/// Constructor of a simple (without configuration) object
template < class T >
class SimpleObjectConstructor : public ObjectConstructorBase
{
public:
	virtual ~SimpleObjectConstructor()	{}

// One of these functions should be here in order to have a real contract
//	virtual T* object() const = 0;
//	virtual T* object( std::string& arg ) const = 0;
};

} // namespace _Wolframe

#endif // _CONSTRUCTOR_HPP_INCLUDED
