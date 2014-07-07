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
/// \file langbind/authorizationFunction.hpp
/// \brief Interface of the authorization function

#ifndef _Wolframe_TYPES_AUTHORIZATION_FUNCTION_HPP_INCLUDED
#define _Wolframe_TYPES_AUTHORIZATION_FUNCTION_HPP_INCLUDED
#include "types/variant.hpp"
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace proc {
/// \brief Forward declaration
class ExecContext;
}
namespace langbind {

/// \class AuthorizationFunction
/// \brief Interface of a an authorization function
class AuthorizationFunction
{
public:
	/// \brief Default constructor
	AuthorizationFunction(){}
	/// \brief Destructor
	virtual ~AuthorizationFunction(){}

	/// \class Attribute
	/// \brief Key value pair as authorization function result
	struct Attribute
	{
		std::string name;
		types::Variant value;

		Attribute(){}
		Attribute( const Attribute& o)
			:name(o.name),value(o.value){}
		Attribute( const std::string& name_, const types::Variant& value_)
			:name(name_),value(value_){}
	};

	virtual bool call( proc::ExecContext* ctx, const std::string& resource, std::vector<Attribute>& attributes) const=0;
};

/// \brief shared ownership reference of an authorization function
typedef boost::shared_ptr<AuthorizationFunction> AuthorizationFunctionR;

}}//namespace
#endif

