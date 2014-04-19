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
/// \file processor/execContext.hpp
/// \brief Processor Provider
#ifndef _WOLFRAME_PROCESSOR_EXEC_CONTEXT_HPP_INCLUDED
#define _WOLFRAME_PROCESSOR_EXEC_CONTEXT_HPP_INCLUDED
#include "procProviderInterface.hpp"
#include "AAAA/user.hpp"
#include "AAAA/authorization.hpp"

namespace _Wolframe {
namespace proc {

/// \class ExecContext
/// \brief Execution context passed to functions for referencing resources and to define authorization dependend processing
class ExecContext
{
public:
	/// \brief Default Constructor
	ExecContext()
		:m_provider(0){}
	/// \brief Constructor
	ExecContext( const ProcessorProviderInterface* p, const AAAA::User& u, AAAA::Authorizator* a)
		:m_provider(p),m_user(u),m_authorizator(a){}
	/// \brief Copy constructor
	ExecContext( const ExecContext& o)
		:m_provider(o.m_provider),m_user(o.m_user),m_authorizator(o.m_authorizator){}

	/// \brief Get the processor provider interface
	const ProcessorProviderInterface* provider() const	{return m_provider;}
	/// \brief Get the user data
	const AAAA::User& user() const				{return m_user;}
	/// \brief Get the authorization instance interface
	AAAA::Authorizator* authorizator()			{return m_authorizator;}

public:
	const ProcessorProviderInterface* m_provider;		///< processor provider interface
	AAAA::User m_user;					///< user data
	AAAA::Authorizator* m_authorizator;			///< instance to query for execution allowance
};

}} //namespace
#endif

