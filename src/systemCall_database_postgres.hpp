#ifndef _SYSTEMCALL_DATABASE_POSTGRES_HPP_INCLUDED
#define _SYSTEMCALL_DATABASE_POSTGRES_HPP_INCLUDED
#include "systemCall.hpp"
#include "logger.hpp"

/// \file systemCall_database_postgres.hpp
/// \brief postgres database connector class

namespace _Wolframe {
namespace syscall {
namespace database {

class Connector :public ConnectorBase
{
public:
	struct Configuration
	{
	};

	Connector( const Configuration* cfg) :m_cfg(cfg){};
	virtual ~Connector(){};

	virtual Connector* duplicate()
	{
		return new Connector( m_cfg);
	}

	virtual int open();
	virtual int call( const Function& fun, Result& res);

private:
	const Configuration* m_cfg;
};

}}}//namespace
