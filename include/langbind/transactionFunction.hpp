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
///\file transactionFunction.hpp
///\brief interface to transaction functions used by scripts or directmap
#ifndef _Wolframe_langbind_TRANSACTION_FUNCTION_HPP_INCLUDED
#define _Wolframe_langbind_TRANSACTION_FUNCTION_HPP_INCLUDED
#include "protocol/commandHandler.hpp"
#include "protocol/inputfilter.hpp"
#include "protocol/outputfilter.hpp"

namespace _Wolframe {
namespace langbind {

///\class TransactionCommandWriter
///\brief Writer class for a transaction command source
///\remark Used as interface for serialization of a form to a transaction command source
class TransactionCommandWriter :public protocol::OutputFilter
{
public:
	///\brief Constructor
	explicit TransactionCommandWriter()
		:protocol::OutputFilter(){}
	///\brief Copy constructor
	TransactionCommandWriter( const TransactionWriter& o)
		:protocol::OutputFilter(o),m_content(o.m_content){}
	///\brief Destructor
	virtual ~TransactionCommandWriter(){}

	std::string& content() const;

protected:
	std::string m_content;			//< command string of the transaction function
};

///\class TransactionCommandWriterFactory
///\brief Factory for a transaction command writer
struct TransactionCommandWriterFactory
{
	TransactionCommandWriterFactory(){}
	virtual ~TransactionCommandWriterFactory(){}
	virtual TransactionCommandWriter* create() const=0;
};

///\class TransactionResultReader
///\brief Reader class for a transaction command result
///\remark Used as interface for deserialization of a transaction command result into a form
class TransactionResultReader :public protocol::InputFilter
{
public:
	///\brief Constructor
	TransactionResultReader()
		:protocol::InputFilter(0),m_content(c){}
	///\brief Copy constructor
	TransactionResultReader( const TransactionResultReader& o)
		:protocol::InputFilter(o),m_content(o.m_content){}
	///\brief Destructor
	virtual ~TransactionResultReader(){}

	void init( const std::string& c)
	{
		m_content = c;
	}
protected:
	std::string m_content;			//< result string of the transaction function
};

///\class TransactionCommandWriterFactory
///\brief Factory for a transaction command writer
struct TransactionResultReaderFactory
{
	TransactionResultReaderFactory(){}
	virtual ~TransactionResultReaderFactory(){}
	virtual TransactionResultReader* create() const=0;
};



///\class TransactionFunction
///\brief Interface to a transaction function
class TransactionFunction
{
public:
	///\brief Constructor
	///\param[in] h Command handler definition for this transaction function
	///\param[in] w transaction command writer
	///\param[in] r transaction result reader (parser)
	TransactionFunction( protocol::CommandBase h, const TransactionCommandWriterBase& w, const TransactionResultReaderBase& r)
		:m_cmdreader(r),m_cmdwriter(w),m_transactionCmdHandlerBase(h){}

	///\brief Copy constructor
	///\param[in] o transaction function to copy
	TransactionFunction( const TransactionFunction& o)
		:m_cmdreader(o.m_cmdreader),m_cmdwriter(o.m_cmdwriter),m_transactionCmdHandlerBase(o.m_transactionCmdHandlerBase){}

	///\brief Destructor
	virtual ~DirectmapCommandHandler() {}

	///\brief Execute the Directmap
	///\param[out] err error code in case of error
	///\return CallResult status (See protocol::IOFilterCommandHandler::CallResult)
	virtual CallResult call( const char*& err);

private:
	TransactionResultReaderBase m_cmdreaderbase;
	TransactionCommandWriterBase m_cmdwriterbase;
	protocol::CommandBase m_transactionCmdHandlerBase;
};

}}//namespace
#endif

