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
///\file protocol/buffers.hpp
///\brief Defines the buffers used by protocol parsers to buffer the commands and their arguments.
///
/// All buffers defined in this module are preallocated fixed size and implement a subset of STL back insertion sequence interface for appending data.
/// This interface is used by the buffering parsing methods defined in protocol/parser.hpp.
/// The protocol uses fixed size buffers because protocol messages are not considered to have a size defined by the client for security reasons.
/// The buffers are not used for buffering processed data. Data is passed as memory blocks of the type defined in protocol/ioblocks.hpp
/// to the application processors.
///
#include <cstddef>
#include <stdexcept>
#include <cstring>
#include <boost/cstdint.hpp>

namespace _Wolframe {
namespace protocol {

///
///\class Buffer
///\brief Constant size byte (ASCII) character buffer that implements a subset of the std::string interface
///\remark The buffer implements the STL back insertion sequence interface required by the protocol parser.
///
class Buffer
{
private:
	std::size_t m_pos;				//< current fill size the buffer in bytes
	std::size_t m_size;				//< size of the buffer in bytes
	char* m_buf;					//< buffer content
	bool m_allocated;

public:
	///\brief Default constructor
	Buffer()					:m_pos(0),m_size(0),m_buf(0),m_allocated(false){}

	///\brief Constructor allocating the buffer
	///\param[in] n size of buffer memory area in bytes
	explicit Buffer( std::size_t n)			:m_pos(0),m_size(n),m_buf(new char[n]),m_allocated(true) {}

	///\brief Constructor getting the buffer memory passed
	///\param[in] b pointer to buffer memory area
	///\param[in] n size of buffer memory area in bytes
	Buffer( char* b, std::size_t n)			:m_pos(0),m_size(n),m_buf(b),m_allocated(false) {}

	///\brief Destructor
	~Buffer()					{if (m_allocated) delete [] m_buf;}

	///\brief Recreate buffer getting the buffer memory passed
	///\param[in] b pointer to buffer memory area
	///\param[in] n size of buffer memory area in bytes
	void init( char* b, std::size_t n)		{if (m_allocated) delete [] m_buf; m_pos=0; m_size=n; m_buf=b; m_allocated=false;}

	///\brief Recreate buffer allocating the buffer
	///\param[in] n size of buffer memory area in bytes
	void init( std::size_t n)			{if (m_allocated) delete [] m_buf; m_pos=0; m_size=n; m_buf=new char[n]; m_allocated=true;}

	Buffer& operator=( const Buffer& o)		{if (m_size < o.m_pos) throw std::logic_error("assignement not possible"); std::memmove( m_buf, o.m_buf, m_pos=o.m_pos); return *this;}

	///\brief Clear the buffer content
	void clear()					{m_pos=0;}

	///\brief Append one character
	///\param[in] ch the character to append
	void push_back( char ch)			{if (m_pos<m_size) m_buf[m_pos++]=ch;}

	///\brief Append a 0-terminated string
	///\param[in] cc the string to append
	void append( const char* cc)			{unsigned int ii=0; while(m_pos<m_size && cc[ii]) m_buf[m_pos++]=cc[ii++];}

	///\brief Append a string
	///\param[in] cc the string to append
	///\param[in] nn the size of the string to append
	void append( const char* cc, unsigned int nn)	{unsigned int ii=0; while(m_pos<m_size && ii<nn) m_buf[m_pos++]=cc[ii++];}

	///\brief Return the number of characters in the buffer
	///\return the number of characters (bytes)
	std::size_t size() const			{return m_pos;}

	///\brief Return the maximum number of characters that can be put into the buffer
	///\return the maximum number of characters (bytes)
	std::size_t allocsize() const			{return m_size;}

	///\brief Change the fill size of the buffer to a smaller size
	void resize( std::size_t n)			{if (n > m_pos) throw std::logic_error("resize to bigger size not possible"); m_pos=n;}

	///\brief Return the buffer content as pointer to string without 0 termination
	///\return the pointer to buffer start
	const char* ptr() const				{return m_buf;}

	///\brief Return the buffer content as pointer to string without 0 termination
	///\return the pointer to buffer start
	char* ptr()					{return m_buf;}

	///\brief Return the buffer content as 0-terminated string
	///\return the C-string
	const char* c_str()				{m_buf[m_pos]=0; return m_buf;}

	///\brief cast operator for direct access
	///\return the C-string
	operator const char*()				{return c_str();}

	///\brief check, if the buffer had an overflow, no space left
	///\return true, if yes
	bool overflow() const				{return m_pos==m_size;}
};


///\class CArgBuffer
///\brief Buffer for multi argument parsing (fixed array of null terminated byte character strings)
//  The buffer implements the STL back insertion sequence interface required by the protocol parser.
//
// This buffer splits the input, A sequence of characters by blanks and it parses escaping and quoted strings in the following way:
// Escaping is done with backslash, strings can be single or double quoted.
// The maximum number of arguments parsed is fix (16). More arguments are appended to the 16th argument.
// A string is starting with a quote and terminating with the same non escaped quote or with the end of line.
// Only blanks are splitting elements.
//
///\tparam Buffer buffer type with an interface as _Wolframe::protocol::Buffer, a subset of the std::string interface used for buffering the elements of the argument vector.
template <class Buffer>
class CArgBuffer
{
private:
	///\brief Some buffer constants
	enum
	{
		Size=16					///< maximum number of arguments buffered
	};
	///\enum State
	///\brief States of this buffer
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
	static const char* stateName( State st)
	{
		static const char* ar[] = {"EndToken","Empty","Content","ContentEsc","SQContent","SQContentEsc","DQContent","DQContentEsc",0};
		return ar[(int)st];
	}

	unsigned int m_pos;				///< cursor position in the parsed token vector
	unsigned int m_buf[ Size];			///< the parsed token vector buffer
	const char* m_sbuf[ Size];			///< the parsed token vector as array of 0-terminated strings
	State m_state;					///< the buffer state
	Buffer* m_content;				///< the buffer used as element buffer of the tokens (passed by the constructor)

	///\brief Action performed in the statemachine for opening a new token parsed
	void openArg()					{if (m_pos<Size) m_buf[m_pos++]=m_content->size();}

public:
	///\brief Constructor
	///\param[in] c buffer to use for the content passed by reference (owned by the caller, but used by this class)
	CArgBuffer( Buffer* c)				:m_pos(0),m_state(Empty),m_content(c) {m_buf[0]=0;m_sbuf[0]=0;}

	///\brief Clear the buffer content
	void clear()					{m_pos=0;m_buf[0]=0;m_sbuf[0]=0;m_state=Empty;m_content->clear();}

	///\brief Add the next character of the parsed content
	///\param[in] ch character to process next
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
					default: m_content->push_back(ch); break;
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
					default: m_content->push_back(ch); break;
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
					default: m_content->push_back(ch); break;
				}
				break;

			case DQContentEsc:
				m_state = DQContent;
				m_content->push_back(ch);
				break;
		}
	}

	///\brief Number of tokens parsed
	///\return the number of tokens in the array parsed
	unsigned int size() const				{return m_pos;}

	///\brief Get the element with index 'idx' or 0, if it does not exist
	///\param[in] idx index of the argument to get
	///\return the argument with index idx counted from 0
	const char* operator[]( unsigned int idx) const		{return (idx<m_pos)?m_content->c_str()+m_buf[idx]:0;}

	///\brief Return a 0-terminated array of 0-terminated strings as convenient in C for program arguments
	///\param[in] cmdname optional command name to return as first element of the array as convenient for C argc/argv
	///\remark the return value of this function may not be valid anymore after a new call of push_back. So it should only be used after parsing
	///\return array of arguments up to 'Size' (with or without the command itself as in C argc/argv)
	const char** argv( const char* cmdname=0)
	{
		if (m_sbuf[0]==0)
		{
			unsigned int ii=0;
			if (cmdname)
			{
				m_sbuf[ii++] = cmdname;
				if (m_pos < Size) m_pos++;
			}
			for (;ii<m_pos; ii++) m_sbuf[ii]=m_content->c_str()+m_buf[ii];
			if (ii<m_pos) m_sbuf[ii]=0;
		}
		return m_sbuf;
	}

	///\brief Synonym of size. Convenient in C for the number of program arguments
	///\param[in] cmdname (optional) name of the command to include as first element of the argument array (and therefore to add to the count returned)
	///\return number of arguments (with or without the command itself as in C argc/argv)
	unsigned int argc( const char* cmdname=0)		{(void)argv(cmdname); return m_pos;}
};

} // namespace protocol
} // namespace _Wolframe

#endif // _Wolframe_PROTOCOL_BUFFERS_HPP_INCLUDED

