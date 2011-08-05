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
///
///\file appObjects.hpp
///\brief interface for application processor scripting language to system objects
///
#ifndef _Wolframe_LANGBIND_HPP_INCLUDED
#define _Wolframe_LANGBIND_HPP_INCLUDED
#include "protocol/formatoutput.hpp"
#include "protocol/inputfilter.hpp"
#include <stack>
#include <string>

namespace _Wolframe {
namespace app {

///\class Output
///\brief Output as seen from the application processor program
struct Output
{
	///\enum ItemType
	///\brief Output state
	enum ItemType
	{
		Data,		///< normal processing
		DoYield,	///< yield because rest of buffer not sufficient to complete operation
		Error		///< logic error in output. Operation is not possible
	};
	///\brief Constructor
	Output() :m_state(0){}
	///\brief Copy constructor
	///\param[in] o copied item
	Output( const Output& o) :m_formatoutput(o.m_formatoutput),m_state(0){}
	///\brief Destructor
	~Output(){}

	///\brief Print the next element
	///\param[in] e1 first element
	///\param[in] e1size first element size
	///\param[in] e2 second element
	///\param[in] e2size second element size
	///\param[in] newline true, if the printed item should start on a new line (for splitting lines to serve line buffering clients)
	///\return state returned
	ItemType print( const char* e1, unsigned int e1size, const char* e2, unsigned int e2size, bool newline);

public:
	protocol::FormatOutputR m_formatoutput;	///< format output reference

private:
	unsigned int m_state;						///< current state for outputs with more than one elements
};

///\class System
///\brief System function call interface as seen from the application processor program
struct System
{
	///\brief Constructor
	System(){}
	///\brief Copy constructor
	///\param[in] o copied item
	System( const System&){}
	///\brief Destructor
	~System(){}

	///\brief Create a new input filter function
	///\param[in] name name of the filter or the default filter if not specified
	///\param[in] buffersize size of buffer in bytes to use for the generated elements
	protocol::InputFilter* createInputFilter( const char* name, unsigned int buffersize=0) const;

	///\brief Create a new input filter function from the default
	protocol::InputFilter* createDefaultInputFilter() const;

	///\brief Create a new format output filter function
	///\param[in] name name of the filter or the default filter if not specified
	///\param[in] buffersize size of buffer in bytes to use for the tag hierarchy context
	protocol::FormatOutput* createFormatOutput( const char* name, unsigned int buffersize=0) const;

	///\brief Create a new format output filter function from the default
	protocol::FormatOutput* createDefaultFormatOutput() const;
};

///\class Input
///\brief input as seen from the application processor program
struct Input
{
	protocol::InputFilterR m_inputfilter;	///< input is defined by the associated input filter

	///\brief Constructor
	Input(){}
	///\brief Copy constructor
	///\param[in] o copied item
	Input( const Input& o) :m_inputfilter(o.m_inputfilter){}
	///\brief Destructor
	~Input(){}
};

///\class Filter
///\brief input/output filter as seen from the application processor program
struct Filter
{
	protocol::FormatOutputR m_formatoutput;		///< format output
	protocol::InputFilterR m_inputfilter;		///< input filter

	///\brief Constructor
	///\param[in] system reference to system function call interface
	///\param[in] name name of the filter as defined in the system
	///\param[in] ib size of buffer used for input
	///\param[in] ob size of buffer used for output
	Filter( System* system, const char* name, unsigned int ib, unsigned int ob)
		:m_formatoutput(system->createFormatOutput(name,ob))
		,m_inputfilter(system->createInputFilter(name,ib)){}

	///\brief Copy constructor
	///\param[in] o copied item
	Filter( const Filter& o)
		:m_formatoutput(o.m_formatoutput)
		,m_inputfilter(o.m_inputfilter){}
	///\brief Destructor
	~Filter(){}
};

///\class InputFilterClosure
///\brief Closure for the input iterator (in Lua returned by 'input.get()')
class InputFilterClosure
{
public:
	///\enum ItemType
	///\brief Input loop state
	enum ItemType
	{
		EndOfData,	///< End of processed content reached
		Data,		///< normal processing, loop can continue
		DoYield,	///< have to yield and request more network input
		Error		///< have to stop processing because of an error
	};

	///\brief Constructor
	///\param[in] ig input filter reference from input
	InputFilterClosure( const protocol::InputFilterR& ig)
		:m_inputfilter(ig)
		,m_type(protocol::InputFilter::Value)
		,m_value(0)
		,m_buf(0)
		,m_bufsize(ig->getGenBufferSize())
		,m_bufpos(0)
		,m_taglevel(0)
	{
		m_buf = new char[ m_bufsize];
	}

	///\brief Internal buffer reset
	void init()
	{
		m_bufpos = 0;
		m_value = 0;
	}

	///\brief Get the next pair of elements
	///\param[out] e1 first element
	///\param[out] e1size first element size
	///\param[out] e2 second element
	///\param[out] e2size second element size
	///\return state returned
	ItemType fetch( const char*& e1, unsigned int& e1size, const char*& e2, unsigned int& e2size);

private:
	protocol::InputFilterR m_inputfilter;			///< rerefence to input with filter
	protocol::InputFilter::ElementType m_type;		///< current state (last value type parsed)
	char* m_value;						///< pointer to local copy of value in m_buf
	char* m_buf;						///< pointer to buffer for local copies of returned values
	std::size_t m_bufsize;					///< allocation size of m_buf
	std::size_t m_bufpos;					///< number of bytes filled in m_buf
	std::size_t m_taglevel;					///< current level in tag hierarchy
};

}}//namespace
#endif

