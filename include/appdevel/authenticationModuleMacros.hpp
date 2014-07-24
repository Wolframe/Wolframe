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
/// \file appdevel/authenticationModuleMacros.hpp
/// \brief Macros for a module for defining an authentication mechanism
#include "appdevel/module/authenticationConstructor.hpp"
#include "module/moduleInterface.hpp"
#include "module/constructor.hpp"
#include <boost/lexical_cast.hpp>

/// \brief Defines a an authentication mechanism
#define WF_AUTHENTICATOR(NAME,UNITCLASS,CONFIGCLASS) \
{\
	struct Builder \
	{\
		static _Wolframe::module::BuilderBase* impl()\
		{\
			static _Wolframe::module::ConfiguredBuilderDescription<\
					_Wolframe::module::AuthenticationConstructor<UNITCLASS, CONFIGCLASS>,\
					CONFIGCLASS >\
				mod( "Authentication " #NAME, "Authentication", #NAME, #NAME "Authentication");\
			return &mod;\
		}\
	};\
	(*this)(&Builder::impl);\
}


