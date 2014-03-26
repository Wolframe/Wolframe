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
///\brief Interface for describing the authorization procedure for a transaction or command to execute
///\file langbind/authorization.hpp
//
#ifndef _LANGBIND_AUTHORIZATION_HPP_INCLUDED
#define _LANGBIND_AUTHORIZATION_HPP_INCLUDED

#include <string>

namespace _Wolframe {
namespace langbind {

///\class Authorization
///\brief Structure describing the authorization procedure for a transaction or command to execute
class Authorization
{
public:
	///\brief Default constructor
	Authorization(){}
	///\brief Copy constructor
	Authorization( const Authorization& o)
		:m_function(o.m_function)
		,m_resource(o.m_resource){}
	///\brief Constructor
	Authorization( const std::string& f, const std::string& r)
		:m_function(f)
		,m_resource(r){}

	///\brief Get function name
	const std::string& function() const;
	///\brief Get resource name
	const std::string& resource() const;

	///\brief Set function and resource name
	void init( const std::string& f, const std::string& r)
	{
		m_function = f;
		m_resource = r;
	}

private:
	std::string m_function;
	std::string m_resource;
};

}} //namespace

#endif
