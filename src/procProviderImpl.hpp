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

#include "processor/procProvider.hpp"
#include "container.hpp"
#include "database/database.hpp"
#include "database/DBprovider.hpp"

#include <list>

namespace _Wolframe {
namespace proc {

// Standard authentication class and authentication provider
class StandardProcessor : public Processor
{
public:
	StandardProcessor();
	~StandardProcessor();
	void close();

	// From the FSM interface
	void receiveData( const void* data, std::size_t size );
	const FSMoperation nextOperation();
	void signal( FSMsignal event );
	std::size_t dataLeft( const void*& begin );
private:
};


class ProcessorProvider::ProcessorProvider_Impl
{
public:
	ProcessorProvider_Impl( const ProcProviderConfig* conf,
				const module::ModulesDirectory* modules);
	~ProcessorProvider_Impl();

	bool resolveDB( const db::DatabaseProvider& db );

	Processor* processor();
private:
	std::string			m_dbLabel;
	const db::Database*		m_db;
	std::list< ProcessorUnit* >	m_proc;
};

}} // namespace _Wolframe::proc

#endif // _WOLFRAME_PROCESSOR_GROUP_HPP_INCLUDED
