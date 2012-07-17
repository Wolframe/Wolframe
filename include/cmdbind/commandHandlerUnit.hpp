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
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
///\file cmdbind/commandHandlerUnit.hpp
///\brief Interface to command handler units
#ifndef _Wolframe_CMDBIND_COMMAND_HANDLER_UNIT_HPP_INCLUDED
#define _Wolframe_CMDBIND_COMMAND_HANDLER_UNIT_HPP_INCLUDED
#include "cmdbind/commandHandler.hpp"
#include <string>
#include <map>
#include <stdexcept>

namespace _Wolframe {
namespace cmdbind {

///\class CommandHandlerUnit
///\brief Set of command handlers adressable by identifiers
class CommandHandlerUnit
{
public:
	///\brief virtual destructor
	virtual ~CommandHandlerUnit()		{}

	///\brief Get an instance of the command handler for the command
	///\param [in] command the name of the command to be executed by the handler
	///\returns pointer to a CommandHandler object or NULL if none is available for command
	virtual CommandHandler* handler( const std::string& command)
	{
		std::map<std::string,Constructor>::const_iterator itr = m_constructormap.find( command);
		if (itr == m_constructormap.end()) return 0;
		return (*itr->second)();
	}

	///\brief Define command handler constructor of a certain type for a certain name
	///\tparam CommandHandlerType the command handler class
	///\param[in] command the name of the command
	template <class CommandHandlerType>
	void defineHandler( const std::string& command)
	{
		struct _ { static CommandHandler* constructor() {return new CommandHandlerType();}};
		std::map<std::string,Constructor>::const_iterator itr = m_constructormap.find( command);
		if (itr != m_constructormap.end()) throw std::runtime_error( "Duplicate definition of command handler");
		m_constructormap[ command] = &_::constructor;
	}

public:
	///\brief Command handler constructor
	typedef CommandHandler* (*Constructor)();

	///\class const_iterator
	///\brief Iterator to enumerate the commands for the unit
	class const_iterator
	{
	public:
		const_iterator()				{}
		const_iterator( const const_iterator& o)	:m_itr(o.m_itr){}

		const std::string& operator->() const		{return m_itr->first;}
		const std::string& operator*() const		{return m_itr->first;}
		const_iterator& operator++()			{++m_itr; return *this;}
		const_iterator operator++(int)			{const_iterator rt(*this); ++m_itr; return rt;}

	private:
		typedef std::map<std::string,Constructor>::const_iterator Itr;
		Itr m_itr;
		friend class CommandHandlerUnit;
			const_iterator( const Itr& i)		:m_itr(i){}
	};

	const_iterator begin() const				{return const_iterator(m_constructormap.begin());}
	const_iterator end() const				{return const_iterator(m_constructormap.end());}

private:
	std::map<std::string,Constructor> m_constructormap;
};

}}//namespace

#endif
