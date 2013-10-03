/************************************************************************
Copyright (C) 2011 - 2013 Project Wolframe.
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
but WITHOUT ANY WARRANTY; without even the ied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
///\file token_filter.cpp
///\brief Filter implementation of a filter for debug input/output

#include "token_filter.hpp"
#include "textwolf/charset.hpp"
#include "textwolf/sourceiterator.hpp"
#include "textwolf/textscanner.hpp"
#include "textwolf/cstringiterator.hpp"
#include <cstring>
#include <cstddef>
#include <algorithm>
#include <boost/algorithm/string.hpp>

using namespace _Wolframe;
using namespace _Wolframe::langbind;

namespace {

static bool getElementType( InputFilter::ElementType& et, char ch)
{
	switch ((TokenType)ch)
	{
		case TokenOpenTag: et = InputFilter::OpenTag; return true;
		case TokenCloseTag: et = InputFilter::CloseTag; return true;
		case TokenAttribute: et = InputFilter::Attribute; return true;
		case TokenValue: et = InputFilter::Value; return true;
		case TokenNextLine: return false;
	}
	return false;
}

static char getElementTag( OutputFilter::ElementType tp)
{
	switch (tp)
	{
		case InputFilter::OpenTag: return (char)TokenOpenTag;
		case InputFilter::CloseTag: return (char)TokenCloseTag;
		case InputFilter::Attribute: return (char)TokenAttribute;
		case InputFilter::Value: return (char)TokenValue;
	}
	return '\0';
}


///\class OutputFilterImpl
template <class IOCharset, class AppCharset=textwolf::charset::UTF8>
struct OutputFilterImpl :public OutputFilter
{
	///\brief Constructor
	OutputFilterImpl( const IOCharset& iocharset_=IOCharset())
		:types::TypeSignature("langbind::OutputFilterImpl (token)", __LINE__)
		,m_elemitr(0)
		,m_output(iocharset_){}

	///\brief Copy constructor
	///\param [in] o output filter to copy
	OutputFilterImpl( const OutputFilterImpl& o)
		:types::TypeSignature("langbind::OutputFilterImpl (token)", __LINE__)
		,OutputFilter(o)
		,m_elembuf(o.m_elembuf)
		,m_elemitr(o.m_elemitr)
		,m_output(o.m_output){}

	///\brief self copy
	///\return copy of this
	virtual OutputFilter* copy() const
	{
		return new OutputFilterImpl( *this);
	}

	///\brief print a value with EOL escaped
	///\param [in] src pointer to attribute value string to print
	///\param [in] srcsize size of src in bytes
	///\param [in,out] buf buffer to print to
	void printToBufferEscEOL( const char* src, std::size_t srcsize, std::string& buf) const
	{
		textwolf::CStringIterator itr( src, srcsize);
		textwolf::TextScanner<textwolf::CStringIterator,AppCharset> ts( itr);

		textwolf::UChar ch;
		while ((ch = ts.chr()) != 0)
		{
			if (ch == '\n')
			{
				m_output.print( '\n', buf);
				m_output.print( (char)TokenNextLine, buf);
			}
			else
			{
				m_output.print( ch, buf);
			}
			++ts;
		}
	}

	bool emptybuf()
	{
		std::size_t nn = m_elembuf.size() - m_elemitr;
		m_elemitr += write( m_elembuf.c_str() + m_elemitr, nn);
		if (m_elemitr == m_elembuf.size())
		{
			m_elembuf.clear();
			m_elemitr = 0;
			return true;
		}
		return false;
	}

	///\brief Implementation of OutputFilter::print(typename OutputFilter::ElementType,const void*,std::size_t)
	///\param [in] type type of the element to print
	///\param [in] element pointer to the element to print
	///\param [in] elementsize size of the element to print in bytes
	///\return true, if success, false else
	virtual bool print( typename OutputFilter::ElementType type, const void* element, std::size_t elementsize)
	{
		setState( Open);
		if (m_elemitr < m_elembuf.size())
		{
			// there is something to print left from last time
			if (!emptybuf())
			{
				setState( EndOfBuffer);
				return false;
			}
			//... we've done the emptying of the buffer left
			return true;
		}
		m_output.print( getElementTag( type), m_elembuf);
		printToBufferEscEOL( (const char*)element, elementsize, m_elembuf);
		m_output.print( '\n', m_elembuf);
		if (!emptybuf())
		{
			setState( EndOfBuffer);
			return false;
		}
		return true;
	}
private:
	std::string m_elembuf;				//< buffer for the currently printed element
	std::size_t m_elemitr;				//< iterator to pass it to output
	IOCharset m_output;
};

///\class InputFilterImpl
template <class IOCharset, class AppCharset=textwolf::charset::UTF8>
struct InputFilterImpl :public InputFilter
{
	typedef textwolf::TextScanner<textwolf::SrcIterator,IOCharset> TextScanner;

	///\brief Constructor
	InputFilterImpl( const char* encoding_, const IOCharset& iocharset_=IOCharset())
		:types::TypeSignature("langbind::InputFilterImpl (token)", __LINE__)
		,m_itr( iocharset_)
		,m_output(AppCharset())
		,m_tag(0)
		,m_taglevel(0)
		,m_elemtype(OpenTag)
		,m_src(0)
		,m_srcsize(0)
		,m_srcend(false)
		,m_linecomplete(false)
		,m_eolnread(false)
		,m_encoding(encoding_?encoding_:"UTF-8"){}

	///\brief Copy constructor
	///\param [in] o output filter to copy
	InputFilterImpl( const InputFilterImpl& o)
		:types::TypeSignature("langbind::InputFilterImpl (token)", __LINE__)
		,InputFilter(o)
		,m_itr(o.m_itr)
		,m_output(o.m_output)
		,m_tag(o.m_tag)
		,m_taglevel(o.m_taglevel)
		,m_elemtype(o.m_elemtype)
		,m_elembuf(o.m_elembuf)
		,m_src(o.m_src)
		,m_srcsize(o.m_srcsize)
		,m_srcend(o.m_srcend)
		,m_linecomplete(o.m_linecomplete)
		,m_eolnread(o.m_eolnread)
		,m_encoding(o.m_encoding){}

	///\brief self copy
	///\return copy of this
	virtual InputFilter* copy() const
	{
		return new InputFilterImpl( *this);
	}

	///\brief implement interface member InputFilterImpl::putInput(const void*,std::size_t,bool)
	virtual void putInput( const void* ptr, std::size_t size, bool end)
	{
		m_src = (const char*)ptr;
		m_srcend = end;
		m_srcsize = size;
		m_itr.setSource( textwolf::SrcIterator( m_src, m_srcsize, m_srcend));
	}

	virtual void getRest( const void*& ptr, std::size_t& size, bool& end)
	{
		std::size_t pos = m_itr.getPosition();
		ptr = m_src + pos;
		size = (m_srcsize > pos)?(m_srcsize - pos):0;
		end = m_srcend;
	}

	///\brief implement interface member InputFilter::getNext( typename InputFilter::ElementType&,const void*&,std::size_t&)
	virtual bool getNext( typename InputFilter::ElementType& type, const void*& element, std::size_t& elementsize)
	{
		if (m_linecomplete)
		{
			m_elembuf.clear();
			m_linecomplete = false;
			if (m_taglevel == -1) return false;
		}
		setState( Open);
		type = Value;
		try
		{
			if (!m_tag)
			{
				char tg;
				do
				{
					tg = m_itr.ascii();
					++m_itr;
				}
				while (tg == '\n' || tg == '\r');
				if (!tg)
				{
					if (m_taglevel != 0)
					{
						setState( InputFilter::Error, "token filter - tags not balanced");
						return false;
					}
					m_taglevel = -1;
					type = CloseTag;
					element = "";
					elementsize = 0;
					return true;
				}
				if (!getElementType( m_elemtype, tg))
				{
					setState( InputFilter::Error, "token filter - unknown token tag");
					return false;
				}
				m_tag = tg;
			}
			textwolf::UChar ch;
			while ((ch = *m_itr) != 0)
			{
				if (m_eolnread)
				{
					if (ch == (char)TokenNextLine)
					{
						m_output.print( '\n', m_elembuf);
						++m_itr;
						m_eolnread = false;
						continue;
					}
					else
					{
						type = m_elemtype;
						element = m_elembuf.c_str();
						elementsize = m_elembuf.size();
						if (m_elemtype == OpenTag)
						{
							++m_taglevel;
						}
						else if (m_elemtype == CloseTag)
						{
							--m_taglevel;
						}
						m_tag = '\0';
						m_linecomplete = true;
						m_eolnread = false;
						return true;
					}
				}
				else if (ch == '\r')
				{
					++m_itr;
					continue;
				}
				else if (ch == '\n')
				{
					++m_itr;
					m_eolnread = true;
					continue;
				}
				else
				{
					m_output.print( ch, m_elembuf);
					++m_itr;
				}
			}
			if (m_elembuf.size() != 0)
			{
				type = m_elemtype;
				element = m_elembuf.c_str();
				elementsize = m_elembuf.size();
				if (m_elemtype == OpenTag)
				{
					++m_taglevel;
				}
				else if (m_elemtype == CloseTag)
				{
					--m_taglevel;
				}
				m_tag = '\0';
				m_linecomplete = true;
				m_eolnread = false;
				return true;
			}
		}
		catch (textwolf::SrcIterator::EoM)
		{
			setState( EndOfMessage);
		}
		return false;
	}

	virtual const char* getEncoding() const
	{
		return m_encoding.empty()?0:m_encoding.c_str();
	}

private:
	TextScanner m_itr;		//< src iterator
	AppCharset m_output;		//< output
	char m_tag;			//< tag defining the currently parsed element type
	int m_taglevel;			//< tag level
	ElementType m_elemtype;		//< current element type
	std::string m_elembuf;		//< buffer for current line => current token
	const char* m_src;		//< pointer to current chunk parsed
	std::size_t m_srcsize;		//< size of the current chunk parsed in bytes
	bool m_srcend;			//< true if end of message is in current chunk parsed
	bool m_linecomplete;		//< true if the last getNext could complete a line
	bool m_eolnread;		//< true if the end of line has been read
	std::string m_encoding;		//< character set encoding
};

}//end anonymous namespace


class TokenFilter :public Filter
{
public:
	TokenFilter( const char* encoding=0)
	{
		if (!encoding || !encoding[0])
		{
			m_inputfilter.reset( new InputFilterImpl<textwolf::charset::UTF8>( "UTF-8"));
			m_outputfilter.reset( new OutputFilterImpl<textwolf::charset::UTF8>());
		}
		else
		{
			std::string enc;
			parseEncoding( enc, encoding);

			if ((enc.size() >= 8 && std::memcmp( enc.c_str(), "isolatin", enc.size())== 0)
			||  (enc.size() >= 7 && std::memcmp( enc.c_str(), "iso8859", enc.size()) == 0))
			{
				const char* codepage = enc.c_str() + ((enc.c_str()[4] == 'l')?8:7);
				if (std::strlen( codepage) > 1 || codepage[0] < '0' || codepage[0] > '9')
				{
					throw std::runtime_error( "unknown iso-latin code page index");
				}
				if (codepage[0] == '1')
				{
					m_inputfilter.reset( new InputFilterImpl<textwolf::charset::IsoLatin>( encoding));
					m_outputfilter.reset( new OutputFilterImpl<textwolf::charset::IsoLatin>());
				}
				else
				{
					m_inputfilter.reset( new InputFilterImpl<textwolf::charset::IsoLatin>( encoding, textwolf::charset::IsoLatin( codepage[0] - '0')));
					m_outputfilter.reset( new OutputFilterImpl<textwolf::charset::IsoLatin>( textwolf::charset::IsoLatin( codepage[0] - '0')));
				}
			}
			else if (enc.size() == 0 || enc == "utf8")
			{
				m_inputfilter.reset( new InputFilterImpl<textwolf::charset::UTF8>( encoding));
				m_outputfilter.reset( new OutputFilterImpl<textwolf::charset::UTF8>());
			}
			else if (enc == "utf16" || enc == "utf16be")
			{
				m_inputfilter.reset( new InputFilterImpl<textwolf::charset::UTF16BE>( encoding));
				m_outputfilter.reset( new OutputFilterImpl<textwolf::charset::UTF16BE>());
			}
			else if (enc == "utf16le")
			{
				m_inputfilter.reset( new InputFilterImpl<textwolf::charset::UTF16LE>( encoding));
				m_outputfilter.reset( new OutputFilterImpl<textwolf::charset::UTF16LE>());
			}
			else if (enc == "ucs2" || enc == "ucs2be")
			{
				m_inputfilter.reset( new InputFilterImpl<textwolf::charset::UCS2BE>( encoding));
				m_outputfilter.reset( new OutputFilterImpl<textwolf::charset::UCS2BE>());
			}
			else if (enc == "ucs2le")
			{
				m_inputfilter.reset( new InputFilterImpl<textwolf::charset::UCS2LE>( encoding));
				m_outputfilter.reset( new OutputFilterImpl<textwolf::charset::UCS2LE>());
			}
			else if (enc == "ucs4" || enc == "ucs4be")
			{
				m_inputfilter.reset( new InputFilterImpl<textwolf::charset::UCS4BE>( encoding));
				m_outputfilter.reset( new OutputFilterImpl<textwolf::charset::UCS4BE>());
			}
			else if (enc == "ucs4le")
			{
				m_inputfilter.reset( new InputFilterImpl<textwolf::charset::UCS4LE>( encoding));
				m_outputfilter.reset( new OutputFilterImpl<textwolf::charset::UCS4LE>());
			}
			else
			{
				throw std::runtime_error( "unknown character set encoding");
			}
		}
	}

	static void parseEncoding( std::string& dest, const std::string& src)
	{
		dest.clear();
		std::string::const_iterator cc=src.begin();
		for (; cc != src.end(); ++cc)
		{
			if (*cc <= ' ') continue;
			if (*cc == '-') continue;
			if (*cc == ' ') continue;
			dest.push_back( ::tolower( *cc));
		}
	}
};

Filter _Wolframe::langbind::createTokenFilter( const std::string& name, const std::vector<FilterArgument>& arg)
{
	const char* filterbasename = "token";
	std::string nam( name);
	std::transform( nam.begin(), nam.end(), nam.begin(), ::tolower);
	if (nam != filterbasename) throw std::runtime_error( "token filter name does not match");
	const char* encoding = 0;
	std::vector<FilterArgument>::const_iterator ai = arg.begin(), ae = arg.end();
	for (; ai != ae; ++ai)
	{
		if (ai->first.empty() || boost::algorithm::iequals( ai->first, "encoding"))
		{
			encoding = ai->second.c_str();
			break;
		}
	}
	return encoding?TokenFilter( encoding):TokenFilter();
}

Filter* _Wolframe::langbind::createTokenFilterPtr( const std::string& name, const std::vector<FilterArgument>& arg)
{
	return new Filter( createTokenFilter( name, arg));
}
