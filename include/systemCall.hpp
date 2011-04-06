#ifndef _SYSTEMCALL_HPP_INCLUDED
#define _SYSTEMCALL_HPP_INCLUDED
#include <ctype.h>
#include <vector>
#include "objectPool.hpp"
#include "logger.hpp"

/// \file systemCall.hpp
/// \brief interface for system function calls like database function calls

namespace _Wolframe {
namespace syscall {

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

	const Parameter& operator[]( unsigned int idx) const
	{
		return m_param[ idx];
	}

	const char* name() const
	{
		return m_name;
	}

private:
	struct Parameter
	{
		const char* m_name;	//< name of parameter, use index if 0
		std::string m_value;	//< value untyped
	};
	std::vector<Parameter> m_param;
	const char* m_name;
	Error m_error;
};


class Result
{
	const char* operator()( unsigned int i, unsigned int j)
	{
		if (i>=m_rows || j>=m_cols || idx[ i*m_rows + m_cols] == 0) return 0;
		return (m_value.c_str() + idx[ i*m_rows + m_cols]);
	}

	Result() :m_rows(0),m_cols(0){}

	void clear()
	{
		m_rows=0;
		m_cols=0;
		m_idx.clear();
		m_value.clear();
	}

	void init( unsigned int rows_, unsigned int cols_)
	{
		clear();
		m_rows = rows_;
		m_cols = cols_;
	}

	void push( const char* result)
	{
		m_value.push_back('\0');
		if (result)
		{
			m_idx.push_back( m_value.size());
			m_value.append( result);
		}
		else
		{
			m_idx.push_back( 0);
		}
	}

private:
	unsigned int m_rows;
	unsigned int m_cols;
	std::vector<std::size_t> m_idx;
	std::string m_value;
};


class ConnectorBase
{
public:
	ConnectorBase(){};
	virtual ~ConnectorBase(){};

	virtual int call( const Function& fun, Result& res)=0;
};


}}//namespace
#endif

