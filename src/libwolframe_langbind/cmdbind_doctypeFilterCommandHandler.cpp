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
	,m_keytype(KeyNone)
	,m_lastchar('\n')
	,m_endbrk(0)
	,m_escapestate(false)
	,m_nullcnt(0)
	,m_inputidx(0)
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

CommandHandler::Operation DoctypeFilterCommandHandler::nextOperation()
{
	if (m_state == Done) return CLOSE;
	return READ;
}

void DoctypeFilterCommandHandler::throw_error( const char* msg) const
{
	std::string ex( "failed to parse doctype from XML (state ");
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

void DoctypeFilterCommandHandler::putInput( const void *begin, std::size_t bytesTransferred)
{
	const char* inp = m_input.charptr();
	std::size_t startidx = (const char*)begin - m_input.charptr();
	std::size_t endidx = bytesTransferred + startidx;
	if (endidx > m_input.size())
	{
		throw std::logic_error( "illegal input range passed to DoctypeFilterCommandHandler");
	}
	m_input.setPos( endidx);
	m_inputidx = startidx;

	LOG_TRACE << "STATE DoctypeCommandHandler " << stateName( m_state) << " (put input)";
	try
	{
		for (; m_inputidx<endidx && m_state != Done; ++m_inputidx)
		{
			if (!inp[m_inputidx])
			{
				++m_nullcnt;
				if (m_nullcnt > 4) throw_error( "Unknown encoding");
			}
			else
			{
				if (m_lastchar == '\n' && inp[m_inputidx] == '.')
				{
					setState( Done);
					break;
				}
				m_lastchar = inp[m_inputidx];
				m_nullcnt = 0;

				switch (m_state)
				{
					case Init:
						if (inp[m_inputidx] == '<')
						{
							m_docformatid = "xml";
							setState( ParseXMLHeader0);
						}
						else if (inp[m_inputidx] == '{')
						{
							m_docformatid = "json";
							setState( ParseJSONHeaderStart);
						}
						else if (!isSpace( inp[m_inputidx]))
						{
							throw_error( "expected '<?' (XML) or '{' (JSON) as first character");
						}
						break;

					case ParseJSONHeaderStart:
						if (isSpace( inp[m_inputidx]))
						{}
						else if (inp[m_inputidx] == '"' || inp[m_inputidx] == '\'')
						{
							m_endbrk = inp[m_inputidx];
							setState( ParseJSONHeaderStringKey);
						}
						else if (isAlpha(inp[m_inputidx]))
						{
							m_keybuf.push_back( inp[m_inputidx]|32);
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
							m_keybuf.push_back( inp[m_inputidx]);
							m_escapestate = false;
							continue;
						}
						if (inp[m_inputidx] == m_endbrk)
						{
							m_endbrk = 0;
							setState( ParseJSONHeaderAssign);
						}
						else if (inp[m_inputidx] == '\\')
						{
							m_escapestate = true;
							continue;
						}
						else
						{
							m_keybuf.push_back( inp[m_inputidx]);
							continue;
						}
						break;

					case ParseJSONHeaderIdentKey:
						if (isAlphaNum(inp[m_inputidx]))
						{
							m_keybuf.push_back( inp[m_inputidx]|32);
						}
						else if (isSpace( inp[m_inputidx]))
						{
							setState( ParseJSONHeaderSeekAssign);
						}
						else if ((unsigned char)inp[m_inputidx] == ':')
						{
							setState( ParseJSONHeaderAssign);
						}
						else
						{
							throw_error( "identifier or string followed by ':' expected as JSON element assignment header");
						}
						break;

					case ParseJSONHeaderSeekAssign:
						if (isSpace( inp[m_inputidx]))
						{}
						else if ((unsigned char)inp[m_inputidx] == ':')
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
						if (isSpace( inp[m_inputidx]))
						{}
						if (inp[m_inputidx] == '"' || inp[m_inputidx] == '\'')
						{
							m_endbrk = inp[m_inputidx];
							setState( ParseJSONHeaderStringValue);
						}
						else if (isAlphaNum(inp[m_inputidx]))
						{
							m_keybuf.push_back( inp[m_inputidx]|32);
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
							m_itembuf.push_back( inp[m_inputidx]);
							m_escapestate = false;
							continue;
						}
						if (inp[m_inputidx] == m_endbrk)
						{
							m_endbrk = 0;
							switch (m_keytype)
							{
								case KeyNone:
									throw std::logic_error("illegal state in JSON document type recognition");
								case KeyDoctype:
									m_doctypeid = m_itembuf;
									m_keytype = KeyNone;
									setState( Done);
									break;
								case KeyEncoding:
									setState( ParseJSONHeaderStart);
									break;
							}
							m_itembuf.clear();
							m_keybuf.clear();
						}
						else if (inp[m_inputidx] == '\\')
						{
							m_escapestate = true;
							continue;
						}
						else
						{
							m_itembuf.push_back( inp[m_inputidx]);
							continue;
						}
						break;

					case ParseJSONHeaderIdentValue:
						if (isSpace( inp[m_inputidx]))
						{
							switch (m_keytype)
							{
								case KeyNone:
									throw std::logic_error("illegal state in JSON document type recognition");
								case KeyDoctype:
									m_doctypeid = m_itembuf;
									m_keytype = KeyNone;
									setState( Done);
									break;
								case KeyEncoding:
									setState( ParseJSONHeaderStart);
									break;
							}
							m_itembuf.clear();
							m_keybuf.clear();
						}
						else
						{
							m_itembuf.push_back( inp[m_inputidx]|32);
						}
						break;

					case ParseXMLHeader0:
						if (inp[m_inputidx] == '?')
						{
							setState( ParseXMLHeader);
						}
						else
						{
							throw_error( "expected '<?'");
						}
						break;

					case ParseXMLHeader:
						if (inp[m_inputidx] == '>')
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
							m_itembuf.push_back( inp[m_inputidx]);
							if (m_itembuf.size() > 128)
							{
								throw_error( "XML header not terminated");
							}
						}
						break;

					case SearchXMLDoctypeTag:
						if (inp[m_inputidx] == '<')
						{
							setState( ParseXMLDoctype0);
						}
						else if ((unsigned char)inp[m_inputidx] > 32)
						{
							throw_error( "expected '<!'");
						}
						break;

					case ParseXMLDoctype0:
						if (inp[m_inputidx] == '!')
						{
							setState( ParseXMLDoctype1);
						}
						else
						{
							throw_error( "expected '<!'");
						}
						break;

					case ParseXMLDoctype1:
						if (inp[m_inputidx] == '-')
						{
							setState( SkipXMLComment);
						}
						else if (inp[m_inputidx] == 'D')
						{
							m_itembuf.push_back( inp[m_inputidx]);
							setState( ParseXMLDoctype2);
						}
						else
						{
							throw_error( "expected '<!DOCTYPE' or <!--");
						}
						break;

					case SkipXMLComment:
						if (inp[m_inputidx] == '>')
						{
							setState( SearchXMLDoctypeTag);
						}
						break;

					case ParseXMLDoctype2:
						if (inp[m_inputidx] <= ' ' && inp[m_inputidx] > 0)
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
							m_itembuf.push_back( inp[m_inputidx]);
							if (m_itembuf.size() > 8)
							{
								throw_error( "expected '<!DOCTYPE'");
							}
						}
						break;

					case ParseXMLDoctype:
						if (inp[m_inputidx] <= ' ' && inp[m_inputidx] > 0)
						{
							m_doctype.push_back( ' ');
						}
						else if (inp[m_inputidx] == '>')
						{
							m_doctypeid = getIdFromXMLDoctype( m_doctype);
							setState( Done);
						}
						else
						{
							m_doctype.push_back( inp[m_inputidx]);
						}
						break;

					case Done:
						break;
				}
			}
		}
	}
	catch (const std::runtime_error& err)
	{
		m_lastError = "failed to extract document type from xml";
		LOG_ERROR << "error in document type recognition: " << err.what();
		m_state = Done;
	}
	m_inputbuffer.append( (const char*)begin, (m_inputidx - startidx));
}

void DoctypeFilterCommandHandler::getInputBlock( void*& begin, std::size_t& maxBlockSize)
{
	begin = m_input.ptr();
	maxBlockSize = m_input.size();
}

void DoctypeFilterCommandHandler::getOutput( const void*& begin, std::size_t& bytesToTransfer)
{
	begin = 0;
	bytesToTransfer = 0;
}

void DoctypeFilterCommandHandler::getDataLeft( const void*& begin, std::size_t& nofBytes)
{
	begin = (const void*)(m_input.charptr() + m_inputidx);
	nofBytes = m_input.pos() - m_inputidx;
}

void DoctypeFilterCommandHandler::getInputBuffer( void*& begin, std::size_t& nofBytes)
{
	begin = m_inputbuffer.base();
	nofBytes = m_inputbuffer.size();
}

