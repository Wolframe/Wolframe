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
///\brief Definition of a transaction function preprocessing step based on TDL
///\file tdlTransactionPreprocStep.hpp
#ifndef _DATABASE_TDL_TRANSACTION_PREPROC_STEP_HPP_INCLUDED
#define _DATABASE_TDL_TRANSACTION_PREPROC_STEP_HPP_INCLUDED
#include "vm/selectorPath.hpp"
#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>

namespace _Wolframe {
namespace proc {
/// \brief Forward declaration
class ExecContext;
}

namespace db {
namespace vm {
/// \brief Forward declaration
class TagTable;
/// \brief Forward declaration
class InputStructure;
}//namespace vm

/// \class TdlTransactionPreprocStep
/// \brief Preprocessing step of a transaction
class TdlTransactionPreprocStep
{
public:
	/// \class Argument
	/// \brief Argument of a preprocessing step
	struct Argument
	{
		enum Type
		{
			SelectorPath,	///< path pointing to an input element
			LoopCounter,	///< counter in the FOREACH loop
			Constant	///< constant value
		};

		Type type;
		std::size_t name;
		std::size_t value;

		Argument()
			:type(LoopCounter),name(0),value(0){}
		Argument( const Argument& o)
			:type(o.type),name(o.name),value(o.value){}
		Argument( Type t, const std::size_t n, std::size_t v)
			:type(t),name(n),value(v){}
	};

public:
	TdlTransactionPreprocStep()
	{
		m_strings.push_back('\0');
		m_selectors.push_back( vm::SelectorPath());
	}
	TdlTransactionPreprocStep( const vm::SelectorPath& selector_, const std::string& function_, const std::vector<std::string>& resultpath_=std::vector<std::string>())
		:m_function(function_),m_resultpath(resultpath_)
	{
		m_strings.push_back('\0');
		m_selectors.push_back( selector_);
	}
	TdlTransactionPreprocStep( const TdlTransactionPreprocStep& o)
		:m_selectors(o.m_selectors)
		,m_function(o.m_function)
		,m_arguments(o.m_arguments)
		,m_strings(o.m_strings)
		,m_resultpath(o.m_resultpath){}

public:
	Argument::Type arg_type( std::size_t i) const
	{
		if (m_arguments.size() <= i) throw std::logic_error("array bounds read");
		return m_arguments.at(i).type;
	}

	const char* arg_name( std::size_t i) const
	{
		if (m_arguments.size() <= i) throw std::logic_error("array bounds read");
		if (!m_arguments.at(i).name) return 0;
		return m_strings.c_str() + m_arguments.at(i).name;
	}

	const char* arg_constant( std::size_t i) const
	{
		if (m_arguments.size() <= i) throw std::logic_error("array bounds read");
		if (m_arguments.at(i).type != Argument::Constant) throw std::logic_error("illegal access");
		return m_strings.c_str() + m_arguments.at(i).value;
	}

	bool arg_loopcounter( std::size_t i) const
	{
		if (m_arguments.size() <= i) throw std::logic_error("array bounds read");
		if (m_arguments.at(i).type != Argument::LoopCounter) throw std::logic_error("illegal access");
		return (m_arguments.at(i).type == Argument::LoopCounter);
	}

	const vm::SelectorPath& arg_selector( std::size_t i) const
	{
		if (m_arguments.size() <= i) throw std::logic_error("array bounds read");
		if (m_arguments.at(i).type != Argument::SelectorPath) throw std::logic_error("illegal access");
		return m_selectors.at( m_arguments.at(i).value);
	}

	const vm::SelectorPath& selector() const
	{
		return m_selectors.at( 0);
	}

	void add_arg_constant( const std::string& name, const std::string& value)
	{
		m_arguments.push_back( Argument( Argument::Constant, getStringIdx( name), getStringIdx( value)));
	}

	void add_arg_loopcounter( const std::string& name)
	{
		m_arguments.push_back( Argument( Argument::LoopCounter, getStringIdx( name), 0));
	}

	void add_arg_selector( const std::string& name, const vm::SelectorPath& value)
	{
		m_arguments.push_back( Argument( Argument::SelectorPath, getStringIdx( name), m_selectors.size()));
		m_selectors.push_back( value);
	}

	const std::vector<std::string>& resultpath() const
	{
		return m_resultpath;
	}

	const std::string& function() const
	{
		return m_function;
	}

	std::size_t nof_args()
	{
		return m_arguments.size();
	}

	void print( std::ostream& out, const vm::TagTable* tagmap) const;
	std::string tostring( const vm::TagTable* tagmap) const;

	void call( proc::ExecContext* context, vm::InputStructure& structure) const;

private:
	std::size_t getStringIdx( const std::string& value)
	{
		std::size_t stringidx = 0;
		if (!value.empty())
		{
			stringidx = m_strings.size();
			m_strings.append( value);
			m_strings.push_back( '\0');
		}
		return stringidx;
	}

private:
	std::vector<vm::SelectorPath> m_selectors;
	std::string m_function;
	std::vector<Argument> m_arguments;
	std::string m_strings;
	std::vector<std::string> m_resultpath;
};

}}//namespace
#endif

