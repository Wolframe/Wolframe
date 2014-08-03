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
/// \file langbind/output.hpp
/// \brief Interface to network output for processor language bindings
#ifndef _Wolframe_langbind_OUTPUT_HPP_INCLUDED
#define _Wolframe_langbind_OUTPUT_HPP_INCLUDED
#include "filter/filter.hpp"

namespace _Wolframe {
namespace langbind {

/// \class Output
/// \brief Output as seen from scripting language binding
class Output
{
public:
	/// \brief Constructor
	Output()
		:m_outputChunkSize(0)
		,m_state(0)
		,m_called(false){}
	/// \brief Copy constructor
	/// \param[in] o copied item
	Output( const Output& o)
		:m_outputfilter(o.m_outputfilter)
		,m_outputChunkSize(o.m_outputChunkSize)
		,m_state(o.m_state)
		,m_called(o.m_called){}
	/// \brief Constructor by output filter
	/// \param[in] flt output filter reference
	Output( const OutputFilterR& flt)
		:m_outputfilter(flt)
		,m_outputChunkSize(0)
		,m_state(0)
		,m_called(false){}
	/// \brief Destructor
	~Output(){}

	/// \brief Print the next element
	/// \param[in] e1 first element
	/// \param[in] e1size first element size
	/// \param[in] e2 second element
	/// \param[in] e2size second element size
	/// \return true, on success, false if we have to yield processing
	bool print( const char* e1, unsigned int e1size, const char* e2, unsigned int e2size);

	const OutputFilterR& outputfilter() const		{return m_outputfilter;}
	OutputFilterR& outputfilter()				{return m_outputfilter;}
	void setOutputFilter( const OutputFilterR& filter);

	bool called() const					{return m_called;}
	void called( bool yes)					{m_called=yes;}

	void setOutputChunkSize( unsigned int outputChunkSize_)
	{
		m_outputChunkSize = outputChunkSize_;
	}

	unsigned int outputChunkSize() const
	{
		return m_outputChunkSize;
	}

private:
	OutputFilterR m_outputfilter;				///< output filter reference
	std::size_t m_outputChunkSize;				///< chunk size for output
	unsigned int m_state;					///< current state for outputs with more than one elements
	bool m_called;						///< has already been called
};

}}//namespace
#endif
