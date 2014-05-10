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
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
///\file cmdbind_doctypeFilterCommandHandler.cpp
#include "cmdbind/doctypeFilterCommandHandler.hpp"
#include "processor/execContext.hpp"
#include "logger-v1.hpp"
#include "types/doctype.hpp"
#include "utils/parseUtils.hpp"
#include "utils/fileUtils.hpp"
#include <cstring>
#include <boost/algorithm/string.hpp>

using namespace _Wolframe;
using namespace _Wolframe::cmdbind;

DoctypeFilterCommandHandler::DoctypeFilterCommandHandler()
	:m_state(Init)
	,m_encoding(UCS1)
	,m_keytype(KeyNone)
	,m_lastchar('\n')
	,m_endbrk(0)
	,m_escapestate(false)
	,m_itr(0)
	,m_end(0)
	,m_src(0)
{}

DoctypeFilterCommandHandler::~DoctypeFilterCommandHandler()
{}

void DoctypeFilterCommandHandler::setInputBuffer( void* buf, std::size_t allocsize)
{
	m_input = protocol::InputBlock( (char*)buf, allocsize);
}

void DoctypeFilterCommandHandler::setOutputBuffer( void*, std::size_t, std::size_t)
{}

std::string DoctypeFilterCommandHandler::doctypeid() const
{
	if (m_state != Done) throw std::logic_error( "illegal call of get doctypeid in this state");
	return m_doctypeid;
}

std::string DoctypeFilterCommandHandler::docformatid() const
{
	if (m_state != Done) throw std::logic_error( "illegal call of get doctypeid in this state");
	return m_docformatid;
}

void DoctypeFilterCommandHandler::throw_error( const char* msg) const
{
	std::string ex( "failed to parse document type (state ");
	ex = ex + stateName( m_state) + ")";
	if (msg) ex = ex + ": " + msg;
	throw std::runtime_error( ex);
}

void DoctypeFilterCommandHandler::setState( State state_)
{
	m_state = state_;
	LOG_TRACE << "STATE DoctypeCommandHandler " << stateName( m_state);
}

static bool isAlpha( char ch)
{
	return (((ch|32) >= 'a' && (ch|32) <= 'z') || ch == '_');
}

static bool isAlphaNum( char ch)
{
	return (((ch|32) >= 'a' && (ch|32) <= 'z') || (ch >= '0' && ch <= '9') || ch == '_');
}

static bool isSpace( char ch)
{
	return ((unsigned char)ch <= 32);
}

static std::string getIdFromXMLDoctype( const std::string& doctype)
{
	std::string rootid;
	std::string publicid;
	std::string systemid;
	std::string::const_iterator itr=doctype.begin(), end=doctype.end();

	if (utils::parseNextToken( rootid, itr, end))
	{
		if (utils::parseNextToken( publicid, itr, end))
		{
			if (publicid == "PUBLIC")
			{
				if (!utils::parseNextToken( publicid, itr, end)
				||  !utils::parseNextToken( systemid, itr, end))
				{
					throw std::runtime_error( "illegal doctype definition (public id)");
				}
				return utils::getFileStem( systemid);
			}
			else if (publicid == "SYSTEM")
			{
				if (!utils::parseNextToken( systemid, itr, end))
				{
					throw std::runtime_error( "illegal doctype definition (system id)");
				}
				return utils::getFileStem( systemid);
			}
			else if (utils::parseNextToken( systemid, itr, end))
			{
				if (itr != end)
				{
					throw std::runtime_error( "illegal doctype definition (not terminated)");
				}
				return utils::getFileStem( systemid);
			}
			else
			{
				return utils::getFileStem( publicid);
			}
		}
		else
		{
			return rootid;
		}
	}
	else
	{
		return "";
	}
}

enum {EndOfBuffer=0xFE,NonAscii=0xFF};
unsigned char DoctypeFilterCommandHandler::nextChar()
{
	unsigned char rt = 0;
	switch (m_encoding)
	{
		case UCS1:
			if (m_itr >= m_end) return EndOfBuffer;
			if ((unsigned char)m_src[m_itr] >= 127)
			{
				rt = NonAscii;
			}
			else
			{
				rt = m_src[m_itr];
			}
			m_inputbuffer.append( (const char*)m_src+m_itr, 1);
			m_itr += 1;
			return rt;
		case UCS2BE:
			if (m_itr+2 > m_end) return EndOfBuffer;
			if (m_src[m_itr+0] || (unsigned char)m_src[m_itr+1] >= 127)
			{
				rt = NonAscii;
			}
			else
			{
				rt = m_src[m_itr+1];
			}
			m_inputbuffer.append( (const char*)m_src+m_itr, 2);
			m_itr += 2;
			return rt;
		case UCS2LE:
			if (m_itr+2 > m_end) return EndOfBuffer;
			if (m_src[m_itr+1] || (unsigned char)m_src[m_itr+0] >= 127)
			{
				rt = NonAscii;
			}
			else
			{
				rt = m_src[m_itr];
			}
			m_inputbuffer.append( (const char*)m_src+m_itr, 2);
			m_itr += 2;
			return rt;
		case UCS4BE:
			if (m_itr+4 > m_end) return EndOfBuffer;
			if (m_src[m_itr] || m_src[m_itr+1] || m_src[m_itr+2] || (unsigned char)m_src[m_itr+3] >= 127)
			{
				rt = NonAscii;
			}
			else
			{
				rt = m_src[m_itr+3];
			}
			m_inputbuffer.append( (const char*)m_src+m_itr, 4);
			m_itr += 4;
			return rt;
		case UCS4LE:
			if (m_itr+4 > m_end) return EndOfBuffer;
			if (m_src[m_itr+1] || m_src[m_itr+2] || m_src[m_itr+3] || (unsigned char)m_src[m_itr+0] >= 127)
			{
				rt = NonAscii;
			}
			else
			{
				rt = m_src[m_itr+0];
			}
			m_inputbuffer.append( (const char*)m_src+m_itr, 4);
			m_itr += 4;
			break;
	}
	return rt;
}

bool DoctypeFilterCommandHandler::getEncoding()
{
	if (m_itr + 2 > m_end) return false;
	if (m_src[m_itr+0] == 0xFF && m_src[m_itr+1] == 0xFE)
	{
		if (m_itr + 4 > m_end) return false;
		if (!m_src[m_itr+2] && !m_src[m_itr+3])
		{
			m_encoding = UCS4LE;		//... BOM for UTF-32LE
			m_inputbuffer.append( (const char*)m_src+m_itr, 4);
			m_itr += 4;
			return true;
		}
		else
		{
			m_encoding = UCS2LE;		//... BOM for UTF-16LE
			m_inputbuffer.append( (const char*)m_src+m_itr, 2);
			m_itr += 2;
			return true;
		}
	}
	else if (m_src[m_itr+0] == 0xFE && m_src[m_itr+1] == 0xFF)
	{
		m_encoding = UCS2BE;			//... BOM for UTF-16BE
		m_inputbuffer.append( (const char*)m_src+m_itr, 2);
		m_itr += 2;
		return true;
	}
	else if (m_src[m_itr+0] && !m_src[m_itr+1])
	{
		if (m_itr + 4 > m_end) return false;
		if (!m_src[m_itr+2])
		{
			m_encoding = UCS4LE;		//... First character is Ascii UTF-32LE
			return true;
		}
		else
		{
			m_encoding = UCS2LE;		//... First character is Ascii UCS-2LE
			return true;
		}
	}
	else if (!m_src[m_itr+0] && m_src[m_itr+1])
	{
		m_encoding = UCS2BE;			//... First character is Ascii UCS-2BE
		return true;
	}
	else if (!m_src[m_itr+0] && !m_src[m_itr+1])
	{
		if (m_itr + 4 > m_end) return false;
		if (!m_src[m_itr+2])
		{
			m_encoding = UCS4BE;		//... First character is Ascii UTF-32BE
			return true;
		}
		else if (m_src[m_itr+2] == 0xFE && m_src[m_itr+2] == 0xFF)
		{
			m_encoding = UCS4BE;		//... BOM for UTF-32BE
			m_inputbuffer.append( (const char*)m_src+m_itr, 4);
			m_itr += 4;
			return true;
		}
	}
	else
	{
		m_encoding = UCS1;
		return true;
	}
	throw std::runtime_error("unknown character set encoding");
}

CommandHandler::Operation DoctypeFilterCommandHandler::nextOperation()
{
	if (m_state == Done) return CLOSE;
	LOG_TRACE << "STATE DoctypeCommandHandler " << stateName( m_state) << " (put input)";
	try
	{
		if (m_state == Init)
		{
			if (!getEncoding())
			{
				return READ;
			}
			else
			{
				m_state = ParseStart;
			}
		}
		unsigned char ch = nextChar();
		for (; m_state != Done; ch = nextChar())
		{
			if (ch == (unsigned char)EndOfBuffer)
			{
				return READ;
			}
			if (ch == '.' && m_lastchar == '\n')
			{
				setState( Done);
				break;
			}
			m_lastchar = ch;

			switch (m_state)
			{
				case Init:
					throw_error("illegal state");
				case ParseStart:
					if (ch == '<')
					{
						m_docformatid = "xml";
						setState( ParseXMLHeader0);
					}
					else if (ch == '{')
					{
						m_docformatid = "json";
						setState( ParseJSONHeaderStart);
					}
					else if (isSpace( ch))
					{
						continue;
					}
					else
					{
						m_docformatid = "text";
						setState( Done);
					}
					break;
				case ParseJSONHeaderStart:
					if (isSpace( ch))
					{}
					else if (ch == '"' || ch == '\'')
					{
						m_endbrk = ch;
						setState( ParseJSONHeaderStringKey);
					}
					else if (isAlpha(ch))
					{
						m_keybuf.push_back( ch|32);
						setState( ParseJSONHeaderIdentKey);
					}
					else
					{
						throw_error( "identifier or string exprected for JSON element key");
					}
					break;

				case ParseJSONHeaderStringKey:
					if (m_escapestate)
					{
						m_keybuf.push_back( ch);
						m_escapestate = false;
						continue;
					}
					if (ch == m_endbrk)
					{
						m_endbrk = 0;
						setState( ParseJSONHeaderSeekAssign);
					}
					else if (ch == '\\')
					{
						m_escapestate = true;
						continue;
					}
					else
					{
						m_keybuf.push_back( ch);
						continue;
					}
					break;

				case ParseJSONHeaderIdentKey:
					if (isAlphaNum(ch))
					{
						m_keybuf.push_back( ch|32);
					}
					else if (isSpace( ch))
					{
						setState( ParseJSONHeaderSeekAssign);
					}
					else if ((unsigned char)ch == ':')
					{
						if (boost::algorithm::iequals( m_keybuf, "doctype"))
						{
							m_keytype = KeyDoctype;
							setState( ParseJSONHeaderAssign);
						}
						else if (boost::algorithm::iequals( m_keybuf, "encoding"))
						{
							m_keytype = KeyEncoding;
							setState( ParseJSONHeaderAssign);
						}
						else
						{
							m_keytype = KeyNone;
							setState( Done);
						}
					}
					else
					{
						throw_error( "identifier or string followed by ':' expected as JSON element assignment header");
					}
					break;

				case ParseJSONHeaderSeekAssign:
					if (isSpace( ch))
					{}
					else if ((unsigned char)ch == ':')
					{
						if (boost::algorithm::iequals( m_keybuf, "doctype"))
						{
							m_keytype = KeyDoctype;
							setState( ParseJSONHeaderAssign);
						}
						else if (boost::algorithm::iequals( m_keybuf, "encoding"))
						{
							m_keytype = KeyEncoding;
							setState( ParseJSONHeaderAssign);
						}
						else
						{
							m_keytype = KeyNone;
							setState( Done);
						}
					}
					else
					{
						throw_error( "identifier or string followed by ':' expected as JSON element assignment header");
					}
					break;

				case ParseJSONHeaderAssign:
					if (isSpace( ch))
					{}
					else if (ch == '"' || ch == '\'')
					{
						m_endbrk = ch;
						setState( ParseJSONHeaderStringValue);
					}
					else if (isAlphaNum(ch))
					{
						m_itembuf.push_back( ch|32);
						setState( ParseJSONHeaderIdentValue);
					}
					else
					{
						throw_error( "identifier or string exprected for JSON element key");
					}
					break;

				case ParseJSONHeaderStringValue:
					if (m_escapestate)
					{
						m_itembuf.push_back( ch);
						m_escapestate = false;
						continue;
					}
					if (ch == m_endbrk)
					{
						m_endbrk = 0;
						switch (m_keytype)
						{
							case KeyNone:
								throw std::runtime_error("syntax error in JSON document (document type recognition)");
							case KeyDoctype:
								m_doctypeid = m_itembuf;
								m_keytype = KeyNone;
								setState( Done);
								break;
							case KeyEncoding:
								setState( ParseJSONHeaderSeekDelim);
								break;
						}
						m_itembuf.clear();
						m_keybuf.clear();
					}
					else if (ch == '\\')
					{
						m_escapestate = true;
						continue;
					}
					else
					{
						m_itembuf.push_back( ch);
						continue;
					}
					break;

				case ParseJSONHeaderIdentValue:
					if (isSpace( ch) || ch == ',' || ch == '}')
					{
						switch (m_keytype)
						{
							case KeyNone:
								throw std::runtime_error("syntax error in JSON document (document type recognition)");
							case KeyDoctype:
								m_doctypeid = m_itembuf;
								m_keytype = KeyNone;
								setState( Done);
								break;
							case KeyEncoding:
								if (ch == '}')
								{
									setState( Done);
								}
								else if (ch == ',')
								{
									setState( ParseJSONHeaderStart);
								}
								else
								{
									setState( ParseJSONHeaderSeekDelim);
								}
								break;
						}
						m_itembuf.clear();
						m_keybuf.clear();
					}
					else
					{
						m_itembuf.push_back( ch|32);
					}
					break;

				case ParseJSONHeaderSeekDelim:
					if (!isSpace( ch))
					{
						if (ch == ',')
						{
							setState( ParseJSONHeaderStart);
						}
						else
						{
							setState( Done);
						}
					}
					break;

				case ParseXMLHeader0:
					if (ch == '?')
					{
						setState( ParseXMLHeader);
					}
					else
					{
						throw_error( "expected '<?'");
					}
					break;

				case ParseXMLHeader:
					if (ch == '>')
					{
						const char* cc = std::strstr( m_itembuf.c_str(), "standalone");
						if (cc)
						{
							cc = std::strchr( cc, '=');
							if (cc) cc = std::strstr( cc, "yes");
							if (cc)
							{
								setState( Done);
								break;
							}
						}
						setState( SearchXMLDoctypeTag);
						m_itembuf.clear();
					}
					else
					{
						m_itembuf.push_back( ch);
						if (m_itembuf.size() > 128)
						{
							throw_error( "XML header not terminated");
						}
					}
					break;

				case SearchXMLDoctypeTag:
					if (ch == '<')
					{
						setState( ParseXMLDoctype0);
					}
					else if ((unsigned char)ch > 32)
					{
						throw_error( "expected '<!'");
					}
					break;

				case ParseXMLDoctype0:
					if (ch == '!')
					{
						setState( ParseXMLDoctype1);
					}
					else
					{
						setState( SearchXMLRootName);
					}
					break;

				case ParseXMLDoctype1:
					if (ch == '-')
					{
						setState( SkipXMLComment);
					}
					else if (ch == 'D')
					{
						m_itembuf.push_back( ch);
						setState( ParseXMLDoctype2);
					}
					else
					{
						throw_error( "expected '<!DOCTYPE' or <!--");
					}
					break;

				case SkipXMLComment:
					if (ch == '>')
					{
						setState( SearchXMLDoctypeTag);
					}
					break;

				case ParseXMLDoctype2:
					if (ch <= ' ' && ch > 0)
					{
						if (m_itembuf != "DOCTYPE")
						{
							throw_error( "expected '<!DOCTYPE'");
						}
						setState( ParseXMLDoctype);
						m_itembuf.clear();
					}
					else
					{
						m_itembuf.push_back( ch);
						if (m_itembuf.size() > 8)
						{
							throw_error( "expected '<!DOCTYPE'");
						}
					}
					break;

				case ParseXMLDoctype:
					if (ch <= ' ' && ch > 0)
					{
						m_doctype.push_back( ' ');
					}
					else if (ch == '>')
					{
						m_doctypeid = getIdFromXMLDoctype( m_doctype);
						setState( Done);
					}
					else
					{
						m_doctype.push_back( ch);
					}
					break;

				case SearchXMLRootName:
					if (ch == '>')
					{
						throw_error( "invalid XML root element");
					}
					if (ch > ' ')
					{
						setState( ParseXMLRootName);
						m_itembuf.clear();
					}
					break;

				case ParseXMLRootName:
					if (ch != '>' && ch > ' ')
					{
						m_itembuf.push_back( ch);
						if (m_itembuf.size() > 128)
						{
							throw_error( "XML root element name too big");
						}
					}
					else
					{
						m_rootelemname = m_itembuf;
						if (ch == '>')
						{
							throw_error( "No document type or schema definition found and document is not standalone");
						}
						setState( SearchXMLRootAttrib);
					}
					break;

				case SearchXMLRootAttrib:
					if (ch == '>')
					{
						throw_error( "No document type or schema definition found and document is not standalone");
					}
					if (ch > ' ')
					{
						setState( ParseXMLRootAttrib);
						m_itembuf.clear();
					}
					break;

				case ParseXMLRootAttrib:
					if (ch != '>' && ch > ' ')
					{
						m_itembuf.push_back( ch);
						if (m_itembuf.size() > 128)
						{
							throw_error( "XML root attribute name too big");
						}
					}
					else
					{
						if (m_itembuf == "xmlns:xsi")
						{
							setState( SearchXMLRootAttribAssign);
						}
						else if (ch == '>')
						{
							throw_error( "No document type or schema definition found and document is not standalone");
						}
						else
						{
							setState( SearchXMLRootAttrib);
						}
					}
					break;

				case SearchXMLRootAttribAssign:
					if (ch == '=')
					{
						setState( SearchXMLRootAttribQuote);
					}
					else if (ch > ' ')
					{
						throw_error( "invalid XML root attribute definition (missing value assingment)");
					}
					break;

				case SearchXMLRootAttribQuote:
					if (ch == '\'')
					{
						setState( ParseXMLRootAttribValueSQ);
					}
					else if (ch == '\"')
					{
						setState( ParseXMLRootAttribValueDQ);
					}
					else if (ch > ' ')
					{
						throw_error( "invalid XML root attribute definition (missing value assingment)");
					}
					break;

				case ParseXMLRootAttribValueSQ:
					if (ch == '\'')
					{
						m_doctypeid = utils::getFileStem( m_itembuf);
						setState( Done);
					}
					else
					{
						m_itembuf.push_back( ch);
						if (m_itembuf.size() > 256)
						{
							throw_error( "XML xmlns:xsi attribute (schema definition) is too big");
						}
					}
					break;

				case ParseXMLRootAttribValueDQ:
					if (ch == '\"')
					{
						m_doctypeid = utils::getFileStem( m_itembuf);
						setState( Done);
					}
					else
					{
						m_itembuf.push_back( ch);
						if (m_itembuf.size() > 256)
						{
							throw_error( "XML xmlns:xsi attribute (schema definition) is too big");
						}
					}
					break;

				case Done:
					break;
			}
		}
	}
	catch (const std::runtime_error& err)
	{
		m_lastError = "failed to evaluate input document type";
		LOG_ERROR << "error in document type recognition: " << err.what();
		m_state = Done;
	}
	return (m_state == Done)?CLOSE:READ;
}

void DoctypeFilterCommandHandler::putInput( const void *begin, std::size_t bytesTransferred)
{
	m_itr = 0;
	m_end = bytesTransferred + ((const char*)begin - m_input.charptr());
	m_src = (const unsigned char*)m_input.charptr();
	m_input.setPos( m_end);

	if ((std::size_t)m_end > m_input.size())
	{
		throw std::logic_error( "illegal input range passed to DoctypeFilterCommandHandler");
	}
}

void DoctypeFilterCommandHandler::getInputBlock( void*& begin, std::size_t& maxBlockSize)
{
	char* blkstart = m_input.charptr();
	unsigned int blksize = m_input.size();
	unsigned int restsize = m_end - m_itr;
	if (m_itr > blksize || restsize > blksize) throw_error("illegal state");
	std::memmove( blkstart, blkstart+m_itr, restsize);
	begin = (void*)(blkstart + restsize);
	maxBlockSize = blksize - restsize;
	if (maxBlockSize == 0) throw std::runtime_error("buffer too small");
}

void DoctypeFilterCommandHandler::getOutput( const void*& begin, std::size_t& bytesToTransfer)
{
	begin = 0;
	bytesToTransfer = 0;
}

void DoctypeFilterCommandHandler::getDataLeft( const void*& begin, std::size_t& nofBytes)
{
	begin = (const void*)(m_input.charptr() + m_itr);
	nofBytes = m_end - m_itr;
}

void DoctypeFilterCommandHandler::getInputBuffer( void*& begin, std::size_t& nofBytes)
{
	begin = m_inputbuffer.base();
	nofBytes = m_inputbuffer.size();
}

