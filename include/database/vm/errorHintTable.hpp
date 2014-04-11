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
///\file database/vm/errorHintTable.hpp
///\brief Defines the mapping of database statement ids to messages to attach on certain error classes (user hints)
#ifndef _DATABASE_VIRTUAL_MACHINE_ERROR_HINT_TABLE_HPP_INCLUDED
#define _DATABASE_VIRTUAL_MACHINE_ERROR_HINT_TABLE_HPP_INCLUDED
#include "database/vm/instructionSet.hpp"
#include <map>
#include <vector>
#include <string>
#include <cstdlib>
#include <boost/algorithm/string.hpp>

namespace _Wolframe {
namespace db {
namespace vm {

struct ErrorHint
{
	std::string errorclass;
	std::string message;
	ErrorHint(){}
	ErrorHint( const ErrorHint& o)
		:errorclass(o.errorclass),message(o.message){}
	ErrorHint( const std::string& e, const std::string& m)
		:errorclass(e),message(m){}
};

class ErrorHintTable
{
public:
	typedef InstructionSet::ArgumentIndex Index;

public:
	ErrorHintTable(){}
	ErrorHintTable( const ErrorHintTable& o)
		:m_hintar(o.m_hintar)
	{}

	const char* findHint( const Index& idx, const std::string& errorclass) const
	{
		if (idx > m_hintar.size() || idx == 0) throw std::runtime_error("hint table index out of bounds");
		std::vector<ErrorHint>::const_iterator hi = m_hintar.at( idx-1).begin(), he = m_hintar.at( idx-1).end();
		for (; hi != he; ++hi)
		{
			if (boost::algorithm::iequals( errorclass, hi->errorclass))
			{
				return hi->message.c_str();
			}
		}
		return 0;
	}

	Index startdef()
	{
		if (m_hintar.size() >= InstructionSet::Max_ArgumentIndex) throw std::runtime_error("number of defined hint sets out of bounds");
		m_hintar.push_back( std::vector<ErrorHint>());
		return m_hintar.size();
	}

	void define( const std::string& errorclass, const std::string& message)
	{
		if (m_hintar.empty()) throw std::logic_error( "hint table definition without start");
		std::vector<ErrorHint>& top = m_hintar.back();
		if (findHint( m_hintar.size(), errorclass)) throw std::runtime_error( std::string( "duplicate definition of hint for error class '") + errorclass + "' for same statement");
		top.push_back( ErrorHint( errorclass, message));
	}

	PatchArgumentMapR join( const ErrorHintTable& oth)
	{
		std::size_t ofs = m_hintar.size();
		m_hintar.insert( m_hintar.end(), oth.m_hintar.begin(), oth.m_hintar.end());
		return PatchArgumentMapR( new PatchArgumentMap_Offset( ofs));
	}

	typedef std::vector< std::vector<ErrorHint> >::const_iterator const_iterator;
	const_iterator begin() const	{return m_hintar.begin();}
	const_iterator end() const	{return m_hintar.end();}

private:
	typedef std::vector< std::vector<ErrorHint> > ErrorHintList;
	ErrorHintList m_hintar;
};

}}}//namespace
#endif

