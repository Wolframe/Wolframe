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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
///
/// \file protocol/parser.hpp
/// \brief Parsing functions of the lexical elements of the protocol like commands, lines and tokens
///

#ifndef _Wolframe_PROTOCOL_PARSER_HPP_INCLUDED
#define _Wolframe_PROTOCOL_PARSER_HPP_INCLUDED
#include <stdexcept>
#include <algorithm>
#include <vector>
#include <boost/cstdint.hpp>
#include <boost/algorithm/string.hpp>
#include <ctype.h>

namespace _Wolframe {
namespace protocol {
///
/// \class Parser
/// \brief helper functions for protocol parsing
///
class Parser
{
public:
	/// \brief Go to the next non space
	/// \tparam IteratorType iterator type used as input for parsing
	/// \param [in,out] src input iterator
	/// \param [in] end iterator marking end of input block
	template <typename IteratorType>
	static bool skipSpaces( IteratorType& src, IteratorType& end)
	{
		while (src < end && (*src == ' ' || *src == '\t')) ++src;
		return (src<end);
	}

	/// \brief Go to the end of line without consuming it
	/// \tparam IteratorType iterator type used as input for parsing
	/// \param [in,out] src input iterator
	/// \param [in] end iterator marking end of input block
	template <typename IteratorType>
	static bool skipLine( IteratorType& src, IteratorType& end)
	{
		while (src < end && *src != '\r' && *src != '\n') ++src;
		return (src<end);
	}

	/// \brief Consume the end of line
	/// \tparam IteratorType iterator type used as input for parsing
	/// \param [in,out] src input iterator
	/// \param [in] end iterator marking end of input block
	template <typename IteratorType>
	static bool consumeEOLN( IteratorType& src, IteratorType& end)
	{
		if (src == end) return false;
		if (*src == '\r') src++;
		if (src == end) return false;
		if (*src != '\n') return false;
		src++;
		return true;
	}

	/// \brief Return true if the end of line has been reached
	/// \tparam IteratorType iterator type used as input for parsing
	/// \param [in] src input iterator
	template <typename IteratorType>
	static bool isEOLN( IteratorType& src)
	{
		return (*src == '\r' || *src == '\n');
	}

	/// \brief Parse the current line and write its contents to a buffer
	/// Go to the end of line and write it to buffer, starting with the first non space.
	/// After call the iterator is pointing to the end of line char or at the EOF char.
	/// \remark End of lines or trailing spaces are not implicitely consumed because this leads to intermediate states that cannot be handled
	/// \remark Control characters are mapped to space (ascii 32) before writing them to 'buf'
	/// \param [in,out] src input iterator
	/// \param [in] end input iterator marking the end of input
	/// \param [in,out] buf output buffer implementing the STL back insertion sequence interface
	/// \return true, if the line was consumed completely
	template <typename IteratorType, typename BufferType>
	static bool getLine( IteratorType& src, IteratorType& end, BufferType& buf)
	{
		if (buf.size() == 0) if (!skipSpaces( src, end)) return false;

		while (src < end && *src != '\n' && *src != '\r')
		{
			if (*src <= ' ') {buf.push_back(' '); ++src; continue;}
			buf.push_back(*src);
			++src;
		}
		return (src < end);
	}
};

/// \class CmdMap
/// \brief implements a map for set of protocol commands
/// \remark implementation as vector because the set of commands is expected to be small.
struct CmdMap :public std::vector<std::string>
{
	/// \brief predicate for case insensitive comparison in boost::algorithm::starts_with
	struct ToLowerPredicate
	{
		bool operator()( const char& ch1, const char& ch2) const {return tolower(ch1)==tolower(ch2);}
	};

	/// \brief retrieve a defined commands index from the map
	/// \param val value to retrieve
	/// \return the index of the command or -1 if not found or not determined enough
	int operator []( const char* value) const
	{
		int rt = -1;
		unsigned int cnt = 0;
		unsigned int valuesize = strlen(value);

		for (const_iterator itr = begin(); itr != end(); itr++)
		{
			if (boost::algorithm::starts_with( *itr, value, ToLowerPredicate()))
			{
				if (valuesize == itr->size())
				{
					return itr-begin(); ///< exact match is always returned
				}
				if (!cnt) rt = itr-begin();
				cnt++;
			}
		}
		return (cnt==1) ? rt:-1;
	}

	/// \brief insert a command into the map.
	/// \param val value to insert
	void insert( const value_type& val)
	{
		push_back( val);
	}
};

/// \class CmdParser
/// \exception Bad
/// \brief Parser for ascii protocol commands
/// \tparam CmdBufferType buffer type to use, implementing the STL back insertion sequence interface
/// \tparam CmdMapType map for the commands defined, implementing the STL back insertion sequence interface for adding and a get(value_type) method for matching
///
template <typename CmdBufferType, class CmdMapType=CmdMap>
class CmdParser :public Parser
{
public:
	/// \brief Constructor
	///
	CmdParser() {}

	/// \brief Destructor
	///
	~CmdParser() {}

	/// \brief Copy constructor
	/// \param [in] o CmdParser to copy
	///
	CmdParser( const CmdParser& o)  :m_cmdmap(o.m_cmdmap) {}

	///
	/// \brief Reset parser command definitions
	///
	void init()
	{
		m_cmdmap = CmdMapType();
	}

	///
	/// \brief Add a command to the protocol parser (case insensitive)
	/// \param [in] cmd command to define
	///
	void add( const char* cmd)
	{
		m_cmdmap.insert( cmd);
	}

	///
	/// \brief Assignement copy
	/// \param [in] o CmdParser to copy
	///
	CmdParser& operator=( const CmdParser& o)
	{
		m_cmdmap = o.m_cmdmap;
		return *this;
	}

	///
	/// \brief Constructor
	/// \param [in] cmd null-terminated array of commands to define
	///
	CmdParser( const char** cmd)
	{
		for (unsigned int ii=0; cmd[ii]; ii++) add(cmd[ii]);
	}

	///
	/// \brief Defines a command. The index of the command is counted from 0 with one increment per add.
	/// \param [in] cmd command to define
	/// \remarks synonym for add(const char*)
	///
	CmdParser& operator[]( const char* cmd)
	{
		add( cmd);
		return *this;
	}

	///
	/// \brief Parse the next command.
	/// \tparam IteratorType iterator type used as input for parsing
	/// \param [in,out] src input iterator
	/// \param [in] end input iterator marking the end of input
	/// \param [in,out] buf the command buffer implementing the STL back insertion sequence interface
	/// \return the index of the command in the order of definition counted from 0 for the first command or return -1, if the command could not be recognized
	/// \remark At the end of buffer -1 is returned also. If \c src equals \c end after call and -1 was returned then the method has to be called again with new data.
	///
	template <typename IteratorType>
	int getCommand( IteratorType& src, IteratorType& end, CmdBufferType& buf) const
	{
		while (src < end && *src > 32)
		{
			char ch = *src;
			++src;
			buf.push_back( ch);
		}
		if (src == end) return -1;
		int rt = m_cmdmap[ buf];
		buf.clear();
		return rt;
	}
  
private:
	CmdMapType m_cmdmap;						///< commands of the parser
};

} // namespace protocol
} // namespace _Wolframe

#endif // _Wolframe_PROTOCOL_PARSER_HPP_INCLUDED


