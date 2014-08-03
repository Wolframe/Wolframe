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
/// \file doctypeDetectorXml.hpp
/// \brief Interface to document type and format recognition for XML

#ifndef _Wolframe_CMDBIND_DOCTYPE_DETECTOR_XML_HPP_INCLUDED
#define _Wolframe_CMDBIND_DOCTYPE_DETECTOR_XML_HPP_INCLUDED
#include "cmdbind/doctypeDetector.hpp"
#include "types/doctypeinfo.hpp"
#include "types/docmetadata.hpp"
#include "utils/asciiCharParser.hpp"
#include <string>
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace cmdbind {

class DoctypeDetectorXml
	:public DoctypeDetector
{
public:
	enum State
	{
		ParseStart,
		ParseXMLHeader0,
		ParseXMLHeader,
		SearchXMLDoctypeTag,
		ParseXMLDoctype0,
		ParseXMLDoctype1,
		ParseXMLDoctype2,
		ParseXMLDoctype,
		SkipXMLComment,
		SearchXMLRootName,
		ParseXMLRootName,
		SearchXMLRootAttrib,
		ParseXMLRootAttrib,
		SearchXMLRootAttribAssign,
		SearchXMLRootAttribQuote,
		ParseXMLRootAttribValueSQ,
		ParseXMLRootAttribValueDQ,
		Done
	};
	
	static const char* stateName( State st)
	{
		static const char* ar[] = {
			"ParseStart",
			"ParseXMLHeader0",
			"ParseXMLHeader",
			"SearchXMLDoctypeTag",
			"ParseXMLDoctype0",
			"ParseXMLDoctype1",
			"ParseXMLDoctype2",
			"ParseXMLDoctype",
			"SkipXMLComment",
			"SearchXMLRootName",
			"ParseXMLRootName",
			"SearchXMLRootAttrib",
			"ParseXMLRootAttrib",
			"SearchXMLRootAttribAssign",
			"SearchXMLRootAttribQuote",
			"ParseXMLRootAttribValueSQ",
			"ParseXMLRootAttribValueDQ",
			"Done"};
		return ar[ (int)st];
	}

public:
	DoctypeDetectorXml()
		:m_state(ParseStart),m_isDoctypeAttrib(false),m_lastchar(0){}

	/// \brief Destructor
	virtual ~DoctypeDetectorXml(){}

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
	bool m_isDoctypeAttrib;				///< true, if the attribute found is a doctype attrib
	utils::AsciiCharParser m_charparser;		///< character by caracter parser for source
	std::string m_itembuf;				///< value item parsed (value depending on state)
	unsigned char m_lastchar;			///< the last character parsed
};

}}//namespace
#endif

