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
/// \file simpleFormLexer.hpp
/// \brief Interface for lexical analysis for the simple form DDL compiler 

#ifndef _Wolframe_DDL_SIMPLEFORM_LEXER_HPP_INCLUDED
#define _Wolframe_DDL_SIMPLEFORM_LEXER_HPP_INCLUDED
#include <string>
#include "types/form.hpp"
#include "utils/parseUtils.hpp"
#include "utils/sourceLineInfo.hpp"
#include "langbind/ddlCompilerInterface.hpp"

namespace _Wolframe {
namespace langbind {
namespace simpleform {

/// \class Lexem
/// \brief Single token in simpleform source with position information
class Lexem
{
public:
	/// \enum Id
	/// \brief Token identifier enumeration
	enum Id
	{
		EndOfFile,	///< end of source file
		INCLUDE,	///< INCLUDE keyword
		FORM,		///< FORM keyword
		STRUCT,		///< STRUCT keyword
		Identifier,	///< identifier
		String,		///< string
		OpenStruct,	///< '{'
		CloseStruct,	///< '}'
		ArrayTag,	///< '[]'
		AttributeTag,	///< '@'
		MandatoryTag,	///< '!'
		OptionalTag,	///< '?'
		Indirection,	///< '^'
		Assign,		///< '='
		MetaDataDef,	///< ':'
		Separator	///< ','
	};
public:
	/// \brief Default constructor
	Lexem()
		:m_id(EndOfFile){}
	/// \brief Constructor
	Lexem( utils::SourceLineInfo position_, Id id_, const std::string& value_=std::string())
		:m_id(id_),m_value(value_),m_position(position_){}
	/// \brief Copy constructor
	Lexem( const Lexem& o)
		:m_id(o.m_id),m_value(o.m_value),m_position(o.m_position){}

	Id id() const				{return m_id;}
	std::string value() const		{return m_value;}
	utils::SourceLineInfo position() const	{return m_position;}
	std::string positionLogtext() const;

private:
	Id m_id;				///< lexem identifier
	std::string m_value;			///< lexem value
	utils::SourceLineInfo m_position;	///< lexem position
};

/// \brief Structure for lexical analysis of one simple form source file
/// \class Lexer
class Lexer
{
public:
	/// \brief Constructor
	Lexer( const std::string::const_iterator& begin, const std::string::const_iterator& end)
		:m_start(begin),m_itr(begin),m_end(end),m_itr_position(begin),m_tokch(0){}
	/// \brief Copy constructor
	Lexer( const Lexer& o)
		:m_start(o.m_start),m_itr(o.m_itr),m_end(o.m_end),m_position(o.m_position),m_itr_position(o.m_itr_position),m_tokch(o.m_tokch){}

	/// \brief Fetch the next lexem (that is not a comment) in the source
	/// \param[in] eolnAsSeparator true, if end of line (CRLF or LF) should be returned as 'Separator' ~ separator of atomic element definitions
	/// \return the lexem
	Lexem next( bool eolnAsSeparator);
	/// \brief Same as 'next(..)' but returns identifier for the keywords 'FORM' and 'STRUCT'
	/// \return the lexem
	Lexem next2( bool eolnAsSeparator);
	/// \brief Get a description of the current token for error messages
	/// \return the token description
	std::string curtoken() const;

	/// \brief Get the current position
	/// \return the position
	utils::SourceLineInfo position() const	{return m_position;}

private:
	std::string::const_iterator m_start;		///< source start
	std::string::const_iterator m_itr;		///< iterator on source
	std::string::const_iterator m_end;		///< end of source
	utils::SourceLineInfo m_position;		///< current position in source
	std::string::const_iterator m_itr_position;	///< iterator from what m_position has been calculated the last time
	char m_tokch;					///< token character of the current token
};

}}}//namespace
#endif
