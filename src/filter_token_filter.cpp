#include "filter/token_filter.hpp"
#include "filter/textwolf.hpp"
#include "filter/textwolf_filterBase.hpp"
#include <cstring>
#include <cstddef>

using namespace _Wolframe;
using namespace langbind;

namespace {

static bool getElementType( protocol::InputFilter::ElementType& et, char ch)
{
	switch (ch)
	{
		case '>': et = protocol::InputFilter::OpenTag; return true;
		case '<': et = protocol::InputFilter::CloseTag; return true;
		case '@': et = protocol::InputFilter::Attribute; return true;
		case '=': et = protocol::InputFilter::Value; return true;
	}
	return false;
}

static char getElementTag( protocol::OutputFilter::ElementType tp)
{
	switch (tp)
	{
		case protocol::InputFilter::OpenTag: return '>';
		case protocol::InputFilter::CloseTag: return '<';
		case protocol::InputFilter::Attribute: return '@';
		case protocol::InputFilter::Value: return '=';
	}
	return '\0';
}


///\class OutputFilter
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

	///\brief print a value with EOL escaped
	///\param [in] src pointer to attribute value string to print
	///\param [in] srcsize size of src in bytes
	///\param [in,out] buf buffer to print to
	static void printToBufferEscEOL( const char* src, std::size_t srcsize, textwolf::StaticBuffer& buf)
	{
		StrIterator itr( src, srcsize);
		textwolf::TextScanner<StrIterator,AppCharset> ts( itr);

		textwolf::UChar ch;
		while ((ch = ts.chr()) != 0)
		{
			if (ch == '\n')
			{
				IOCharset::print( ' ', buf);
			}
			else
			{
				IOCharset::print( ch, buf);
			}
			++ts;
		}
	}


	///\brief Implementation of protocol::OutputFilter::print(typename protocol::OutputFilter::ElementType,const void*,std::size_t)
	///\param [in] type type of the element to print
	///\param [in] element pointer to the element to print
	///\param [in] elementsize size of the element to print in bytes
	///\return true, if success, false else
	virtual bool print( typename protocol::OutputFilter::ElementType type, const void* element, std::size_t elementsize)
	{
		textwolf::StaticBuffer buf( rest(), restsize());
		FilterBase<IOCharset,AppCharset>::printToBuffer( getElementTag( type), buf);
		printToBufferEscEOL( (const char*)element, elementsize, buf);
		FilterBase<IOCharset,AppCharset>::printToBufferEOL( buf);
		if (buf.overflow())
		{
			if (pos() == 0 && size() != 0)
			{
				setState( Error, "buffer too small for one element");
			}
			else
			{
				setState( EndOfBuffer);
			}
			return false;
		}
		incPos( buf.size());
		return true;
	}
};

///\class InputFilter
template <class IOCharset, class AppCharset=textwolf::charset::UTF8>
struct InputFilterImpl :public protocol::InputFilter
{
	///\brief Constructor
	InputFilterImpl()
		:m_tag(0)
	{
		m_itr.setSource( SrcIterator( this));
	}

	///\brief Copy constructor
	///\param [in] o output filter to copy
	InputFilterImpl( const InputFilterImpl& o)
		:protocol::InputFilter( o)
		,m_itr(o.m_itr)
		,m_tag(o.m_tag)
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
			if (m_tag)
			{
				if (!getElementType( type, m_tag))
				{
					setState( protocol::InputFilter::Error, "textwolf: Unknown token tag");
				}
			}
			else
			{
				char tg = m_itr.ascii();
				if (!getElementType( type, tg))
				{
					setState( protocol::InputFilter::Error, "textwolf: Unknown token tag");
				}
				m_tag = tg;
				++m_itr;
			}
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
					m_tag = '\0';
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
	textwolf::TextScanner<SrcIterator,AppCharset> m_itr;	//< src iterator
	char m_tag;						//< tag defining the currently parsed element type
	std::string m_buf;					//< buffer for current line => current token
};

}//end anonymous namespace


class TokenFilter :public Filter
{
public:
	TokenFilter( const char* encoding=0)
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

Filter TokenFilterFactory::create( const char* encoding) const
{
	return TokenFilter( encoding);
}

