/************************************************************************
Copyright (C) 2011 Project Wolframe.
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
#ifndef _Wolframe_PROTOCOL_FORMATOUTPUT_INTERFACE_HPP_INCLUDED
#define _Wolframe_PROTOCOL_FORMATOUTPUT_INTERFACE_HPP_INCLUDED
///\file protocol/formatoutput.hpp
///\brief Output interfaces for the application processor

#include <cstddef>
#include "countedReference.hpp"
#include "protocol/ioblocks.hpp"

namespace _Wolframe {
namespace protocol {

///\class FormatOutput
///\brief Provides an interface for the application processor for its hierarchically structured output like XML.
///
/// The intention of this class is to get a thin binding of the scripting language
/// in the application layer to the network output.
///
struct FormatOutput :public OutputBlock
{
	///\enum State
	///\brief State of the output used in the application processor iterating loop to decide wheter to yield execution or not.
	enum State
	{
		Open,		///< serving data - normal input processing
		EndOfBuffer,	///< have to yield processing because the print buffer size is not sufficient
		Error		///< have to stop processing with an error
	};

	///\enum ElementType
	///\brief Hierarchical ouput structure element type
	///
	/// Describes the role of the element in the output structure.
	/// This information is interpreted by the engine to create the correct output string
	/// like '&lt;/name&gt;' for 'name' as CloseTag in XML output.
	///
	enum ElementType
	{
		OpenTag,	///< Open a new hierarchy level (Tag,Property Subtree, etc.)
		Attribute,	///< An attribute Name that will be immediately followed by an attribute value as ElementType::Value
		Value,		///< An attribute value if the preceding element was an ElementType::Attribute, a content element else
		CloseTag	///< Close current hierarchy level
	};

	///\brief Constructor
	FormatOutput()
		:OutputBlock(0,0),m_errorCode(0),m_state(Open){}

	virtual ~FormatOutput(){}

	///\brief self copy
	///\return copy of this
	virtual FormatOutput* copy() const=0;

	///\brief Assignement of the data members
	///\param [in] o format output to assign the data members of
	///\remark the output function specified with the constructor is not overwritten
	FormatOutput& operator = (const FormatOutput& o)
	{
		set( const_cast<void*>(o.ptr()), o.size());
		setPos( o.pos());
		m_errorCode = o.m_errorCode;
		m_state = o.m_state;
		return *this;
	}

	///\brief Defines the buffer to use for output.
	///\param [in] data pointer to buffer to use
	///\param [in] datasize allocation size of data in bytes
	///
	/// Initializes the structure without touching the output function itself
	void init( void* data, size_type datasize)
	{
		set( data, datasize);
		m_errorCode = 0;
		m_state = Open;
	}

	///\brief Empty initialization to force a yield execution
	/// The forced yield execution signals the connectionHandler that it should define an output buffer for the application processor for printing.
	void init()
	{
		init( 0, 0);
	}

	///\brief Get the current cursor position as pointer for the next print
	///\return the current cursor position (where to print the next element to)
	void* cur() const			{return (void*)(charptr()+pos());}

	///\brief Print the next element to the buffer
	///\param [in] type type of element to print
	///\param [in] element content of element to print
	///\param [in] elementsize size of element to print in bytes
	///\return true, on success, false, if failed
	///
	virtual bool print( ElementType type, const void* element, size_type elementsize)=0;

	///\brief Get error code in case of error state
	///\return the error code
	int getError() const			{return m_errorCode;}

	///\brief Set format output state with error code
	///\param [in] s new state
	///\param [in] e (optional) error code to set
	void setState( State s, int e=0)		{m_state=s;m_errorCode=e;}

	///\brief Return the current state
	///\return the current state
	State state() const
	{
		return m_state;
	}

private:
	int m_errorCode;	///< error code
	State m_state;		///< state
};

///\typedef FormatOutputR
///\brief Shared format output reference
typedef CountedReference<FormatOutput> FormatOutputR;

}}//namespace
#endif

