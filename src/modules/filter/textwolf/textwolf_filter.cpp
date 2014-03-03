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
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
///\file textwolf_filter.cpp
///\brief Filter implementation reading/writing xml with the textwolf xml library
#include "textwolf_filter.hpp"
#include "types/doctype.hpp"
#include "textwolf/sourceiterator.hpp"
#include "textwolf/xmlparser.hpp"
#include "textwolf/xmlprinter.hpp"
#include "textwolf/cstringiterator.hpp"
#include "types/countedReference.hpp"
#include <string>
#include <cstddef>
#include <algorithm>
#include <boost/algorithm/string.hpp>

using namespace _Wolframe;
using namespace langbind;

namespace {

typedef textwolf::XMLParser<std::string>  XMLParser;

///\class InputFilterImpl
///\brief input filter for XML using textwolf
struct InputFilterImpl
	:public InputFilter
{
	typedef InputFilter Parent;

	///\brief Constructor
	InputFilterImpl()
		:utils::TypeSignature("langbind::InputFilterImpl (textwolf)", __LINE__)
		,m_src(0)
		,m_srcsize(0)
		,m_srcend(false)
		{}
	///\brief Copy constructor
	///\param [in] o output filter to copy
	InputFilterImpl( const InputFilterImpl& o)
		:utils::TypeSignature("langbind::InputFilterImpl (textwolf)", __LINE__)
		,InputFilter( o)
		,m_parser( o.m_parser)
		,m_src(o.m_src)
		,m_srcsize(o.m_srcsize)
		,m_srcend(o.m_srcend)
		{}

	///\brief Implementation of FilterBase::getValue( const char*, std::string&)
	virtual bool getValue( const char* name, std::string& val)
	{
		if (std::strcmp( name, "empty") == 0)
		{
			val = m_parser.withEmpty()?"true":"false";
			return true;
		}
		if (std::strcmp( name, "tokenize") == 0)
		{
			val = m_parser.doTokenize()?"true":"false";
			return true;
		}
		return Parent::getValue( name, val);
	}

	///\brief Implementation of InputFilter::getDocType(std::string&)
	virtual bool getDocType( std::string& val)
	{
		types::DocType doctype;
		if (getDocType( doctype))
		{
			val = doctype.tostring();
			return true;
		}
		else
		{
			return false;
		}
	}

	///\brief Implementation of FilterBase::setValue( const char*, const std::string&)
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
		std::size_t pp = m_parser.getPosition();
		ptr = m_src + pp;
		size = (m_srcsize > pp)?(m_srcsize-pp):0;
		end = m_srcend;
	}

	///\brief implement interface member InputFilter::getNext(typename InputFilter::ElementType&,const void*&,std::size_t&)
	virtual bool getNext( InputFilter::ElementType& type, const void*& element, std::size_t& elementsize)
	{
		struct ElementTypeMap :public textwolf::CharMap<int,-1,textwolf::XMLScannerBase::NofElementTypes>
		{
			ElementTypeMap()
			{
				(*this)
				(textwolf::XMLScannerBase::ErrorOccurred,-2)
				(textwolf::XMLScannerBase::TagAttribName,(int)Attribute)
				(textwolf::XMLScannerBase::TagAttribValue,(int)Value)
				(textwolf::XMLScannerBase::OpenTag,(int)OpenTag)
				(textwolf::XMLScannerBase::CloseTag,(int)CloseTag)
				(textwolf::XMLScannerBase::CloseTagIm,(int)CloseTag)
				(textwolf::XMLScannerBase::Content,(int)Value)
				(textwolf::XMLScannerBase::Exit,(int)CloseTag);
			}
		};
		static const ElementTypeMap tmap;
		try
		{
			for (;;)
			{
				const char* ee;
				textwolf::XMLScannerBase::ElementType et = m_parser.getNext( ee, elementsize);
				element = (const void*)ee;
				int st = tmap[ et];
				if (st < 0)
				{
					if (st == -1) continue;
					setState( Error, "textwolf: syntax error in XML");
					return false;
				}
				else
				{
					type = (InputFilterImpl::ElementType)st;
					return true;
				}
			}
		}
		catch (textwolf::SrcIterator::EoM)
		{
			setState( EndOfMessage);
			return false;
		};
		return false;
	}

	///\brief Implements InputFilter::getMetadata()
	virtual bool getMetadata()
	{
		try
		{
			const char* ee;
			std::size_t eesize;
			for (;;) switch (m_parser.state())
			{
				case XMLParser::ParseHeader:
					m_parser.getNext( ee, eesize);
					continue;

				case XMLParser::ParseSource:
				case XMLParser::ParseDoctype:
					return true;
			}
		}
		catch (textwolf::SrcIterator::EoM)
		{
			setState( EndOfMessage);
			return false;
		};
	}

private:
	///\brief Get the document type definition, if available
	///\param [out] doctype definition parsed
	///\return true, if success, false, if not.
	///\remark Check the state when false is returned
	bool getDocType( types::DocType& doctype)
	{
		try
		{
			const char* err;
			if (m_parser.state() != XMLParser::ParseSource)
			{
				if (!m_parser.parseHeader( err))
				{
					setState( Error, err);
					return false;
				}
				if (m_parser.isStandalone())
				{
					return true;
				}
			}
			doctype.rootid = m_parser.getDoctypeRoot();
			doctype.publicid = m_parser.getDoctypePublic();
			doctype.systemid = m_parser.getDoctypeSystem();
			return true;
		}
		catch (textwolf::SrcIterator::EoM)
		{
			setState( EndOfMessage);
			return false;
		};
	}

	///\brief Implements 'ContentFilterAttributes::getEncoding() const'
	virtual const char* getEncoding() const
	{
		return m_parser.getEncoding();
	}

	///\brief Implements FilterBase::setFlags()
	virtual bool setFlags( Flags f)
	{
		if (0!=((int)f & (int)langbind::FilterBase::SerializeWithIndices))
		{
			return false;
		}
		return InputFilter::setFlags( f);
	}

private:
	typedef textwolf::XMLParser<std::string> XMLParser;
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
	OutputFilterImpl( const ContentFilterAttributes* attr=0)
		:utils::TypeSignature("langbind::OutputFilterImpl (textwolf)", __LINE__)
		,OutputFilter(attr)
		,m_elemitr(0)
		,m_encodingSet(false){}

	///\brief Copy constructor
	///\param [in] o output filter to copy
	OutputFilterImpl( const OutputFilterImpl& o)
		:utils::TypeSignature("langbind::OutputFilterImpl (textwolf)", __LINE__)
		,OutputFilter(o)
		,m_elembuf(o.m_elembuf)
		,m_elemitr(o.m_elemitr)
		,m_encoding(o.m_encoding)
		,m_encodingSet(o.m_encodingSet)
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

	///\brief Implementation of OutputFilter::setDocType(const std::string&,const std::string&)
	//\param[in] systemid SYSTEM part of XML doctype
	//\param[in] rootelement XML root element
	virtual void setDocType( const std::string& systemid, const std::string& rootelement)
	{
		types::DocType doctype( rootelement.c_str(), 0, systemid.c_str());
		const char* ro = doctype.rootid.empty()?0:doctype.rootid.c_str();
		const char* pu = doctype.publicid.empty()?0:doctype.publicid.c_str();
		const char* sy = doctype.systemid.empty()?0:doctype.systemid.c_str();
		
		m_printer.setDocumentType( ro, pu, sy);
	}

	void setEncoding( const std::string& value)
	{
		m_encoding = value;
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
		switch (type)
		{
			case OutputFilter::OpenTag:
				if (!m_encodingSet)
				{
					const char* enc = encoding();
					m_printer.setEncoding( enc?enc:"UTF-8", m_elembuf);
					m_encodingSet = true;
				}
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

	const char* encoding() const
	{
		if (m_encoding.empty())
		{
			if (attributes())
			{
				return attributes()->getEncoding();
			}
			return 0;
		}
		else
		{
			return m_encoding.c_str();
		}
	}

	///\brief Implementation of FilterBase::getValue( const char*, std::string&)
	virtual bool getValue( const char* name, std::string& val)
	{
		if (std::strcmp( name, "encoding") == 0)
		{
			const char* ee = encoding();
			if (ee)
			{
				val = ee;
				return true;
			}
			return false;
		}
		return Parent::getValue( name, val);
	}

	///\brief Implementation of FilterBase::setValue( const char*, const std::string&)
	virtual bool setValue( const char* name, const std::string& value)
	{
		if (std::strcmp( name, "encoding") == 0)
		{
			m_encoding = value;
			return true;
		}
		return Parent::setValue( name, value);
	}

private:
	typedef textwolf::XMLPrinter<std::string> XMLPrinter;

	std::string m_elembuf;				//< buffer for the currently printed element
	std::size_t m_elemitr;				//< iterator to pass it to output
	std::string m_encoding;				//< common attributes of input and output filter
	bool m_encodingSet;				//< true, if the output character set encoding has been defined
	XMLPrinter m_printer;				//< xml printer object
};

}//end anonymous namespace

class TextwolfXmlFilter :public Filter
{
public:
	TextwolfXmlFilter( const char* encoding=0)
	{
		m_inputfilter.reset( new InputFilterImpl());
		OutputFilterImpl* oo = new OutputFilterImpl( m_inputfilter.get());
		m_outputfilter.reset( oo);
		if (encoding)
		{
			oo->setEncoding( encoding);
		}
	}
};


class TextwolfXmlFilterType :public FilterType
{
public:
	TextwolfXmlFilterType(){}
	virtual ~TextwolfXmlFilterType(){}

	virtual Filter* create( const std::vector<FilterArgument>& arg) const
	{
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
		return new TextwolfXmlFilter( encoding);
	}
};

FilterType* _Wolframe::langbind::createTextwolfXmlFilterType()
{
	return new TextwolfXmlFilterType();
}

