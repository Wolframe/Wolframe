/*
---------------------------------------------------------------------
    The template library textwolf implements an input iterator on
    a set of XML path expressions without backward references on an
    STL conforming input iterator as source. It does no buffering
    or read ahead and is dedicated for stream processing of XML
    for a small set of XML queries.
    Stream processing in this context refers to processing the
    document without buffering anything but the current result token
    processed with its tag hierarchy information.

    Copyright (C) 2010,2011,2012,2013,2014 Patrick Frey

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 3.0 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

--------------------------------------------------------------------

	The latest version of textwolf can be found at 'http://github.com/patrickfrey/textwolf'
	For documentation see 'http://patrickfrey.github.com/textwolf'

--------------------------------------------------------------------
*/
/// \file textwolf/exception.hpp
/// \brief Definition of exceptions with containing error codes thrown by textwolf

#ifndef __TEXTWOLF_EXCEPTION_HPP__
#define __TEXTWOLF_EXCEPTION_HPP__
#include <exception>
#include <stdexcept>

namespace textwolf {

/// \class throws_exception
/// \brief Base class for structures that can throw exceptions for non recoverable errors
struct throws_exception
{
	/// \enum Cause
	/// \brief Enumeration of error cases
	enum Cause
	{
		Unknown,			///< uknown error
		DimOutOfRange,			///< memory reserved for statically allocated table or memory block is too small. Increase the size of memory block passed to the XML path select automaton. Usage error !
		StateNumbersNotAscending,	///< XML scanner automaton definition check failed. Labels of states must be equal to their indices. Internal textwold error !
		InvalidParamState,		///< parameter check (for state) in automaton definition failed. Internal textwold error !
		InvalidParamChar,		///< parameter check (for control character) in automaton definition failed. Internal textwold error !
		DuplicateStateTransition,	///< duplicate transition definition in automaton. Internal textwold error !
		InvalidState,			///< invalid state definition in automaton. Internal textwold error !
		IllegalParam,			///< parameter check in automaton definition failed. Internal textwold error !
		IllegalAttributeName,		///< invalid string for a tag or attribute in the automaton definition. Usage error !
		OutOfMem,			///< out of memory in the automaton definition. System error (std::bad_alloc) !
		ArrayBoundsReadWrite,		///< invalid array access. Internal textwold error !
		NotAllowedOperation		///< defining an operation in an automaton definition that is not allowed there. Usage error !
	};
};

/// \class exception
/// \brief textwolf exception class
struct exception	:public std::runtime_error
{
	typedef throws_exception::Cause Cause;
	Cause cause;					//< exception cause tag

	/// \brief Constructor
	/// \return exception object
	exception (Cause p_cause) throw()
		:std::runtime_error("textwolf error in XML"), cause(p_cause) {}
	/// \brief Copy constructor
	exception (const exception& orig) throw()
		:std::runtime_error("textwolf error in XML"), cause(orig.cause) {}
	/// \brief Destructor
	virtual ~exception() throw() {}

	/// \brief Assignement
	/// \param[in] orig exception to copy
	/// \return *this
	exception& operator= (const exception& orig) throw()
			{cause=orig.cause; return *this;}

	/// \brief Exception message
	/// \return exception cause as string
	virtual const char* what() const throw()
	{
		// enumeration of exception causes as strings
		static const char* nameCause[ 12] = {
			"Unknown","DimOutOfRange","StateNumbersNotAscending","InvalidParamState",
			"InvalidParamChar","DuplicateStateTransition","InvalidState","IllegalParam",
			"IllegalAttributeName","OutOfMem","ArrayBoundsReadWrite","NotAllowedOperation"
		};
		return nameCause[ (unsigned int) cause];
	}
};

}//namespace
#endif
