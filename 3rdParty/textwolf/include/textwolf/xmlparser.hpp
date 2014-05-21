/*
---------------------------------------------------------------------
    The template library textwolf implements an input iterator on
    a set of XML path expressions without backward references on an
    STL conforming input iterator as source. It does no buffering
    or read ahead and is dedicated for stream processing of XML
    for a small set of XML queries.
    Stream processing in this Object refers to processing the
    document without buffering anything but the current result token
    processed with its tag hierarchy information.

    Copyright (C) 2010,2011,2012 Patrick Frey

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 3.0 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

--------------------------------------------------------------------

	The latest version of textwolf can be found at 'http://github.com/patrickfrey/textwolf'
	For documentation see 'http://patrickfrey.github.com/textwolf'

--------------------------------------------------------------------
*/
///\file textwolf/xmlparser.hpp
///\brief textwolf XML parser interface hiding character encoding properties and using a chunk wise feedable iterator on characters

#ifndef __TEXTWOLF_XML_PARSER_HPP__
#define __TEXTWOLF_XML_PARSER_HPP__
#include "textwolf/charset.hpp"
#include "textwolf/xmlscanner.hpp"
#include "textwolf/sourceiterator.hpp"
#include "textwolf/xmlhdriterator.hpp"
#include <cstring>
#include <cstdlib>

///\namespace textwolf
///\brief Toplevel namespace of the library
namespace textwolf {

struct XMLParserBase
{
	enum Flag
	{
		DoTokenize
	};

	typedef XMLScannerBase::ElementType (*GetNextProc)( void* obj, const char*& elemptr, std::size_t& elemsize);
	typedef void (*DeleteObj)( void* obj);
	typedef void (*ClearObj)( void* obj);
	typedef void* (*CopyObj)( void* obj);
	typedef void (*SetFlag)( void* obj, Flag f, bool value);
	typedef void (*PutInput)( void* obj, const char* ptr, std::size_t size, bool end);
	typedef std::size_t (*GetPosition)( const void* obj);

	struct MethodTable
	{
		GetNextProc m_getNext;
		DeleteObj m_del;
		CopyObj m_copy;
		SetFlag m_setFlag;
		PutInput m_putInput;
		GetPosition m_getPosition;

		MethodTable()				:m_getNext(0),m_del(0),m_copy(0),m_setFlag(0),m_putInput(0),m_getPosition(0){}
		MethodTable( const MethodTable& o)	:m_getNext(o.m_getNext),m_del(o.m_del),m_copy(o.m_copy),m_setFlag(o.m_setFlag),m_putInput(o.m_putInput),m_getPosition(o.m_getPosition){}
	};

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

template <class ObjSrcIterator, class BufferType, class IOCharset, class AppCharset>
struct XMLParserObject
{
	typedef XMLScanner<ObjSrcIterator,IOCharset,AppCharset,BufferType> This;

	static void* copyObj( void* obj_)
	{
		This* obj = (This*)obj_;
		This* rt = new This( *obj);
		return rt;
	}

	static void deleteObj( void* obj)
	{
		delete (This*)obj;
	}

	static void setFlag( void* obj_, XMLParserBase::Flag flag, bool value)
	{
		This* obj = (This*)obj_;
		switch (flag)
		{
			case XMLParserBase::DoTokenize: obj->doTokenize( value); break;
		}
	}

	static XMLScannerBase::ElementType getNextProc( void* obj_, const char*& elemptr, std::size_t& elemsize)
	{
		This* obj = (This*)obj_;
		XMLScannerBase::ElementType rt = obj->nextItem();
		if (rt == XMLScannerBase::ErrorOccurred)
		{
			obj->getError( &elemptr);
			if (elemptr)
			{
				elemsize = std::strlen( elemptr);
				return rt;
			}
		}
		elemptr = obj->getItem();
		elemsize = obj->getItemSize();
		return rt;
	}

	static void putInput( void* obj_, const char* ptr, std::size_t size, bool end)
	{
		This* obj = (This*)obj_;
		obj->setSource( SrcIterator( ptr, size, end));
	}

	static std::size_t getPosition( const void* obj_)
	{
		const This* obj = (const This*)obj_;
		return obj->getPosition();
	}

	static void* create( XMLParserBase::MethodTable& mt, IOCharset chs=IOCharset())
	{
		mt.m_getNext = getNextProc;
		mt.m_del = deleteObj;
		mt.m_copy = copyObj;
		mt.m_setFlag = setFlag;
		mt.m_putInput = putInput;
		mt.m_getPosition = getPosition;
		return new This( chs);
	}
};

///\brief Class for XML parsing independent of the character set
///\tparam BufferType type to use as buffer (STL back insertion interface)
///\tparam XMLAttributes setter/getter class for document attributes
template <class BufferType=std::string>
class XMLParser :public XMLParserBase
{
public:
	XMLParser()
		:m_state(ParseHeader)
		,m_src(0)
		,m_srcsize(0)
		,m_srceof(false)
		,m_srcofs(0)
		,m_obj(0)
		,m_headerAttrType(None)
		,m_withEmpty(true)
		,m_doTokenize(false)
		,m_standalone(true)
		,m_doctype_state(0)
		,m_lastelem(0)
		,m_lastelemsize(0)
		,m_lastelemtype(XMLScannerBase::ErrorOccurred)
	{
		m_obj = XMLParserObject<XmlHdrSrcIterator,BufferType,charset::UTF8,charset::UTF8>::create( m_mt, charset::UTF8());
	}

	XMLParser( const XMLParser& o)
		:m_state(o.m_state)
		,m_src(o.m_src)
		,m_srcsize(o.m_srcsize)
		,m_srceof(o.m_srceof)
		,m_srcofs(o.m_srcofs)
		,m_mt(o.m_mt)
		,m_obj(0)
		,m_headerAttrType(o.m_headerAttrType)
		,m_withEmpty(o.m_withEmpty)
		,m_doTokenize(o.m_doTokenize)
		,m_standalone(o.m_standalone)
		,m_doctype_state(o.m_doctype_state)
		,m_doctype_root(o.m_doctype_root)
		,m_doctype_public(o.m_doctype_public)
		,m_doctype_system(o.m_doctype_system)
		,m_lastelem(o.m_lastelem)
		,m_lastelemsize(o.m_lastelemsize)
		,m_lastelemtype(o.m_lastelemtype)
	{
		if (o.m_obj)
		{
			m_obj = m_mt.m_copy( o.m_obj);
		}
	}

	~XMLParser()
	{
		if (m_obj) m_mt.m_del( m_obj);
	}

	bool withEmpty() const
	{
		return m_withEmpty;
	}

	bool withEmpty( bool val)
	{
		bool rt = m_withEmpty;
		m_withEmpty = val;
		return rt;
	}

	const std::string& getDoctypeRoot() const
	{
		return m_doctype_root;
	}

	const std::string& getDoctypePublic() const
	{
		return m_doctype_public;
	}

	const std::string& getDoctypeSystem() const
	{
		return m_doctype_system;
	}

	const char* getEncoding() const
	{
		if (m_encoding.empty())
		{
			if (m_state != ParseSource)
			{
				throw std::runtime_error("character set encoding not defined yet in input");
			}
			return 0;
		}
		return m_encoding.c_str();
	}

	enum State
	{
		ParseHeader,			//< parsing the XML header section
		ParseDoctype,			//< parsing DOCTYPE definition if available
		ParseSource,			//< parsing the XML content section
		ParseSourceReady		//< parsing the XML content section, but the follow element has already been parsed (one look forward)
	};
	static const char* stateName( State i)
	{
		static const char* ar[] = {"ParseHeader", "ParseDoctype", "ParseSource", "ParseSourceReady"};
		return ar[ (int)i];
	}

	State state() const
	{
		return m_state;
	}

	bool doTokenize() const
	{
		return m_doTokenize;
	}

	bool doTokenize( bool val)
	{
		bool rt = m_doTokenize;
		if (m_obj) m_mt.m_setFlag( m_obj, DoTokenize, m_doTokenize = val);
		return rt;
	}

	bool isStandalone() const
	{
		return m_standalone;
	}

	void putInput( const char* ptr, std::size_t size, bool end)
	{
		m_src = ptr;
		m_srcsize = size;
		m_srceof = end;
		if (m_obj) m_mt.m_putInput( m_obj, ptr, size, end);
	}

	std::size_t getPosition() const
	{
		return m_obj?(m_mt.m_getPosition( m_obj)+m_srcofs):0;
	}

	bool parseHeader( const char*& err)
	{
		err = 0;
		if (!m_obj) return XMLScannerBase::ErrorOccurred;
		if (m_state == ParseSource || m_state == ParseSourceReady) return true;
		for(;;)
		{
			const char* elemptr;
			std::size_t elemsize;
			XMLScannerBase::ElementType elemtype = m_mt.m_getNext( m_obj, elemptr, elemsize);
			if ((int)elemtype > (int)XMLScannerBase::DocAttribEnd)
			{
				err = "xml header not complete";
				return false;
			}
			switch (m_state)
			{
				case ParseHeader:
					if (elemtype == XMLScannerBase::HeaderStart)
					{
						continue;
					}
					else if (elemtype == XMLScannerBase::HeaderAttribName)
					{
						if (elemsize == 8 && std::memcmp( elemptr, "encoding", 8) == 0)
						{
							m_headerAttrType = Encoding;
						}
						else if (elemsize == 7 && std::memcmp( elemptr, "version", 7) == 0)
						{
							m_headerAttrType = Version;
						}
						else if (elemsize == 10 && std::memcmp( elemptr, "standalone", 8) == 0)
						{
							m_headerAttrType = Standalone;
						}
						else
						{
							m_headerAttrType = None;
						}
						continue;
					}
					else if (elemtype == XMLScannerBase::HeaderAttribValue)
					{
						switch (m_headerAttrType)
						{
							case None:
								err = "unknown attribute name in xml header";
								return false;

							case Standalone:
								if (elemsize == 3 && std::memcmp( elemptr, "yes", 3) == 0)
								{
									m_standalone = true;
									continue;
								}
								else
								{
									m_standalone = false;
									continue;
								}
							case Version:
								continue;
							case Encoding:
								m_encoding = std::string( elemptr, elemsize);
								continue;
						}
						continue;
					}
					else if (elemtype == XMLScannerBase::HeaderEnd)
					{
						if (!createParser())
						{
							err = "unknown charset encoding";
							return false;
						}
						else
						{
							if (m_standalone)
							{
								m_state = ParseSource;
								return true;
							}
							m_state = ParseDoctype;
							continue;
						}
					}
					else
					{
						err = "unexpected element in xml header";
						return false;
					}

				case ParseDoctype:
					if (elemtype == XMLScannerBase::DocAttribValue)
					{
						if (m_doctype_state == 0)
						{
							if (elemsize == 7 && std::memcmp( elemptr, "DOCTYPE", elemsize) == 0)
							{
								m_doctype_root.clear();
								m_doctype_public.clear();
								m_doctype_system.clear();
								m_doctype_state = 1;
								continue;
							}
							else
							{
								// ... other than DOCTYPE definitions are not of interest
								m_doctype_state = 5;
								continue;
							}
						}
						else if (m_doctype_state == 1)
						{
							m_doctype_root.append( (const char*)elemptr, elemsize);
							m_doctype_state = 2;
							continue;
						}
						else if (m_doctype_state == 2)
						{
							if (elemsize == 6 && std::memcmp( elemptr, "PUBLIC", elemsize) == 0)
							{
								m_doctype_state = 3;
								continue;
							}
							else if (elemsize == 6 && std::memcmp( elemptr, "SYSTEM", elemsize) == 0)
							{
								m_doctype_state = 4;
								continue;
							}
							else
							{
								err = "error in xml <!DOCTYPE definition: SYSTEM or PUBLIC expected";
								return false;
							}
						}
						else if (m_doctype_state == 3)
						{
							m_doctype_public.append( (const char*)elemptr, elemsize);
							m_doctype_state = 4;
							continue;
						}
						else if (m_doctype_state == 4)
						{
							m_doctype_system.append( (const char*)elemptr, elemsize);
							m_doctype_state = 5;
							continue;
						}
					}
					else if (elemtype == XMLScannerBase::DocAttribEnd)
					{
						if (m_doctype_state < 5)
						{
							err = "error in xml <!DOCTYPE definition";
							return false;
						}
						m_state = ParseSource;
						m_doctype_state = 0;
						return true;
					}
					else if (elemtype == XMLScannerBase::OpenTag)
					{
						m_lastelem = elemptr;
						m_lastelemsize = elemsize;
						m_lastelemtype = elemtype;
						m_state = ParseSourceReady;
						m_doctype_state = 0;
						return true;
					}
					else
					{
						m_lastelem = 0;
						m_lastelemsize = 0;
						m_lastelemtype = XMLScannerBase::ErrorOccurred;
						m_state = ParseSource;
						m_doctype_state = 0;
						return true;
					}
				case ParseSourceReady:
				case ParseSource:
					err = "illegal state in xml header parser";
					return false;
			}
		}
	}

	bool hasMetadataParsed() const
	{
		return m_state == ParseSource || m_state == ParseSourceReady;
	}

	bool ungetElement( XMLScannerBase::ElementType type, const char* elemptr, std::size_t elemsize)
	{
		if (m_lastelemtype == XMLScannerBase::ErrorOccurred)
		{
			m_lastelem = elemptr;
			m_lastelemsize = elemsize;
			m_lastelemtype = type;
			return true;
		}
		return false;
	}

	XMLScannerBase::ElementType getKeptElement( const char*& elemptr, std::size_t& elemsize)
	{
		elemptr = m_lastelem;
		elemsize = m_lastelemsize;
		XMLScannerBase::ElementType elemtype = m_lastelemtype;
		m_lastelem = 0;
		m_lastelemsize = 0;
		m_lastelemtype = XMLScannerBase::ErrorOccurred;
		return elemtype;
	}

	XMLScannerBase::ElementType getNext( const char*& elemptr, std::size_t& elemsize)
	{
		if (!m_obj) return XMLScannerBase::ErrorOccurred;
		if (m_state != ParseSource)
		{
			if (m_state != ParseSourceReady)
			{
				const char* err = 0;
				if (!parseHeader( err) || !hasMetadataParsed())
				{
					elemptr = err?err:"unknown error";
					elemsize = std::strlen( elemptr);
					return XMLScannerBase::ErrorOccurred;
				}
			}
			if (m_state == ParseSourceReady)
			{
				m_state = ParseSource;
				return getKeptElement( elemptr, elemsize);
			}
		}
		for (;;)
		{
			XMLScannerBase::ElementType elemtype = m_mt.m_getNext( m_obj, elemptr, elemsize);
			if (elemtype == XMLScannerBase::Content && !m_withEmpty)
			{
				std::size_t ii=0;
				const unsigned char* cc = (const unsigned char*)elemptr;
				for (;ii<elemsize && cc[ii] <= ' '; ++ii);
				if (ii==elemsize) continue;
			}
			return elemtype;
		}
	}

private:
	bool createParser()
	{
		std::string enc;
		parseEncoding( enc, m_encoding);

		if (m_obj)
		{
			m_srcofs = getPosition();
			m_mt.m_del( m_obj);
			m_obj = 0;
		}
		if ((enc.size() >= 8 && std::memcmp( enc.c_str(), "isolatin", 8)== 0)
		||  (enc.size() >= 7 && std::memcmp( enc.c_str(), "iso8859", 7) == 0))
		{
			const char* codepage = enc.c_str() + ((enc.c_str()[4] == 'l')?8:7);
			if (std::strlen( codepage) > 1 || codepage[0] < '0' || codepage[0] > '9')
			{
				return false;
			}
			if (codepage[0] == '1')
			{
				m_obj = XMLParserObject<SrcIterator,BufferType,charset::IsoLatin,charset::UTF8>::create( m_mt);
			}
			else
			{
				m_obj = XMLParserObject<SrcIterator,BufferType,charset::IsoLatin,charset::UTF8>::create( m_mt, charset::IsoLatin( codepage[0] - '0'));
			}
		}
		else if (enc.size() == 0 || enc == "utf8")
		{
			m_obj = XMLParserObject<SrcIterator,BufferType,charset::UTF8,charset::UTF8>::create( m_mt);
		}
		else if (enc == "utf16" || enc == "utf16be")
		{
			m_obj = XMLParserObject<SrcIterator,BufferType,charset::UTF16BE,charset::UTF8>::create( m_mt);
		}
		else if (enc == "utf16le")
		{
			m_obj = XMLParserObject<SrcIterator,BufferType,charset::UTF16LE,charset::UTF8>::create( m_mt);
		}
		else if (enc == "ucs2" || enc == "ucs2be")
		{
			m_obj = XMLParserObject<SrcIterator,BufferType,charset::UCS2BE,charset::UTF8>::create( m_mt);
		}
		else if (enc == "ucs2le")
		{
			m_obj = XMLParserObject<SrcIterator,BufferType,charset::UCS2LE,charset::UTF8>::create( m_mt);
		}
		else if (enc == "utf32" || enc == "ucs4" || enc == "utf32be" || enc == "ucs4be")
		{
			m_obj = XMLParserObject<SrcIterator,BufferType,charset::UCS4BE,charset::UTF8>::create( m_mt);
		}
		else if (enc == "utf32le" || enc == "ucs4le")
		{
			m_obj = XMLParserObject<SrcIterator,BufferType,charset::UCS4LE,charset::UTF8>::create( m_mt);
		}
		if (m_obj) putInput( m_src+m_srcofs, m_srcsize-m_srcofs, m_srceof);
		return m_obj;
	}
private:
	State m_state;				//< parser section parsing state
	const char* m_src;			//< pointer to source chunk
	std::size_t m_srcsize;			//< size of source chunk
	bool m_srceof;				//< end of input reached
	std::size_t m_srcofs;			//< offset for current position calculation
	MethodTable m_mt;			//< method table of m_obj
	void* m_obj;				//< pointer to parser objecct
	enum HeaderAttribType
	{
		None,
		Version,
		Encoding,
		Standalone
	};
	HeaderAttribType m_headerAttrType;	//< flag used in state 'ParseHeader', marks the last attribute name parsed
	bool m_withEmpty;			//< do produce empty tokens (containing only spaces)
	bool m_doTokenize;			//< do tokenize (whitespace sequences as delimiters)
	bool m_standalone;			//< true if there is no document scema defintion
	std::string m_encoding;			//< defines the XML encoding detected in input
	int m_doctype_state;			//< parser doctype section parsing state
	std::string m_doctype_root;		//< document type definition root element
	std::string m_doctype_public;		//< document type public identifier
	std::string m_doctype_system;		//< document type system URI of validation schema
	const char* m_lastelem;			//< last element parsed (for a kind of unget implementation)
	std::size_t m_lastelemsize;		//< size of last element parsed (for a kind of unget implementation)
	XMLScannerBase::ElementType m_lastelemtype;//< type of last element
};

} //namespace
#endif
