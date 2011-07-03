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

#include <list>

namespace _Wolframe {

class WolframeProcGroupConfig : public config::ConfigurationBase
{
	friend class WolframeProcGroup;
	friend class config::ConfigurationParser;
public:
	std::list<WolframeProcConfig*>	m_procConfig;

	/// constructor & destructor
	WolframeProcGroupConfig()
		: ConfigurationBase( "Processor(s)", NULL, "Processor configuration" )	{}
	~WolframeProcGroupConfig();

	/// methods
	bool check() const;
	void print( std::ostream& os, size_t indent ) const;
	virtual void setCanonicalPathes( const std::string& referencePath );
};


class WolframeProcGroup
{
public:
	WolframeProcGroup( const WolframeProcGroupConfig& conf );
	~WolframeProcGroup();

	const WolframeProcessorChannel* procChannel() const;
private:
	std::list<WolframeProcContainer*>	m_proc;
};

} // namespace _Wolframe

#endif // _WOLFRAME_PROCESSOR_GROUP_HPP_INCLUDED
