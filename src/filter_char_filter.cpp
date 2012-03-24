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
///\file char_filter.cpp
///\brief Filter implementation reading/writing character by character

#include "filter/char_filter.hpp"
#include "filter/textwolf_filterBase.hpp"
#include "filter/textwolf.hpp"
#include <cstring>
#include <cstddef>

using namespace _Wolframe;
using namespace langbind;

namespace {

///\class InputFilter
///\brief input filter for single characters
template <class IOCharset, class AppCharset=textwolf::charset::UTF8>
struct InputFilterImpl :public protocol::InputFilter
{
	typedef textwolf::StaticBuffer BufferType;

	///\brief Constructor
	InputFilterImpl() :protocol::InputFilter(8) {}

	///\brief Copy constructor
	///\param [in] o output filter to copy
	InputFilterImpl( const InputFilterImpl& o)
		:protocol::InputFilter( o)
		,m_itr(o.m_itr) {}

	///\brief self copy
	///\return copy of this
	virtual protocol::InputFilter* copy() const
	{
		return new InputFilterImpl( *this);
	}

	///\brief implement interface member protocol::InputFilter::getNext( typename protocol::InputFilter::ElementType*,void*,std::size_t,std::size_t*)
	virtual bool getNext( typename protocol::InputFilter::ElementType* type, void* buffer, std::size_t buffersize, std::size_t* bufferpos)
	{
		BufferType buf( (char*)buffer, buffersize, *bufferpos);
		setState( Open);
		*type = Value;
		m_itr.setSource( SrcIterator( this));
		try
		{
			textwolf::UChar ch;
			if ((ch = *m_itr) != 0)
			{
				++m_itr;
				AppCharset::print( ch, buf);
				if (buf.overflow())
				{
					setState( Error, "textwolf: buffer too small to hold one character");
					return false;
				}
				*bufferpos = buf.size();
				return true;
			}
		}
		catch (SrcIterator::EoM)
		{
			setState( EndOfMessage);
			*bufferpos = buf.size();
		}
		return false;
	}
private:
	textwolf::TextScanner<SrcIterator,IOCharset> m_itr;
};

///\class OutputFilter
///\brief output filter filter for single characters
template <class IOCharset, class AppCharset=textwolf::charset::UTF8>
struct OutputFilterImpl :public protocol::OutputFilter
{
	typedef protocol::EscapingBuffer<textwolf::StaticBuffer> EscapingBuffer;

	///\brief Constructor
	OutputFilterImpl()
		:m_bufstate(EscapingBuffer::SRC){}

	///\brief Copy constructor
	///\param [in] o output filter to copy
	OutputFilterImpl( const OutputFilterImpl& o)
		:protocol::OutputFilter(o)
		,m_bufstate(o.m_bufstate)
	{}

	///\brief self copy
	///\return copy of this
	virtual protocol::OutputFilter* copy() const
	{
		return new OutputFilterImpl( *this);
	}

	///\brief Implementation of protocol::OutputFilter::print(typename protocol::OutputFilter::ElementType,const void*,std::size_t)
	///\param [in] type type of the element to print
	///\param [in] element pointer to the element to print
	///\param [in] elementsize size of the element to print in bytes
	///\return true, if success, false else
	virtual bool print( typename protocol::OutputFilter::ElementType type, const void* element, std::size_t elementsize)
	{
		if (type == Value)
		{
			textwolf::StaticBuffer basebuf( rest(), restsize());
			EscapingBuffer buf( &basebuf, m_bufstate);

			FilterBase<IOCharset,AppCharset>::printToBuffer( (const char*)element, elementsize, buf);
			if (basebuf.overflow())
			{
				setState( EndOfBuffer);
				return false;
			}
			incPos( basebuf.size());
			m_bufstate = buf.state();
		}
		return true;
	}
private:
	typename EscapingBuffer::State m_bufstate;	///< state of escaping the output
};
}//end anonymous namespace


struct CharFilter :public Filter
{
	CharFilter()
	{
		m_inputfilter.reset( new InputFilterImpl<textwolf::charset::UTF8>());
		m_outputfilter.reset( new OutputFilterImpl<textwolf::charset::UTF8>());
	}
	CharFilter( const char *encoding)
	{
		TextwolfEncoding::Id te = TextwolfEncoding::getId( encoding);
		switch (te)
		{
			case TextwolfEncoding::Unknown:
				break;
			case TextwolfEncoding::IsoLatin:
				m_inputfilter.reset( new InputFilterImpl<textwolf::charset::IsoLatin1>());
				m_outputfilter.reset( new OutputFilterImpl<textwolf::charset::IsoLatin1>());
				break;
			case TextwolfEncoding::UTF8:
				m_inputfilter.reset( new InputFilterImpl<textwolf::charset::UTF8>());
				m_outputfilter.reset( new OutputFilterImpl<textwolf::charset::UTF8>());
				break;
			case TextwolfEncoding::UTF16:
				m_inputfilter.reset( new InputFilterImpl<textwolf::charset::UTF16BE>());
				m_outputfilter.reset( new OutputFilterImpl<textwolf::charset::UTF16BE>());
				break;
			case TextwolfEncoding::UTF16BE:
				m_inputfilter.reset( new InputFilterImpl<textwolf::charset::UTF16BE>());
				m_outputfilter.reset( new OutputFilterImpl<textwolf::charset::UTF16BE>());
				break;
			case TextwolfEncoding::UTF16LE:
				m_inputfilter.reset( new InputFilterImpl<textwolf::charset::UTF16LE>());
				m_outputfilter.reset( new OutputFilterImpl<textwolf::charset::UTF16LE>());
				break;
			case TextwolfEncoding::UCS2BE:
				m_inputfilter.reset( new InputFilterImpl<textwolf::charset::UCS2BE>());
				m_outputfilter.reset( new OutputFilterImpl<textwolf::charset::UCS2BE>());
				break;
			case TextwolfEncoding::UCS2LE:
				m_inputfilter.reset( new InputFilterImpl<textwolf::charset::UCS2LE>());
				m_outputfilter.reset( new OutputFilterImpl<textwolf::charset::UCS2LE>());
				break;
			case TextwolfEncoding::UCS4BE:
				m_inputfilter.reset( new InputFilterImpl<textwolf::charset::UCS4BE>());
				m_outputfilter.reset( new OutputFilterImpl<textwolf::charset::UCS4BE>());
				break;
			case TextwolfEncoding::UCS4LE:
				m_inputfilter.reset( new InputFilterImpl<textwolf::charset::UCS4LE>());
				m_outputfilter.reset( new OutputFilterImpl<textwolf::charset::UCS4LE>());
				break;
		}
	}
};

Filter CharFilterFactory::create( const char* encoding) const
{
	return CharFilter( encoding);
}

