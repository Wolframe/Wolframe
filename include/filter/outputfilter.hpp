/************************************************************************
Copyright (C) 2011 - 2013 Project Wolframe.
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
///\file filter.hpp
///\brief Interface for filter modules

#ifndef _Wolframe_FILTER_OUTPUTFILTER_INTERFACE_HPP_INCLUDED
#define _Wolframe_FILTER_OUTPUTFILTER_INTERFACE_HPP_INCLUDED
#include "types/countedReference.hpp"
#include "filter/filterbase.hpp"
#include <string>
#include <cstring>
#define WOLFRAME_OUTPUT_WITH_CHECKSUM

namespace _Wolframe {
namespace langbind {

///\class OutputFilter
///\brief Output filter
class OutputFilter :public FilterBase
{
public:
	///\enum State
	///\brief State of the input filter used in the application processor iterating loop to decide what to do
	enum State
	{
		Open,		//< serving data - normal input processing
		EndOfBuffer,	//< have to yield processing because end of message reached
		Error		//< have to stop processing with an error
	};

	///\brief Default constructor
	OutputFilter()
		:types::TypeSignature("langbind::OutputFilter", __LINE__)
		,m_state(Open)
		,m_buf(0)
		,m_size(0)
		,m_pos(0)
#ifdef WOLFRAME_OUTPUT_WITH_CHECKSUM
		,m_chksum(0)
		,m_chkpos(0)
#endif
	{}

	///\brief Copy constructor
	///\param[in] o output filter to copy
	OutputFilter( const OutputFilter& o)
		:types::TypeSignature(o)
		,FilterBase(o)
		,m_state(o.m_state)
		,m_buf(o.m_buf)
		,m_size(o.m_size)
		,m_pos(o.m_pos)
#ifdef WOLFRAME_OUTPUT_WITH_CHECKSUM
		,m_chksum(o.m_chksum)
		,m_chkpos(o.m_chkpos)
#endif
		{}

	///\brief Destructor
	virtual ~OutputFilter(){}

	///\brief Get a self copy
	///\return allocated pointer to copy of this
	virtual OutputFilter* copy() const=0;

	///\brief Declare the next input chunk to the filter
	///\param [in] buf the start of the input chunk
	///\param [in] bufsize the size of the input chunk in bytes
	void setOutputBuffer( void* buf, std::size_t bufsize)
	{
		if (m_state == EndOfBuffer && bufsize > 0) m_state = Open;
		m_buf = (char*)buf;
		m_size = bufsize;
		m_pos = 0;
	}

	///\brief Get the output size printed
	///\return size of the output printed in bytes
	std::size_t getPosition() const
	{
		return m_pos;
	}

	///\brief Print the follow element to the buffer
	///\param [in] type type of element to print
	///\param [in] element content of element to print
	///\param [in] elementsize size of element to print in bytes
	///\return true, on success, false, if failed
	virtual bool print( ElementType type, const void* element, std::size_t elementsize)=0;

	///\brief Set type of the document.
	///\remark For some types of filters (non buffering) the type has to be set before the first print
	virtual void setDocType( const std::string& /*doctype*/)
	{
		throw std::runtime_error("document type can not be set for this type of filter");
	}

	///\brief Print the follow element to the buffer
	///\param [in] type type of element to print
	///\param [in] element content of element to print
	///\return true, on success, false, if failed
	bool print( ElementType type, const std::string& element)
	{
		return print( type, element.c_str(), element.size());
	}

	///\brief Get the current state
	///\return the current state
	State state() const				{return m_state;}

	///\brief Set output filter state with error message
	///\param [in] s new state
	///\param [in] msg (optional) error to set
	void setState( State s, const char* msg=0)	{m_state=s; setError(msg);}

	///\brief Assigns the output filter state of another output filter
	///\param [in] o the output filter to get the state from
	void assignState( const OutputFilter& o)
	{
		m_state = o.m_state;
		m_buf = o.m_buf;
		m_size = o.m_size;
		m_pos = o.m_pos;
	}

#ifdef WOLFRAME_OUTPUT_WITH_CHECKSUM
	unsigned int chksum() const
	{
		return m_chksum;
	}
	unsigned int chkpos() const
	{
		return m_chkpos;
	}
	static void calculateCheckSum( unsigned int& chksum_, std::size_t pos_, const char* buf_, std::size_t bufsize_)
	{
		std::size_t ii = 0;
		for (; ii < bufsize_; ++ii) chksum_ += ((unsigned char)buf_[ pos_+ii] + 1U);
//		chksum_ += bufsize_;
	}
#else
	static void calculateCheckSum( unsigned int&, std::size_t, const char* , std::size_t ){}

	unsigned int chksum() const
	{
		return 0;
	}
	unsigned int chkpos() const
	{
		return 0;
	}
#endif

protected:
	std::size_t write( const void* dt, std::size_t dtsize)
	{
		std::size_t nn = m_size - m_pos;
		if (nn > dtsize) nn = dtsize;
		std::memcpy( m_buf+m_pos, dt, nn);
#ifdef WOLFRAME_OUTPUT_WITH_CHECKSUM
		calculateCheckSum( m_chksum, m_pos, m_buf, nn);
		m_chkpos += nn;
#endif
		m_pos += nn;
		return nn;
	}
private:
	State m_state;				//< state
	char* m_buf;				//< buffer base pointer
	std::size_t m_size;			//< buffer size in bytes
	std::size_t m_pos;			//< write byte position
#ifdef WOLFRAME_OUTPUT_WITH_CHECKSUM
	unsigned int m_chksum;			//< check sum for error detection
	unsigned int m_chkpos;			//< check bytes written for error detection
#endif
};

///\typedef OutputFilterR
///\brief Shared output filter reference
typedef types::CountedReference<OutputFilter> OutputFilterR;

}}//namespace
#endif


