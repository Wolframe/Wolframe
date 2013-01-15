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
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
//
// Constructor classes
//

#include "config/configurationBase.hpp"

#ifndef _CONSTRUCTOR_HPP_INCLUDED
#define _CONSTRUCTOR_HPP_INCLUDED

namespace _Wolframe	{

/// Constructor base class
class ObjectConstructorBase
{
public:
	enum ObjectType	{
		AUTHENTICATION_OBJECT,
		AUDIT_OBJECT,
		AUTHORIZATION_OBJECT,
		DATABASE_OBJECT,
		FILTER_OBJECT,
		DDL_COMPILER_OBJECT,
		FORM_FUNCTION_OBJECT,
		NORMALIZE_FUNCTION_OBJECT,
		PRINT_FUNCTION_OBJECT,
		CMD_HANDLER_OBJECT,
		LANGUAGE_EXTENSION_OBJECT,
		TEST_OBJECT
	};

	static const char* objectTypeName( ObjectType tp)
	{
		static const char* ar[] =
		{
			"Authentication",
			"Audit",
			"Authorization",
			"Database",
			"Filter",
			"DLL Compiler",
			"Form Function",
			"Normalize Function",
			"Transaction Function",
			"Print Function",
			"Command Handler",
			"Language Extension",
			"Test"
		};
		return ar[ (int)tp];
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
