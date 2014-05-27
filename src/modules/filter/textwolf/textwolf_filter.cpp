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
/// \file textwolf_filter.cpp
/// \brief Filter implementation reading/writing xml with the textwolf xml library
#include "textwolf_filter.hpp"
#include "types/docmetadata.hpp"
#include "utils/fileUtils.hpp"
#include "textwolf/sourceiterator.hpp"
#include "textwolf/xmlparser.hpp"
#include "textwolf/xmlprinter.hpp"
#include "textwolf/cstringiterator.hpp"
#include "types/countedReference.hpp"
#include "logger-v1.hpp"
#include <string>
#include <cstddef>
#include <algorithm>
#include <boost/algorithm/string.hpp>

using namespace _Wolframe;
using namespace langbind;

namespace {

typedef textwolf::XMLParser<std::string>  XMLParser;

/// \class InputFilterImpl
/// \brief input filter for XML using textwolf
struct InputFilterImpl
	:public InputFilter
{
	typedef InputFilter Parent;

	/// \brief Constructor
	InputFilterImpl()
		:utils::TypeSignature("langbind::InputFilterImpl (textwolf)", __LINE__)
		,InputFilter("textwolf")
		,m_src(0)
		,m_srcsize(0)
		,m_srcend(false)
		,m_metadatastate(MS_Init)
		{}
	/// \brief Copy constructor
	/// \param [in] o output filter to copy
	InputFilterImpl( const InputFilterImpl& o)
		:utils::TypeSignature("langbind::InputFilterImpl (textwolf)", __LINE__)
		,InputFilter( o)
		,m_parser( o.m_parser)
		,m_src(o.m_src)
		,m_srcsize(o.m_srcsize)
		,m_srcend(o.m_srcend)
		,m_metadatastate(o.m_metadatastate)
		,m_elembuffer(o.m_elembuffer)
		{}

	/// \brief Implementation of FilterBase::getValue( const char*, std::string&)
	virtual bool getValue( const char* id, std::string& val) const
	{
		if (std::strcmp( id, "empty") == 0)
		{
			val = m_parser.withEmpty()?"true":"false";
			return true;
		}
		if (std::strcmp( id, "tokenize") == 0)
		{
			val = m_parser.doTokenize()?"true":"false";
			return true;
		}
		return Parent::getValue( id, val);
	}


	/// \brief Implementation of FilterBase::setValue( const char*, const std::string&)
	virtual bool setValue( const char* id, const std::string& value)
	{
		if (std::strcmp( id, "empty") == 0)
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
		if (std::strcmp( id, "tokenize") == 0)
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
		return Parent::setValue( id, value);
	}

	/// \brief self copy
	/// \return copy of this
	virtual InputFilter* copy() const
	{
		return new InputFilterImpl( *this);
	}

	/// \brief Implement InputFilter::initcopy()
	virtual InputFilter* initcopy() const
	{
		bool withEmpty_ = m_parser.withEmpty();
		bool doTokenize_ = m_parser.doTokenize();
		InputFilterImpl* rt = new InputFilterImpl();
		rt->m_parser.withEmpty( withEmpty_);
		rt->m_parser.doTokenize( doTokenize_);
		return rt;
	}

	/// \brief implement interface member InputFilter::putInput(const void*,std::size_t,bool)
	virtual void putInput( const void* ptr, std::size_t size, bool end)
	{
		m_src = (const char*)ptr;
		m_srcend = end;
		m_srcsize = size;
		m_parser.putInput( m_src, m_srcsize, m_srcend);
		setState( Open);
	}

	/// \brief Implement InputFilter::getRest(const void*&,std::size_t&,bool&)
	virtual void getRest( const void*& ptr, std::size_t& size, bool& end)
	{
		std::size_t pp = m_parser.getPosition();
		ptr = m_src + pp;
		size = (m_srcsize > pp)?(m_srcsize-pp):0;
		end = m_srcend;
	}

	/// \brief implement interface member InputFilter::getNext(typename InputFilter::ElementType&,const void*&,std::size_t&)
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
		if (m_metadatastate != MS_Done)
		{
			if (!getMetaData()) return false;
		}
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
					if (st == -2)
					{
						setState( Error, ee);
						return false;
					}
					else
					{
						setState( Error, "syntax error in XML");
						return false;
					}
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

	/// \brief Implements InputFilter::getMetaData()
	virtual const types::DocMetaData* getMetaData()
	{
		if (m_metadatastate == MS_Done) return getMetaDataRef().get();

		try
		{
			for (;;) switch (m_parser.state())
			{
				case XMLParser::ParseHeader:
				case XMLParser::ParseDoctype:
				{
					const char* err = 0;
					if (!m_parser.parseHeader( err) || !m_parser.hasMetadataParsed())
					{
						setState( Error, err?err:"unknown error");
					}
					continue;
				}
				case XMLParser::ParseSource:
				case XMLParser::ParseSourceReady:
				{
					const char* ee;
					std::size_t eesize;
					textwolf::XMLScannerBase::ElementType et;

					switch (m_metadatastate)
					{
						case MS_Init:
							m_metadatastate = MS_Root;
							ee = m_parser.getEncoding();
							if (ee)
							{
								getMetaDataRef()->setAttribute( "encoding", ee);
							}
							if (!m_parser.getDoctypePublic().empty())
							{
								getMetaDataRef()->setAttribute( "PUBLIC", m_parser.getDoctypePublic());
							}
							if (!m_parser.getDoctypeSystem().empty())
							{
								getMetaDataRef()->setAttribute( "SYSTEM", m_parser.getDoctypeSystem());
								getMetaDataRef()->setDoctype( types::DocMetaData::extractStem( m_parser.getDoctypeSystem()));
							}
							/*no break here!*/
						case MS_Root:
							et = m_parser.getNext( ee, eesize);
							if (et == textwolf::XMLScannerBase::OpenTag)
							{
								getMetaDataRef()->setAttribute( "root", std::string( ee, eesize));
								m_metadatastate = MS_AttribName;
							}
							else
							{
								setState( Error, "root element expected");
								return 0;
							}
							/*no break here!*/
						case MS_AttribName:
							et = m_parser.getNext( ee, eesize);
							if (et == textwolf::XMLScannerBase::OpenTag || et == textwolf::XMLScannerBase::CloseTag || et == textwolf::XMLScannerBase::CloseTagIm || et == textwolf::XMLScannerBase::Content)
							{
								m_parser.ungetElement( et, ee, eesize);
								m_metadatastate = MS_Done;
								continue;
							}
							else if (et == textwolf::XMLScannerBase::TagAttribName)
							{
								m_elembuffer = std::string( ee, eesize);
								m_metadatastate = MS_AttribValue;
							}
							else
							{
								setState( Error, "root element attribute or document start expected");
								return 0;
							}
							/*no break here!*/
						case MS_AttribValue:
							et = m_parser.getNext( ee, eesize);
							if (et == textwolf::XMLScannerBase::TagAttribValue)
							{
								if (m_elembuffer == "xmlns")
								{
									getMetaDataRef()->setAttribute( "xmlns", std::string( ee, eesize));
								}
								else if (m_elembuffer == "xmlns:xsi")
								{
									getMetaDataRef()->setAttribute( "xmlns:xsi", std::string( ee, eesize));
								}
								else if (m_elembuffer == "xmlns:schemaLocation")
								{
									getMetaDataRef()->setAttribute( "xmlns:xsi", std::string( ee, eesize));
									getMetaDataRef()->setDoctype( types::DocMetaData::extractStem( std::string( ee, eesize)));
								}
								else
								{
									std::string msg = std::string("unknown XML root element attribute '") + m_elembuffer + "'";
									setState( Error, msg.c_str());
								}
							}
							else
							{
								setState( Error, "root element attribute value expected");
								return 0;
							}
							m_metadatastate = MS_AttribName;
							break;
							
						case MS_Done:
							LOG_DEBUG << "[textwolf input] document meta data: {" << getMetaDataRef()->tostring() << "}";
							return getMetaDataRef().get();
					}
				}
			}
		}
		catch (textwolf::SrcIterator::EoM)
		{
			setState( EndOfMessage);
			return 0;
		};
	}

	/// \brief Implement InputFilter::checkMetaData(const types::DocMetaData&) const
	virtual bool checkMetaData( const types::DocMetaData& md)
	{
		if (m_metadatastate != MS_Done)
		{
			setState( Error, "input filter did not parse its meta data yet - cannot check them therefore");
		}
		// Check the XML root element:
		const char* form_rootelem = md.getAttribute( "root");
		const char* doc_rootelem = getMetaDataRef()->getAttribute( "root");
		if (form_rootelem)
		{
			if (!doc_rootelem)
			{
				setState( Error, "input document has no root element defined");
				return false;
			}
			if (0!=std::strcmp(form_rootelem,doc_rootelem))
			{
				std::string msg = std::string("input document root element '") + doc_rootelem + "' does not match the root element '" + form_rootelem + "'' required";
				setState( Error, msg.c_str());
				return false;
			}
		}
		return true;
	}

private:
	/// \brief Implements FilterBase::setFlags()
	virtual bool setFlags( Flags f)
	{
		if (0!=((int)f & (int)langbind::FilterBase::SerializeWithIndices))
		{
			return false;
		}
		return InputFilter::setFlags( f);
	}

	/// \brief Implements FilterBase::checkSetFlags()const
	virtual bool checkSetFlags( Flags f) const
	{
		return (0==((int)f & (int)langbind::FilterBase::SerializeWithIndices));
	}

private:
	typedef textwolf::XMLParser<std::string> XMLParser;
	XMLParser m_parser;			///< XML parser
	const char* m_src;			///< pointer to current chunk parsed
	std::size_t m_srcsize;			///< size of the current chunk parsed in bytes
	bool m_srcend;				///< true if end of message is in current chunk parsed
	enum MetadataState
	{
		MS_Init,			///< parsing document header
		MS_Root,			///< scanning header for document root
		MS_AttribName,			///< scanning header for meta data document attribute name or end of meta data
		MS_AttribValue,			///< scanning header for meta data document attribute value
		MS_Done				///< meta data parsed, now parsing content
	};
	MetadataState m_metadatastate;		///< state of document meta data parsing
	std::string m_elembuffer;		///< buffer for element
};

/// \class OutputFilter
/// \brief output filter filter for XML using textwolf
struct OutputFilterImpl :public OutputFilter
{
	typedef OutputFilter Parent;

	/// \brief Constructor
	OutputFilterImpl( const types::DocMetaDataR& inheritMetaData_)
		:utils::TypeSignature("langbind::OutputFilterImpl (textwolf)", __LINE__)
		,OutputFilter("textwolf", inheritMetaData_)
		,m_elemitr(0)
		,m_emptyDocument(false)
		,m_headerPrinted(false)
		,m_gotFinalClose(false){}

	/// \brief Copy constructor
	/// \param [in] o output filter to copy
	OutputFilterImpl( const OutputFilterImpl& o)
		:utils::TypeSignature("langbind::OutputFilterImpl (textwolf)", __LINE__)
		,OutputFilter(o)
		,m_elembuf(o.m_elembuf)
		,m_elemitr(o.m_elemitr)
		,m_emptyDocument(o.m_emptyDocument)
		,m_headerPrinted(o.m_headerPrinted)
		,m_gotFinalClose(o.m_gotFinalClose)
		,m_printer(o.m_printer){}

	virtual ~OutputFilterImpl(){}

	/// \brief self copy
	/// \return copy of this
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

	bool printHeader()
	{
		types::DocMetaData md( getMetaData());
		LOG_DEBUG << "[textwolf output] document meta data: {" << md.tostring() << "}";
		const char* standalone = md.getAttribute( "standalone");
		const char* root = md.getAttribute( "root");
		if (!root)
		{
			setState( Error, "no XML root element defined");
			return false;
		}
		const char* encoding = md.getAttribute( "encoding");
		if (!encoding) encoding = "UTF-8";

		if (standalone && 0==std::strcmp( standalone, "yes"))
		{
			if (!m_printer.createPrinter( encoding))
			{
				setState( Error, "failed to create XML serializer for this encoding");
				return false;
			}
			if (!m_printer.printDocumentStart( root, 0/*public*/, 0/*system*/, 0/*xmlns*/, 0/*xsi*/, 0/*schemaLocation*/, m_elembuf))
			{
				setState( Error, "failed to print XML document header");
				return false;
			}
		}
		else
		{
			const char* doctype_public = md.getAttribute( "PUBLIC");
			const char* doctype_system = md.getAttribute( "SYSTEM");
			std::string doctype_system_buf;
			if (doctype_system && !md.doctype().empty())
			{
				doctype_system_buf = types::DocMetaData::replaceStem( doctype_system, md.doctype());
				doctype_system = doctype_system_buf.c_str();
			}
			const char* xmlns = md.getAttribute( "xmlns");
			const char* xsi = md.getAttribute( "xmlns:xsi");
			const char* schemaLocation = md.getAttribute( "xmlns:schemaLocation");
			std::string schemaLocation_buf;
			if (schemaLocation && !md.doctype().empty())
			{
				schemaLocation_buf = types::DocMetaData::replaceStem( schemaLocation, md.doctype());
				schemaLocation = schemaLocation_buf.c_str();
			}
			if (!m_printer.createPrinter( encoding))
			{
				setState( Error, "failed to create XML serializer for this encoding");
				return false;
			}
			if (!m_printer.printDocumentStart( root, doctype_public, doctype_system, xmlns, xsi, schemaLocation, m_elembuf))
			{
				setState( Error, "failed to print XML document header");
				return false;
			}
		}
		return true;
	}

	/// \brief Implementation of OutputFilter::close()
	virtual bool close()
	{
		if (!m_gotFinalClose)
		{
			return print( FilterBase::CloseTag, 0, 0);
		}
		if (m_elemitr < m_elembuf.size())
		{
			// there is something to print left from last time
			if (!emptybuf())
			{
				setState( EndOfBuffer);
				return false;
			}
		}
		return true;
	}

	/// \brief Implementation of OutputFilter::print(OutputFilter::ElementType,const void*,std::size_t)
	/// \param [in] type type of the element to print
	/// \param [in] element pointer to the element to print
	/// \param [in] elementsize size of the element to print in bytes
	/// \return true, if success, false else
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
		if (!m_headerPrinted)
		{
			if (m_emptyDocument)
			{
				setState( Error, "textwolf: illegal print operation after final close (empty document)");
				return false;
			}
			if (type == FilterBase::CloseTag)
			{
				m_gotFinalClose = true;
				m_emptyDocument = true;
				return true;
			}
			else
			{
				if (!printHeader()) return false;
			}
			m_headerPrinted = true;
		}
		switch (type)
		{
			case FilterBase::OpenTag:
				if (!m_printer.printOpenTag( (const char*)element, elementsize, m_elembuf))
				{
					setState( Error, "textwolf: illegal print operation (open tag)");
					return false;
				}
				break;
			case FilterBase::CloseTag:
				if (!m_printer.printCloseTag( m_elembuf))
				{
					if (m_gotFinalClose)
					{
						setState( Error, "textwolf: illegal print operation (close tag)");
						return false;
					}
					m_gotFinalClose = true;
				}
				break;
			case FilterBase::Attribute:
				if (!m_printer.printAttribute( (const char*)element, elementsize, m_elembuf))
				{
					setState( Error, "textwolf: illegal print operation (attribute)");
					return false;
				}
				break;
			case FilterBase::Value:
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

	/// \brief Implementation of FilterBase::getValue( const char*, std::string&)
	virtual bool getValue( const char* id, std::string& val) const
	{
		return Parent::getValue( id, val);
	}

	/// \brief Implementation of FilterBase::setValue( const char*, const std::string&)
	virtual bool setValue( const char* id, const std::string& value)
	{
		return Parent::setValue( id, value);
	}

private:
	typedef textwolf::XMLPrinter<std::string> XMLPrinter;

	std::string m_elembuf;				///< buffer for the currently printed element
	std::size_t m_elemitr;				///< iterator to pass it to output
	bool m_emptyDocument;				///< true, if the printed document is empty
	bool m_headerPrinted;				///< true, if the XML header has already been printed
	bool m_gotFinalClose;				///< true, if we got the final close tag
	XMLPrinter m_printer;				///< xml printer object
};

}//end anonymous namespace

class TextwolfXmlFilter :public Filter
{
public:
	TextwolfXmlFilter( const char* encoding=0)
	{
		m_inputfilter.reset( new InputFilterImpl());
		OutputFilterImpl* oo = new OutputFilterImpl( m_inputfilter->getMetaDataRef());
		m_outputfilter.reset( oo);
		if (encoding)
		{
			m_outputfilter->setAttribute( "encoding", encoding);
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

class TextwolfXmlFilterType :public FilterType
{
public:
	TextwolfXmlFilterType()
		:FilterType("textwolf"){}
	virtual ~TextwolfXmlFilterType(){}

	virtual Filter* create( const std::vector<FilterArgument>& arg) const
	{
		return new TextwolfXmlFilter( getArgumentEncoding( arg));
	}
};

FilterType* _Wolframe::langbind::createTextwolfXmlFilterType()
{
	return new TextwolfXmlFilterType();
}

