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
///\file filter_textwolf_filter.cpp
///\brief Filter implementation reading/writing xml with the textwolf xml library
#include "filter/textwolf_filter.hpp"
#include "filter/textwolf/sourceiterator.hpp"
#include "filter/textwolf/xmlparser.hpp"
#include "filter/textwolf/xmlprinter.hpp"
#include "filter/textwolf/cstringiterator.hpp"
#include <string>
#include <cstddef>

using namespace _Wolframe;
using namespace langbind;


namespace {

typedef textwolf::XMLParser<textwolf::CStringIterator,std::string>  XMLParser;

struct XMLFilterAttributes
{
	CountedReference<std::string> m_encoding;

	void setEncoding( const std::string& encoding)
	{
		m_encoding.reset( new std::string( encoding));
	}
	const std::string& getEncoding()
	{
		static std::string empty;
		return m_encoding.get()?(*m_encoding.get()):empty;
	}
	XMLFilterAttributes(){}
	XMLFilterAttributes( const XMLFilterAttributes& o) :m_encoding(o.m_encoding){}
};

///\class InputFilterImpl
///\brief input filter for XML using textwolf
struct InputFilterImpl:public InputFilter
{
	typedef InputFilter Parent;

	///\brief Constructor
	InputFilterImpl( const XMLFilterAttributes& attr)
		:m_attributes(attr)
		,m_parser( attr)
		,m_src(0)
		,m_srcsize(0)
		,m_srcend(false){}


	///\brief Copy constructor
	///\param [in] o output filter to copy
	InputFilterImpl( const InputFilterImpl& o)
		:InputFilter( o)
		,m_attributes( o.m_attributes)
		,m_parser( o.m_parser)
		,m_src(o.m_src)
		,m_srcsize(o.m_srcsize)
		,m_srcend(o.m_srcend){}

	///\brief Get a member value of the filter
	///\param [in] name case sensitive name of the variable
	///\param [in] val the value returned
	///\return true on success, false, if the variable does not exist or the operation failed
	virtual bool getValue( const char* name, std::string& val) const
	{
		if (std::strcmp( name, "empty") == 0)
		{
			val = m_parser.withEmpty();
			return true;
		}
		if (std::strcmp( name, "tokenize") == 0)
		{
			val = m_parser.doTokenize();
			return true;
		}
		return Parent::getValue( name, val);
	}

	///\brief Set a member value of the filter
	///\param [in] name case sensitive name of the variable
	///\param [in] value new value of the variable to set
	///\return true on success, false, if the variable does not exist or the operation failed
	virtual bool setValue( const char* name, const std::string& value)
	{
		if (std::strcmp( name, "empty") == 0)
		{
			if (std::strcmp( value.c_str(), "true") == 0)
			{
				m_parser.withEmpty(true);
			}
			else if (std::strcmp( value.c_str(), "false") == 0)
			{
				m_parser.withEmpty(false);
			}
			else
			{
				return false;
			}
			return true;
		}
		if (std::strcmp( name, "tokenize") == 0)
		{
			if (std::strcmp( value.c_str(), "true") == 0)
			{
				m_parser.doTokenize(true);
			}
			else if (std::strcmp( value.c_str(), "false") == 0)
			{
				m_parser.doTokenize(false);
			}
			else
			{
				return false;
			}
			return true;
		}
		return Parent::setValue( name, value);
	}

	///\brief self copy
	///\return copy of this
	virtual InputFilter* copy() const
	{
		return new InputFilterImpl( *this);
	}

	///\brief implement interface member InputFilter::putInput(const void*,std::size_t,bool)
	virtual void putInput( const void* ptr, std::size_t size, bool end)
	{
		m_src = (const char*)ptr;
		m_srcend = end;
		m_srcsize = size;
		m_parser.putInput( m_src, m_srcsize, m_srcend);
	}

	virtual void getRest( const void*& ptr, std::size_t& size, bool& end)
	{
		ptr = m_src + m_parser.getPosition();
		size = m_srcsize - m_parser.getPosition();
		end = m_srcend;
	}

	///\brief implement interface member InputFilter::getNext(typename InputFilter::ElementType&,const void*&,std::size_t&)
	virtual bool getNext( typename InputFilter::ElementType& type, const void*& element, std::size_t& elementsize)
	{
		struct ElementTypeMap :public textwolf::CharMap<int,-1,textwolf::XMLScannerBase::NofElementTypes>
		{
			ElementTypeMap()
			{
				(*this)
				(textwolf::XMLScannerBase::None,-1)
				(textwolf::XMLScannerBase::ErrorOccurred,-1)
				(textwolf::XMLScannerBase::HeaderStart,(int)OpenTag)
				(textwolf::XMLScannerBase::HeaderAttribName,(int)Attribute)
				(textwolf::XMLScannerBase::HeaderAttribValue,(int)Value)
				(textwolf::XMLScannerBase::HeaderEnd,(int)CloseTag)
				(textwolf::XMLScannerBase::TagAttribName,(int)Attribute)
				(textwolf::XMLScannerBase::TagAttribValue,(int)Value)
				(textwolf::XMLScannerBase::OpenTag,(int)OpenTag)
				(textwolf::XMLScannerBase::CloseTag,(int)CloseTag)
				(textwolf::XMLScannerBase::CloseTagIm,(int)CloseTag)
				(textwolf::XMLScannerBase::Content,(int)Value)
				(textwolf::XMLScannerBase::Exit,-1);
			}
		};
		static const ElementTypeMap tmap;
		try
		{
			const char* ee;
			textwolf::XMLScannerBase::ElementType et = m_parser.getNext( ee, elementsize);
			element = (const void*)ee;

			int st = tmap[ et];
			if (st == -1)
			{
				if (et != textwolf::XMLScannerBase::Exit)
				{
					setState( Error, "textwolf: syntax error in XML");
				}
				return false;
			}
			else
			{
				type = (InputFilterImpl::ElementType)st;
				return true;
			}
		}
		catch (textwolf::SrcIterator::EoM)
		{
			setState( EndOfMessage);
			return false;
		};
		return false;
	}
private:
	typedef textwolf::XMLParser<std::string,XMLFilterAttributes> XMLParser;

	XMLFilterAttributes m_attributes;	//< common attributes of input and output filter
	XMLParser m_parser;			//< XML parser
	const char* m_src;			//< pointer to current chunk parsed
	std::size_t m_srcsize;			//< size of the current chunk parsed in bytes
	bool m_srcend;				//< true if end of message is in current chunk parsed
};

///\class OutputFilter
///\brief output filter filter for XML using textwolf
struct OutputFilterImpl :public OutputFilter
{
	typedef OutputFilter Parent;

	///\brief Constructor
	///\param [in] bufsize (optional) size of internal buffer to use (for the tag hierarchy stack)
	OutputFilterImpl( const XMLFilterAttributes& attr)
		:m_elemitr(0)
		,m_attributes(attr)
		,m_printer(attr){}

	///\brief Copy constructor
	///\param [in] o output filter to copy
	OutputFilterImpl( const OutputFilterImpl& o)
		:OutputFilter(o)
		,m_elembuf(o.m_elembuf)
		,m_elemitr(o.m_elemitr)
		,m_attributes(o.m_attributes)
		,m_printer(o.m_printer){}

	virtual ~OutputFilterImpl(){}

	///\brief self copy
	///\return copy of this
	virtual OutputFilter* copy() const
	{
		return new OutputFilterImpl( *this);
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
			// we've done the printing left
			return true;
		}
		switch (type)
		{
			case OutputFilter::OpenTag:
				if (!m_printer.printOpenTag( (const char*)element, elementsize, m_elembuf))
				{
					setState( Error, "textwolf: illegal print operation (open tag)");
					return false;
				}
				break;
			case OutputFilter::CloseTag:
				if (!m_printer.printCloseTag( m_elembuf))
				{
					setState( Error, "textwolf: illegal print operation (close tag)");
					return false;
				}
				break;
			case OutputFilter::Attribute:
				if (!m_printer.printAttribute( (const char*)element, elementsize, m_elembuf))
				{
					setState( Error, "textwolf: illegal print operation (attribute)");
					return false;
				}
				break;
			case OutputFilter::Value:
				if (!m_printer.printValue( (const char*)element, elementsize, m_elembuf))
				{
					setState( Error, "textwolf: illegal print operation (value)");
					return false;
				}
				break;
		}
		if (!emptybuf())
		{
			setState( EndOfBuffer);
			return false;
		}
		return true;
	}

private:
	typedef textwolf::XMLPrinter<std::string,XMLFilterAttributes> XMLPrinter;

	std::string m_elembuf;				//< buffer for the currently printed element
	std::size_t m_elemitr;				//< iterator to pass it to output
	XMLFilterAttributes m_attributes;		//< common attributes of input and output filter
	XMLPrinter m_printer;				//< xml printer object
};

}//end anonymous namespace

class TextwolfXmlFilter :public Filter
{
public:
	TextwolfXmlFilter( const char* encoding=0)
	{
		XMLFilterAttributes attr;
		if (encoding)
		{
			attr.setEncoding( encoding);
		}
		m_inputfilter.reset( new InputFilterImpl( attr));
		m_outputfilter.reset( new OutputFilterImpl( attr));
	}
};

Filter TextwolfXmlFilterFactory::create( const char* encoding) const
{
	return TextwolfXmlFilter( encoding);
}

