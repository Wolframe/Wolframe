#include "filter/line_filter.hpp"
#include "filter/textwolf.hpp"
#include "filter/textwolf_filterBase.hpp"
#include <cstring>
#include <cstddef>

using namespace _Wolframe;
using namespace langbind;

namespace {

///\class OutputFilter
template <class IOCharset, class AppCharset=textwolf::charset::UTF8>
struct OutputFilterImpl :public protocol::OutputFilter
{
	///\brief Constructor
	OutputFilterImpl()
		:m_bufstate(protocol::EscapingBuffer<textwolf::StaticBuffer>::SRC){}

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
			protocol::EscapingBuffer<textwolf::StaticBuffer> buf( &basebuf, m_bufstate);
			FilterBase<IOCharset,AppCharset>::printToBuffer( (const char*)element, elementsize, buf);
			FilterBase<IOCharset,AppCharset>::printToBuffer( '\n', buf);
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
	typename protocol::EscapingBuffer<textwolf::StaticBuffer>::State m_bufstate;	///< state of escaping the output
};

///\class InputFilter
template <class IOCharset, class AppCharset=textwolf::charset::UTF8>
struct InputFilterImpl :public protocol::InputFilter
{
	///\brief Constructor
	InputFilterImpl()
	{
		m_itr.setSource( SrcIterator( this));
	}

	///\brief Copy constructor
	///\param [in] o output filter to copy
	InputFilterImpl( const InputFilterImpl& o)
		:protocol::InputFilter( o)
		,m_itr(o.m_itr)
		,m_buf(o.m_buf)
	{
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
		if (state() == Open)
		{
			m_buf.clear();
		}
		else
		{
			setState( Open);
		}
		type = Value;
		try
		{
			textwolf::UChar ch;
			while ((ch = *m_itr) != 0)
			{
				if (ch == '\r')
				{
					++m_itr;
					continue;
				}
				if (ch == '\n')
				{
					element = m_buf.c_str();
					elementsize = m_buf.size();
					++m_itr;
					return true;
				}
				else
				{
					AppCharset::print( ch, m_buf);
					++m_itr;
				}
			}
			if (m_buf.size() != 0)
			{
				element = m_buf.c_str();
				elementsize = m_buf.size();
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
	textwolf::TextScanner<SrcIterator,IOCharset> m_itr;	//< iterator on source
	std::string m_buf;					//< buffer for current line
};

}//end anonymous namespace


class LineFilter :public Filter
{
public:
	LineFilter( const char *encoding=0)
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

Filter LineFilterFactory::create( const char* encoding) const
{
	return LineFilter( encoding);
}

