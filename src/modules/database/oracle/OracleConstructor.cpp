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
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
//
// Oracle constructor
//

#include "Oracle.hpp"
#include "logger-v1.hpp"

namespace _Wolframe {
namespace db {

OracleDbUnit* OracleConstructor::object( const config::NamedConfiguration& conf )
{
	const OracleConfig& cfg = dynamic_cast< const OracleConfig& >( conf );

	OracleDbUnit* m_db = new OracleDbUnit( cfg.m_ID, cfg.host(), cfg.port(), cfg.dbName(),
						       cfg.user(), cfg.password(),
						       cfg.sslMode, cfg.sslCert, cfg.sslKey,
						       cfg.sslRootCert, cfg.sslCRL,
						       cfg.connectTimeout,
						       cfg.connections, cfg.acquireTimeout,
						       cfg.statementTimeout,
						       cfg.programFiles());
	MOD_LOG_TRACE << "Oracle database unit for '" << cfg.m_ID << "' created";
	return m_db;
}

}} // namespace _Wolframe::db
