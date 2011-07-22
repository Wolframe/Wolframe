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
///\file char_filter.hpp
///\brief Filter reading/writing character by character

#ifndef _Wolframe_FILTER_CHAR_HPP_INCLUDED
#define _Wolframe_FILTER_CHAR_HPP_INCLUDED
#include "protocol/inputfilter.hpp"
#include "protocol/formatoutput.hpp"
#include "filters/filterBase.hpp"
#include <cstring>

namespace _Wolframe {
namespace filter {

template <class IOCharset, class AppCharset=textwolf::charset::UTF8>
struct CharFilter :FilterBase<IOCharset, AppCharset>
{
	typedef FilterBase<IOCharset, AppCharset> ThisFilterBase;
	typedef typename protocol::FormatOutput::ElementType ElementType;
	typedef typename protocol::FormatOutput::size_type size_type;
	typedef textwolf::StaticBuffer BufferType;
	typedef protocol::EscapingBuffer<textwolf::StaticBuffer> EscBufferType;

	///\class InputFilter
	///\brief input filter for single characters
	struct InputFilter :public protocol::InputFilter
	{
		///\brief Constructor
		InputFilter() :protocol::InputFilter(8) {}

		///\brief Copy constructor
		///\param [in] o format output to copy
		InputFilter( const InputFilter& o)
			:protocol::InputFilter( o)
			,m_itr(o.m_itr) {}

		///\brief self copy
		///\return copy of this
		virtual InputFilter* copy() const
		{
			return new InputFilter( *this);
		}

		enum ErrorCodes
		{
			Ok,
			ErrBufferTooSmall
		};

		///\brief implement interface member protocol::InputFilter::getNext( ElementType*,void*,size_type,size_type*)
		virtual bool getNext( ElementType* type, void* buffer, size_type buffersize, size_type* bufferpos)
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
						setState( Error, ErrBufferTooSmall);
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
		textwolf::TextScanner<SrcIterator,AppCharset> m_itr;
	};

	///\class FormatOutput
	///\brief format output filter for single characters
	struct FormatOutput :public protocol::FormatOutput
	{
		///\brief Constructor
		FormatOutput()
			:m_bufstate(EscBufferType::SRC){}

		///\brief Copy constructor
		///\param [in] o format output to copy
		FormatOutput( const FormatOutput& o)
			:m_bufstate(o.m_bufstate)
		{}

		///\brief self copy
		///\return copy of this
		virtual FormatOutput* copy() const
		{
			return new FormatOutput( *this);
		}

		///\brief Implementation of protocol::InputFilter::print(ElementType,const void*,size_type)
		///\param [in] type type of the element to print
		///\param [in] element pointer to the element to print
		///\param [in] elementsize size of the element to print in bytes
		virtual bool print( ElementType type, const void* element, size_type elementsize)
		{
			if (type == Value)
			{
				EscBufferType buf( rest(), restsize(), m_bufstate);
				ThisFilterBase::printToBuffer( (const char*)element, elementsize, buf);
				if (buf.overflow())
				{
					setState( EndOfBuffer);
					return false;
				}
				incPos( buf.size());
				m_bufstate = buf.state();
			}
			return true;
		}
	private:
		typename EscBufferType::State m_bufstate;	///< state of escaping the output
	};
};

}}//namespace
#endif


