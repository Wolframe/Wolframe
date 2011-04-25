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
/// \file langbind.hpp
/// \brief interface for application processor scripting language to system objects
///
#ifndef _Wolframe_LANGBIND_HPP_INCLUDED
#define _Wolframe_LANGBIND_HPP_INCLUDED
#include "protocol/formatoutput.hpp"
#include "protocol/generator.hpp"
#include <boost/shared_ptr.hpp>
#include <stack>
#include <string>

namespace _Wolframe {
namespace app {

struct Output
{
	enum ItemType {Data,DoYield,Error};

	Output() :m_state(0){}
	Output( const Output& o) :m_formatoutput(o.m_formatoutput),m_state(0){}
	~Output(){}

	/// \brief print the next element
	/// \param[in] e1 first element
	/// \param[in] e1size first element size
	/// \param[in] e2 second element
	/// \param[in] e2size second element size
	/// \return state returned
	ItemType print( const char* e1, unsigned int e1size, const char* e2, unsigned int e2size);

public:
	boost::shared_ptr<protocol::FormatOutput> m_formatoutput;	///< format output reference
private:
	unsigned int m_state;						///< current state for outputs with more than one elements
	std::stack<std::string> m_opentags;				///< stack of open tags
};

struct System
{
	System(){}
	virtual ~System(){}

	virtual protocol::Generator* createGenerator( const char* name=0) const;
	virtual protocol::FormatOutput* createFormatOutput( const char* name=0) const;
};

struct Input
{
	boost::shared_ptr<protocol::Generator> m_generator;

	Input(){};
	Input( const Input& o) :m_generator(o.m_generator){}
	~Input(){};
};

struct Filter
{
	boost::shared_ptr<protocol::FormatOutput> m_formatoutput;
	boost::shared_ptr<protocol::Generator> m_generator;

	Filter( System* system, const char* name)
		:m_formatoutput(system->createFormatOutput(name))
		,m_generator(system->createGenerator(name)){}

	Filter( const Filter& o)
		:m_formatoutput(o.m_formatoutput)
		,m_generator(o.m_generator){}
	~Filter(){};
};

class InputGeneratorClosure
{
public:
	enum ItemType {EndOfData,Data,DoYield,Error};

	InputGeneratorClosure( const boost::shared_ptr<protocol::Generator>& ig)
		:m_generator(ig),m_type(protocol::Generator::Value),m_value(0),m_buf(0),m_bufsize(0),m_bufpos(0){}

	void init()
	{
		m_bufpos = 0;
		m_value = 0;
	}

	/// \brief get the next pair of elements
	/// \param[out] e1 first element
	/// \param[out] e1size first element size
	/// \param[out] e2 second element
	/// \param[out] e2size second element size
	/// \return state returned
	ItemType fetch( const char*& e1, unsigned int& e1size, const char*& e2, unsigned int& e2size);

private:
	boost::shared_ptr<protocol::Generator> m_generator;	///< rerefence to input with filter
	protocol::Generator::ElementType m_type;		///< current state (last value type parsed)
	char* m_value;						///< pointer to local copy of value in m_buf
	char* m_buf;						///< pointer to buffer for local copies of returned values
	std::size_t m_bufsize;					///< allocation size of m_buf
	std::size_t m_bufpos;					///< number of bytes filled in m_buf
};

}}//namespace
#endif

