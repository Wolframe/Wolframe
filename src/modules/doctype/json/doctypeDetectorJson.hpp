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
/// \file doctypeDetectorJson.hpp
/// \brief Interface to document type and format recognition for JSON

#ifndef _Wolframe_CMDBIND_DOCTYPE_DETECTOR_JSON_HPP_INCLUDED
#define _Wolframe_CMDBIND_DOCTYPE_DETECTOR_JSON_HPP_INCLUDED
#include "cmdbind/doctypeDetector.hpp"
#include "types/doctypeinfo.hpp"
#include "types/docmetadata.hpp"
#include "utils/asciiCharParser.hpp"
#include <string>
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace cmdbind {

/// \class DoctypeDetectorJson
/// \brief Document type and format detection for JSON.
class DoctypeDetectorJson
	:public DoctypeDetector
{
public:
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

public:
	DoctypeDetectorJson()
		:m_state(ParseStart)
		,m_keytype(KeyNone)
		,m_endbrk(0)
		,m_lastchar(0)
		,m_escapestate(false){}

	/// \brief Destructor
	virtual ~DoctypeDetectorJson(){}

	virtual void putInput( const char* chunk, std::size_t chunksize)
	{
		m_charparser.putInput( chunk, chunksize);
	}

	void setState( State state_);

	virtual bool run();

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
}}//namespace
#endif


