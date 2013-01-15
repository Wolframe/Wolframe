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
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
///\file filter_expression_filter.cpp
///\brief Filter implementation reading/writing of expressions

#include "filter/expression_filter.hpp"
#include "textwolf/charset.hpp"
#include "textwolf/sourceiterator.hpp"
#include "textwolf/textscanner.hpp"
#include "textwolf/cstringiterator.hpp"
#include <string>
#include <vector>
#include <cstring>
#include <cstddef>
#include <cctype>

using namespace _Wolframe;
using namespace langbind;

namespace {

enum TokenType
{
	NoToken,				//< undefined token
	Identifier,				//< identifier
	Integer,				//< integer
	Float,					//< floating point number
	String,					//< string (SQL string with doubled single quotes for escaping quotes)
	OpenBracket,				//< open bracket '('
	CloseBracket,				//< close bracket ')'
	Comma,					//< comma ','
	Equal					//< equal '='
};

///\class OutputFilterImpl
template <class IOCharset, class AppCharset=textwolf::charset::UTF8>
struct OutputFilterImpl :public OutputFilter
{
	///\brief Constructor
	OutputFilterImpl()
		:m_elemitr(0)
		,m_lasttype(CloseTag){}

	///\brief Copy constructor
	///\param [in] o output filter to copy
	OutputFilterImpl( const OutputFilterImpl& o)
		:OutputFilter(o)
		,m_elembuf(o.m_elembuf)
		,m_elemitr(o.m_elemitr)
		,m_lasttype(o.m_lasttype){}

	///\brief self copy
	///\return copy of this
	virtual OutputFilter* copy() const
	{
		return new OutputFilterImpl( *this);
	}

	///\brief Prints a character string
	///\param [in] src pointer to string to print
	///\param [in] srcsize size of src in bytes
	void printToBuffer( const char* src, std::size_t srcsize)
	{
		textwolf::CStringIterator itr( src, srcsize);
		textwolf::TextScanner<textwolf::CStringIterator,AppCharset> ts( itr);

		textwolf::UChar ch;
		while ((ch = ts.chr()) != 0)
		{
			IOCharset::print( ch, m_elembuf);
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

	TokenType getTokenType( const char* e, std::size_t n)
	{
		TokenType rt = NoToken;
		textwolf::TextScanner<textwolf::CStringIterator,AppCharset> itr( textwolf::CStringIterator( e, n));
		textwolf::UChar ch;

		if ((ch = itr.chr()) != 0)
		{
			if (ch > 128 || std::isalpha( ch) || ch == '_')
			{
				rt = Identifier;
				for (++itr; (ch = itr.chr()) != 0; ++itr)
				{
					if (ch > 128 || std::isalnum( ch) || ch == '_') continue;
					rt = String;
					break;
				}
			}
			else if (ch >= '0' && ch <= '9')
			{
				rt = Integer;
				for (++itr; (ch = itr.chr()) != 0; ++itr)
				{
					if (ch >= '0' && ch <= '9') continue;
					if (ch == '.' && rt == Integer)
					{
						rt = Float;
						continue;
					}
					rt = String;
					break;
				}
			}
			else
			{
				rt = String;
			}
			for (; (ch = itr.chr()) != 0; ++itr)
			{
				if (rt != String && ch == ' ')
				{
					rt = String;
					continue;
				}
				if (ch < 32)
				{
					rt = NoToken;
					break;
				}
			}
		}
		return rt;
	}

	bool printToken( const void* element, std::size_t elementsize)
	{
		TokenType typ = getTokenType( (const char*)element, elementsize);
		textwolf::TextScanner<textwolf::CStringIterator,AppCharset> itr( textwolf::CStringIterator( (const char*)element, elementsize));
		textwolf::UChar ch;

		if (typ == NoToken)
		{
			setState( Error, "non printable token printed");
			return false;
		}
		else if (typ == String)
		{
			IOCharset::print( '\'', m_elembuf);
			for (; (ch = itr.chr()) != 0; ++itr)
			{
				if (ch == '\'') IOCharset::print( '\'', m_elembuf);
				IOCharset::print( ch, m_elembuf);
			}
			IOCharset::print( '\'', m_elembuf);
		}
		else
		{
			printToBuffer( (const char*)element, elementsize);
		}
		return true;
	}

	///\brief Implementation of OutputFilter::print(typename OutputFilter::ElementType,const void*,std::size_t)
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
		switch (type)
		{
			case OpenTag:
				printToBuffer( (const char*)element, elementsize);
				IOCharset::print( '(', m_elembuf);
				m_stk.push_back( State());
				break;

			case Attribute:
				printToBuffer( (const char*)element, elementsize);
				IOCharset::print( '=', m_elembuf);
				break;

			case CloseTag:
				IOCharset::print( ')', m_elembuf);
				m_stk.pop_back();
				break;

			case Value:
				if (m_lasttype == Attribute)
				{
					printToBuffer( (const char*)element, elementsize);
				}
				else
				{
					if (m_stk.back().m_argc > 0)
					{
						IOCharset::print( ',', m_elembuf);
					}
					if (!printToken( (const char*)element, elementsize)) return false;
					if (m_lasttype != Attribute)
					{
						m_stk.pop_back();
					}
					break;
				}
		}
		m_lasttype = type;
		if (!emptybuf())
		{
			setState( EndOfBuffer);
			return false;
		}
		return true;
	}
private:
	struct State
	{
		State() :m_argc(0){}
		std::size_t m_argc;			//< argument counter
	};
	std::string m_elembuf;				//< buffer for the currently printed element
	std::size_t m_elemitr;				//< iterator to pass it to output
	std::vector<State> m_stk;			//< state stack
	OutputFilter::ElementType m_lasttype;		//< last printed type
};

///\class InputFilterImpl
template <class IOCharset, class AppCharset=textwolf::charset::UTF8>
struct InputFilterImpl :public InputFilter
{
	typedef textwolf::TextScanner<textwolf::SrcIterator,IOCharset> TextScanner;

	///\brief Constructor
	InputFilterImpl()
		:m_src(0)
		,m_srcsize(0)
		,m_srcend(false)
		,m_tokentype(NoToken)
		,m_quotcnt(0){}

	///\brief Copy constructor
	///\param [in] o output filter to copy
	InputFilterImpl( const InputFilterImpl& o)
		:InputFilter( o)
		,m_itr(o.m_itr)
		,m_elembuf(o.m_elembuf)
		,m_src(o.m_src)
		,m_srcsize(o.m_srcsize)
		,m_srcend(o.m_srcend)
		,m_stk(o.m_stk)
		,m_tokentype(o.m_tokentype)
		,m_quotcnt(o.m_quotcnt){}

	///\brief Self copy
	///\return copy of this
	virtual InputFilter* copy() const
	{
		return new InputFilterImpl( *this);
	}

	///\brief Implement interface member InputFilter::putInput(const void*,std::size_t,bool)
	virtual void putInput( const void* ptr, std::size_t size, bool end)
	{
		m_src = (const char*)ptr;
		m_srcend = end;
		m_srcsize = size;
		m_itr.setSource( textwolf::SrcIterator( m_src, m_srcsize, m_srcend));
	}

	///\brief Implement interface member InputFilter::getRest(const void*&,std::size_t&,bool&)
	virtual void getRest( const void*& ptr, std::size_t& size, bool& end)
	{
		ptr = m_src + m_itr.getPosition();
		size = m_srcsize - m_itr.getPosition();
		end = m_srcend;
	}

	bool checkTokenDelimiter( textwolf::UChar ch)
	{
		if (ch == ')' || ch == ')' || ch == '=' || ch == ',' || std::isspace( ch))
		{
			return true;
		}
		setState( Error, "Illegal token in expression");
		return false;
	}

	bool parseToken()
	{
		if (m_tokentype == NoToken)
		{
			m_elembuf.clear();
			m_quotcnt = 0;

			textwolf::UChar ch;
			while ((ch = *m_itr) != 0)
			{
				if (ch > 128 || std::isalpha( ch) || ch == '_')
				{
					AppCharset::print( ch, m_elembuf);
					++m_itr;
					m_tokentype = Identifier;
					break;
				}
				else if ((ch >= '0' && ch <= '9') || ch == '-' || ch == '+')
				{
					AppCharset::print( ch, m_elembuf);
					++m_itr;
					m_tokentype = Integer;
					break;
				}
				else if (std::isspace( ch))
				{
					++m_itr;
					continue;
				}
				else if (ch == '\'')
				{
					++m_itr;
					m_tokentype = String;
					break;
				}
				else if (ch == ',')
				{
					++m_itr;
					m_tokentype = Comma;
					return true;
				}
				else if (ch == ',')
				{
					++m_itr;
					m_tokentype = Equal;
					return true;
				}
				else if (ch == '(')
				{
					++m_itr;
					m_tokentype = OpenBracket;
					return true;
				}
				else if (ch == ')')
				{
					++m_itr;
					m_tokentype = CloseBracket;
					return true;
				}
				else
				{
					setState( Error, "illegal token in expression");
					return false;
				}
			}
			for (;;) switch (m_tokentype)
			{
				case OpenBracket:
				case CloseBracket:
				case Comma:
				case Equal:
				case NoToken:
					setState( Error, "illegal state");
					return false;

				case Identifier:
					while ((ch = *m_itr) != 0)
					{
						if (ch > 128 || std::isalnum( ch) || ch == '_')
						{
							AppCharset::print( ch, m_elembuf);
							++m_itr;
						}
						else
						{
							break;
						}
					}
					return checkTokenDelimiter( ch);

				case Float:
				case Integer:
					while ((ch = *m_itr) != 0)
					{
						if (ch >= '0' && ch <= '9')
						{
							AppCharset::print( ch, m_elembuf);
							++m_itr;
						}
						else
						{
							break;
						}
					}
					if (ch == '.')
					{
						if (m_tokentype == Integer)
						{
							AppCharset::print( ch, m_elembuf);
							++m_itr;
							m_tokentype = Float;
							continue;
						}
						else
						{
							setState( Error, "illegal token in expression");
							return false;
						}
					}
					return checkTokenDelimiter( ch);

				case String:
					while ((ch = *m_itr) != 0)
					{
						if (ch == '\'')
						{
							if (m_quotcnt == 1)
							{
								m_quotcnt = 0;
								AppCharset::print( ch, m_elembuf);
								++m_itr;
							}
							else
							{
								m_quotcnt = 1;
							}
						}
						else if (m_quotcnt == 1)
						{
							m_quotcnt = 0;
							return checkTokenDelimiter( ch);
						}
						else if (ch < 32)
						{
							setState( Error, "non printable character in string");
							return false;
						}
						AppCharset::print( ch, m_elembuf);
						++m_itr;
					}
			}
		}
		setState( Error, "illegal state");
		return false;
	}

	///\brief implement interface member InputFilter::getNext( typename InputFilter::ElementType&,const void*&,std::size_t&)
	virtual bool getNext( typename InputFilter::ElementType& type, const void*& element, std::size_t& elementsize)
	{
		setState( Open);
		type = Value;
		try
		{
			if (parseToken())
			{
				switch (m_tokentype)
				{
					case NoToken:
						setState( Error, "illegal state");
						return false;
					case OpenBracket:
					case CloseBracket:
					case Comma:
					case Equal:
					case Identifier:
					case Float:
					case Integer:
					case String:
						type = Value;
						element = m_elembuf.c_str();
						elementsize = m_elembuf.size();
						return true;
				}
			}
		}
		catch (textwolf::SrcIterator::EoM)
		{
			setState( EndOfMessage);
		}
		return false;
	}
private:
	TextScanner m_itr;				//< iterator on source
	std::string m_elembuf;				//< buffer for current token
	typename InputFilter::ElementType m_lasttype;	//< last element type parsed
	const char* m_src;				//< pointer to current chunk parsed
	std::size_t m_srcsize;				//< size of the current chunk parsed in bytes
	bool m_srcend;					//< true if end of message is in current chunk parsed
	struct State
	{
		State() :m_argc(0){}
		std::size_t m_argc;			//< argument counter
	};
	std::vector<State> m_stk;			//< state stack
	TokenType m_tokentype;				//< the currently parsed token type
	int m_quotcnt;					//< counted escaping quotes in a row
};

}//end anonymous namespace


class ExpressionFilter :public Filter
{
public:
	ExpressionFilter( const char *encoding=0)
	{
		if (!encoding)
		{
			m_inputfilter.reset( new InputFilterImpl<textwolf::charset::UTF8>());
			m_outputfilter.reset( new OutputFilterImpl<textwolf::charset::UTF8>());
		}
		else
		{
			std::string enc;
			parseEncoding( enc, encoding);

			if ((enc.size() >= 8 && std::memcmp( enc.c_str(), "isolatin", 8)== 0)
			||  (enc.size() >= 7 && std::memcmp( enc.c_str(), "iso8859", 7) == 0))
			{
				m_inputfilter.reset( new InputFilterImpl<textwolf::charset::IsoLatin1>());
				m_outputfilter.reset( new OutputFilterImpl<textwolf::charset::IsoLatin1>());
			}
			else if (enc.size() == 0 || enc == "utf8")
			{
				m_inputfilter.reset( new InputFilterImpl<textwolf::charset::UTF8>());
				m_outputfilter.reset( new OutputFilterImpl<textwolf::charset::UTF8>());
			}
			else if (enc == "utf16" || enc == "utf16be")
			{
				m_inputfilter.reset( new InputFilterImpl<textwolf::charset::UTF16BE>());
				m_outputfilter.reset( new OutputFilterImpl<textwolf::charset::UTF16BE>());
			}
			else if (enc == "utf16le")
			{
				m_inputfilter.reset( new InputFilterImpl<textwolf::charset::UTF16LE>());
				m_outputfilter.reset( new OutputFilterImpl<textwolf::charset::UTF16LE>());
			}
			else if (enc == "ucs2" || enc == "ucs2be")
			{
				m_inputfilter.reset( new InputFilterImpl<textwolf::charset::UCS2BE>());
				m_outputfilter.reset( new OutputFilterImpl<textwolf::charset::UCS2BE>());
			}
			else if (enc == "ucs2le")
			{
				m_inputfilter.reset( new InputFilterImpl<textwolf::charset::UCS2LE>());
				m_outputfilter.reset( new OutputFilterImpl<textwolf::charset::UCS2LE>());
			}
			else if (enc == "ucs4" || enc == "ucs4be")
			{
				m_inputfilter.reset( new InputFilterImpl<textwolf::charset::UCS4BE>());
				m_outputfilter.reset( new OutputFilterImpl<textwolf::charset::UCS4BE>());
			}
			else if (enc == "ucs4le")
			{
				m_inputfilter.reset( new InputFilterImpl<textwolf::charset::UCS4LE>());
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

Filter ExpressionFilterFactory::create( const char* encoding) const
{
	return ExpressionFilter( encoding);
}

