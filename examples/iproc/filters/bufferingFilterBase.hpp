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
#include <cstddef>

namespace _Wolframe {
namespace filter {

template <class Content, class BufferType=std::string>
struct BufferingInputFilter :public protocol::InputFilter
{
	enum ErrorCodes
	{
		Ok,
		ErrOpenDoc,
		ErrOutOfMem,
		ErrNoContent,
		ErrOutputBufferTooSmall
	};

	///\brief Constructor
	BufferingInputFilter( Content* cref, std::size_t bufsize)
		:protocol::InputFilter(bufsize),m_inputConsumed(false),m_content(cref){}

	///\brief Destructor
	~BufferingInputFilter(){}

	///\brief Copy constructor
	///\param [in] o format output to copy
	BufferingInputFilter( const BufferingInputFilter& o)
		:protocol::InputFilter(o),m_inputConsumed(o.m_inputConsumed),m_buffer(o.m_buffer),m_content(o.m_content){}

	///\brief self copy
	///\return copy of this
	virtual BufferingInputFilter* copy() const
	{
		return new BufferingInputFilter( *this);
	}

	virtual bool getNext( ElementType* type, void* buffer, std::size_t buffersize, std::size_t* bufferpos)
	{
		if (!m_inputConsumed && !bufferInput())
		{
			return false;
		}
		setState( Open);
		Content* dc = m_content.get();
		if (!dc)
		{
			setState( Error, ErrNoContent);
			return false;
		}
		if (!dc->fetch( type, buffer, buffersize, bufferpos))
		{
			if (!dc->end())
			{
				setState( Error, ErrOutputBufferTooSmall);
			}
			return false;
		}
		return true;
	}
private:
	bool bufferInput()
	{
		bool rt = true;
		char* data = (char*)ptr();
		std::size_t datasize = size();

		try
		{
			for (unsigned int ii=0; ii<datasize; ii++)
			{
				m_buffer.push_back( data[ ii]);
			}
		}
		catch (std::bad_alloc)
		{
			setState( Error, ErrOutOfMem);
			return false;
		}
		if (gotEoD())
		{
			m_inputConsumed = true;
			protocol::InputFilter::protocolInput( (void*)&m_buffer.at(0), m_buffer.size(), true);

			if (!m_content->open( ptr(), size()))
			{
				setState( Error, ErrOpenDoc);
				rt = false;
			}
			else
			{
				setState( Open);
			}
		}
		else
		{
			setState( EndOfMessage);
			rt = false;
		}
		return rt;
	}
private:
	bool m_inputConsumed;			///< true, if we have consumed the whole input
	BufferType m_buffer;			///< STL back insertion sequence for buffering the input
	CountedReference<Content> m_content;	///< Content that provides an iterator on the input
};

}}//namespace
#endif

