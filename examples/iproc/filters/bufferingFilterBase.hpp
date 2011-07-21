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

namespace _Wolframe {
namespace filter {

template <class Container, class BufferType=std::string>
struct BufferingInputFilter :public protocol::InputFilter
{
	enum ErrorCodes
	{
		Ok,
		ErrOutputBufferTooSmall
	};

	///\brief Constructor
	BufferingFilter()
		:m_content(0){}

	///\brief Destructor
	~BufferingFilter()
	{
		if (m_content) delete m_content;
	}

	///\brief Copy constructor
	///\param [in] o format output to copy
	BufferingFilter( const BufferingFilter& o)
		:m_buffer(o.m_buffer),m_content(0)
	{
		if (o.m_content)
		{
			protocol::InputFilter::protocolInput( (void*)&m_buffer.at(0), m_buffer.size(), true);
			m_content = new Container( ptr(), size());
			m_itr = m_content->begin() + (o.m_itr - o.m_content->begin());
			m_end = m_content->end();
		}
	}

	///\brief self copy
	///\return copy of this
	virtual BufferingFilter* copy() const
	{
		return new BufferingFilter( *this);
	}

	virtual void protocolInput( void* data, size_type datasize, bool eoD)
	{
		for (unsigned int ii=0; ii<datasize; ii++)
		{
			buffer.push_back( ((unsigned char*)data)[ii]);
		}
		if (eoD)
		{
			protocol::InputFilter::protocolInput( (void*)&m_buffer.at(0), m_buffer.size(), true);
			m_content = new Container( ptr(), size());
			m_itr = m_content->begin();
			m_end = m_content->end();
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
		if (buffersize - *bufferpos < m_itr->size())
		{
			setState( Error, ErrOutputBufferTooSmall);
			return false;
		}
		memcpy( buffer, &m_itr->at(0), m_itr->size());
		*bufferpos += m_itr->size();
		++m_itr;
	}
private:
	BufferType m_buffer;			///< STL back insertion sequence for buffering the input
	Container* m_content;			///< Container that provides an iterator on the input
	typename Container::iterator m_itr;	///< iterator
	typename Container::iterator m_end;	///< end of input mark iterator
};

}}//namespace
#endif

