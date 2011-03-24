//
// function.hpp
//
#ifndef _FUNCTION_HPP_INCLUDED
#define _FUNCTION_HPP_INCLUDED
#include <ctype.h>
#include <vector>
#include <boost/property_tree/ptree.hpp>
#include "objectPool.hpp"
#include "logger.hpp"

//\brief interface for system function calls like database function calls
namespace _Wolframe {
namespace syscall {

class Function;

class Connector
{
private:
	struct Data;
public:
	Data* m_data;
	Connector() :m_data(0) {};
	virtual ~Connector(){};
	virtual int open( const boost::property_tree::ptree& cfg, unsigned int idx);
	virtual int call( const Function* fun);
	virtual const char* fetch( unsigned int row, unsigned int col);
};

class Function
{
public:
	enum Error
	{
		Ok,
		ErrOutOfMem,
		ErrInvalidCommand,
		ErrInvalidArgument,
		ErrExecution
	};
	static const char* getErrorName( Error e)
	{
		static const char* ar[] = {"", "out of mem", "invalid command", "invalid argument", "execution error"};
		return ar[e];
	}

	Function( const char* name) :m_error(Ok),m_name(name)
	{
		unsigned int ii=0;
		while (isalnum( name[ii])) ii++;
		if (!name[ii])
		{
			m_error = ErrInvalidCommand;
			break;
		}
	}

	Error getError() const
	{
		return m_error;
	}

	template <typename ArgumentType>
	void addParameter( const char* name, const ArgumentType& value)
	{
		unsigned int ii=0;
		if (name)
		{
			while (isalnum( name[ii])) ii++;
			if (!name[ii])
			{
				m_error = ErrInvalidArgument;
				break;
			}
		}
		if (m_error == Ok)
		{
			try
			{
				m_param.push_back( Command( name, boost::lexical_cast<std::string>( value));
			}
			catch (std::bad_alloc)
			{
				m_error = ErrOutOfMem;
			}
			catch (...)
			{
				m_error = ErrInvalidArgument;
			};
		}

	}

	template <typename ArgumentType>
	Command& operator()( const char* name, const ArgumentType& value)
	{
		addParameter( name, value);
		return this;
	}

private:
	friend class Connector;
	struct Parameter
	{
		const char* m_name;	//< name of parameter, use index if 0
		std::string m_value;	//< value untyped
	};
	std::vector<Parameter> m_param;
	const char* m_name;
	Error m_error;
};


class ConnectorPool :public ObjectPool<Connector>
{
	struct Refused :public std::runtime_error {Refused() :std::runtime_error("refused system object request"){}};

	ConnectorPool( const char* cn) :m_connectorName(cn){};

	template <class ConnectorI>
	bool open( const boost::property_tree::ptree& config, unsigned int nofInstances)
	{
		unsigned int ii;
		for (ii=0; ii<nofInstances; ii++)
		{
			Connector* conn = new Connector();
			int err = conn->open( config, ii);
			if (err)
			{
				LOG_ERROR << "failed to open "<< m_connectorName << " connector. error code:" << err;
				delete conn;
				return false;
			}
			add( conn);
		}
	}

	struct Element :public Function
	{
	public:
		Element( ConnectorPool* pool, const char* name) :m_pool(pool),m_conn(0),m_fun(0)
		{
			m_conn = pool->get();
			if (!m_conn)
			{
				LOG_ERROR << "failed to get "<< m_connectorName << " object for system call";
				throw Refused();
			}
		}
		~Element()
		{
			if (m_conn)
			{
				m_conn = m_pool->add(m_conn);
				m_conn = 0;
			}
		}

		template <typename ArgumentType>
		Element& operator()( const char* name, const ArgumentType& value)
		{
			addParameter( name, value);
			return *this;
		}

		bool exec( std::vector<std::string>& result)
		{
			Function::Error rt = m_fun.getError();
			if (rt == Function::Ok)
			{
				int err = m_conn->call( fun, result);
				if (err)
				{
					LOG_ERROR << "error " << err << " in " << m_connectorName << " calling function " << m_fun.m_name;
					return false;
				}
			}
			else
			{
				LOG_ERROR << "error calling function " << m_fun.m_name << ":" << Function::getErrorName( rt);
				return false;
			}
			return true;
		}

	private:
		ConnectorPool* m_pool;
		Connector* m_conn;
	};

	boost::shared_ptr<Element> function( const char* name)
	{
		return boost::shared_ptr<Element>( new Element( this, name));
	}
};

}}//namespace
#endif

