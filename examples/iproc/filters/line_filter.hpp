#ifndef _Wolframe_FILTER_LINE_HPP_INCLUDED
#define _Wolframe_FILTER_LINE_HPP_INCLUDED
#include "protocol/inputfilter.hpp"
#include "protocol/formatoutput.hpp"
#include "filters/filterBase.hpp"
#include <cstring>

namespace _Wolframe {
namespace filter {

///\class LineFilter 
///\brief Line filter template (input/output line by line)
///\tparam IOCharset character set encoding of input and output
///\tparam AppCharset character set encoding of the application processor
template <class IOCharset, class AppCharset=textwolf::charset::UTF8>
struct LineFilter :FilterBase<IOCharset, AppCharset>
{
	typedef FilterBase<IOCharset, AppCharset> ThisFilterBase;
	typedef typename protocol::FormatOutput::ElementType ElementType;
	typedef typename protocol::FormatOutput::size_type size_type;
	typedef textwolf::StaticBuffer BufferType;
	typedef protocol::EscapingBuffer<textwolf::StaticBuffer> EscBufferType;

	///\class FormatOutput
	struct FormatOutput :public protocol::FormatOutput
	{
		///\brief Constructor
		///\param [in] eoln end of line marker
		FormatOutput( const char* eoln="\r\n")
			:m_eoln(eoln),m_bufstate(EscBufferType::SRC){}

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
		const char* m_eoln;				///< end of line marker
		typename EscBufferType::State m_bufstate;	///< state of escaping the output
	};

	///\class InputFilter
	struct InputFilter :public protocol::InputFilter
	{
		///\enum ErrorCodes
		///\brief Enumeration of error codes
		enum ErrorCodes {Ok=0,ErrBufferTooSmall=1};

		///\brief Implementation of protocol::InputFilter::getNext( ElementType*, void*, size_type, size_type*)
		virtual bool getNext( ElementType* type, void* buffer, size_type buffersize, size_type* bufferpos)
		{
			BufferType buf( (char*)buffer + *bufferpos, buffersize - *bufferpos);
			setState( Open);
			*type = Value;
			CharIterator itr( (char*)ptr(), size());
			textwolf::TextScanner<CharIterator,AppCharset> ts( itr);

			textwolf::UChar ch;
			while ((ch = *itr) != 0)
			{
				if (ch == '\r') continue;
				if (ch == '\n')
				{
					*bufferpos += buf.size();
					++itr;
					skip( itr.pos());
					return true;
				}
				else
				{
					AppCharset::print( ch, buf);
					if (buf.overflow())
					{
						setState( protocol::InputFilter::Error, ErrBufferTooSmall);
						return false;
					}
					++itr;
				}
			}
			if (!gotEoD()) setState( EndOfMessage);
			return false;
		}
	};
};

}}//namespace
#endif

