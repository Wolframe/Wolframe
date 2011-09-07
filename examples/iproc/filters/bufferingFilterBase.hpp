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
///\file bufferingFilterBase.hpp
///\brief Base class definitions for filters that process the content as a whole

#ifndef _Wolframe_BUFFERING_FILTERBASE_HPP_INCLUDED
#define _Wolframe_BUFFERING_FILTERBASE_HPP_INCLUDED
#include "protocol/inputfilter.hpp"
#include "countedReference.hpp"
#include <string>

namespace _Wolframe {
namespace filter {

template <class Content, class BufferType=std::string>
struct BufferingInputFilter :public protocol::InputFilter
{
	enum ErrorCodes
	{
		Ok,
		ErrOpenDoc,
		ErrNoContent,
		ErrOutputBufferTooSmall
	};

	///\brief Constructor
	BufferingInputFilter(){}

	///\brief Destructor
	~BufferingInputFilter(){}

	///\brief Copy constructor
	///\param [in] o format output to copy
	BufferingInputFilter( const BufferingInputFilter& o)
		:protocol::InputFilter(o),m_buffer(o.m_buffer),m_content(o.m_content){}

	///\brief self copy
	///\return copy of this
	virtual BufferingInputFilter* copy() const
	{
		return new BufferingInputFilter( *this);
	}

	virtual void protocolInput( void* data, size_type datasize, bool eoD)
	{		
		for (unsigned int ii=0; ii<datasize; ii++)
		{
			m_buffer.push_back( ((unsigned char*)data)[ii]);
		}
		if (eoD)
		{
			protocol::InputFilter::protocolInput( (void*)&m_buffer.at(0), m_buffer.size(), true);
			Content* dc = new (std::nothrow) Content();
			if (!dc)
			{
				setState( Error, ErrOpenDoc);
			}
			if (!dc->open( ptr(), size()))
			{
				setState( Error, ErrOpenDoc);
				delete dc;
				dc = 0;
			}
			m_content.reset( dc);
		}
	}

	virtual bool getNext( ElementType* type, void* buffer, size_type buffersize, size_type* bufferpos)
	{
		if (!gotEoD())
		{
			setState( EndOfMessage);
			return false;
		}
		setState( Open);
		if (!m_content.get())
		{
			setState( Error, ErrNoContent);
			return false;
		}
		if (!m_content.get()->getNext( type, buffer, buffersize, bufferpos))
		{
			setState( Error, ErrOutputBufferTooSmall);
			return false;
		}
		return true;
	}
private:
	BufferType m_buffer;			///< STL back insertion sequence for buffering the input
	CountedReference<Content> m_content;	///< Content that provides an iterator on the input
};

}}//namespace
#endif

