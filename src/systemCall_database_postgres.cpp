#include "systemCall.hpp"
#include "logger.hpp"
#include <postgresql/libpq-fe.h>

using namespace _Wolframe::syscall;

struct Connector::Data
{
	PGconn* m_conn;
	PGresult* m_result;
	unsigned int m_cols;
	unsigned int m_rows;

	Data() :m_conn(0),m_result(0),m_cols(0),m_rows(0){}
	~Data()
	{
		if (m_result) PQclear(m_result);
		if (m_conn) PQfinish(m_conn);
	}
	void init()
	{
		if (m_result) PQclear(m_result);
		m_cols = 0;
		m_rows = 0;
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

int Connector::call( const Function& fun)
{
	std::string cmd;
	data->init();

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
	data->m_result = PQexec( data->m_conn, cmd);
	if (!data->m_result)
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
				data->m_rows = PQntuples( data->m_result);
				data->m_cols = PQnfields( data->m_result);

				for (int kk=0; kk<resSize; kk++)
				{
					char* rr = PQgetvalue( res, kk, 0);
					if (!rr)
					{
						result.push_back( rr);
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
}

const char* Connector::fetch( unsigned int row, unsigned int col);
{
	if (row >= data->m_rows || col >= data->m_cols)
	{
		LOG_ERROR << "database result index out of range";
		return 0;
	}
	return PQgetvalue( res, row, col);
}

