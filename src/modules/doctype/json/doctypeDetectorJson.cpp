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
/// \file doctypeDetectorJson.cpp
/// \brief Implementation of document type and format recognition for XML
#include "doctypeDetectorJson.hpp"
#include "cmdbind/doctypeDetector.hpp"
#include "types/doctypeinfo.hpp"
#include "types/docmetadata.hpp"
#include "utils/asciiCharParser.hpp"
#include "utils/parseUtils.hpp"
#include "utils/fileUtils.hpp"
#include "logger-v1.hpp"
#include <cstring>
#include <stdexcept>

using namespace _Wolframe;
using namespace _Wolframe::cmdbind;

namespace {
enum State
{
	ParseStart,
	ParseJSONHeaderStart,
	ParseJSONHeaderStringKeyDash,
	ParseJSONHeaderStringKey,
	ParseJSONHeaderSeekAssign,
	ParseJSONHeaderAssign,
	ParseJSONHeaderIdentKey,
	ParseJSONHeaderIdentValue,
	ParseJSONHeaderStringValue,
	ParseJSONHeaderSeekDelim,
	Done
};

static const char* stateName( State st)
{
	static const char* ar[] = {
		"ParseStart",
		"ParseJSONHeaderStart",
		"ParseJSONHeaderStringKeyDash",
		"ParseJSONHeaderStringKey",
		"ParseJSONHeaderSeekAssign",
		"ParseJSONHeaderAssign",
		"ParseJSONHeaderIdentKey",
		"ParseJSONHeaderIdentValue",
		"ParseJSONHeaderStringValue",
		"ParseJSONHeaderSeekDelim",
		"Done"};
	return ar[ (int)st];
}

enum KeyType
{
	KeyNone, KeyDoctype, KeyEncoding
};

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

/// \class DoctypeDetectorJson
/// \brief Document type and format detection for JSON.
class DoctypeDetectorJson
	:public DoctypeDetector
{
public:
	DoctypeDetectorJson()
		:m_state(ParseStart),m_endbrk(0),m_lastchar(0){}

	/// \brief Destructor
	virtual ~DoctypeDetectorJson(){}

	virtual void putInput( const char* chunk, std::size_t chunksize)
	{
		m_charparser.putInput( chunk, chunksize);
	}

	void setState( State state_)
	{
		m_state = state_;
		LOG_DATA << "STATE DoctypeDetectorJson " << stateName( m_state);
	}

	virtual bool run()
	{
		unsigned char ch = m_charparser.getNext();
		for (; ch != 0 && m_state != Done; ch = m_charparser.getNext())
		{
			if (ch == '.' && m_lastchar == '\n')
			{
				setState( Done);
				break;
			}
			m_lastchar = ch;

			switch (m_state)
			{
				case ParseStart:
					if (ch == '{')
					{
						m_itembuf.clear();
						setState( ParseJSONHeaderStart);
					}
					else if (isSpace( ch))
					{
						continue;
					}
					else
					{
						setState( Done);
					}
					break;

				case ParseJSONHeaderStart:
					if (isSpace( ch))
					{}
					else if (ch == '"' || ch == '\'')
					{
						m_endbrk = ch;
						setState( ParseJSONHeaderStringKeyDash);
					}
					else if (ch == '-')
					{
						setState( ParseJSONHeaderIdentKey);
					}
					else if (isAlpha(ch))
					{
						m_itembuf.push_back( ch|32);
						setState( ParseJSONHeaderIdentKey);
					}
					else
					{
						//... not recognized as a JSON document
						setState( Done);
					}
					break;

				case ParseJSONHeaderStringKeyDash:
					if (ch == m_endbrk)
					{
						m_endbrk = 0;
						setState( ParseJSONHeaderSeekAssign);
					}
					else if (ch == '\\')
					{
						setState( ParseJSONHeaderStringKey);
						m_escapestate = true;
						continue;
					}
					else if (ch == '-')
					{
						setState( ParseJSONHeaderStringKey);
						//... ignoring leading dash in key
					}
					else
					{
						m_itembuf.push_back( ch);
					}
					break;
				case ParseJSONHeaderStringKey:
					if (m_escapestate)
					{
						m_itembuf.push_back( ch);
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
						m_itembuf.push_back( ch);
						continue;
					}
					break;

				case ParseJSONHeaderIdentKey:
					if (isAlphaNum(ch))
					{
						m_itembuf.push_back( ch|32);
					}
					else if (isSpace( ch))
					{
						setState( ParseJSONHeaderSeekAssign);
					}
					else if ((unsigned char)ch == ':')
					{
						if (boost::algorithm::iequals( m_itembuf, "doctype"))
						{
							m_itembuf.clear();
							m_keytype = KeyDoctype;
							setState( ParseJSONHeaderAssign);
						}
						else if (boost::algorithm::iequals( m_itembuf, "encoding"))
						{
							m_itembuf.clear();
							m_keytype = KeyEncoding;
							setState( ParseJSONHeaderAssign);
						}
						else
						{
							m_itembuf.clear();
							m_keytype = KeyNone;
							m_info.reset( new types::DoctypeInfo("JSON", ""));
							setState( Done);
						}
					}
					else
					{
						//... not recognized as a JSON document
						setState( Done);
					}
					break;

				case ParseJSONHeaderSeekAssign:
					if (isSpace( ch))
					{}
					else if ((unsigned char)ch == ':')
					{
						if (boost::algorithm::iequals( m_itembuf, "doctype"))
						{
							m_itembuf.clear();
							m_keytype = KeyDoctype;
							setState( ParseJSONHeaderAssign);
						}
						else if (boost::algorithm::iequals( m_itembuf, "encoding"))
						{
							m_itembuf.clear();
							m_keytype = KeyEncoding;
							setState( ParseJSONHeaderAssign);
						}
						else
						{
							m_itembuf.clear();
							m_keytype = KeyNone;
							m_info.reset( new types::DoctypeInfo("JSON", ""));
							setState( Done);
						}
					}
					else
					{
						//... not recognized as a JSON document
						setState( Done);
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
						m_lasterror = "identifier or string exprected for JSON element key";
						setState( Done);
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
								m_lasterror = "syntax error in JSON document (document type recognition)";
								setState( Done);
								break;
							case KeyDoctype:
								m_info.reset( new types::DoctypeInfo("JSON", m_itembuf));
								m_keytype = KeyNone;
								setState( Done);
								break;
							case KeyEncoding:
								setState( ParseJSONHeaderSeekDelim);
								break;
						}
						m_itembuf.clear();
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
								m_lasterror = "syntax error in JSON document (document type recognition)";
								setState( Done);
								break;
							case KeyDoctype:
								m_info.reset( new types::DoctypeInfo("JSON", m_itembuf));
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

				case Done:
					break;
			}
		}
		if (ch == 0 && m_state != Done)
		{
			if (m_charparser.lastError())
			{
				m_lasterror = m_charparser.lastError();
			}
			return false;
		}
		return m_lasterror.empty();
	}

	virtual const char* lastError() const
	{
		return m_lasterror.empty()?0:m_lasterror.c_str();
	}

	virtual const types::DoctypeInfoR& info() const
	{
		return m_info;
	}

private:
	types::DoctypeInfoR m_info;			///< the result of doctype detection
	std::string m_lasterror;			///< the last error occurred
	State m_state;					///< processing state machine state
	utils::AsciiCharParser m_charparser;		///< character by caracter parser for source
	KeyType m_keytype;				///< type of meta data attribute value parsed
	std::string m_itembuf;				///< value item parsed
	unsigned char m_endbrk;				///< end quote in state parsing string
	unsigned char m_lastchar;			///< the last character parsed
	bool m_escapestate;				///< true if the last character was a backslash, so the next character is escaped
};
}//anonymous namespace


cmdbind::DoctypeDetector* cmdbind::createDoctypeDetectorJson()
{
	return new DoctypeDetectorJson();
}


