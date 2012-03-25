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
#ifndef _Wolframe_PROTOCOL_INPUT_FILTER_INTERFACE_HPP_INCLUDED
#define _Wolframe_PROTOCOL_INPUT_FILTER_INTERFACE_HPP_INCLUDED
///\file protocol/inputfilter.hpp
///\brief Input interface for the application processor

#include <cstddef>
#include <string>
#include <cstring>
#include <boost/lexical_cast.hpp>

#include "countedReference.hpp"

namespace _Wolframe {
namespace protocol {

///\class InputFilter
///\brief Input filter generator data with iterator function for the application processor to iterate on network input content elements
struct InputFilter
{
	///\enum State
	///\brief State of the input filter used in the application processor iterating loop to decide wheter to yield execution or not.
	enum State
	{
		Open,		///< serving data - normal input processing
		EndOfMessage,	///< have to yield processing because end of message reached
		Error		///< have to stop processing with an error
	};
	///\enum ElementType
	///\brief Content element type that describes the role of the element in the structured input
	enum ElementType
	{
		OpenTag,	///< open new hierarchy level
		Attribute,	///< attribute name
		Value,		///< content or attribute value
		CloseTag	///< close current hierarchy level
	};

	static const char* elementTypeName( ElementType i)
	{
		static const char* ar[] = {"OpenTag","Attribute","Value","CloseTag"};
		return ar[(int)i];
	}

	///\brief self copy
	///\return copy of this
	virtual InputFilter* copy() const=0;

	///\brief create the follow filter for processing
	///\return the follow filter
	///\remark this mechanism is used for chaining filters in case processing has to be changed
	virtual InputFilter* createFollow() const {return 0;}

	///\brief Get next element call as methof call
	///\param [out] type element type parsed
	///\param [out] element pointer to element returned
	///\param [out] elementsize size of the element returned
	///\return true, if success, false, if not.
	///\remark Check the state when false is returned
	virtual bool getNext( ElementType& type, const void*& element, std::size_t& elementsize)=0;

	///\brief Return the current state
	///\return the current state
	State state() const
	{
		return m_state;
	}

	///\brief Feed the generator with network input
	///\param [in] data pointer to memory block passed as input
	///\param [in] datasize of memory block passed as input
	///\param [in] eoD true, if end of data has been detected
	///\param [in] pos_ interator position of the input filter
	void protocolInput( void* data, std::size_t datasize, bool eoD, std::size_t pos_=0)
	{
		m_gotEoD = eoD;
		m_ptr = data;
		m_size = datasize;
		m_pos = pos_;
	}

	///\brief Assignement of the content processed only
	///\param [in] o InputFilter to copy the content processed from
	void assignContent(const InputFilter& o)
	{
		m_ptr = o.m_ptr;
		m_pos = o.m_pos;
		m_size = o.m_size;
		m_gotEoD = o.m_gotEoD;
		setState( o.m_state, o.m_errorbuf);
	}

	///\brief Constructor
	explicit InputFilter()
		:m_ptr(0)
		,m_pos(0)
		,m_size(0)
		,m_gotEoD(false)
		,m_state(Open)
	{
		m_errorbuf[0] = 0;
	}

	///\brief Copy constructor
	InputFilter( const InputFilter& o)
		:m_ptr(o.m_ptr)
		,m_pos(o.m_pos)
		,m_size(o.m_size)
		,m_gotEoD(o.m_gotEoD)
	{
		setState( o.m_state, o.m_errorbuf);
	}

	///\brief Destructor
	virtual ~InputFilter(){}

	///\brief Get a member value of the filter
	// param [in] name case sensitive name of the variable
	// param [in] value buffer for the value returned
	///\return true on success, false, if the variable does not exist or the operation failed
	virtual bool getValue( const char* /*name*/, std::string& /*value*/)
	{
		return false;
	}

	///\brief Set a member value of the filter
	// param [in] name case sensitive name of the variable
	// param [in] value new value of the variable to set
	///\return true on success, false, if the variable does not exist or the operation failed
	virtual bool setValue( const char* /*name*/, const std::string& /*value*/)
	{
		return false;
	}

	///\brief Get the las error in case of error state
	///\return the error string or 0
	const char* getError() const			{return (m_state==Error)?m_errorbuf:0;}
	///\brief Get end of data flag passed to input filter. Tells if more network input has to be passed to the application processor
	bool gotEoD() const				{return m_gotEoD;}
	///\brief Get data at current iterator cursor position
	void* ptr() const				{return(void*)((char*)m_ptr+m_pos);}
	///\brief Find out if there is more data left to parse
	///\return true, if yes
	bool hasLeft() const				{return (m_pos < m_size);}
	///\brief Get rest of data from the current iterator cursor position
	std::size_t size() const			{return (m_pos<m_size)?(m_size-m_pos):0;}
	///\brief Skip forward current iterator cursor position
	///\param [in] n number of bytes to skip
	void skip( std::size_t n)			{if ((m_pos+n)>=m_size) m_pos=m_size; else m_pos+=n;}

	///\brief Set input filter state with error message
	///\param [in] s new state
	///\param [in] msg (optional) error to set
	void setState( State s, const char* msg=0)
	{
		m_state = s;
		if (msg)
		{
			std::size_t msglen = std::strlen( msg);
			if (msglen >= ErrorBufSize) msglen = (std::size_t)ErrorBufSize-1;
			std::memcpy( m_errorbuf, msg, msglen);
			m_errorbuf[ msglen] = 0;
		}
		else
		{
			m_errorbuf[ 0] = 0;
		}
	}

	///\brief Get the current iterator position
	///\return the current iterator position as byte offset
	std::size_t pos() const				{return m_pos;}
private:
	void* m_ptr;			//< pointer to network input buffer
	std::size_t m_pos;		//< current iterator cursor position
	std::size_t m_size;		//< size of network input buffer
	bool m_gotEoD;			//< got end of data flag
	State m_state;			//< state
	enum {ErrorBufSize=128};
	char m_errorbuf[ ErrorBufSize];	//< error string
};

///\typedef InputFilterR
///\brief Shared input filter reference
typedef CountedReference<InputFilter> InputFilterR;

}}//namespace
#endif



