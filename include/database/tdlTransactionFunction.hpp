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
//\brief Definition of a transaction function based on TDL
//\file database/tdlTransactionFunction.hpp
#ifndef _DATABASE_TDL_TRANSACTION_FUNCTION_HPP_INCLUDED
#define _DATABASE_TDL_TRANSACTION_FUNCTION_HPP_INCLUDED
#include "database/vm/program.hpp"
#include "database/vmTransactionInput.hpp"
#include "database/vmTransactionOutput.hpp"
#include "database/tdlTransactionPreprocStep.hpp"
#include "filter/typedfilter.hpp"
#include <string>
#include <vector>
#include <iostream>
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace db {
namespace tf {
//\brief Forward declaration
class InputStructure;
typedef boost::shared_ptr<InputStructure> InputStructureR;
}

//\brief Forward declaration
class TdlTransactionFunction;

//\class TdlTransactionFunctionInput
//\brief Input structure for calling transaction functions
class TdlTransactionFunctionInput
	:public langbind::TypedOutputFilter
{
public:
	explicit TdlTransactionFunctionInput( const TdlTransactionFunction* func_);
	TdlTransactionFunctionInput( const TdlTransactionFunctionInput& o);
	virtual ~TdlTransactionFunctionInput();

	//\brief Get a self copy
	//\return allocated pointer to copy of this
	virtual langbind::TypedOutputFilter* copy() const
	{
		return new TdlTransactionFunctionInput(*this);
	}

	virtual bool print( ElementType type, const types::VariantConst& element);
	void finalize( const proc::ProcessorProviderInterface* provider);

	virtual VmTransactionInput get() const;

	const tf::InputStructure& structure() const
	{
		return *m_structure.get();
	}

	const TdlTransactionFunction* func() const
	{
		return m_func;
	}

private:
	tf::InputStructureR m_structure;
	const TdlTransactionFunction* m_func;
	langbind::TypedInputFilter::ElementType m_lasttype;
};

class TdlTransactionFunction
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

private:
	std::string m_name;
	std::string m_resultfilter;
	std::string m_authfunction;
	std::string m_authresource;
	std::vector<TdlTransactionPreprocStep> m_preproc;
	vm::ProgramR m_program;
};

typedef boost::shared_ptr<TdlTransactionFunction> TdlTransactionFunctionR;

}}//namespace
#endif

