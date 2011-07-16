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
// Wolframe processor group
//

#ifndef _WOLFRAME_PROCESSOR_GROUP_HPP_INCLUDED
#define _WOLFRAME_PROCESSOR_GROUP_HPP_INCLUDED

#include "config/configurationBase.hpp"
#include "moduleInterface.hpp"
#include "WolframeProcContainer.hpp"
#include "database/database.hpp"
#include "database/DBprovider.hpp"

#include <list>

namespace _Wolframe {
namespace proc {

class ProcessorGroupConfig : public config::ConfigurationBase
{
	friend class ProcessorGroup;
	friend class config::ConfigurationParser;
public:
	/// constructor & destructor
	ProcessorGroupConfig()
		: ConfigurationBase( "Processor(s)", NULL, "Processor configuration" )	{}
	~ProcessorGroupConfig();

	/// methods
	bool check() const;
	void print( std::ostream& os, size_t indent ) const;
	virtual void setCanonicalPathes( const std::string& referencePath );
private:
	std::string					m_dbLabel;
	std::list< config::ContainerConfiguration* >	m_procConfig;
};


class ProcessorGroup
{
public:
	ProcessorGroup( const ProcessorGroupConfig& conf );
	~ProcessorGroup();

	bool resolveDB( db::DatabaseProvider& db );

	const ProcessorChannel* procChannel() const;
private:
	std::string				m_dbLabel;
	const db::Database*			m_db;
	std::list<WolframeProcContainer*>	m_proc;
};

}} // namespace _Wolframe::proc

#endif // _WOLFRAME_PROCESSOR_GROUP_HPP_INCLUDED
