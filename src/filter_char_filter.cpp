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
	///\brief Constructor
	InputFilterImpl()
		:m_buf( m_bufmem, sizeof(m_bufmem))
	{
		m_itr.setSource( SrcIterator( this));
	}

	///\brief Copy constructor
	///\param [in] o output filter to copy
	InputFilterImpl( const InputFilterImpl& o)
		:protocol::InputFilter( o)
		,m_itr(o.m_itr)
		,m_buf( m_bufmem, sizeof(m_bufmem))
	{
		m_buf.resize( o.m_buf.size());
		std::memcpy( m_bufmem, o.m_bufmem, o.m_buf.size());
		m_itr.setSource( SrcIterator( this));
	}

	///\brief self copy
	///\return copy of this
	virtual protocol::InputFilter* copy() const
	{
		return new InputFilterImpl( *this);
	}

	///\brief implement interface member protocol::InputFilter::getNext( typename protocol::InputFilter::ElementType&,const void*&,std::size_t&)
	virtual bool getNext( typename protocol::InputFilter::ElementType& type, const void*& element, std::size_t& elementsize)
	{
		setState( Open);
		type = Value;
		try
		{
			textwolf::UChar ch;
			if ((ch = *m_itr) != 0)
			{
				++m_itr;
				AppCharset::print( ch, m_buf);
				element = m_buf.ptr();
				elementsize = m_buf.size();
				m_buf.clear();
				return true;
			}
		}
		catch (SrcIterator::EoM)
		{
			setState( EndOfMessage);
		}
		return false;
	}
private:
	textwolf::TextScanner<SrcIterator,IOCharset> m_itr;
	char m_bufmem[16];
	textwolf::StaticBuffer m_buf;
};

///\class OutputFilter
///\brief output filter filter for single characters
template <class IOCharset, class AppCharset=textwolf::charset::UTF8>
struct OutputFilterImpl :public protocol::OutputFilter
{
	///\brief Constructor
	OutputFilterImpl(){}

	///\brief Copy constructor
	///\param [in] o output filter to copy
	OutputFilterImpl( const OutputFilterImpl& o)
		:protocol::OutputFilter(o){}

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
			textwolf::StaticBuffer buf( rest(), restsize());

			FilterBase<IOCharset,AppCharset>::printToBuffer( (const char*)element, elementsize, buf);
			if (buf.overflow())
			{
				setState( EndOfBuffer);
				return false;
			}
			incPos( buf.size());
		}
		return true;
	}
};
}//end anonymous namespace


struct CharFilter :public Filter
{
	CharFilter( const char *encoding=0)
	{
		if (!encoding)
		{
			m_inputfilter.reset( new InputFilterImpl<textwolf::charset::UTF8>());
			m_outputfilter.reset( new OutputFilterImpl<textwolf::charset::UTF8>());
		}
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

