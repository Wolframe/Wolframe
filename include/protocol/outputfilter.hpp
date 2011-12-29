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
#ifndef _Wolframe_PROTOCOL_OUTPUTFILTER_INTERFACE_HPP_INCLUDED
#define _Wolframe_PROTOCOL_OUTPUTFILTER_INTERFACE_HPP_INCLUDED
///\file protocol/outputfilter.hpp
///\brief Output interfaces for the application processor

#include <cstddef>
#include <string>
#include "countedReference.hpp"
#include "protocol/ioblocks.hpp"

namespace _Wolframe {
namespace protocol {

struct EndOfLineMarker
{
	static const char* value() {return "\r\n";}
	static unsigned int size() {return 2;}
};

///\class OutputFilter
///\brief Provides an interface for the application processor for its hierarchically structured output like XML.
///
/// The intention of this class is to get a thin binding of the scripting language
/// in the application layer to the network output.
///
struct OutputFilter :public OutputBlock
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

	static const char* elementTypeName( ElementType i)
	{
		static const char* ar[] = {"OpenTag","Attribute","Value","CloseTag"};
		return ar[(int)i];
	}

	///\brief Constructor
	OutputFilter()
		:OutputBlock(0,0),m_errorCode(0),m_state(Open){}
	OutputFilter( void* p, std::size_t n, std::size_t i=0)
		:OutputBlock(p,n,i),m_errorCode(0),m_state(Open){}
	OutputFilter( const OutputFilter& o)
		:OutputBlock(o),m_errorCode(o.m_errorCode),m_state(o.m_state){}

	virtual ~OutputFilter(){}

	///\brief self copy
	///\return copy of this
	virtual OutputFilter* copy() const=0;

	///\brief create the follow output filter for processing
	///\return the follow output filter
	///\remark this mechanism is used for chaining filters in case processing has to be changed
	virtual OutputFilter* createFollow() {return 0;}

	///\brief Assignement of the content processed only
	///\param [in] o output filter to assign the data members of
	void assignContent( const OutputFilter& o)
	{
		set( const_cast<void*>(o.ptr()), o.size());
		setPos( o.pos());
		m_state = o.m_state;
	}

	///\brief Defines the buffer to use for output.
	///\param [in] data pointer to buffer to use
	///\param [in] datasize allocation size of data in bytes
	///
	/// Initializes the structure without touching the output function itself
	void init( void* data, std::size_t datasize)
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
	void* cur() const				{return (void*)(charptr()+pos());}

	///\brief Print the next element to the buffer
	///\param [in] type type of element to print
	///\param [in] element content of element to print
	///\param [in] elementsize size of element to print in bytes
	///\return true, on success, false, if failed
	virtual bool print( ElementType type, const void* element, std::size_t elementsize)=0;

	///\brief Print the next element to the buffer
	///\param [in] type type of element to print
	///\param [in] element content of element to print
	///\return true, on success, false, if failed
	bool print( ElementType type, const std::string& element)
	{
		return print( type, element.c_str(), element.size());
	}

	///\brief Get error code in case of error state
	///\return the error code
	int getError() const								{return m_errorCode;}

	///\brief Get the last error, if the filter got into an error state
	///\return the last error as string or 0
	virtual const char* getLastError() const					{return m_errorCode?"unknown":0;}

	///\brief Get a member value of the filter
	///\param [in] name case sensitive name of the variable
	///\param [in] val buffer for the value returned
	///\return true on success, false, if the variable does not exist or the operation failed
	virtual bool getValue( const char* /*name*/, std::string& /*val*/)		{return false;}

	///\brief Set a member value of the filter
	///\param [in] name case sensitive name of the variable
	///\param [in] val new value of the variable to set
	///\return true on success, false, if the variable does not exist or the operation failed
	virtual bool setValue( const char* /*name*/, const std::string& /*val*/)	{return false;}

	///\brief Set output filter state with error code
	///\param [in] s new state
	///\param [in] e (optional) error code to set
	void setState( State s, int e=0)
	{
		m_state = s;
		m_errorCode = e;
	}

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

///\typedef OutputFilterR
///\brief Shared output filter reference
typedef CountedReference<OutputFilter> OutputFilterR;

}}//namespace
#endif

