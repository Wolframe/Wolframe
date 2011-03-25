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

