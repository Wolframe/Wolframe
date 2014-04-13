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
///\brief Implementaion of the methods of a transaction function based on TDL
///\file tdlTransactionFunction.cpp
#include "tdlTransactionFunction.hpp"
#include "database/transaction.hpp"
#include "vm/inputStructure.hpp"
#include "utils/typeSignature.hpp"
#include "langbind/formFunction.hpp"
#include "processor/procProviderInterface.hpp"
#include "logger-v1.hpp"
#include <string>
#include <vector>

#undef LOWLEVEL_DEBUG


using namespace _Wolframe;
using namespace _Wolframe::db;

///\class TdlTransactionFunctionInput
///\brief Input structure for calling transaction functions
class TdlTransactionFunctionClosure::InputStructure

	:public langbind::TypedOutputFilter
{
public:
	explicit InputStructure( const TdlTransactionFunction* func_);

	InputStructure( const InputStructure& o);

	virtual ~InputStructure(){}

	///\brief Get a self copy
	///\return allocated pointer to copy of this
	virtual langbind::TypedOutputFilter* copy() const;

	virtual bool print( ElementType type, const types::VariantConst& element);

	void finalize( const proc::ProcessorProviderInterface* provider);

	virtual VmTransactionInputR get() const;

	const vm::InputStructure& structure() const;

	const TdlTransactionFunction* func() const;

private:
	vm::InputStructureR m_structure;
	const TdlTransactionFunction* m_func;
	langbind::TypedInputFilter::ElementType m_lasttype;
};


TdlTransactionFunctionClosure::InputStructure::InputStructure( const TdlTransactionFunction* func_)
	:utils::TypeSignature("db::TdlTransactionFunctionInput", __LINE__)
	,langbind::TypedOutputFilter("transactionFunctionInput")
	,m_structure(new vm::InputStructure( func_->program()->pathset.tagtab()))
	,m_func(func_)
	,m_lasttype(langbind::TypedInputFilter::CloseTag)
{}

TdlTransactionFunctionClosure::InputStructure::InputStructure( const InputStructure& o)
	:utils::TypeSignature("db::TdlTransactionFunctionInput", __LINE__)
	,langbind::TypedOutputFilter(o)
	,m_structure(o.m_structure)
	,m_func(o.m_func)
	,m_lasttype(o.m_lasttype)
{}

langbind::TypedOutputFilter* TdlTransactionFunctionClosure::InputStructure::copy() const
{
	return new InputStructure(*this);
}

bool TdlTransactionFunctionClosure::InputStructure::print( ElementType type, const types::VariantConst& element)
{
	LOG_DATA << "[transaction input] push element " << langbind::InputFilter::elementTypeName( type) << " '" << utils::getLogString( element) << "'' :" << element.typeName( element.type());
	switch (type)
	{
		case langbind::TypedInputFilter::OpenTag:
			m_structure->openTag( element);
		break;
		case langbind::TypedInputFilter::CloseTag:
			m_structure->closeTag();
		break;
		case langbind::TypedInputFilter::Attribute:
			m_structure->openTag( element);
		break;
		case langbind::TypedInputFilter::Value:
			m_structure->pushValue( element);
			if (m_lasttype == langbind::TypedInputFilter::Attribute)
			{
				m_structure->closeTag();
			}
		break;
	}
	m_lasttype = type;
	return true;
}

void TdlTransactionFunctionClosure::InputStructure::finalize( const proc::ProcessorProviderInterface* provider)
{
	std::vector<TdlTransactionPreprocStep>::const_iterator pi = m_func->preproc().begin(), pe = m_func->preproc().end();
	for (; pi != pe; ++pi)
	{
		LOG_TRACE << "[transaction input] execute preprocessing function " << pi->tostring( m_func->program()->pathset.tagtab());
		pi->call( provider, *m_structure);
	}
	LOG_DATA << "[transaction input] after preprocess " << m_structure->tostring();
}

VmTransactionInputR TdlTransactionFunctionClosure::InputStructure::get() const
{
	return VmTransactionInputR( new VmTransactionInput( *m_func->program(), *m_structure));
}

const vm::InputStructure& TdlTransactionFunctionClosure::InputStructure::structure() const
{
	return *m_structure.get();
}

const TdlTransactionFunction* TdlTransactionFunctionClosure::InputStructure::func() const
{
	return m_func;
}

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

void TdlTransactionFunctionClosure::init( const proc::ProcessorProviderInterface* p, const langbind::TypedInputFilterR& i, serialize::Context::Flags f)
{
	m_provider = p;
	m_inputstructptr = new TdlTransactionFunctionClosure::InputStructure( m_func);
	m_inputstruct.reset( m_inputstructptr);
	i->setFlags( langbind::TypedInputFilter::SerializeWithIndices);
	m_input.init( i, m_inputstruct);
	m_state = 1;
	m_flags = f;
}

langbind::TypedInputFilterR TdlTransactionFunctionClosure::result() const
{
	return m_result;
}

TdlTransactionFunctionClosure* TdlTransactionFunction::createClosure() const
{
	return new TdlTransactionFunctionClosure( this);
}


void TdlTransactionFunction::print( std::ostream& out) const
{
	// Print header:
	out << "TRANSACTION " << m_name << std::endl;

	// Print authorization constraints:
	if (!m_authfunction.empty())
	{
		out << "AUTHORIZE (" << m_authfunction;
		if (!m_authresource.empty())
		{
			out << ", " << m_authresource;
		}
		out << ")" << std::endl;
	}

	// Print result filter (result INTO got part of the generated code):
	if (!m_resultfilter.empty())
	{
		out << "RESULT FILTER " << m_resultfilter << std::endl;
	}

	// Print preprocessing steps:
	if (!m_preproc.empty())
	{
		out << "PREPROC" << std::endl;
		std::vector<TdlTransactionPreprocStep>::const_iterator pi = m_preproc.begin(), pe = m_preproc.end();
		for (; pi != pe; ++pi)
		{
			pi->print( out, m_program->pathset.tagtab());
			out << std::endl;
		}
		out << "ENDPROC" << std::endl;
	}
#ifdef LOWLEVEL_DEBUG
	// Print code without symbols:
	out << "BEGIN RAW" << std::endl;
	m_program->code.printRaw( out);
	out << "END RAW" << std::endl;
#endif
	// Print code with symbolic information:
	out << "BEGIN" << std::endl;
	m_program->print( out);
	out << "END" << std::endl;
}


TdlTransactionFunctionClosure::TdlTransactionFunctionClosure( const TdlTransactionFunction* f)
	:utils::TypeSignature("prgbind::TransactionFunctionClosure", __LINE__)
	,m_provider(0)
	,m_func(f)
	,m_state(0)
	,m_inputstructptr(0)
	,m_flags(serialize::Context::None)
	{}

TdlTransactionFunctionClosure::TdlTransactionFunctionClosure( const TdlTransactionFunctionClosure& o)
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

bool TdlTransactionFunctionClosure::call()
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
			// Execute function:
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
			// Build output:
			if (m_func->resultfilter().empty())
			{
				// ... result filter does not exist, so return the transaction result as function result
				m_result = res.get();
			}
			else
			{
				// ... result filter exists, so we pipe the transaction result through it to get the final result
				langbind::TypedInputFilterR unfilderedResult = res.get();
				const langbind::FormFunction* func = m_provider->formFunction( m_func->resultfilter());
				if (!func)
				{
					throw std::runtime_error( std::string( "transaction result filter function '") + m_func->resultfilter() + "' not found (must be defined as form function)");
				}
				langbind::FormFunctionClosureR filterclosure = langbind::FormFunctionClosureR( func->createClosure());
				filterclosure->init( m_provider, unfilderedResult);
			
				if (!filterclosure->call())
				{
					throw std::runtime_error( std::string( "failed to call filter function '") + m_func->resultfilter() + "' with result of transaction (input not complete)");
				}
				langbind::TypedInputFilterR filteredResult = filterclosure->result();
				if (unfilderedResult->flag( langbind::TypedInputFilter::PropagateNoCase))
				{
					filteredResult->setFlags( langbind::TypedInputFilter::PropagateNoCase);
				}
				m_result = filteredResult;
			}

			// Propagate flags for output:
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

