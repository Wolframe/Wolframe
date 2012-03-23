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
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
//
// echo processor
//

#ifndef _ECHO_PROCESSOR_HPP_INCLUDED
#define _ECHO_PROCESSOR_HPP_INCLUDED

#include "processor/processor.hpp"
#include "container.hpp"
#include "moduleInterface.hpp"

namespace _Wolframe {

class EchoProcessor : public proc::Processor
{
public:
	~EchoProcessor()				{}
	const char* typeName() const			{ return "EchoProcessor"; }
	void close()					{}

	void receiveData( const void* data, std::size_t size );
	const FSMoperation nextOperation();
	void signal( FSMsignal event );
	std::size_t dataLeft( const void*& begin );
};


class EchoProcessorUnit : public proc::ProcessorUnit
{
public:
	EchoProcessorUnit();
	~EchoProcessorUnit()				{}
	const char* typeName() const			{ return "EchoProcessor"; }
	proc::Processor* processor() const		{ return new EchoProcessor; }
};


class EchoProcConfig : public config::ObjectConfiguration
{
	friend class EchoProcContainer;
public:
	EchoProcConfig( const char* cfgName, const char* logParent, const char* logName )
		: config::ObjectConfiguration( cfgName, logParent, logName ) {}

	const char* objectName() const			{ return "EchoProcessor"; }

	/// methods
	bool parse( const config::ConfigurationTree& pt, const std::string& node,
		    const module::ModulesDirectory* modules );
	bool check() const;
	void print( std::ostream& os, size_t indent ) const;
	void setCanonicalPathes( const std::string& referencePath );
private:
	unsigned short	m_timeout;
};


class EchoProcContainer : public ObjectContainer< proc::ProcessorUnit >
{
public:
	EchoProcContainer( const EchoProcConfig& conf );
	~EchoProcContainer()				{}

	virtual const char* objectName() const		{ return m_proc->typeName(); }
	virtual proc::ProcessorUnit* object() const	{ return m_proc; }
	void dispose()					{ m_proc = NULL; delete this; }
private:
	EchoProcessorUnit*	m_proc;
};

} // namespace _Wolframe

#endif // _ECHO_PROCESSOR_HPP_INCLUDED
