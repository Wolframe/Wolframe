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
///\file cmdbind/doctypeFilterCommandHandler.hpp
///\brief Command handler that does nothing but extracting the document type from the content and buffering the input processed. The document type extracted and the current command determine what command handler has to process the buffered plus the unprocessed content.
#ifndef _Wolframe_cmdbind_DOCTYPE_FILTER_COMMAND_HANDLER_HPP_INCLUDED
#define _Wolframe_cmdbind_DOCTYPE_FILTER_COMMAND_HANDLER_HPP_INCLUDED
#include "protocol/ioblocks.hpp"
#include "cmdbind/commandHandler.hpp"
#include "system/connectionHandler.hpp"

namespace _Wolframe {
namespace cmdbind {

///\class DoctypeFilterCommandHandler
///\brief Command handler for exctracting the document type identifier from a content
///\remark This is a pre-processing command handler. It is returning the consumed input as data left for following processing
class DoctypeFilterCommandHandler :public CommandHandler
{
public:
	typedef CommandHandler Parent;

	///\brief Constructor
	DoctypeFilterCommandHandler();
	///\brief Destructor
	virtual ~DoctypeFilterCommandHandler();

	///\brief See Parent::setInputBuffer(void*,std::size_t,std::size_t,std::size_t)
	virtual void setInputBuffer( void* buf, std::size_t allocsize);

	///\brief See Parent::setOutputBuffer(void*,std::size_t,std::size_t)
	virtual void setOutputBuffer( void* buf, std::size_t size, std::size_t pos);

	///\brief See Parent::nextOperation()
	virtual Operation nextOperation();

	///\brief See Parent::putInput(const void*,std::size_t);
	virtual void putInput( const void *begin, std::size_t bytesTransferred);

	///\brief See Parent::getInputBlock(void*&,std::size_t&)
	virtual void getInputBlock( void*& begin, std::size_t& maxBlockSize);

	///\brief See Parent::getOutput(const void*&,std::size_t&)
	virtual void getOutput( const void*& begin, std::size_t& bytesToTransfer);

	///\brief See Parent::getDataLeft(const void*&,std::size_t&)
	virtual void getDataLeft( const void*& begin, std::size_t& nofBytes);

	///\brief See Get the document type of the document
	std::string doctypeid() const;

	///\brief See Get the document format class of the document (XML,JSON,etc..)
	std::string docformatid() const;

	///\brief Get all input consumed (buffered)
	void getInputBuffer( void*& begin, std::size_t& nofBytes);

private:
	enum State
	{
		Init,
		ParseStart,
		ParseJSONHeaderStart,
		ParseJSONHeaderStringKey,
		ParseJSONHeaderSeekAssign,
		ParseJSONHeaderAssign,
		ParseJSONHeaderIdentKey,
		ParseJSONHeaderIdentValue,
		ParseJSONHeaderStringValue,
		ParseJSONHeaderSeekDelim,
		ParseXMLHeader0,
		ParseXMLHeader,
		SearchXMLDoctypeTag,
		ParseXMLDoctype0,
		ParseXMLDoctype1,
		ParseXMLDoctype2,
		ParseXMLDoctype,
		SkipXMLComment,
		Done
	};
	static const char* stateName( State st)
	{
		static const char* ar[] = {
			"Init",
			"ParseStart",
			"ParseJSONHeaderStart",
			"ParseJSONHeaderStringKey",
			"ParseJSONHeaderSeekAssign",
			"ParseJSONHeaderAssign",
			"ParseJSONHeaderIdentKey",
			"ParseJSONHeaderIdentValue",
			"ParseJSONHeaderStringValue",
			"ParseJSONHeaderSeekDelim",
			"ParseXMLHeader0",
			"ParseXMLHeader",
			"SearchXMLDoctypeTag",
			"ParseXMLDoctype0",
			"ParseXMLDoctype1",
			"ParseXMLDoctype2",
			"ParseXMLDoctype",
			"SkipXMLComment",
			"Done"};
		return ar[ (int)st];
	}

	void throw_error( const char* msg=0) const;
	void setState( State state_);

	enum Encoding
	{
		UCS1,UCS2BE,UCS2LE,UCS4BE,UCS4LE
	};
	bool getEncoding();
	unsigned char nextChar();

private:
	enum KeyType
	{
		KeyNone, KeyDoctype, KeyEncoding
	};
	State m_state;					//< processing state machine state
	Encoding m_encoding;				//< character set encoding
	KeyType m_keytype;				//< type of key parsed (JSON)
	char m_lastchar;				//< last character parsed
	char m_endbrk;					//< end character of a parsed token (string)
	bool m_escapestate;				//< substate for escaping (value depending on state)
	unsigned int m_itr;				//< iterator position
	unsigned int m_end;				//< end position
	const unsigned char* m_src;			//< current source base pointer
	protocol::InputBlock m_input;			//< input buffer
	protocol::CharBuffer m_inputbuffer;		//< buffer for consumed input (is returned to caller because this is a preprocessing command handler)
	std::string m_itembuf;				//< value item parsed (value depending on state)
	std::string m_keybuf;				//< key item parsed (value depending on state)
	std::string m_doctype;				//< document type extracted
	std::string m_doctypeid;			//< document type identifier extracted
	std::string m_docformatid;			//< document format type identifier
};
}}
#endif

