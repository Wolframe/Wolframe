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
	/// \param [in,out] buf output buffer with a minimal subset of std::string interface
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

/// \class CmdParser
/// \exception Bad
/// \brief Parser for ascii protocol commands
/// \tparam buffer type to use. (std::string or a buffer structure defined in protocol/buffers.hpp)
///
template <typename CmdBufferType>
class CmdParser :public Parser
{
public:
	/// \class Bad
	/// \brief Exception for illegal protocol parser definition (too many commands).
	///
	/// \remark The parsers are exception free at processing time. Only add and the operator '[]' for automata construction throw.
	///
	class Bad :public std::logic_error {public: Bad():std::logic_error("too many elements in CmdParser") {};};

	/// \brief Constructor
	///
	CmdParser() :m_size(0){};

	/// \brief Destructor
	///
	~CmdParser() {};

	/// \brief Copy constructor
	/// \param [in] o CmdParser to copy
	///
	CmdParser( const CmdParser& o) :m_size(o.m_size)
	{
		for (unsigned int ii=0; ii<m_size; ii++) m_elem[ii]=o.m_elem[ii];
	};

	///
	/// \brief Reset parser command definitions
	///
	void init()
	{
		m_size = 0;
	};

	///
	/// \brief Add a command to the protocol parser (case insensitive)
	/// \param [in] cmd command to define
	///
	void add( const char* cmd)
	{
		CmdBufferType ct;
		for (unsigned int ii=0; cmd[ii]; ii++) ct.push_back(cmd[ii]);
		if (m_size == MaxNofCommands) throw Bad();
		m_elem[m_size++] = *ct;
	};

	///
	/// \brief Assignement copy
	/// \param [in] o CmdParser to copy
	///
	CmdParser& operator=( const CmdParser& o)
	{
		m_size = o.m_size;
		for (unsigned int ii=0; ii<m_size; ii++) m_elem[ii]=o.m_elem[ii];
		return *this;
	};

	///
	/// \brief Constructor
	/// \param [in] cmd null-terminated array of commands to define
	///
	CmdParser( const char** cmd)
	{
		for (unsigned int ii=0; cmd[ii]; ii++) add(cmd[ii]);
	};

	///
	/// \brief Defines a command. The index of the command is counted from 0 with one increment per add.
	/// \param [in] cmd command to define
	/// \remarks synonym for add(const char*)
	///
	CmdParser& operator[]( const char* cmd)
	{
		add( cmd);
		return *this;
	};

	///
	/// \brief Parse the next command.
	/// \tparam IteratorType iterator type used as input for parsing
	/// \param [in,out] src input iterator
	/// \param [in] end input iterator marking the end of input
	/// \param [in,out] buf the command buffer used
	/// \return the index of the command in the order of definition counted from 0 for the first command or return -1, if the command could not be recognized
	/// \remark At the end of buffer -1 is returned also. If \c src equals \c end after call and -1 was returned then the method has to be called again with new data.
	///
	template <typename IteratorType>
	int getCommand( IteratorType& src, IteratorType& end, CmdBufferType& buf) const
	{
		while (src < end && *src > 32)
		{
			buf.push_back( *src);
			++src;
		}
		if (src == end) return -1;

		if (*buf == -1)
		{
			buf.clear();
			return -1;
		}
		for (unsigned int ii=0; ii<m_size; ii++)
		{
			if (m_elem[ii] == *buf)
			{
				buf.clear();
				return (int)ii;
			}
		}
		buf.clear();
		return -1;
	}
  
private:
	enum {MaxNofCommands=32};					///< the maximum number commands that can be defined for a command parser
	typedef typename CmdBufferType::ValueType CmdValueType;		///< value type of a command coded as integer
	CmdValueType m_elem[ MaxNofCommands];				///< commands of the parser in order of their definition
	unsigned int m_size;						///< number of commands defined
};

} // namespace protocol
} // namespace _Wolframe

#endif // _Wolframe_PROTOCOL_PARSER_HPP_INCLUDED


