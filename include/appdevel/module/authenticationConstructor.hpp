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
/// \file appdevel/module/authenticationConstructor.hpp
/// \brief Interface to constructors of authenticators
#ifndef _Wolframe_MODULE_AUTHENTICATION_CONSTRUCTOR_HPP_INCLUDED
#define _Wolframe_MODULE_AUTHENTICATION_CONSTRUCTOR_HPP_INCLUDED
#include "module/constructor.hpp"
#include "module/moduleInterface.hpp"
#include "module/constructor.hpp"
#include "AAAA/authUnit.hpp"
#include <boost/lexical_cast.hpp>
#include <string>

namespace _Wolframe {
namespace module {

/// \class AuthenticationConstructor
/// \brief Constructor of an authentication unit
template<class UNIT, class CONFIG>
class AuthenticationConstructor
	:public _Wolframe::ConfiguredObjectConstructor<AAAA::AuthenticationUnit>
{
public:
	AuthenticationConstructor(){}

	virtual ~AuthenticationConstructor(){}
	virtual UNIT* object( const _Wolframe::config::NamedConfiguration& cfgi)
	{
		const CONFIG* cfg = dynamic_cast<const CONFIG*>(&cfgi);
		if (!cfg) throw std::logic_error( "internal: wrong configuration interface passed to authentication constructor");
		UNIT* rt = new UNIT( *cfg);
		return rt;
	}

	virtual ObjectConstructorBase::ObjectType objectType() const
	{
		return ObjectConstructorBase::AUTHENTICATION_OBJECT;
	}
	
	virtual const char* objectClassName() const
	{
		return "AuthenticationConstructor";
	}
};

}} //namespace
#endif

