/************************************************************************

 Copyright (C) 2011 - 2013 Project Wolframe.
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
///\brief Implementation of the builder of transaction input and reader of output
///\file transactionFunction.cpp
#include "types/allocators.hpp"
#include "utils/parseUtils.hpp"
#include "database/transactionFunction.hpp"
#include "transactionfunction/InputStructure.hpp"
#include "transactionfunction/OutputResultStructure.hpp"
#include "transactionfunction/Path.hpp"
#include "transactionfunction/OutputImpl.hpp"
#include "database/transaction.hpp"
#include "logger/logger-v1.hpp"
#include <cstring>
#include <sstream>
#include <iostream>
#include <locale>
#include <boost/algorithm/string.hpp>

using namespace _Wolframe;
using namespace _Wolframe::db;

class FunctionCall
{
public:
	FunctionCall()
		:m_nonemptyResult(false)
		,m_uniqueResult(false){}
	FunctionCall( const FunctionCall& o);
	FunctionCall( const std::string& name, const Path& selector, const std::vector<Path>& arg, bool setNonemptyResult_, bool setUniqueResult_, std::size_t level_, const types::keymap<std::string>& hints_=types::keymap<std::string>());

	const Path& selector() const					{return m_selector;}
	const std::vector<Path>& arg() const				{return m_arg;}
	const std::string& name() const					{return m_name;}

	bool hasNonemptyResult() const					{return m_nonemptyResult;}
	bool hasUniqueResult() const					{return m_uniqueResult;}
	std::size_t level() const					{return m_level;}

	const char* getErrorHint( const std::string& errorclass) const	{types::keymap<std::string>::const_iterator hi = m_hints.find( errorclass); return (hi==m_hints.end())?0:hi->second.c_str();}

private:
	std::vector<std::string> m_resultnames;
	std::string m_name;
	Path m_selector;
	std::vector<Path> m_arg;
	bool m_nonemptyResult;
	bool m_uniqueResult;
	std::size_t m_level;
	types::keymap<std::string> m_hints;
};

struct TransactionFunction::Impl
{
	TransactionFunctionOutput::ResultStructureR m_resultstruct;
	std::vector<FunctionCall> m_call;
	TagTable m_tagmap;

	Impl( const TransactionFunctionDescription& description, const types::keymap<TransactionFunctionR>& functionmap);
	Impl( const Impl& o);
};


FunctionCall::FunctionCall( const std::string& n, const Path& s, const std::vector<Path>& a, bool q, bool u, std::size_t l, const types::keymap<std::string>& hints_)
	:m_name(n)
	,m_selector(s)
	,m_arg(a)
	,m_nonemptyResult(q)
	,m_uniqueResult(u)
	,m_level(l)
	,m_hints(hints_){}

FunctionCall::FunctionCall( const FunctionCall& o)
	:m_name(o.m_name)
	,m_selector(o.m_selector)
	,m_arg(o.m_arg)
	,m_nonemptyResult(o.m_nonemptyResult)
	,m_uniqueResult(o.m_uniqueResult)
	,m_level(o.m_level)
	,m_hints(o.m_hints){}

TransactionFunctionInput::TransactionFunctionInput( const TransactionFunction* func_)
	:types::TypeSignature("database::TransactionFunctionInput", __LINE__)
	,m_structure( new Structure( &func_->impl().m_tagmap))
	,m_func(func_)
	,m_lasttype( langbind::TypedInputFilter::Value){}

TransactionFunctionInput::~TransactionFunctionInput(){}

TransactionFunctionInput::TransactionFunctionInput( const TransactionFunctionInput& o)
	:types::TypeSignature("database::TransactionFunctionInput", __LINE__)
	,langbind::TypedOutputFilter(o)
	,m_structure( o.m_structure)
	,m_func(o.m_func)
	,m_lasttype(o.m_lasttype){}

bool TransactionFunctionInput::print( ElementType type, const types::VariantConst& element)
{
	LOG_DATA << "[transaction input] push element " << langbind::InputFilter::elementTypeName( type) << " '" << element.tostring() << "'";
	switch (type)
	{
		case langbind::TypedInputFilter::OpenTag:
			m_structure->openTag( element.tostring());
		break;
		case langbind::TypedInputFilter::CloseTag:
			m_structure->closeTag();
		break;
		case langbind::TypedInputFilter::Attribute:
			m_structure->openTag( element.tostring());
		break;
		case langbind::TypedInputFilter::Value:
			m_structure->pushValue( element.tostring());
			if (m_lasttype == langbind::TypedInputFilter::Attribute)
			{
				m_structure->closeTag();
			}
		break;
	}
	m_lasttype = type;
	return true;
}

static void bindArguments( TransactionInput& ti, const FunctionCall& call, const TransactionFunctionInput* inputst, const TransactionFunctionInput::Structure::Node& selectornode)
{
	typedef TransactionFunctionInput::Structure::Node Node;

	std::vector<Path>::const_iterator pi=call.arg().begin(), pe=call.arg().end();
	for (std::size_t argidx=1; pi != pe; ++pi,++argidx)
	{
		switch (pi->referenceType())
		{
			case Path::ResultIndex:
				ti.bindCommandArgAsResultReference( pi->resultReferenceIndex());
				break;
			case Path::ResultSymbol:
				ti.bindCommandArgAsResultReference( pi->resultReferenceSymbol());
				break;
			case Path::Constant:
				ti.bindCommandArgAsValue( pi->constantReference());
				break;

			case Path::Find:
			case Path::Root:
			case Path::Next:
			case Path::Up:
			{
				std::vector<Node> param;
				pi->selectNodes( inputst->structure(), selectornode, param);
				if (param.size() == 0)
				{
					ti.bindCommandArgAsNull();
				}
				else
				{
					std::vector<Node>::const_iterator gs = param.begin(), gi = param.begin()+1, ge = param.end();
					for (; gi != ge; ++gi)
					{
						if (*gs != *gi) throw std::runtime_error( "more than one node selected in db call argument");
					}
					const types::Variant* valref = inputst->structure().nodevalue( *gs);
					if (valref)
					{
						ti.bindCommandArgAsValue( *valref);
					}
					else
					{
						ti.bindCommandArgAsNull();
					}
				}
			}
		}
	}
}

static void getOperationInput( const TransactionFunctionInput* this_, TransactionInput& rt, std::size_t startfidx, std::size_t level, std::vector<FunctionCall>::const_iterator ci, std::vector<FunctionCall>::const_iterator ce, const std::vector<TransactionFunctionInput::Structure::Node>& rootnodearray)
{
	typedef TransactionFunctionInput::Structure::Node Node;
	std::size_t fidx = startfidx;
	for (; ci != ce; ++ci,++fidx)
	{
		TransactionInput::cmd_iterator ti = rt.begin(), te = rt.end();
		for (; ti != te; ++ti)
		{
			if (ti->functionidx() == fidx)
			{
				if (ci->hasNonemptyResult())
				{
					ti->setNonemptyResult();
				}
				if (ci->hasUniqueResult())
				{
					ti->setUniqueResult();
				}
			}
		}

		// Select the nodes to execute the command with:
		std::vector<Node> nodearray;
		std::vector<Node>::const_iterator ni = rootnodearray.begin(), ne = rootnodearray.end();
		for (; ni != ne; ++ni)
		{
			ci->selector().selectNodes( this_->structure(), *ni, nodearray);
		}
		std::vector<FunctionCall>::const_iterator ca = ci;
		if (ci->level() > level)
		{
			if (!ci->name().empty()) throw std::logic_error("passing arguments expected when calling OPERATION");
			std::size_t nextfidx = fidx;
			for (++ca; ca != ce; ++ca,++nextfidx)
			{
				if (ca->level() < level || (ca->level() == level && ca->name().empty())) break;
			}
			std::vector<Node>::const_iterator vi=nodearray.begin(), ve=nodearray.end();
			for (; vi != ve; ++vi)
			{
				rt.startCommand( fidx, ci->level(), ci->name());
				bindArguments( rt, *ci, this_, *vi);
				std::vector<Node> opnodearray;
				opnodearray.push_back( *vi);
				getOperationInput( this_, rt, fidx+1, ci->level(), ci+1, ca, opnodearray);
			}
			ci = ca;
			ci--;
			fidx = nextfidx;
		}
		else
		{
			// For each selected node do expand the function call arguments:
			std::vector<Node>::const_iterator vi=nodearray.begin(), ve=nodearray.end();
			for (; vi != ve; ++vi)
			{
				rt.startCommand( fidx, ci->level(), ci->name());
				bindArguments( rt, *ci, this_, *vi);
			}
		}
	}
}

TransactionInput TransactionFunctionInput::get() const
{
	TransactionInput rt;
	std::vector<FunctionCall>::const_iterator ci = m_func->impl().m_call.begin(), ce = m_func->impl().m_call.end();

	std::vector<Structure::Node> nodearray;
	nodearray.push_back( structure().root());
	getOperationInput( this, rt, 0, 1, ci, ce, nodearray);
	return rt;
}


TransactionFunctionOutput::TransactionFunctionOutput( const ResultStructureR& resultstruct_, const db::TransactionOutput& data_)
	:types::TypeSignature("database::TransactionFunctionOutput", __LINE__)
	,m_impl( new Impl( resultstruct_, db::TransactionOutputR( new db::TransactionOutput( data_))))
{}

TransactionFunctionOutput::TransactionFunctionOutput( const TransactionFunctionOutput& o)
	:types::TypeSignature("database::TransactionFunctionOutput", __LINE__)
	,langbind::TypedInputFilter(o)
	,m_impl( new Impl( *o.m_impl))
{}

TransactionFunctionOutput::~TransactionFunctionOutput()
{
	delete m_impl;
}

bool TransactionFunctionOutput::getNext( ElementType& type, types::VariantConst& element)
{
	bool rt = m_impl->getNext( type, element, flag( SerializeWithIndices));
	return rt;
}

void TransactionFunctionOutput::resetIterator()
{
	m_impl->resetIterator();
}

static int hasOpenSubstruct( const std::vector<TransactionFunctionDescription::Block>& blocks, std::size_t idx)
{
	std::size_t ii = 0;
	for (; ii<blocks.size(); ++ii) if (idx == blocks.at(ii).startidx) return ii;
	return -1;
}

static int hasCloseSubstruct( const std::vector<TransactionFunctionDescription::Block>& blocks, std::size_t idx)
{
	std::size_t ii = 0;
	for (; ii<blocks.size(); ++ii) if (idx == blocks.at(ii).startidx + blocks.at(ii).size && blocks.at(ii).size != 0) return ii;
	return -1;
}

TransactionFunction::Impl::Impl( const TransactionFunctionDescription& description, const types::keymap<TransactionFunctionR>& functionmap)
	:m_resultstruct( new TransactionFunctionOutput::ResultStructure())
	,m_tagmap(description.casesensitive)
{
	typedef TransactionFunctionDescription::OperationStep::Error Error;
	typedef TransactionFunctionDescription::OperationStep::Call Call;
	VariableTable varmap;

	int blkidx;
	std::vector<TransactionFunctionDescription::OperationStep>::const_iterator di = description.steps.begin(), de = description.steps.end();
	for (; di != de; ++di)
	{
		std::size_t eidx = di - description.steps.begin();
		try
		{
			// Create substructure context for RESULT INTO instructions:
			if ((blkidx=hasCloseSubstruct( description.blocks, eidx)) >= 0)
			{
				const std::vector<std::string>& ps = description.blocks.at(blkidx).path_INTO;
				std::vector<std::string>::const_iterator pi = ps.begin(), pe = ps.end();
				for (; pi != pe; ++pi)
				{
					m_resultstruct->closeTag();
				}
			}
			if ((blkidx=hasOpenSubstruct( description.blocks, eidx)) >= 0)
			{
				const std::vector<std::string>& ps = description.blocks.at(blkidx).path_INTO;
				std::vector<std::string>::const_iterator pi = ps.begin(), pe = ps.end();
				for (; pi != pe; ++pi)
				{
					m_resultstruct->openTag( *pi);
				}
			}

			// Build Function call object for parsed function:
			Path selector( di->selector_FOREACH, &m_tagmap);
			std::vector<Path> param;
			std::vector<Call::Param>::const_iterator ai = di->call.paramlist.begin(), ae = di->call.paramlist.end();
			for (; ai != ae; ++ai)
			{
				Path pp( *ai, &varmap, &m_tagmap);
				param.push_back( pp);
			}
			types::keymap<TransactionFunctionR>::const_iterator fui = functionmap.find( di->call.funcname);
			if (fui == functionmap.end())
			{
				FunctionCall cc( di->call.funcname, selector, param, di->nonempty, di->unique, 1, di->hints);
				if (!di->path_INTO.empty())
				{
					// this is just the wrapping structure, iteration is only done with the last tag (othwerwise
					// we get into definition problems)
					bool hasOutput = (di->path_INTO.size() > 1 || (di->path_INTO.size() == 1 && di->path_INTO[0] != "."));
					for (std::vector<std::string>::size_type i = 0; i < di->path_INTO.size()-1; i++)
					{
						m_resultstruct->openTag( di->path_INTO[i] );
					}
					m_resultstruct->addMark( ResultElement::FunctionStart, m_call.size());
					if (hasOutput) {
						m_resultstruct->openTag( di->path_INTO[di->path_INTO.size()-1]);
					}
					if (!di->unique) m_resultstruct->addMark( ResultElement::IndexStart, m_call.size());
					m_resultstruct->addValueReference( m_call.size());
					if (!di->unique) m_resultstruct->addMark( ResultElement::IndexEnd, m_call.size());
					if (hasOutput) {
						m_resultstruct->closeTag( );
					}
					m_resultstruct->addMark( ResultElement::FunctionEnd, m_call.size());
					for (std::vector<std::string>::size_type i = 0; i < di->path_INTO.size()-1; i++)
					{
						m_resultstruct->closeTag( );
					}
				}
				m_call.push_back( cc);
			}
			else
			{
				Impl* func = fui->second->m_impl;
				std::map<int,int> rwtab = m_tagmap.insert( func->m_tagmap);
				if (!di->hints.empty())
				{
					throw Error( eidx, "No ON ERROR hints supported for call of OPERATION");
				}
				if (di->nonempty)
				{
					throw Error( eidx, "NONEMTY not supported for call of OPERATION");
				}
				if (di->unique)
				{
					throw Error( eidx, "UNIQUE not supported for call of OPERATION");
				}
				if (!di->path_INTO.empty() && !di->path_INTO[0].empty( ) )
				{
					m_resultstruct->addMark( ResultElement::OperationStart, m_call.size());

					bool hasOutput = (di->path_INTO.size() > 1 || (di->path_INTO.size() == 1 && di->path_INTO[0] != "."));
					if (hasOutput)
					{
						std::vector<std::string>::const_iterator it;
						std::vector<std::string>::const_iterator end = di->path_INTO.end();
						for (it = di->path_INTO.begin( ); it != end; it++)
						{
							m_resultstruct->openTag( *it );
						}
					}
					m_resultstruct->addEmbeddedResult( *func->m_resultstruct, m_call.size()+1);
					if (hasOutput)
					{
						for (std::vector<std::string>::size_type i = 0; i < di->path_INTO.size(); i++)
						{
							m_resultstruct->closeTag();
						}
					}
				}
				FunctionCall paramstk( "", selector, param, false, false, 1 + 1/*level*/);
				m_call.push_back( paramstk);

				std::vector<FunctionCall>::const_iterator fsi = func->m_call.begin(), fse = func->m_call.end();
				int scope_functionidx_incr = m_call.size();
				for (; fsi != fse; ++fsi)
				{
					Path fselector = fsi->selector();
					fselector.rewrite( rwtab, scope_functionidx_incr);
					std::vector<Path> fparam = fsi->arg();
					std::vector<Path>::iterator fai = fparam.begin(), fae = fparam.end();
					for (; fai != fae; ++fai) fai->rewrite( rwtab, scope_functionidx_incr);
					FunctionCall cc( fsi->name(), fselector, fparam, false, false, fsi->level() + 1);
					m_call.push_back( cc);
				}
				if (!di->path_INTO.empty() && !di->path_INTO[0].empty( ) )
				{
					m_resultstruct->addMark( ResultElement::OperationEnd, m_call.size());
				}
			}
		}
		catch (const std::runtime_error& e)
		{
			throw Error( eidx, e.what());
		}
	}
	// Create substructure context for RESULT INTO instructions:
	if ((blkidx=hasCloseSubstruct( description.blocks, description.steps.size())) >= 0)
	{
		const std::vector<std::string>& ps = description.blocks.at(blkidx).path_INTO;
		std::vector<std::string>::const_iterator pi = ps.begin(), pe = ps.end();
		for (; pi != pe; ++pi)
		{
			m_resultstruct->closeTag();
		}
	}
}

TransactionFunction::Impl::Impl( const Impl& o)
	:m_resultstruct(o.m_resultstruct)
	,m_call(o.m_call)
	,m_tagmap(o.m_tagmap){}


TransactionFunction::TransactionFunction( const std::string& name_, const TransactionFunctionDescription& description, const types::keymap<TransactionFunctionR>& functionmap)
	:m_name(name_)
	,m_authorization( description.auth)
	,m_impl( new Impl( description, functionmap)){}

TransactionFunction::TransactionFunction( const TransactionFunction& o)
	:m_name(o.m_name)
	,m_authorization(o.m_authorization)
	,m_impl( new Impl( *o.m_impl)){}

TransactionFunction::~TransactionFunction()
{
	delete m_impl;
}

const char* TransactionFunction::getErrorHint( const std::string& errorclass, int functionidx) const
{
	if (functionidx < 0 || functionidx >= (int)m_impl->m_call.size()) return 0;
	return m_impl->m_call.at( functionidx).getErrorHint( errorclass);
}

TransactionFunctionInput* TransactionFunction::getInput() const
{
	return new TransactionFunctionInput( this);
}

TransactionFunctionOutput* TransactionFunction::getOutput( const db::TransactionOutput& o) const
{
	return new TransactionFunctionOutput( m_impl->m_resultstruct, o);
}

TransactionFunction* db::createTransactionFunction( const std::string& name, const TransactionFunctionDescription& description, const types::keymap<TransactionFunctionR>& functionmap)
{
	return new TransactionFunction( name, description, functionmap);
}


