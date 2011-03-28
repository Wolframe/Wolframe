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
/// \file protocol/formatoutput.hpp
/// \brief Output interfaces for the application processor

#include <cstddef>

namespace _Wolframe {
namespace protocol {

/// \class FormatOutput
/// \brief Provides an interface for the application processor for its hierarchically structured output like XML.
///
/// The intention of this class is to get a thin binding of the scripting language
/// in the application layer to the network output.
///
struct FormatOutput
{
	typedef std::size_t size_type;	///< size_type of the output vector

	/// \enum ElementType
	/// \brief Hierarchical ouput structure element type
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

	/// \related FormatOutput
	/// \brief Print next element call function type
	/// \param [in] type type of element to print
	/// \param [in] element content of element to print
	/// \param [in] elementsize size of element to print in bytes
	/// \return true, on success, false, if failed
	///
	/// The output function is not a method, because it should also be possible to write an ANSI C implmentation of an output function with related data.
	typedef bool (*Print)( FormatOutput* this_, ElementType type, void* element, size_type elementsize);

	/// \brief Constructor
	/// \param [in] op print function pointer
	FormatOutput( const Print& op) :m_ptr(0),m_pos(0),m_size(0),m_print(op){}

	/// \brief Assignement of the data members
	/// \param [in] o format output to assign the data members of
	/// \remark the output function specified with the constructor is not overwritten
	FormatOutput& operator = (const FormatOutput& o)
	{
		m_ptr = o.m_ptr;
		m_pos = o.m_pos;
		m_size = o.m_size;
		return *this;
	}

	/// \brief Defines the buffer to use for output.
	/// \param [in] data pointe to buffer to use
	/// \param [in] datasize allocation size of data in bytes
	///
	/// Initializes the structure without touching the output function itself
	void init( void* data, size_type datasize)
	{
		m_ptr = data;
		m_size = datasize;
		m_pos = 0;
	}

	/// \brief Empty initialization to force a yield execution
	/// The forced yield execution signals the connectionHandler that it should define an output buffer for the application processor for printing.
	void init()
	{
		init( 0, 0);
	}

	/// \brief Get the current cursor position as pointer for the next print
	void* cur() const			{return (void*)((char*)m_ptr+m_pos);}
	/// \brief Get the size of the buffer left for printing the next element
	size_type restsize() const		{return (m_pos<m_size)?(m_size-m_pos):0;}
	/// \brief Get the current cursor position
	size_type pos() const			{return m_pos;}
	/// \brief Get the allocation size of the buffer
	size_type size() const			{return m_size;}
	/// \brief Get the pointer to the start of the buffer
	void* ptr() const			{return m_ptr;}
	/// \brief Shift current cursor poition by some bytes
	/// \param [in] n number of bytes to shift
	void incr( size_type n)			{if ((m_pos+n)>=m_size) m_pos=m_size; else m_pos+=n;}

	/// \brief Print the next element to the buffer
	/// \param [in] type type of element to print
	/// \param [in] element content of element to print
	/// \param [in] elementsize size of element to print in bytes
	/// \return true, on success, false, if failed
	bool print( ElementType type, void* element, size_type elementsize)
	{
		return m_print( this, type, element, elementsize);
	}

private:
	void* m_ptr;		///< pointer to print buffer
	size_type m_pos;	///< current cursor position in print buffer
	size_type m_size;	///< size of print buffer in bytes
	Print m_print;		///< pointer to print function implementation
};

}}//namespace
#endif

