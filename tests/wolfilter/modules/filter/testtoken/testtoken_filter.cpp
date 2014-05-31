/************************************************************************
Copyright (C) 2011 - 2014 Project Wolframe.
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
///\file testtoken_filter.cpp
///\brief Filter implementation of a filter for wolfilter test input/output

#include "testtoken_filter.hpp"
#include <string>
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
struct OutputFilterImpl :public OutputFilter
{
	///\brief Constructor
	explicit OutputFilterImpl( const types::DocMetaDataR& md)
		:utils::TypeSignature("langbind::OutputFilterImpl (token)", __LINE__)
		,OutputFilter("token", md)
		,m_elemitr(0)
		{}

	///\brief Copy constructor
	///\param [in] o output filter to copy
	OutputFilterImpl( const OutputFilterImpl& o)
		:utils::TypeSignature("langbind::OutputFilterImpl (token)", __LINE__)
		,OutputFilter(o)
		,m_elembuf(o.m_elembuf)
		,m_elemitr(o.m_elemitr)
		{}

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
		std::size_t srcidx = 0;

		for (; srcidx < srcsize; ++srcidx)
		{
			if (src[srcidx] == '\n')
			{
				buf.push_back( '\n');
				buf.push_back( (char)TokenNextLine);
			}
			else
			{
				buf.push_back( src[srcidx]);
			}
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

	///\brief Implementation of OutputFilter::print(OutputFilter::ElementType,const void*,std::size_t)
	///\param [in] type type of the element to print
	///\param [in] element pointer to the element to print
	///\param [in] elementsize size of the element to print in bytes
	///\return true, if success, false else
	virtual bool print( OutputFilter::ElementType type, const void* element, std::size_t elementsize)
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
		m_elembuf.push_back( getElementTag( type));
		printToBufferEscEOL( (const char*)element, elementsize, m_elembuf);
		m_elembuf.push_back( '\n');
		if (!emptybuf())
		{
			setState( EndOfBuffer);
			return false;
		}
		return true;
	}

	virtual bool close() {return true;}

private:
	std::string m_elembuf;				//< buffer for the currently printed element
	std::size_t m_elemitr;				//< iterator to pass it to output
};

///\class InputFilterImpl
struct InputFilterImpl :public InputFilter
{
	///\brief Constructor
	InputFilterImpl()
		:utils::TypeSignature("langbind::InputFilterImpl (token)", __LINE__)
		,InputFilter("token")
		,m_tag(0)
		,m_taglevel(0)
		,m_elemtype(OpenTag)
		,m_src(0)
		,m_srcsize(0)
		,m_srcpos(0)
		,m_srcend(false)
		,m_linecomplete(false)
		,m_eolnread(false){}

	///\brief Copy constructor
	///\param [in] o output filter to copy
	InputFilterImpl( const InputFilterImpl& o)
		:utils::TypeSignature("langbind::InputFilterImpl (token)", __LINE__)
		,InputFilter(o)
		,m_tag(o.m_tag)
		,m_taglevel(o.m_taglevel)
		,m_elemtype(o.m_elemtype)
		,m_elembuf(o.m_elembuf)
		,m_src(o.m_src)
		,m_srcsize(o.m_srcsize)
		,m_srcpos(o.m_srcpos)
		,m_srcend(o.m_srcend)
		,m_linecomplete(o.m_linecomplete)
		,m_eolnread(o.m_eolnread){}

	///\brief Implement InputFilterImpl::copy()
	virtual InputFilter* copy() const
	{
		return new InputFilterImpl( *this);
	}

	///\brief Implement InputFilterImpl::initcopy()
	virtual InputFilter* initcopy() const
	{
		return new InputFilterImpl();
	}

	///\brief Implement InputFilterImpl::putInput(const void*,std::size_t,bool)
	virtual void putInput( const void* ptr, std::size_t size, bool end)
	{
		m_src = (const char*)ptr;
		m_srcend = end;
		m_srcsize = size;
		m_srcpos = 0;
	}

	virtual void getRest( const void*& ptr, std::size_t& size, bool& end)
	{
		ptr = m_src + m_srcpos;
		size = (m_srcsize > m_srcpos)?(m_srcsize - m_srcpos):0;
		end = m_srcend;
	}

	///\brief Implement InputFilter::getNext( InputFilter::ElementType&,const void*&,std::size_t&)
	virtual bool getNext( InputFilter::ElementType& type, const void*& element, std::size_t& elementsize)
	{
		if (m_linecomplete)
		{
			m_elembuf.clear();
			m_linecomplete = false;
			if (m_taglevel == -1) return false;
		}
		setState( Open);
		type = Value;

		if (!m_tag)
		{
			char tg;
			do
			{
				if (m_srcpos < m_srcsize)
				{
					tg = m_src[m_srcpos];
					++m_srcpos;
				}
				else
				{
					tg = 0;
				}
			}
			while (tg == '\n' || tg == '\r');
			if (!tg)
			{
				if (!m_srcend)
				{
					setState( EndOfMessage);
					return false;
				}
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
		unsigned char ch = 0;
		while (m_srcpos < m_srcsize)
		{
			ch = m_src[ m_srcpos];
			if (m_eolnread)
			{
				if (ch == (char)TokenNextLine)
				{
					m_elembuf.push_back( '\n');
					++m_srcpos;
					m_eolnread = false;
					continue;
				}
				else
				{
					break;
				}
			}
			else if (ch == '\r')
			{
				++m_srcpos;
				continue;
			}
			else if (ch == '\n')
			{
				++m_srcpos;
				m_eolnread = true;
				continue;
			}
			else
			{
				m_elembuf.push_back( ch);
				++m_srcpos;
			}
		}
		if (m_eolnread)
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
		if (!m_srcend)
		{
			setState( EndOfMessage);
		}
		return false;
	}

	virtual bool checkSetFlags( Flags f) const
	{
		return (0==((int)f & (int)langbind::FilterBase::SerializeWithIndices));
	}

	virtual bool setFlags( Flags f)
	{
		if (0!=((int)f & (int)langbind::FilterBase::SerializeWithIndices))
		{
			return false;
		}
		return InputFilter::setFlags( f);
	}

	virtual const types::DocMetaData* getMetaData()
	{
		return getMetaDataRef().get();
	}

private:
	char m_tag;			//< tag defining the currently parsed element type
	int m_taglevel;			//< tag level
	ElementType m_elemtype;		//< current element type
	std::string m_elembuf;		//< buffer for current line => current token
	const char* m_src;		//< pointer to current chunk parsed
	std::size_t m_srcsize;		//< size of the current chunk parsed in bytes
	std::size_t m_srcpos;		//< source iterator
	bool m_srcend;			//< true if end of message is in current chunk parsed
	bool m_linecomplete;		//< true if the last getNext could complete a line
	bool m_eolnread;		//< true if the end of line has been read
};

}//end anonymous namespace


class TokenFilter :public Filter
{
public:
	TokenFilter( const char* encoding=0)
	{
		if (!encoding || !encoding[0])
		{
			m_inputfilter.reset( new InputFilterImpl());
			m_outputfilter.reset( new OutputFilterImpl( m_inputfilter->getMetaDataRef()));
		}
		else
		{
			std::string enc;
			parseEncoding( enc, encoding);

			if (enc.size() == 0 || enc == "utf8")
			{
				m_inputfilter.reset( new InputFilterImpl());
				m_outputfilter.reset( new OutputFilterImpl( m_inputfilter->getMetaDataRef()));
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


static const char* getArgumentEncoding( const std::vector<FilterArgument>& arg)
{
	const char* encoding = 0;
	std::vector<FilterArgument>::const_iterator ai = arg.begin(), ae = arg.end();
	for (; ai != ae; ++ai)
	{
		if (ai->first.empty() || boost::algorithm::iequals( ai->first, "encoding"))
		{
			if (encoding)
			{
				if (ai->first.empty())
				{
					throw std::runtime_error( "too many filter arguments");
				}
				else
				{
					throw std::runtime_error( "duplicate filter argument 'encoding'");
				}
			}
			encoding = ai->second.c_str();
			break;
		}
		else
		{
			throw std::runtime_error( std::string( "unknown filter argument '") + ai->first + "'");
		}
	}
	return encoding;
}

class TokenFilterType :public FilterType
{
public:
	TokenFilterType()
		:FilterType("token"){}
	virtual ~TokenFilterType(){}

	virtual Filter* create( const std::vector<FilterArgument>& arg) const
	{
		return new TokenFilter( getArgumentEncoding( arg));
	}
};

FilterType* _Wolframe::langbind::createTokenFilterType()
{
	return new TokenFilterType();
}

