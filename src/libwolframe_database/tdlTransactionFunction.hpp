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
///\brief Definition of a transaction function based on TDL
///\file tdlTransactionFunction.hpp
#ifndef _DATABASE_TDL_TRANSACTION_FUNCTION_HPP_INCLUDED
#define _DATABASE_TDL_TRANSACTION_FUNCTION_HPP_INCLUDED
#include "database/vmTransactionInput.hpp"
#include "database/vmTransactionOutput.hpp"
#include "langbind/formFunction.hpp"
#include "processor/execContext.hpp"
#include "filter/redirectFilterClosure.hpp"
#include "filter/typedfilter.hpp"
#include "vm/program.hpp"
#include "tdlTransactionPreprocStep.hpp"
#include <string>
#include <vector>
#include <iostream>
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace db {
namespace vm {
///\brief Forward declaration
class InputStructure;
typedef boost::shared_ptr<InputStructure> InputStructureR;
}

///\brief Forward declaration
class TdlTransactionFunction;

///\///\brief 
///\brief Closure (execution context) of a transaction function based on TDL
class TdlTransactionFunctionClosure
	:public utils::TypeSignature
	,public langbind::FormFunctionClosure
{
public:
	TdlTransactionFunctionClosure( const db::TdlTransactionFunction* f);
	TdlTransactionFunctionClosure( const TdlTransactionFunctionClosure& o);

	virtual bool call();

	virtual void init( const proc::ProcessorProviderInterface* p, const langbind::TypedInputFilterR& i, serialize::Context::Flags f);

	virtual langbind::TypedInputFilterR result() const;

private:
	class InputStructure;
	const proc::ProcessorProviderInterface* m_provider;	//< processor provider to get transaction object
	const TdlTransactionFunction* m_func;			//< function to execute
	int m_state;						//< current state of call
	langbind::RedirectFilterClosure m_input;		//< builder of structure from input
	InputStructure* m_inputstructptr;			//< input structure implementation interface
	langbind::TypedOutputFilterR m_inputstruct;		//< input structure
	langbind::TypedInputFilterR m_result;			//< function call result
	serialize::Context::Flags m_flags;			//< flags for input serialization
};


class TdlTransactionFunction
	:public langbind::FormFunction
{
public:
	TdlTransactionFunction(){}
	TdlTransactionFunction( const TdlTransactionFunction& o)
		:m_resultfilter(o.m_resultfilter),m_authfunction(o.m_authfunction),m_authresource(o.m_authresource),m_preproc(o.m_preproc),m_program(o.m_program){}
	TdlTransactionFunction( const std::string& name_, const std::string& rf, const std::string& af, const std::string& ar, const std::vector<TdlTransactionPreprocStep>& pp, const vm::ProgramR& prg)
		:m_name(name_),m_resultfilter(rf),m_authfunction(af),m_authresource(ar),m_preproc(pp),m_program(prg){}

	void print( std::ostream& out) const;

	const std::string& name() const					{return m_name;}
	const std::string& resultfilter() const				{return m_resultfilter;}
	const std::string& authfunction() const				{return m_authfunction;}
	const std::string& authresource() const				{return m_authresource;}
	const std::vector<TdlTransactionPreprocStep>& preproc() const	{return m_preproc;}
	const vm::ProgramR& program() const				{return m_program;}

	////\brief Build the function output
	langbind::TypedInputFilterR getOutput( const proc::ProcessorProviderInterface* provider, const VmTransactionOutput& output) const;

	virtual TdlTransactionFunctionClosure* createClosure() const;

private:
	std::string m_name;		 			//< function name
	std::string m_resultfilter;				//< name of result filter function to call with the transaction result
	std::string m_authfunction;				//< authorization function name
	std::string m_authresource;				//< authorization resource name
	std::vector<TdlTransactionPreprocStep> m_preproc;	//< preprocessing steps to perform on input before transaction execution
	vm::ProgramR m_program;					//< program to execute
};

typedef boost::shared_ptr<TdlTransactionFunction> TdlTransactionFunctionR;

}}//namespace
#endif

