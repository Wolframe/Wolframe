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
	typedef typename FilterBase<IOCharset, AppCharset>::FormatOutputBase FormatOutputBase;

	///\class FormatOutput
	struct FormatOutput :public FormatOutputBase
	{
		const char* m_eoln;

		///\brief Constructor
		FormatOutput( unsigned int bufsize=128, const char* eoln="\r\n") :FormatOutputBase(bufsize),m_eoln(eoln){}

		///\brief Implementation of protocol::InputFilter::print(ElementType,const void*,size_type)
		///\param [in] type type of the element to print
		///\param [in] element pointer to the element to print
		///\param [in] elementsize size of the element to print in bytes
		virtual bool print( protocol::FormatOutput::ElementType type, const void* element, protocol::FormatOutput::size_type elementsize)
		{
			if (type == protocol::FormatOutput::Value)
			{
				protocol::FormatOutput::size_type bufpos=FormatOutputBase::m_bufpos;
				if (!FormatOutputBase::printElem( (const char*)element, elementsize, bufpos)) return false;
				if (!FormatOutputBase::printElem( m_eoln, bufpos)) return false;

				if (!FormatOutputBase::ContentOutputBlock::print( FormatOutputBase::m_buf+FormatOutputBase::m_bufpos, bufpos-FormatOutputBase::m_bufpos))
				{
					protocol::FormatOutput::setState( protocol::FormatOutput::EndOfBuffer);
					return false;
				}
			}
			return true;
		}
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
			setState( Open);
			*type = Value;
			CharIterator itr( (char*)ptr(), size());
			textwolf::TextScanner<CharIterator,AppCharset> ts( itr);

			textwolf::UChar ch;
			size_type pos = 0;
			while ((ch = *itr) != 0)
			{
				if (ch == '\r') continue;
				if (ch == '\n')
				{
					++itr;
					*bufferpos += pos;
					skip( itr.pos());
					return true;
				}
				size_type nn = IOCharset::print( ch, (char*)buffer+*bufferpos, buffersize-*bufferpos);
				if (nn > 0) pos += nn; else return false;
			}
			if (!gotEoD()) setState( EndOfMessage);
			return false;
		}
	};
};

}}//namespace
#endif

