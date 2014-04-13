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
///\brief Implements the loading of programs in the transaction definition language
///\file prgbind_transactionProgram.cpp
#include "prgbind/transactionProgram.hpp"
#include "prgbind/programLibrary.hpp"
#include "langbind/redirectFilterClosure.hpp"
#include "processor/procProviderInterface.hpp"
#include "database/tdlTransactionFunction.hpp"
#include "database/databaseError.hpp"
#include "database/database.hpp"
#include "database/vm/program.hpp"
#include "utils/fileUtils.hpp"
#include "database/loadTransactionProgram.hpp"
#include "logger-v1.hpp"

using namespace _Wolframe;
using namespace _Wolframe::prgbind;

static std::string removeCRLF( const std::string& src)
{
	std::string rt;
	std::string::const_iterator si = src.begin(), se = src.end();
	for (; si != se; ++si)
	{
		if (*si == '\r' || *si == '\n')
		{
			if (rt.size() && rt[ rt.size()-1] != ' ') rt.push_back(' ');
		}
		else
		{
			rt.push_back( *si);
		}
	}
	return rt;
}

static std::string databaseError_logtext( const db::DatabaseError& err, const std::string& name, const db::vm::Program& program)
{
	std::ostringstream logmsg;
	logmsg << "error in transaction '" << name << "' [IP " << err.ip << "]"
		<< " for database '" << err.dbname << "'"
		<< " error class '" << err.errorclass << "'";
	if (err.errorcode)
	{
		logmsg << " database internal error code " << err.errorcode << "'";
	}
	if (err.errormsg.size())
	{
		logmsg << " " << removeCRLF( err.errormsg);
	}
	if (err.errordetail.size())
	{
		logmsg << " " << removeCRLF( err.errordetail);
	}
	if (err.errorhint.size())
	{
		logmsg << " " << err.errorhint;
	}
	std::string locationstr;
	utils::FileLineInfo posinfo;

	if (program.getSourceReference( err.ip, posinfo))
	{
		logmsg << " TDL source location " << posinfo.logtext();
	}
	return logmsg.str();
}

static std::string databaseError_throwtext( const db::DatabaseError& err, const std::string& name)
{
	std::ostringstream throwmsg;
	//... shorter message is thrown. no internals in message as written into the logs
	throwmsg << "error in transaction '" << name << "':";
	if (err.errordetail.size())
	{
		throwmsg << " " << removeCRLF( err.errordetail);
	}
	else if (err.errormsg.size())
	{
		//... error message is only thrown if errordetail is empty
		throwmsg << " " << removeCRLF( err.errormsg);
	}
	if (err.errorhint.size())
	{
		throwmsg << " " << err.errorhint;
	}
	return throwmsg.str();
}


class TransactionFunctionClosure
	:public utils::TypeSignature
	,public langbind::FormFunctionClosure
{
public:
	TransactionFunctionClosure( const db::TdlTransactionFunction* f)
		:utils::TypeSignature("prgbind::TransactionFunctionClosure", __LINE__)
		,m_provider(0)
		,m_func(f)
		,m_state(0)
		,m_inputstructptr(0)
		,m_flags(serialize::Context::None)
		{}

	TransactionFunctionClosure( const TransactionFunctionClosure& o)
		:utils::TypeSignature(o)
		,m_provider(o.m_provider)
		,m_func(o.m_func)
		,m_state(o.m_state)
		,m_input(o.m_input)
		,m_inputstructptr(o.m_inputstructptr)
		,m_inputstruct(o.m_inputstruct)
		,m_result(o.m_result)
		,m_flags(o.m_flags)
		{}

	virtual bool call()
	{
		switch (m_state)
		{
			case 0:
				throw std::runtime_error( "input not initialized");
			case 1:
				LOG_DEBUG << "execute transaction '" << m_func->name() << "'";

				if (!m_input.call()) return false;
				m_state = 2;
			case 2:
			{
				m_inputstructptr->finalize( m_provider);
				db::VmTransactionInput inp( *m_func->program(), m_inputstructptr->structure());
				db::VmTransactionOutput res;
				{
					types::CountedReference<db::Transaction> trsr( m_provider->transaction( m_func->name()));
					if (!trsr.get()) throw std::runtime_error( "failed to allocate transaction object");
					trsr->begin();

					if (trsr->execute( inp, res))
					{
						trsr->commit();
					}
					else
					{
						const db::DatabaseError* err = trsr->getLastError();
						if (err)
						{
							LOG_ERROR << databaseError_logtext( *err, m_func->name(), *m_func->program());
							std::string msg( databaseError_throwtext( *err, m_func->name()));
							throw std::runtime_error( msg);
						}
						else
						{
							db::DatabaseError ue( "UNKNOWN", 0, "unknown error (transaction program)");
							LOG_ERROR << databaseError_logtext( ue, m_func->name(), *m_func->program());
							throw std::runtime_error( databaseError_throwtext( ue, m_func->name()));
						}
					}
				}
				m_result = m_func->getOutput( m_provider, res);
				if (!res.isCaseSensitive())
				{
					//... If not case sensitive result then propagate this
					//	to be respected in mapping to structures.
					m_result->setFlags( langbind::TypedInputFilter::PropagateNoCase);
				}
				m_state = 3;
				return true;
			}
			default:
				return true;
		}
	}

	virtual void init( const proc::ProcessorProviderInterface* p, const langbind::TypedInputFilterR& i, serialize::Context::Flags f)
	{
		m_provider = p;
		m_inputstruct.reset( m_inputstructptr = m_func->getInput());
		i->setFlags( langbind::TypedInputFilter::SerializeWithIndices);
		m_input.init( i, m_inputstruct);
		m_state = 1;
		m_flags = f;
	}

	virtual langbind::TypedInputFilterR result() const
	{
		return m_result;
	}

private:
	const proc::ProcessorProviderInterface* m_provider;	//< processor provider to get transaction object
	const db::TdlTransactionFunction* m_func;		//< function to execute
	int m_state;						//< current state of call
	langbind::RedirectFilterClosure m_input;		//< builder of structure from input
	db::TdlTransactionFunctionInput* m_inputstructptr;	//< input structure implementation interface
	langbind::TypedOutputFilterR m_inputstruct;		//< input structure
	langbind::TypedInputFilterR m_result;			//< function call result
	serialize::Context::Flags m_flags;			//< flags for input serialization
};


class TransactionFunction
	:public langbind::FormFunction
{
public:
	TransactionFunction( const db::TdlTransactionFunctionR& f)
		:m_impl(f){}

	virtual TransactionFunctionClosure* createClosure() const
	{
		return new TransactionFunctionClosure( m_impl.get());
	}

private:
	db::TdlTransactionFunctionR m_impl;
};



bool TransactionDefinitionProgram::is_mine( const std::string& filename) const
{
	return boost::algorithm::to_lower_copy( utils::getFileExtension( filename)) == ".tdl";
}

void TransactionDefinitionProgram::loadProgram( ProgramLibrary& library, db::Database* transactionDB, const std::string& filename)
{
	static const db::LanguageDescription defaultLanguageDescr;
	const db::LanguageDescription* languageDescr = (transactionDB)?transactionDB->getLanguageDescription():&defaultLanguageDescr;

	LOG_DEBUG << "Loading transaction program '" << filename << "':";

	try
	{
		std::string databaseID;
		std::string databaseClassName;
		if (transactionDB)
		{
			databaseID = transactionDB->ID();
			databaseClassName = transactionDB->className();
		}
		std::vector<std::pair<std::string,db::TdlTransactionFunctionR> > funclist
			= db::loadTransactionProgramFile2( filename, databaseID, databaseClassName, languageDescr);

		std::vector<std::pair<std::string,db::TdlTransactionFunctionR> >::const_iterator fi = funclist.begin(), fe = funclist.end();
		for (; fi != fe; ++fi)
		{
			langbind::FormFunctionR func( new TransactionFunction( fi->second));
			library.defineFormFunction( fi->first, func);
			LOG_DEBUG << "Loaded transaction function '" << fi->first << "'";
		}
	}
	catch (const std::runtime_error& e)
	{
		throw std::runtime_error( std::string( "error loading transaction definition program '") + filename + "': " + e.what());
	}
}


