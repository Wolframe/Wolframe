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
#ifndef _Wolframe_PROTOCOL_BUFFERS_HPP_INCLUDED
#define _Wolframe_PROTOCOL_BUFFERS_HPP_INCLUDED
///
/// \file protocol/buffers.hpp
/// \brief Defines the buffers used by protocol parsers to buffer the commands and their arguments.
///
/// All buffers defined in this module are preallocated fixed size and implement a subset of STL back insertion sequence interface for appending data.
/// This interface is used by the buffering parsing methods defined in protocol/parser.hpp.
/// The protocol uses fixed size buffers because protocol messages are not considered to have a size defined by the client for security reasons.
/// The buffers are not used for buffering processed data. Data is passed as memory blocks of the type defined in protocol/ioblocks.hpp
/// to the application processors.
///
#include <cstddef>
#include <boost/cstdint.hpp>

namespace _Wolframe {
namespace protocol {

///
/// \class Buffer
/// \brief Constant size single byte (ASCII) character buffer that implements a subset of the std::string interface
///  The buffer implements the STL back insertion sequence interface required by the protocol parser.
/// \tparam SIZE maximum number of character bytes buffered (without terminating 0 byte that is counted extra)
///
template <unsigned int SIZE=128>
class Buffer
{
private:
	typedef std::size_t size_type;		///< size type of this buffer vector

	/// \brief Some buffer constants
	enum
	{
		Size=SIZE			///< maximume size of the buffer in bytes as enum definition
	};
	size_type m_pos;			///< current cursor position of the buffer (number of added characters)
	char m_buf[ Size+1];			///< buffer content

public:
	Buffer()				:m_pos(0){}
	///\brief Clear the buffer content
	void clear()				{m_pos=0;}
	///\brief Append one character
	void push_back( char ch)		{if (m_pos<Size) m_buf[m_pos++]=ch;}
	///\brief Append a 0-terminated string
	void append( const char* cc)		{unsigned int ii=0; while(m_pos<Size && cc[ii]) m_buf[m_pos++]=cc[ii++];}
	///\brief Return the number of characters in the buffer
	size_type size() const			{return m_pos;}
	///\brief Return the buffer content as 0-terminated string
	const char* c_str()			{m_buf[m_pos]=0; return m_buf;}
	operator const char*()			{return c_str();};
};


/// \class CArgBuffer
/// \brief Buffer for multi argument parsing (fixed array of null terminated byte character strings)
///  The buffer implements the STL back insertion sequence interface required by the protocol parser.
///
/// This buffer splits the input, A sequence of characters by blanks and it parses escaping and quoted strings in the following way:
/// Escaping is done with backslash, strings can be single or double quoted.
/// The maximum number of arguments parsed is fix (16). More arguments are appended to the 16th argument.
/// A string is starting with a quote and terminating with the same non escaped quote or with the end of line.
/// Only blanks are splitting elements.
///
/// \tparam Buffer buffer type with an interface as _Wolframe::protocol::Buffer, a subset of the std::string interface used for buffering the elements of the argument vector.
template <class Buffer>
class CArgBuffer
{
private:
	/// \brief Some buffer constants
	enum
	{
		Size=16					///< maximum number of arguments buffered
	};
	/// \enum State
	/// \brief States of this buffer
	enum State
	{
		EndToken,				///< end of a token but no separating blank read yet.
		Empty,					///< end of token or just initialized. next non blank character starts a new token.
		Content,				///< parsing a sub token defined by non blank and not quote characters
		ContentEsc,				///< escape encountered in State::Content state
		SQContent,				///< parsing a sub token starting with a single quote
		SQContentEsc,				///< escape encountered in State::SQContent state
		DQContent,				///< parsing a sub token starting with a double quote
		DQContentEsc				///< escape encountered in State::DQContent state
	};
	unsigned int m_pos;				///< cursor position in the parsed token vector
	unsigned int m_buf[ Size];			///< the parsed token vector buffer
	const char* m_sbuf[ Size];			///< the parsed token vector as array of 0-terminated strings
	State m_state;					///< the buffer state
	Buffer* m_content;				///< the buffer used as element buffer of the tokens (passed by the constructor)

	/// \brief Action performed in the statemachine for opening a new token parsed
	void openArg()					{if (m_pos<Size) m_buf[m_pos++]=m_content->size();}

public:
	/// \brief Constructor
	/// \param [in] c buffer to use for the content passed by reference (owned by the caller, but used by this class)
	CArgBuffer( Buffer* c)				:m_pos(0),m_state(Empty),m_content(c) {m_buf[0]=0;m_sbuf[0]=0;}

	/// \brief Clear the buffer content
	void clear()					{m_pos=0;m_buf[0]=0;m_sbuf[0]=0;m_content->clear();}

	/// \brief Add the next character of the parsed content
	/// \param [in] ch character to process next
	void push_back( char ch)
	{
		switch (m_state)
		{
			case EndToken:
				switch (ch)
				{
					case '\'': m_state = SQContent; break;
					case '\"': m_state = DQContent; break;
					case '\\': m_state = ContentEsc; break;
					case ' ': m_content->push_back(0); m_state = Empty; break;
					default: m_state = Content; m_content->push_back(ch); break;
				}
				break;

			case Empty:
				switch (ch)
				{
					case '\'': m_state = SQContent; openArg(); break;
					case '\"': m_state = DQContent; openArg(); break;
					case '\\': m_state = ContentEsc; openArg(); break;
					case ' ': break;
					default: m_state = Content; openArg(); m_content->push_back(ch); break;
				}
				break;

			case Content:
				switch (ch)
				{
					case '\'': m_state = SQContent; break;
					case '\"': m_state = DQContent; break;
					case '\\': m_state = ContentEsc; break;
					case ' ':  m_content->push_back(0); m_state = Empty; break;
					default:	m_content->push_back(ch); break;
				}
				break;

			case ContentEsc:
				m_state = Content;
				m_content->push_back(ch);
				break;

			case SQContent:
				switch (ch)
				{
					case '\'': m_state = EndToken; break;
					case '\\': m_state = SQContentEsc; break;
					default:	m_content->push_back(ch); break;
				}
				break;

			case SQContentEsc:
				m_state = SQContent;
				m_content->push_back(ch);
				break;

			case DQContent:
				switch (ch)
				{
					case '\"': m_state = EndToken; break;
					case '\\': m_state = DQContentEsc; break;
					default:   m_content->push_back(ch); break;
				}
				break;

			case DQContentEsc:
				m_state = DQContent;
				m_content->push_back(ch);
				break;
		}
	};

	/// \brief Number of tokens parsed
	unsigned int size() const				{return m_pos;}

	/// \brief Get the element with index 'idx' or 0, if it does not exist
	/// \param [in] idx index of the argument to get
	/// \return the argument with index idx counted from 0
	const char* operator[]( unsigned int idx) const		{return (idx<m_pos)?m_content->c_str()+m_buf[idx]:0;}

	/// \brief Return a 0-terminated array of 0-terminated strings as convenient in C for program arguments
	/// \param [in] cmdname optional command name to return as first element of the array as convenient for C argc/argv
	/// \remark the return value of this function may not be valid anymore after a new call of push_back. So it should only be used after parsing
	const char** argv( const char* cmdname=0)
	{
		if (m_sbuf[0]==0)
		{
			unsigned int ii=0;
			if (cmdname) m_sbuf[ii++] = cmdname;
			for (;ii<m_pos; ii++) m_sbuf[ii]=m_content->c_str()+m_buf[ii];
			if (ii<m_pos) m_sbuf[ii]=0;
		}
		return m_sbuf;
	}

	/// \brief Synonym of size. Convenient in C for the number of program arguments
	unsigned int argc( const char* cmdname=0)		{(void)argv(cmdname); return m_pos;}
};

} // namespace protocol
} // namespace _Wolframe

#endif // _Wolframe_PROTOCOL_BUFFERS_HPP_INCLUDED

