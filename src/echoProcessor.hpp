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

#include "WolframeProcContainer.hpp"
#include "moduleInterface.hpp"

namespace _Wolframe {

class EchoProcConfig : public module::ModuleConfiguration< EchoProcConfig, config::ContainerConfiguration >
{
	friend class EchoProcContainer;
	friend class config::ConfigurationParser;
public:
	EchoProcConfig( const char* cfgName, const char* logParent, const char* logName )
		: module::ModuleConfiguration< EchoProcConfig, config::ContainerConfiguration >( cfgName, logParent, logName ) {}

	const char* typeName() const			{ return "EchoProcessor"; }

	/// methods
	bool check() const;
	void print( std::ostream& os, size_t indent ) const;
	void setCanonicalPathes( const std::string& referencePath );
private:
	unsigned short	m_timeout;
};


class EchoProcContainer : public module::ModuleContainer< EchoProcContainer, EchoProcConfig,
		WolframeProcContainer, config::ContainerConfiguration >
{
public:
	EchoProcContainer( const EchoProcConfig& conf );
	~EchoProcContainer()				{}

	virtual const char* typeName() const		{ return "EchoProcessor"; }
//	virtual const WolframeProcessor& processor()	{ return NULL; }

};

} // namespace _Wolframe

#endif // _ECHO_PROCESSOR_HPP_INCLUDED
