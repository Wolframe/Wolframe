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
#include "textwolf/cstringiterator.hpp"
#include "textwolf/xmlhdrparser.hpp"
#include "textwolf/xmlprinter.hpp"
#include "types/countedReference.hpp"
#include "logger-v1.hpp"
#include <string>
#include <cstddef>
#include <algorithm>
#include <boost/algorithm/string.hpp>

using namespace _Wolframe;
using namespace langbind;

#undef _Wolframe_LOWLEVEL_DEBUG

#define TYPEDEF_XMLScanner(ENCODING)\
	typedef textwolf::XMLScanner<textwolf::SrcIterator,textwolf::charset::ENCODING,textwolf::charset::UTF8,std::string> XMLScanner;

#define DOWITH_XMLScanner(INSTR)\
	switch (m_encoding)\
	{\
		case UTF8:\
		{\
			TYPEDEF_XMLScanner(UTF8);\
			INSTR;\
			break;\
		}\
		case IsoLatin:\
		{\
			TYPEDEF_XMLScanner(IsoLatin);\
			INSTR;\
			break;\
		}\
		case UTF16LE:\
		{\
			TYPEDEF_XMLScanner(UTF16LE);\
			INSTR;\
			break;\
		}\
		case UTF16BE:\
		{\
			TYPEDEF_XMLScanner(UTF16BE);\
			INSTR;\
			break;\
		}\
		case UCS2LE:\
		{\
			TYPEDEF_XMLScanner(UCS2LE);\
			INSTR;\
			break;\
		}\
		case UCS2BE:\
		{\
			TYPEDEF_XMLScanner(UCS2BE);\
			INSTR;\
			break;\
		}\
		case UCS4LE:\
		{\
			TYPEDEF_XMLScanner(UCS4LE);\
			INSTR;\
			break;\
		}\
		case UCS4BE:\
		{\
			TYPEDEF_XMLScanner(UCS4BE);\
			INSTR;\
			break;\
		}\
	}

#define TYPEDEF_XMLPrinter(ENCODING)\
	typedef textwolf::XMLPrinter<textwolf::charset::ENCODING,textwolf::charset::UTF8,std::string> XMLPrinter;

#define DOWITH_XMLPrinter(INSTR)\
	switch (m_encoding)\
	{\
		case UTF8:\
		{\
			TYPEDEF_XMLPrinter(UTF8);\
			INSTR;\
			break;\
		}\
		case IsoLatin:\
		{\
			TYPEDEF_XMLPrinter(IsoLatin);\
			INSTR;\
			break;\
		}\
		case UTF16LE:\
		{\
			TYPEDEF_XMLPrinter(UTF16LE);\
			INSTR;\
			break;\
		}\
		case UTF16BE:\
		{\
			TYPEDEF_XMLPrinter(UTF16BE);\
			INSTR;\
			break;\
		}\
		case UCS2LE:\
		{\
			TYPEDEF_XMLPrinter(UCS2LE);\
			INSTR;\
			break;\
		}\
		case UCS2BE:\
		{\
			TYPEDEF_XMLPrinter(UCS2BE);\
			INSTR;\
			break;\
		}\
		case UCS4LE:\
		{\
			TYPEDEF_XMLPrinter(UCS4LE);\
			INSTR;\
			break;\
		}\
		case UCS4BE:\
		{\
			TYPEDEF_XMLPrinter(UCS4BE);\
			INSTR;\
			break;\
		}\
	}

enum Encoding
{
	UTF8,
	IsoLatin,
	UTF16LE,
	UTF16BE,
	UCS2LE,
	UCS2BE,
	UCS4LE,
	UCS4BE
};
enum {NofEncodings=UCS4BE};

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

static bool getEncoding( Encoding& encoding, int& codepage, const std::string& encstr)
{
	std::string enc;
	parseEncoding( enc, encstr);

	codepage = 0;
	encoding = UTF8;

	if ((enc.size() >= 8 && std::memcmp( enc.c_str(), "isolatin", 8)== 0)
	||  (enc.size() >= 7 && std::memcmp( enc.c_str(), "iso8859", 7) == 0))
	{
		const char* codepagestr = enc.c_str() + ((enc.c_str()[4] == 'l')?8:7);
		if (std::strlen( codepagestr) > 1 || codepagestr[0] < '0' || codepagestr[0] > '9')
		{
			return false;
		}
		codepage = codepagestr[0] - '0';
		return true;
	}
	else if (enc.size() == 0 || enc == "utf8")
	{
		encoding = UTF8;
		return true;
	}
	else if (enc == "utf16" || enc == "utf16be")
	{
		encoding = UTF16BE;
		return true;
	}
	else if (enc == "utf16le")
	{
		encoding = UTF16LE;
		return true;
	}
	else if (enc == "ucs2" || enc == "ucs2be")
	{
		encoding = UCS2BE;
		return true;
	}
	else if (enc == "ucs2le")
	{
		encoding = UCS2LE;
		return true;
	}
	else if (enc == "utf32" || enc == "ucs4" || enc == "utf32be" || enc == "ucs4be")
	{
		encoding = UCS4BE;
		return true;
	}
	else if (enc == "utf32le" || enc == "ucs4le")
	{
		encoding = UCS4LE;
		return true;
	}
	return false;
}


namespace {

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
		,m_encoding(UTF8)
		,m_codepage(0)
		,m_withEmpty(true)
		,m_doTokenize(false)
		,m_parser(0)
		,m_eom()
		,m_src(0)
		,m_srcsize(0)
		,m_srcend(false)
		,m_metadatastate(MS_Init)
		,m_doctype_parsed(false)
		,m_lasttype(textwolf::XMLScannerBase::None)
	{}
	/// \brief Copy constructor
	/// \param [in] o output filter to copy
	InputFilterImpl( const InputFilterImpl& o)
		:utils::TypeSignature("langbind::InputFilterImpl (textwolf)", __LINE__)
		,InputFilter(o)
		,m_encoding(o.m_encoding)
		,m_codepage(o.m_codepage)
		,m_withEmpty(o.m_withEmpty)
		,m_doTokenize(o.m_doTokenize)
		,m_parser(0)
		,m_eom()	//... ! by intention (non copyable, only used during getNext, where no copy is possible)
		,m_src(o.m_src)
		,m_srcsize(o.m_srcsize)
		,m_srcend(o.m_srcend)
		,m_metadatastate(o.m_metadatastate)
		,m_doctype_parsed(o.m_doctype_parsed)
		,m_elembuffer(o.m_elembuffer)
		,m_lasttype(o.m_lasttype)
	{
		if (o.m_parser)
		{
			DOWITH_XMLScanner(
				m_parser = new XMLScanner( *(XMLScanner*)o.m_parser);
			);
		};
	}

	virtual ~InputFilterImpl()
	{
		if (m_parser)
		{
			DOWITH_XMLScanner( 
				delete (XMLScanner*)m_parser;
			);
		}
	}

	/// \brief Implementation of FilterBase::getValue( const char*, std::string&)
	virtual bool getValue( const char* id, std::string& val) const
	{
		if (0==std::strcmp( id, "empty"))
		{
			val = m_withEmpty?"true":"false";
			return true;
		}
		if (0==std::strcmp( id, "tokenize"))
		{
			val = m_doTokenize?"true":"false";
			return true;
		}
		return Parent::getValue( id, val);
	}


	/// \brief Implementation of FilterBase::setValue( const char*, const std::string&)
	virtual bool setValue( const char* id, const std::string& value)
	{
		if (0==std::strcmp( id, "empty"))
		{
			if (0==std::strcmp( value.c_str(), "true"))
			{
				m_withEmpty = true;
			}
			else if (0==std::strcmp( value.c_str(), "false"))
			{
				m_withEmpty = false;
			}
			else
			{
				return false;
			}
			return true;
		}
		if (0==std::strcmp( id, "tokenize"))
		{
			if (0==std::strcmp( value.c_str(), "true"))
			{
				m_doTokenize = true;
			}
			else if (0==std::strcmp( value.c_str(), "false"))
			{
				m_doTokenize = false;
			}
			else
			{
				return false;
			}
			if (m_parser)
			{
				DOWITH_XMLScanner( ((XMLScanner*)m_parser)->doTokenize( m_doTokenize));
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
		InputFilterImpl* rt = new InputFilterImpl();
		rt->m_withEmpty = m_withEmpty;
		rt->m_doTokenize = m_doTokenize;
		return rt;
	}

	/// \brief implement interface member InputFilter::putInput(const void*,std::size_t,bool)
	virtual void putInput( const void* ptr, std::size_t size, bool end)
	{
		if (!m_parser)
		{
			m_hdrparser.putInput( (const char*) ptr, size);
			if (m_hdrparser.parse())
			{
				if (m_hdrparser.encoding())
				{
					if (!getEncoding( m_encoding, m_codepage, m_hdrparser.encoding()))
					{
						std::string msg = std::string("textwolf: unknown encoding '") + m_hdrparser.encoding() + "'";
						setState( Error, msg.c_str());
					}
					getMetaDataRef()->setAttribute( "encoding", m_hdrparser.encoding());
				}
				else
				{
					m_encoding = UTF8;
				}
				m_src = (const char*)m_hdrparser.consumedData().c_str();
				m_srcsize = m_hdrparser.consumedData().size();
				m_srcend = end;
				DOWITH_XMLScanner( 
					m_parser = new XMLScanner();
					((XMLScanner*)m_parser)->setSource( textwolf::SrcIterator( m_src, m_srcsize, end?0:&m_eom));
					((XMLScanner*)m_parser)->doTokenize( m_doTokenize);
				);
				setState( Open);
			}
			else if (m_hdrparser.lasterror())
			{
				setState( Error, m_hdrparser.lasterror());
			}
			else if (m_hdrparser.charsConsumed() > 64)
			{
				setState( Error, "xml header not not terminated");
			}
		}
		else
		{
			m_src = (const char*)ptr;
			m_srcsize = size;
			m_srcend = end;
			DOWITH_XMLScanner( 
				((XMLScanner*)m_parser)->setSource( textwolf::SrcIterator( m_src, m_srcsize, end?0:&m_eom))
			);
			setState( Open);
		}
	}

	/// \brief Implement InputFilter::getRest(const void*&,std::size_t&,bool&)
	virtual void getRest( const void*& ptr, std::size_t& size, bool& end)
	{
		std::size_t pp = 0;
		if (m_parser)
		{
			DOWITH_XMLScanner( 
				pp = ((XMLScanner*)m_parser)->getPosition();
			);
		}
		ptr = m_src + pp;
		size = (m_srcsize > pp)?(m_srcsize-pp):0;
		end = m_srcend;
	}

	static int getElementType( textwolf::XMLScannerBase::ElementType et)
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
		return tmap[ et];
	}

	bool mapElementType( InputFilter::ElementType& type, textwolf::XMLScannerBase::ElementType et, const char* ee)
	{
		int st = getElementType( et);
		if (st < 0)
		{
			if (st == -1) return false;
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
		type = (InputFilterImpl::ElementType)st;
		return true;
	}

	textwolf::XMLScannerBase::ElementType getLastItem( const char*& element, std::size_t& elementsize) const
	{
		textwolf::XMLScannerBase::ElementType et = m_lasttype;
		DOWITH_XMLScanner(
			element = ((XMLScanner*)m_parser)->getItem();
			elementsize = ((XMLScanner*)m_parser)->getItemSize();
		);
		return et;
	}

	textwolf::XMLScannerBase::ElementType getNextItem( const char*& element, std::size_t& elementsize)
	{
		textwolf::XMLScannerBase::ElementType et;
		DOWITH_XMLScanner(
			et = ((XMLScanner*)m_parser)->nextItem();
			element = ((XMLScanner*)m_parser)->getItem();
			elementsize = ((XMLScanner*)m_parser)->getItemSize();
		);
		m_lasttype = et;
		return et;
	}

	void setParserError()
	{
		const char* ee;
		DOWITH_XMLScanner(
			((XMLScanner*)m_parser)->getError( &ee);
		);
		setState( Error, ee);
	}

	/// \brief implement interface member InputFilter::getNext(typename InputFilter::ElementType&,const void*&,std::size_t&)
	virtual bool getNext( InputFilter::ElementType& type, const void*& element, std::size_t& elementsize)
	{
		if (m_metadatastate != MS_Done)
		{
			if (state() == Error) return false;
			if (!getMetaData()) return false;

			while (m_metadatastate == MS_DoneElemCached)
			{
				const char* ee;
				textwolf::XMLScannerBase::ElementType et = getLastItem( ee, elementsize);
				element = (const void*)ee;

				if (!mapElementType( type, et, ee))
				{
					m_metadatastate = MS_Done;
					if (state() == Error)
					{
						return false;
					}
					//... fall through into following try block
				}
				else
				{
					m_metadatastate = MS_Done;
					if (et == textwolf::XMLScannerBase::Content && !m_withEmpty)
					{
						std::size_t ii=0;
						for (;ii<elementsize && (unsigned char)ee[ii] <= 32; ++ii);
						if (ii<elementsize)
						{
							return true;
						}
						else
						{
							//... fall through into following try block
						}
					}
					else
					{
						return true;
					}
				}
			}
		}
		if (setjmp(m_eom) != 0)
		{
			setState( EndOfMessage);
			return 0;
		}
		for (;;)
		{
			const char* ee;
			textwolf::XMLScannerBase::ElementType et = getNextItem( ee, elementsize);
			element = (const void*)ee;
			if (!mapElementType( type, et, ee))
			{
				if (state() == Error)
				{
					return false;
				}
				continue;
			}
			if (et == textwolf::XMLScannerBase::Content && !m_withEmpty)
			{
				std::size_t ii=0;
				for (;ii<elementsize && (unsigned char)ee[ii] <= 32; ++ii);
				if (ii==elementsize) continue;
			}
#ifdef _Wolframe_LOWLEVEL_DEBUG
			LOG_DATA2 << "[textwolf filter] " << metadataStateName() << " fetch element " << textwolf::XMLScannerBase::getElementTypeName(et) << " '" << std::string(ee,elementsize) << "'";
#endif
			return true;
		}
		return false;
	}

	bool setDocAttributeValue( const char* ee, std::size_t eesize)
	{
		types::DocMetaData* md = getMetaDataRef().get();
		switch (m_metadatastate)
		{
			case MS_Init:
			case MS_Root:
			case MS_AttribName:
			case MS_AttribValue:
			case MS_DocType:
			case MS_Done:
			case MS_DoneElemCached:
				setState( Error, "unecpected DOCTYPE attribute");
				return false;
			case MS_DocEntitySkip:
				break;
			case MS_DocTypeDone:
				m_metadatastate = MS_DocEntitySkip;
				break;
			case MS_DocEntityDef:
				if (eesize == 7 && 0==std::strcmp( ee, "DOCTYPE"))
				{
					if (m_doctype_parsed)
					{
						setState( Error, "duplicate definition of !DOCTYPE in XML document");
						return false;
					}
					m_doctype_parsed = true;
					m_metadatastate = MS_DocType_ROOT;
				}
				else
				{
					m_metadatastate = MS_DocEntitySkip;
				}
				break;
			case MS_DocType_ROOT:
				md->setAttribute( "root", std::string(ee,eesize));
				md->setAttribute( "ROOT", std::string(ee,eesize));
				md->setDoctype( types::DocMetaData::extractStem( std::string( ee, eesize)));
				m_metadatastate = MS_DocType_DefType;
				break;
			case MS_DocType_DefType:
				if (eesize == 6 && 0==std::strcmp( ee, "PUBLIC"))
				{
					m_metadatastate = MS_DocType_PUBLIC;
				}
				else if (eesize == 6 && 0==std::strcmp( ee, "SYSTEM"))
				{
					m_metadatastate = MS_DocType_SYSTEM;
				}
				else
				{
					setState( Error, "ecpected SYSTEM or PUBLIC in DOCTYPE declaration after '<!DOCTYPE root'");
					return false;
				}
				break;
			case MS_DocType_PUBLIC:
				md->setAttribute( "PUBLIC", std::string(ee,eesize));
				m_metadatastate = MS_DocType_SYSTEM;
				break;
			case MS_DocType_SYSTEM:
				md->setAttribute( "SYSTEM", std::string(ee,eesize));
				md->setDoctype( types::DocMetaData::extractStem( std::string( ee, eesize)));
				m_metadatastate = MS_DocTypeDone;
				break;
		}
		return true;
	}

	bool setRootAttributeValue( const std::string& name_, const char* ee, std::size_t eesize)
	{
		types::DocMetaData* md = getMetaDataRef().get();
		const char* elemstart = std::strchr( name_.c_str(), ':');
		if (!elemstart) elemstart = name_.c_str();
		if (m_elembuffer == "xmlns")
		{
			md->setAttribute( name_, std::string( ee, eesize));
		}
		else if (0==std::memcmp( name_.c_str(), "xmlns:", 6/*strlen("xmlns:")*/))
		{
			md->setAttribute( name_, std::string( ee, eesize));
		}
		else if (0==std::memcmp( name_.c_str(), "xsi:", 4/*strlen("xsi:")*/))
		{
			md->setAttribute( name_, std::string( ee, eesize));
		}
		else
		{
			std::string msg = std::string("unknown XML root element attribute '") + m_elembuffer + "'";
			setState( Error, msg.c_str());
			return false;
		}
		if (0==std::strcmp( elemstart, "schemaLocation") || 0==std::strcmp( elemstart, "noNamespaceSchemaLocation"))
		{
			md->setDoctype( types::DocMetaData::extractStem( std::string( ee, eesize)));
		}
		return true;
	}

	/// \brief Implements InputFilter::getMetaData()
	virtual const types::DocMetaData* getMetaData()
	{
		if (m_metadatastate == MS_Done || m_metadatastate == MS_DoneElemCached)
		{
			return getMetaDataRef().get();
		}
		if (!m_parser)
		{
			if (state() != Error)
			{
				setState( EndOfMessage);
			}
			return 0;
		}
		if (setjmp(m_eom) != 0)
		{
			setState( EndOfMessage);
			return 0;
		}
		while (m_metadatastate != MS_Done && m_metadatastate != MS_DoneElemCached)
		{
			const char* ee;
			std::size_t eesize;
			textwolf::XMLScannerBase::ElementType et = getNextItem( ee, eesize);
#ifdef _Wolframe_LOWLEVEL_DEBUG
			LOG_DATA2 << "[textwolf filter] " << metadataStateName() << " fetch element " << textwolf::XMLScannerBase::getElementTypeName(et) << " '" << std::string(ee,eesize) << "'";
#endif
			switch (et)
			{
				case textwolf::XMLScannerBase::None:
					setState( Error, "unexpected end of document");
					return 0;
				case textwolf::XMLScannerBase::ErrorOccurred:
					setParserError();
					return 0;
				case textwolf::XMLScannerBase::HeaderStart:
				case textwolf::XMLScannerBase::HeaderAttribName:
					break;
				case textwolf::XMLScannerBase::HeaderAttribValue:
				case textwolf::XMLScannerBase::HeaderEnd:
					m_metadatastate = MS_Root;
					break;
				case textwolf::XMLScannerBase::DocAttribValue:
					if (m_metadatastate == MS_Root)
					{
						m_metadatastate = MS_DocEntityDef;
					}
					setDocAttributeValue( ee, eesize);
					break;
				case textwolf::XMLScannerBase::DocAttribEnd:
					m_metadatastate = MS_Root;
					break;
				case textwolf::XMLScannerBase::TagAttribName:
					if (m_metadatastate == MS_AttribName)
					{
						m_metadatastate = MS_AttribValue;
						m_elembuffer = std::string( ee, eesize);
					}
					else
					{
						setState( Error, "unexpected tag attribute in meta data parsing");
						return 0;
					}
					break;
				case textwolf::XMLScannerBase::TagAttribValue:
					if (m_metadatastate == MS_AttribValue)
					{
						if (!setRootAttributeValue( m_elembuffer, ee, eesize))
						{
							return 0;
						}
						m_metadatastate = MS_AttribName;
					}
					else
					{
						setState( Error, "unexpected tag attribute value in meta data parsing");
						return 0;
					}
					break;
				case textwolf::XMLScannerBase::OpenTag:
					if (m_metadatastate == MS_Root || m_metadatastate == MS_Init)
					{
						getMetaDataRef()->setAttribute( "root", std::string( ee, eesize));
						m_metadatastate = MS_AttribName;
					}
					else
					{
						m_metadatastate = MS_DoneElemCached;
						break;
					}
					break;
				case textwolf::XMLScannerBase::CloseTagIm:
				case textwolf::XMLScannerBase::CloseTag:
				case textwolf::XMLScannerBase::Content:
					m_metadatastate = MS_DoneElemCached;
					break;

				case textwolf::XMLScannerBase::Exit:
					setState( Error, "unexpected end of document in meta data section");
					return 0;
			}
		}
		return getMetaDataRef().get();
	}

	/// \brief Implement InputFilter::checkMetaData(const types::DocMetaData&) const
	virtual bool checkMetaData( const types::DocMetaData& md)
	{
		if (m_metadatastate != MS_Done && m_metadatastate != MS_DoneElemCached)
		{
			setState( Error, "input filter did not parse its meta data yet - cannot check them therefore");
			return false;
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
		const char* doctype_root = md.getAttribute( "ROOT");
		if (doctype_root)
		{
			if (!doc_rootelem)
			{
				setState( Error, "input document has no root element defined");
				return false;
			}
			if (0!=std::strcmp( doctype_root, doc_rootelem))
			{
				setState( Error, "declared !DOCTYPE root element does not match to XML root element");
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
	textwolf::XmlHdrParser m_hdrparser;	///< XML header parser
	Encoding m_encoding;			///< encoding of content parsed
	int m_codepage;				///< code page of encoding
	bool m_withEmpty;			///< true, if empty tokens are returned too (default)
	bool m_doTokenize;			///< true, if content chunks are tokenized by spaces
	textwolf::XMLScannerBase* m_parser;	///< variant of XML scanner, one of them selected by m_encoding (type textwolf::XMLScanner<..>)
	jmp_buf m_eom;				///< end of message fallback jump
	const char* m_src;			///< pointer to current chunk parsed
	std::size_t m_srcsize;			///< size of the current chunk parsed in bytes
	bool m_srcend;				///< true if end of message is in current chunk parsed
	enum MetadataState
	{
		MS_Init,			///< parsing document header
		MS_DocEntityDef,		///< starting parsing an XML entity definition
		MS_DocEntitySkip,		///< parsing an entity definition to be ignored
		MS_DocType,			///< parsing a !DOCTYPE entity definition
		MS_DocType_ROOT,		///< parsing the root element in a !DOCTYPE entity definition
		MS_DocType_DefType,		///< parsing the identifier SYSTEM or PUBLIC of a !DOCTYPE entity definition
		MS_DocType_SYSTEM,		///< parsing the identifier SYSTEM attribute value of a !DOCTYPE entity definition
		MS_DocType_PUBLIC,		///< parsing the identifier PUBLIC attribute value of a !DOCTYPE entity definition
		MS_DocTypeDone,			///< finished the parsing of a !DOCTYPE entity definition
		MS_Root,			///< scanning header for document root
		MS_AttribName,			///< scanning header for meta data document attribute name or end of meta data
		MS_AttribValue,			///< scanning header for meta data document attribute value
		MS_Done,			///< meta data parsed, now parsing content
		MS_DoneElemCached		///< done, but last element is buffered
	};
	static const char* metadataStateName( MetadataState i)
	{
		static const char* ar[MS_DoneElemCached+1] =
		{
			"Init", "DocEntityDef", "DocEntitySkip", "DocType",
			"DocType_ROOT", "DocType_DefType", "DocType_SYSTEM", "DocType_PUBLIC",
			"DocTypeDone", "Root", "AttribName", "AttribValue", "Done", "DoneElemCached"
		};
		return ar[i];
	}
	const char* metadataStateName() const
	{
		return metadataStateName(m_metadatastate);
	}

	MetadataState m_metadatastate;		///< state of document meta data parsing
	bool m_doctype_parsed;			///< true, if the !DOCTYPE definition has already been parsed
	std::string m_elembuffer;		///< buffer for element
	textwolf::XMLScannerBase::ElementType m_lasttype; ///< type of last element fetched (MS_DoneElemCached)
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
		,m_encoding(UTF8)
		,m_codepage(0)
		,m_printer(0)
		,m_elemitr(0)
		,m_emptyDocument(false)
		,m_headerPrinted(false)
		,m_gotFinalClose(false){}

	/// \brief Copy constructor
	/// \param [in] o output filter to copy
	OutputFilterImpl( const OutputFilterImpl& o)
		:utils::TypeSignature("langbind::OutputFilterImpl (textwolf)", __LINE__)
		,OutputFilter(o)
		,m_encoding(o.m_encoding)
		,m_codepage(o.m_codepage)
		,m_printer(0)
		,m_elembuf(o.m_elembuf)
		,m_elemitr(o.m_elemitr)
		,m_emptyDocument(o.m_emptyDocument)
		,m_headerPrinted(o.m_headerPrinted)
		,m_gotFinalClose(o.m_gotFinalClose)
	{
		if (o.m_printer)
		{
			DOWITH_XMLPrinter(
				m_printer = new XMLPrinter( *(XMLPrinter*)o.m_printer);
			);
		}
	}

	virtual ~OutputFilterImpl()
	{
		if (m_printer)
		{
			DOWITH_XMLPrinter(
				delete (XMLPrinter*) m_printer;
			);
		}
	}

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

	static bool metadataContainSchemaReference( const types::DocMetaData& md)
	{
		std::vector<types::DocMetaData::Attribute>::const_iterator ai = md.attributes().begin(), ae = md.attributes().end();
		for (; ai != ae; ++ai)
		{
			if (0==std::memcmp( ai->name.c_str(), "xmlns", 5)
			||  0==std::memcmp( ai->name.c_str(), "xsi:", 4))
			{
				return true;
			}
		}
		return false;
	}

	bool printHeader()
	{
		types::DocMetaData md( getMetaData());
		LOG_DEBUG << "[textwolf output] document meta data: {" << md.tostring() << "}";

		const char* standalone = md.getAttribute( "standalone");

		const char* doctype_root = md.getAttribute( "ROOT");
		const char* doctype_public = md.getAttribute( "PUBLIC");
		const char* doctype_system = md.getAttribute( "SYSTEM");
		const char* root = md.getAttribute( "root");
		if (doctype_root && root && 0!=std::strcmp(root,doctype_root))
		{
			doctype_root = root;
		}
		if (!standalone)
		{
			if (doctype_system || doctype_root)
			{
				standalone = "no";
			}
			else if (!metadataContainSchemaReference( md))
			{
				standalone = "yes";
			}
		}

		if (!root)
		{
			setState( Error, "no XML root element defined");
			return false;
		}
		const char* encoding = md.getAttribute( "encoding");
		if (!encoding)
		{
			m_encoding = UTF8;
			encoding = "UTF-8";
		}
		else
		{
			if (!getEncoding( m_encoding, m_codepage, encoding))
			{
				std::string msg = std::string("cannot print XML with encoding '") + encoding + "'";
				setState( Error, msg.c_str());
				return false;
			}
		}
		if (m_printer)
		{
			DOWITH_XMLPrinter(
				delete (XMLPrinter*) m_printer;
			);
			m_printer = 0;
		}
		DOWITH_XMLPrinter(
			m_printer = new XMLPrinter();
			((XMLPrinter*)m_printer)->printHeader( encoding, standalone, m_elembuf)
		)
		if (standalone && 0==std::strcmp( standalone, "yes"))
		{
			bool res;
			DOWITH_XMLPrinter(
				res = ((XMLPrinter*)m_printer)->printOpenTag( root, std::strlen(root), m_elembuf);
			);
			if (!res)
			{
				setState( Error, "failed to output XML root element");
			}
		}
		else if (standalone && 0==std::strcmp( standalone, "no"))
		{
			bool res;
			std::string doctype_system_buf;
			if (doctype_system && !md.doctype().empty())
			{
				doctype_system_buf = types::DocMetaData::replaceStem( doctype_system, md.doctype());
				doctype_system = doctype_system_buf.c_str();
			}
			if (doctype_system || doctype_root)
			{
				DOWITH_XMLPrinter(
					res = ((XMLPrinter*)m_printer)->printDoctype( doctype_root?doctype_root:root, doctype_public, doctype_system, m_elembuf);
				);
				if (!res)
				{
					setState( Error, "failed to output XML DOCTYPE");
				}
			}
			DOWITH_XMLPrinter(
				res = ((XMLPrinter*)m_printer)->printOpenTag( root, std::strlen(root), m_elembuf);
			);
			if (!res)
			{
				setState( Error, "failed to output XML root element");
			}
		}
		else
		{
			bool res;
			DOWITH_XMLPrinter(
				res = ((XMLPrinter*)m_printer)->printOpenTag( root, std::strlen(root), m_elembuf);
			);
			if (!res)
			{
				setState( Error, "failed to output XML root element");
			}
			std::vector<types::DocMetaData::Attribute>::const_iterator ai = md.attributes().begin(), ae = md.attributes().end();
			for (; ai != ae; ++ai)
			{
				bool doPrint = false;
				std::string value;
				const char* valueptr = ai->value.c_str();

				if (0==std::memcmp( ai->name.c_str(), "xmlns", 5)
				||  0==std::memcmp( ai->name.c_str(), "xsi:", 4))
				{
					doPrint = true;
				}
				const char* cc = std::strchr( ai->name.c_str(), ':');
				if (cc != 0)
				{
					if (0==std::strcmp(cc+1,"schemaLocation")
					||  0==std::strcmp(cc+1,"noNamespaceSchemaLocation"))
					{
						if (!md.doctype().empty())
						{
							value = types::DocMetaData::replaceStem( ai->value, md.doctype());
							valueptr = value.c_str();
						}
					}
				}
				if (doPrint)
				{
					DOWITH_XMLPrinter(
						res = ((XMLPrinter*)m_printer)->printAttribute( ai->name.c_str(), ai->name.size(), m_elembuf)
							&& ((XMLPrinter*)m_printer)->printValue( valueptr, std::strlen(valueptr), m_elembuf);
					);
					if (!res)
					{
						setState( Error, "failed to output XML root attribute value");
					}
				}
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
		bool res;
#ifdef _Wolframe_LOWLEVEL_DEBUG
		LOG_DATA2 << "[textwolf filter] print element " << FilterBase::elementTypeName(type) << " '" << std::string((const char*)element, elementsize) << "'";
#endif
		switch (type)
		{
			case FilterBase::OpenTag:
				DOWITH_XMLPrinter(
					res = ((XMLPrinter*)m_printer)->printOpenTag( (const char*)element, elementsize, m_elembuf);
				);
				if (!res)
				{
					setState( Error, "textwolf: illegal print operation (open tag)");
					return false;
				}
				break;
			case FilterBase::CloseTag:
				DOWITH_XMLPrinter(
					res = ((XMLPrinter*)m_printer)->printCloseTag( m_elembuf);
				);
				if (!res)
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
				DOWITH_XMLPrinter(
					res = ((XMLPrinter*)m_printer)->printAttribute( (const char*)element, elementsize, m_elembuf);
				);
				if (!res)
				{
					setState( Error, "textwolf: illegal print operation (attribute)");
					return false;
				}
				break;
			case FilterBase::Value:
				DOWITH_XMLPrinter(
					res = ((XMLPrinter*)m_printer)->printValue( (const char*)element, elementsize, m_elembuf);
				);
				if (!res)
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
	Encoding m_encoding;			///< encoding of content parsed
	int m_codepage;				///< code page of encoding
	void* m_printer;			///< variant of XML printer, one of them selected by m_encoding (type textwolf::XMLScanner<..>)
	std::string m_elembuf;			///< buffer for the currently printed element
	std::size_t m_elemitr;			///< iterator to pass it to output
	bool m_emptyDocument;			///< true, if the printed document is empty
	bool m_headerPrinted;			///< true, if the XML header has already been printed
	bool m_gotFinalClose;			///< true, if we got the final close tag
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

