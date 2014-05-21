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
/// \file filter/outputfilter.hpp
/// \brief Interface for output filter

#ifndef _Wolframe_FILTER_OUTPUTFILTER_INTERFACE_HPP_INCLUDED
#define _Wolframe_FILTER_OUTPUTFILTER_INTERFACE_HPP_INCLUDED
#include "types/countedReference.hpp"
#include "types/docmetadata.hpp"
#include "filter/filterbase.hpp"
#include <string>
#include <cstring>

namespace _Wolframe {
namespace langbind {

/// \class OutputFilter
/// \brief Output filter
class OutputFilter
	:public FilterBase
{
public:
	/// \enum State
	/// \brief State of the input filter used in the application processor iterating loop to decide what to do
	enum State
	{
		Start,		///< state after first initialization
		Open,		///< serving data - normal input processing
		EndOfBuffer,	///< have to yield processing because end of message reached
		Error		///< have to stop processing with an error
	};

	/// \brief Constructor
	OutputFilter( const char* name_, const types::DocMetaDataR& inheritMetadata_)
		:utils::TypeSignature("langbind::OutputFilter", __LINE__)
		,FilterBase(name_)
		,m_state(Start)
		,m_buf(0)
		,m_size(0)
		,m_pos(0)
		,m_inheritMetadata(inheritMetadata_)
	{}

	/// \brief Constructor
	explicit OutputFilter( const char* name_)
		:utils::TypeSignature("langbind::OutputFilter", __LINE__)
		,FilterBase(name_)
		,m_state(Start)
		,m_buf(0)
		,m_size(0)
		,m_pos(0)
	{}

	/// \brief Copy constructor
	/// \param[in] o output filter to copy
	OutputFilter( const OutputFilter& o)
		:utils::TypeSignature(o)
		,FilterBase(o)
		,m_state(o.m_state)
		,m_buf(o.m_buf)
		,m_size(o.m_size)
		,m_pos(o.m_pos)
		,m_inheritMetadata(o.m_inheritMetadata)
		,m_metadata(o.m_metadata)
		{}

	/// \brief Destructor
	virtual ~OutputFilter(){}

	/// \brief Get a self copy
	/// \return allocated pointer to copy of this
	virtual OutputFilter* copy() const=0;

	/// \brief Declare the next input chunk to the filter
	/// \param [in] buf the start of the input chunk
	/// \param [in] bufsize the size of the input chunk in bytes
	void setOutputBuffer( void* buf, std::size_t bufsize)
	{
		if (m_state == EndOfBuffer && bufsize > 0) m_state = Open;
		m_buf = (char*)buf;
		m_size = bufsize;
		m_pos = 0;
	}

	/// \brief Get the output size printed
	/// \return size of the output printed in bytes
	std::size_t getPosition() const
	{
		return m_pos;
	}

	/// \brief Print the follow element to the buffer
	/// \param [in] type type of element to print
	/// \param [in] element content of element to print
	/// \param [in] elementsize size of element to print in bytes
	/// \return true, on success, false, if failed
	virtual bool print( ElementType type, const void* element, std::size_t elementsize)=0;

	/// \brief Print the follow element to the buffer
	/// \param [in] type type of element to print
	/// \param [in] element content of element to print
	/// \return true, on success, false, if failed
	bool print( ElementType type, const std::string& element)
	{
		return print( type, element.c_str(), element.size());
	}

	/// \brief Set type of the document.
	/// \param [in] doctype type of the document
	/// \remark For some types of filters (non buffering) the type has to be set before the first print
	void setMetaData( const types::DocMetaData& md)
	{
		if (state() != Start)
		{
			throw std::runtime_error( "cannot set output meta data anymore after first call of print");
		}
		m_metadata = md;
	}

	types::DocMetaData getMetaData() const
	{
		types::DocMetaData rt( *m_inheritMetadata);
		rt.join( m_metadata);
		return rt;
	}

	void inheritMetaData( const types::DocMetaDataR mdr)
	{
		m_inheritMetadata = mdr;
	}

	/// \brief Get the current state
	/// \return the current state
	State state() const					{return m_state;}

	/// \brief Set output filter state with error message
	/// \param [in] s new state
	/// \param [in] msg (optional) error to set
	void setState( State s, const char* msg=0)		{m_state=s; setError(msg);}

	/// \brief Assigns the output filter state of another output filter
	/// \param [in] o the output filter to get the state from
	void assignState( const OutputFilter& o)
	{
		m_state = o.m_state;
		m_buf = o.m_buf;
		m_size = o.m_size;
		m_pos = o.m_pos;
	}

	/// \brief Set one document meta data element
	void setAttribute( const std::string& name_, const std::string& value_)
	{
		if (state() != Start)
		{
			throw std::runtime_error( "cannot set output meta data anymore after first call of print");
		}
		m_metadata.setAttribute( name_, value_);
	}

	/// \brief Set the document type meta data element
	void setDoctype( const std::string& id_)
	{
		if (state() != Start)
		{
			throw std::runtime_error( "cannot set document type anymore after first call of print");
		}
		m_metadata.setDoctype( id_);
	}

protected:
	std::size_t write( const void* dt, std::size_t dtsize)
	{
		std::size_t nn = m_size - m_pos;
		if (nn > dtsize) nn = dtsize;
		std::memcpy( m_buf+m_pos, dt, nn);
		m_pos += nn;
		return nn;
	}
private:
	State m_state;					///< state
	char* m_buf;					///< buffer base pointer
	std::size_t m_size;				///< buffer size in bytes
	std::size_t m_pos;				///< write byte position
	types::DocMetaDataR m_inheritMetadata;		///< reference to meta data inherited from input
	types::DocMetaData m_metadata;			///< document meta data
};

/// \typedef OutputFilterR
/// \brief Shared output filter reference
typedef types::CountedReference<OutputFilter> OutputFilterR;

}}//namespace
#endif


