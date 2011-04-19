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

namespace _Wolframe {
namespace mtproc {

struct Input
{
	boost::shared_ptr<protocol::Generator> m_generator;

	Input(){};
	Input( const Input& o) :m_generator(o.m_generator){}
	~Input(){};
};

struct Output
{
	boost::shared_ptr<protocol::FormatOutput> m_formatoutput;

	Output(){};
	Output( const Output& o) :m_formatoutput(o.m_formatoutput){}
	~Output(){};
};

struct System
{
	System(){}
	virtual ~System(){}

	virtual protocol::Generator* createGenerator( const char* name) const;
	virtual protocol::FormatOutput* createFormatOutput( const char* name) const;
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

class GeneratorClosure
{
public:
	enum ItemType {EndOfData,Data,DoYield,Error};

	GeneratorClosure() :m_type(protocol::Generator::Value),m_value(0),m_buf(0),m_bufsize(0),m_bufpos(0){}

	void init()
	{
		m_bufpos = 0;
		m_value = 0;
	}

	ItemType fetch( const char*& e1, const char*& e2);

private:
	boost::shared_ptr<protocol::Generator> m_generator;
	protocol::Generator::ElementType m_type;
	char* m_value;
	char* m_buf;
	std::size_t m_bufsize;
	std::size_t m_bufpos;
};

}}//namespace
#endif

