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
#include "database/tdlTransactionFunction.hpp"
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

TdlTransactionFunctionInput::TdlTransactionFunctionInput( const TdlTransactionFunction* func_)
	:utils::TypeSignature("db::TdlTransactionFunctionInput", __LINE__)
	,langbind::TypedOutputFilter("transactionFunctionInput")
	,m_structure(new vm::InputStructure( func_->program()->pathset.tagtab()))
	,m_func(func_)
	,m_lasttype(langbind::TypedInputFilter::CloseTag)
{}

TdlTransactionFunctionInput::TdlTransactionFunctionInput( const TdlTransactionFunctionInput& o)
	:utils::TypeSignature("db::TdlTransactionFunctionInput", __LINE__)
	,langbind::TypedOutputFilter(o)
	,m_structure(o.m_structure)
	,m_func(o.m_func)
	,m_lasttype(o.m_lasttype)
{}

bool TdlTransactionFunctionInput::print( ElementType type, const types::VariantConst& element)
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

void TdlTransactionFunctionInput::finalize( const proc::ProcessorProviderInterface* provider)
{
	std::vector<TdlTransactionPreprocStep>::const_iterator pi = m_func->preproc().begin(), pe = m_func->preproc().end();
	for (; pi != pe; ++pi)
	{
		pi->call( provider, *m_structure);
	}
	LOG_DATA << "[transaction input] after preprocess " << m_structure->tostring();
}

VmTransactionInputR TdlTransactionFunctionInput::get() const
{
	return VmTransactionInputR( new VmTransactionInput( *m_func->program(), *m_structure));
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

TdlTransactionFunctionInput* TdlTransactionFunction::getInput() const
{
	return new TdlTransactionFunctionInput( this);
}

langbind::TypedInputFilterR TdlTransactionFunction::getOutput( const proc::ProcessorProviderInterface* provider, const VmTransactionOutput& output) const
{
	if (m_resultfilter.empty())
	{
		// ... result filter does not exist, so return the transaction result as function result
		return output.get();
	}
	else
	{
		// ... result filter exists, so we pipe the transaction result through it to get the final result
		langbind::TypedInputFilterR unfilderedResult = output.get();
		const langbind::FormFunction* func = provider->formFunction( m_resultfilter);
		if (!func)
		{
			throw std::runtime_error( std::string( "transaction result filter function '") + m_resultfilter + "' not found (must be defined as form function)");
		}
		langbind::FormFunctionClosureR closure = langbind::FormFunctionClosureR( func->createClosure());
		closure->init( provider, unfilderedResult);
	
		if (!closure->call())
		{
			throw std::runtime_error( std::string( "failed to call filter function '") + m_resultfilter + "' with result of transaction (input not complete)");
		}
		langbind::TypedInputFilterR filteredResult = closure->result();
		if (unfilderedResult->flag( langbind::TypedInputFilter::PropagateNoCase))
		{
			filteredResult->setFlags( langbind::TypedInputFilter::PropagateNoCase);
		}
		return filteredResult;
	}
}

