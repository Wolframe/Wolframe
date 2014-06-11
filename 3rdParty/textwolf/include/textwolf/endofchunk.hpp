/*
---------------------------------------------------------------------
    The template library textwolf implements an input iterator on
    a set of XML path expressions without backward references on an
    STL conforming input iterator as source. It does no buffering
    or read ahead and is dedicated for stream processing of XML
    for a small set of XML queries.
    Stream processing in this Object refers to processing the
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
///\file textwolf/endofchunk.hpp
///\brief Mechanism for triggering an end of chunk event for the caller in the top level context, so that he han feed the parser with more data

#ifndef __TEXTWOLF_END_OF_CHUNK_HPP__
#define __TEXTWOLF_END_OF_CHUNK_HPP__
#include <setjmp.h>
#include <cstring>

///\namespace textwolf
///\brief Toplevel namespace of the library
namespace textwolf {

///\class EndOfChunkTrigger
///\brief Context for triggering an end of chunk event for the caller in the top level context
class EndOfChunkTrigger
{
public:
	EndOfChunkTrigger()
		{std::memset( &m_env, 0, sizeof(m_env));}

	///\brief Test if the end of chunk event has been triggered or not
	///\return true, if yes, false if not
	bool set()
	{
		m_set = true;
		return (setjmp(m_env) != 0);
	}

	///\brief Jump back into the callers context without cleaning up
	///\remark Long jump assumes that there are no data to be cleanup up in the context up the top level callers context
	void go()
	{
		if (m_set)
		{
			m_set = false;
			longjmp( m_env, 1);
		}
	}

private:
	jmp_buf m_env;					//< top level caller context
	bool m_set;					//< true, if context has been initialized

	EndOfChunkTrigger( const EndOfChunkTrigger&){}	//... non copyable
	void operator=( const EndOfChunkTrigger&){}	//... non copyable
};

}//namespace
#endif

