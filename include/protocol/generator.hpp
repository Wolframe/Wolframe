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
#ifndef _Wolframe_PROTOCOL_GENERATOR_INTERFACE_HPP_INCLUDED
#define _Wolframe_PROTOCOL_GENERATOR_INTERFACE_HPP_INCLUDED
/// \file protocol/generator.hpp
/// \brief input interface for the application processor

#include <cstddef>

namespace _Wolframe {
namespace protocol {

/// \class Generator
/// \brief closure class with iterator function for iterating on network input content elements
struct Generator
{
	typedef std::size_t size_type;

	/// \enum State
	/// \brief state of the generator used in the application processor iterating loop to decide wheter to yield execution or not.
	enum State
	{
		Open,		///< serving data - normal input processing
		EndOfMessage,	///< have to yield processing because end of message reached
		Error		///< have to yield with error and stop processing with an error 
	};
	/// \enum ElementType
	/// \brief content element type that describes the role of the element in the structured input
	enum ElementType
	{
		OpenTag,	///< open new hierarchy level
		Attribute,	///< attribute name
		Value,		///< content or attribute value
		CloseTag	///< close current hierarchy level
	};

	/// \brief get next element call
	typedef bool (*GetNext)( Generator* this_, ElementType* type, void* buffer, size_type buffersize, size_type* bufferpos);

	/// \brief get next element call as methof call
	bool getNext( ElementType* type, void* buffer, size_type buffersize, size_type* bufferpos)
	{
		return m_getNext( this, type, buffer, buffersize, bufferpos);
	}

	/// \brief return the current state
	State state() const
	{
		return m_state;
	}

	/// \brief feed the closure with network input
	void protocolInput( void* data, size_type datasize, bool eoD)
	{
		m_gotEoD = eoD;
		m_ptr = data;
		m_size = datasize;
		m_pos = 0;
	}

	/// \brief assignement
	Generator& operator = (const Generator& o)
	{
		m_ptr = o.m_ptr;
		m_pos = o.m_pos;
		m_size = o.m_size;
		m_gotEoD = o.m_gotEoD;
		m_state = o.m_state;
		m_errorCode = o.m_errorCode;
		return *this;
	}

	/// \brief constructor
	Generator( const GetNext& gn) :m_ptr(0),m_pos(0),m_size(0),m_gotEoD(false),m_state(Open),m_errorCode(0),m_getNext(gn){}

	/// \brief get error code in case of error state
	int getError() const				{return m_errorCode;}
	/// \brief get end of data flag passed to generator. Tells if more network input has to be passed to the application processor
	bool gotEoD() const				{return m_gotEoD;}
	/// \brief get data at current iterator cursor position
	void* ptr() const				{return(void*)((char*)m_ptr+m_pos);}
	/// \brief get rest of data from the current iterator cursor position
	size_type size() const			{return (m_pos<m_size)?(m_size-m_pos):0;}
	/// \brief skip current iterator cursor position n bytes forward
	void skip( size_type n)			{if ((m_pos+n)>=m_size) m_pos=m_size; else m_pos+=n;}
	/// \brief set generator state with error code
	void setState( State s, int e=0)		{m_state=s;m_errorCode=e;}

private:
	void* m_ptr;		///< pointer to network input buffer
	size_type m_pos;		///< current iterator cursor position
	size_type m_size;		///< size of network input buffer
	bool m_gotEoD;		///< got end of data flag
	State m_state;		///< generator state
	int m_errorCode;		///< error code
	GetNext m_getNext;		///< get next method pointer
};

}}//namespace
#endif



