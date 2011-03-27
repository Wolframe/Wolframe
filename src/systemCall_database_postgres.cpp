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
#include "systemCall.hpp"
#include "logger.hpp"
#include <postgresql/libpq-fe.h>

using namespace _Wolframe::syscall;

struct Connector::Data
{
	PGconn* m_conn;

	Data() :m_conn(0)
	~Data()
	{
		if (m_conn) PQfinish(m_conn);
	}
};

virtual ~Connector()
{
	if (data) delete data;
}

int Connector::open( const boost::property_tree::ptree& cfg, unsigned int idx)
{
	if (data) delete data;
	data = new data;
	data->m_conn = PQconnectdb("host=127.0.0.1 user=test password=test dbname=test");
	if (!conn || PQstatus( conn) != CONNECTION_OK)
	{
		LOG_ERROR << "connection to postgres database failed";
		return (int)PQstatus( conn);
	}
	return 0;
}

int Connector::call( const Function& fun, Result& result)
{
	std::string cmd;
	data->init();
	result.clear();

	try
	{
		unsigned int ii;
		cmd.append( "SELECT ");
		cmd.append( fun.m_name);
		cmd.append( "(");
		for (ii=0; ii<fun.m_param.size(); ii++)
		{
			if (ii > 0) cmd.append( ",");
			if (fun.m_param[ii].m_name)
			{
				cmd.append( fun.m_name);
				cmd.append( " := ");
			}
			cmd.append( fun.m_param[ii].m_value);
		}
		cmd.append( ");");
	}
	catch (std::bad_alloc)
	{
		LOG_ERROR << "out of memory building database command";
		return -1;
	}
	PGresult* res = PQexec( data->m_conn, cmd);
	if (!res)
	{
		LOG_ERROR << "out of memory executing database command";
		return -1;
	}
	else
	{
		int rt = 0;
		ExecStatusType resStatus = PQresultStatus( res);

		if (resStatus == PGRES_COMMAND_OK){}
		else if (resStatus == PGRES_TUPLES_OK)
		{
			try
			{
				int rowsize = PQntuples( res);
				int colsize = PQnfields( res);

				result.init( rowsize, colsize);
				for (int row=0; row<rowsize; row++)
				{
					for (int col=0; col<colsize; col++)
					{
						result.push( PQgetvalue( res, row, col));
					}
				}
			}
			catch (std::bad_alloc)
			{
				LOG_ERROR << "out of memory building database result";
				rt = -1;
			}
		}
		else
		{
			const char* resStatusErrStr = PQresultErrorMessage( res);
			LOG_ERROR << "error executing postgres call: " << resStatusErrStr;
		}
		return rt;
	}
	PQclear( res);
}

