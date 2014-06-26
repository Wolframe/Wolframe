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
#include <boost/scoped_ptr.hpp>

#undef _Wolframe_LOWLEVEL_DEBUG


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

	void finalize( proc::ExecContext* context);

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

void TdlTransactionFunctionClosure::InputStructure::finalize( proc::ExecContext* context)
{
	std::vector<TdlTransactionPreprocStep>::const_iterator pi = m_func->preproc().begin(), pe = m_func->preproc().end();
	for (; pi != pe; ++pi)
	{
		LOG_TRACE << "[transaction input] execute preprocessing function " << pi->tostring( m_func->program()->pathset.tagtab());
		pi->call( context, *m_structure);
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

std::string inputfilter_logtext( const langbind::TypedInputFilterR& inp)
{
	std::ostringstream out;
	langbind::FilterBase::ElementType type;
	types::VariantConst element;
	int taglevel = 0;
	while (taglevel >= 0 && inp->getNext( type, element))
	{
		switch (type)
		{
			case langbind::FilterBase::OpenTag:
				taglevel++;
				out << " " << element.tostring() << " {";
				break;
			case langbind::FilterBase::CloseTag:
				taglevel--;
				if (taglevel >= 0)
				{
					out << "}";
				}
				break;
			case langbind::FilterBase::Attribute:
				taglevel++;
				out << element.tostring() << "=";
				break;
			case langbind::FilterBase::Value:
				taglevel++;
				if (element.defined())
				{
					std::string value = element.tostring();
					if (value.size() > 30)
					{
						value.resize( 30);
						value.append("...");
					}
					out << "'" << value << "'";
				}
				else
				{
					out << "NULL";
				}
				break;
		}
	}
	return out.str();
}

void TdlTransactionFunctionClosure::init( proc::ExecContext* c, const langbind::TypedInputFilterR& i, serialize::Flags::Enum f)
{
	m_context = c;
	m_inputstructptr = new TdlTransactionFunctionClosure::InputStructure( m_func);
	m_inputstruct.reset( m_inputstructptr);
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
	if (!m_authorizationFunction.empty())
	{
		out << "AUTHORIZE (" << m_authorizationFunction;
		if (!m_authorizationResource.empty())
		{
			out << ", " << m_authorizationResource;
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

	// Print auditing steps:
	std::vector<TdlAuditStep>::const_iterator ai = m_audit.begin(), ae = m_audit.end();
	for (; ai != ae; ++ai)
	{
		out << "AUDIT " << ((ai->level() == TdlAuditStep::Critical)?"CRITICAL ":"") << ai->function() << std::endl;
	}

#ifdef _Wolframe_LOWLEVEL_DEBUG
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
	,m_context(0)
	,m_func(f)
	,m_state(0)
	,m_input(false/*doPrintFinalClose*/)
	,m_inputstructptr(0)
	,m_flags(serialize::Flags::None)
	{}

TdlTransactionFunctionClosure::TdlTransactionFunctionClosure( const TdlTransactionFunctionClosure& o)
	:utils::TypeSignature(o)
	,m_context(o.m_context)
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
	std::string errmsg;
	switch (m_state)
	{
		case 0:
			throw std::runtime_error( "input not initialized");
		case 1:
			LOG_DEBUG << "check authorization '" << m_func->name() << "'";
			if (m_context->checkAuthorization( m_func->authorizationFunction(), m_func->authorizationResource(), errmsg))
			{
				LOG_DEBUG << "authorization allows exection of function '" << m_func->name() << "'";
			}
			else
			{
				throw std::runtime_error( std::string( "execution of transaction function '") + m_func->name() + "' denied in this authorization context: " + errmsg);
			}
			LOG_DEBUG << "execute transaction '" << m_func->name() << "'";
			m_state = 2;
			/*no break here!*/
		case 2:
			if (!m_input.call()) return false;
			m_state = 3;
			/*no break here!*/
		case 3:
		{
			// Execute function:
			m_inputstructptr->finalize( m_context);
			db::VmTransactionInput inp( *m_func->program(), m_inputstructptr->structure());
			db::VmTransactionOutput res;
			{
				boost::scoped_ptr<db::Transaction> trsr( m_context->provider()->transaction( m_func->name()));
				if (!trsr.get()) throw std::runtime_error( "failed to allocate transaction object");
				trsr->begin();

				// Execute transaction:
				if (!trsr->execute( inp, res))
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

				// Make audit calls:
				std::vector<TdlAuditStep>::const_iterator ai = m_func->audit().begin(), ae = m_func->audit().end();
				std::size_t auditFunctionIdx = 1;
				for (; ai != ae; ++ai,++auditFunctionIdx)
				{
					// ... for each audit function take the transaction output defined as input of the audit function and call it
					try
					{
						LOG_DEBUG << "calling audit function '" << ai->function() << "'";
						LOG_DATA << "audit function call: " << ai->function() << "(" << inputfilter_logtext( res.get( auditFunctionIdx)) << ")";

						langbind::TypedInputFilterR auditParameter = res.get( auditFunctionIdx);
						const langbind::FormFunction* auditfunc = m_context->provider()->formFunction( ai->function());
						if (!auditfunc)
						{
							throw std::runtime_error( std::string( "transaction audit function '") + ai->function() + "' not found (must be defined as form function)");
						}
						langbind::FormFunctionClosureR auditclosure = langbind::FormFunctionClosureR( auditfunc->createClosure());
						auditclosure->init( m_context, auditParameter);
					
						if (!auditclosure->call())
						{
							throw std::runtime_error( std::string( "failed to call audit function '") + ai->function() + "' (input not complete)");
						}
						langbind::TypedInputFilterR auditresult = auditclosure->result();
						langbind::FilterBase::ElementType et;
						types::VariantConst ev;

						if (auditresult->getNext( et, ev) && et != langbind::FilterBase::CloseTag)
						{
							LOG_WARNING << "called audit function '" << ai->function() << "' that returned a non empty result";
						}
					}
					catch (const std::runtime_error& e)
					{
						if (ai->level() == TdlAuditStep::Critical)
						{
							trsr->rollback();
							throw std::runtime_error( std::string("critical audit function '") + ai->function() + "' failed and so the transaction function '" + m_func->name() + "' (rollback): '" + e.what());
						}
						else
						{
							LOG_ERROR << "non critical audit function '" << ai->function() << "' failed for the transaction function '" << m_func->name() << "'";
						}
					}
					
				}

				// Commit:
				trsr->commit();
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
				const langbind::FormFunction* func = m_context->provider()->formFunction( m_func->resultfilter());
				if (!func)
				{
					throw std::runtime_error( std::string( "transaction result filter function '") + m_func->resultfilter() + "' not found (must be defined as form function)");
				}
				langbind::FormFunctionClosureR filterclosure = langbind::FormFunctionClosureR( func->createClosure());
				filterclosure->init( m_context, unfilderedResult);
			
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
			m_state = 4;
			return true;
		}
		default:
			return true;
	}
}

