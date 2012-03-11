#include "filter/line_filter.hpp"
#include "filter/textwolf.hpp"
#include "filter/textwolf_filterBase.hpp"
#include <cstring>
#include <cstddef>

using namespace _Wolframe;
using namespace filter;

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
			FilterBase<IOCharset,AppCharset>::printToBufferEOL( buf);
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
	InputFilterImpl( std::size_t genbufsize)
		:protocol::InputFilter( genbufsize) {}

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

	///\brief Implementation of protocol::InputFilter::getNext( typename protocol::InputFilter::ElementType*, void*, std::size_t, std::size_t*)
	virtual bool getNext( typename protocol::InputFilter::ElementType* type, void* buffer, std::size_t buffersize, std::size_t* bufferpos)
	{
		textwolf::StaticBuffer buf( (char*)buffer, buffersize, *bufferpos);
		setState( Open);
		*type = Value;
		m_itr.setSource( SrcIterator( this));
		try
		{
			textwolf::UChar ch;
			while ((ch = *m_itr) != 0)
			{
				if (ch == '\r') continue;
				if (ch == '\n')
				{
					*bufferpos = buf.size();
					++m_itr;
					return true;
				}
				else
				{
					AppCharset::print( ch, buf);
					if (buf.overflow())
					{
						setState( protocol::InputFilter::Error, "textwolf: buffer too small to hold one line");
						return false;
					}
					++m_itr;
				}
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

}//end anonymous namespace


LineFilter::LineFilter( const char *encoding, std::size_t bufsize)
{
	TextwolfEncoding::Id te = TextwolfEncoding::getId( encoding);
	switch (te)
	{
		case TextwolfEncoding::Unknown:
			break;
		case TextwolfEncoding::IsoLatin:
			m_inputFilter.reset( new InputFilterImpl<textwolf::charset::IsoLatin1>(bufsize));
			m_outputFilter.reset( new OutputFilterImpl<textwolf::charset::IsoLatin1>());
			break;
		case TextwolfEncoding::UTF8:
			m_inputFilter.reset( new InputFilterImpl<textwolf::charset::UTF8>(bufsize));
			m_outputFilter.reset( new OutputFilterImpl<textwolf::charset::UTF8>());
			break;
		case TextwolfEncoding::UTF16:
			m_inputFilter.reset( new InputFilterImpl<textwolf::charset::UTF16BE>(bufsize));
			m_outputFilter.reset( new OutputFilterImpl<textwolf::charset::UTF16BE>());
			break;
		case TextwolfEncoding::UTF16BE:
			m_inputFilter.reset( new InputFilterImpl<textwolf::charset::UTF16BE>(bufsize));
			m_outputFilter.reset( new OutputFilterImpl<textwolf::charset::UTF16BE>());
			break;
		case TextwolfEncoding::UTF16LE:
			m_inputFilter.reset( new InputFilterImpl<textwolf::charset::UTF16LE>(bufsize));
			m_outputFilter.reset( new OutputFilterImpl<textwolf::charset::UTF16LE>());
			break;
		case TextwolfEncoding::UCS2BE:
			m_inputFilter.reset( new InputFilterImpl<textwolf::charset::UCS2BE>(bufsize));
			m_outputFilter.reset( new OutputFilterImpl<textwolf::charset::UCS2BE>());
			break;
		case TextwolfEncoding::UCS2LE:
			m_inputFilter.reset( new InputFilterImpl<textwolf::charset::UCS2LE>(bufsize));
			m_outputFilter.reset( new OutputFilterImpl<textwolf::charset::UCS2LE>());
			break;
		case TextwolfEncoding::UCS4BE:
			m_inputFilter.reset( new InputFilterImpl<textwolf::charset::UCS4BE>(bufsize));
			m_outputFilter.reset( new OutputFilterImpl<textwolf::charset::UCS4BE>());
			break;
		case TextwolfEncoding::UCS4LE:
			m_inputFilter.reset( new InputFilterImpl<textwolf::charset::UCS4LE>(bufsize));
			m_outputFilter.reset( new OutputFilterImpl<textwolf::charset::UCS4LE>());
			break;
	}
}


