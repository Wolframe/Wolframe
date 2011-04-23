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
/// \brief Input interface for the application processor

#include <cstddef>

namespace _Wolframe {
namespace protocol {

/// \class Generator
/// \brief Generator data with iterator function for the application processor to iterate on network input content elements
struct Generator
{
	typedef std::size_t size_type;

	/// \enum State
	/// \brief State of the generator used in the application processor iterating loop to decide wheter to yield execution or not.
	enum State
	{
		Open,		///< serving data - normal input processing
		EndOfMessage,	///< have to yield processing because end of message reached
		Error		///< have to yield with error and stop processing with an error 
	};
	/// \enum ElementType
	/// \brief Content element type that describes the role of the element in the structured input
	enum ElementType
	{
		OpenTag,	///< open new hierarchy level
		Attribute,	///< attribute name
		Value,		///< content or attribute value
		CloseTag	///< close current hierarchy level
	};

	/// \brief Get next element call as methof call
	/// \param [out] type element type parsed
	/// \param [in,out] buffer for returned elements
	/// \param [in] buffersize size of the buffer for the returned elements
	/// \param [in,out] bufferpos before parsed element in and bufferpos after parsed element out
	/// \return true, if success, false, if not.
	/// \remark Check the generator state when false is returned
	virtual bool getNext( ElementType* type, void* buffer, size_type buffersize, size_type* bufferpos)=0;

	/// \brief Return the current state
	/// \return the current state
	State state() const
	{
		return m_state;
	}

	/// \brief Feed the generator with network input
	/// \param [in] data pointer to memory block passed as input
	/// \param [in] datasize of memory block passed as input
	/// \param [in] eoD true, if end of data has been detected
	void protocolInput( void* data, size_type datasize, bool eoD)
	{
		m_gotEoD = eoD;
		m_ptr = data;
		m_size = datasize;
		m_pos = 0;
	}

	/// \brief Assignement copy
	/// \param [in] o Generator to copy
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

	/// \brief Constructor
	/// \param [in] gn GetNext function pointer
	Generator() :m_ptr(0),m_pos(0),m_size(0),m_gotEoD(false),m_state(Open),m_errorCode(0){}
	virtual ~Generator(){}

	/// \brief Get error code in case of error state
	int getError() const				{return m_errorCode;}
	/// \brief Get end of data flag passed to generator. Tells if more network input has to be passed to the application processor
	bool gotEoD() const				{return m_gotEoD;}
	/// \brief Get data at current iterator cursor position
	void* ptr() const				{return(void*)((char*)m_ptr+m_pos);}
	/// \brief Get rest of data from the current iterator cursor position
	size_type size() const				{return (m_pos<m_size)?(m_size-m_pos):0;}
	/// \brief Skip forward current iterator cursor position
	/// \param [in] n number of bytes to skip
	void skip( size_type n)				{if ((m_pos+n)>=m_size) m_pos=m_size; else m_pos+=n;}
	/// \brief Set generator state with error code
	/// \param [in] s new generator state
	/// \param [in] e (optional) generator error code to set
	void setState( State s, int e=0)		{m_state=s;m_errorCode=e;}

private:
	void* m_ptr;		///< pointer to network input buffer
	size_type m_pos;	///< current iterator cursor position
	size_type m_size;	///< size of network input buffer
	bool m_gotEoD;		///< got end of data flag
	State m_state;		///< generator state
	int m_errorCode;	///< error code
};

}}//namespace
#endif



