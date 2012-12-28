/************************************************************************

 Copyright (C) 2011, 2012 Project Wolframe.
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
#include "utils/miscUtils.hpp"
#include "database/loadTransactionProgram.hpp"
#include "langbind/appObjects.hpp"

using namespace _Wolframe;
using namespace _Wolframe::prgbind;

class TransactionFunctionClosure
	:public langbind::FormFunctionClosure
	,public langbind::TransactionFunctionClosure
{
public:
	TransactionFunctionClosure( const db::TransactionFunction* f)
		:types::TypeSignature("prgbind::TransactionFunctionClosure", __LINE__)
		,langbind::TransactionFunctionClosure(f){}

	virtual bool call()
	{
		return langbind::TransactionFunctionClosure::call();
	}

	virtual void init( const proc::ProcessorProvider* p, const langbind::TypedInputFilterR& i, serialize::Context::Flags)
	{
		langbind::TransactionFunctionClosure::init(p,i);
	}

	virtual langbind::TypedInputFilterR result() const
	{
		return langbind::TransactionFunctionClosure::result();
	}
};

class TransactionFunction
	:public langbind::FormFunction
{
public:
	TransactionFunction( const db::TransactionFunctionR& f)
		:m_impl(f){}

	virtual TransactionFunctionClosure* createClosure() const
	{
		return new TransactionFunctionClosure( m_impl.get());
	}

private:
	const proc::ProcessorProvider* m_provider;
	db::TransactionFunctionR m_impl;
};



bool TransactionDefinitionProgram::is_mine( const std::string& filename) const
{
	return boost::algorithm::to_lower_copy( utils::getFileExtension( filename)) == ".tdl";
}

void TransactionDefinitionProgram::loadProgram( ProgramLibrary& library, db::Database* transactionDB, const std::string& filename)
{
	std::string dbsource;
	types::keymap<std::string> embeddedStatementMap;
	static const db::LanguageDescription defaultLanguageDescr;
	const db::LanguageDescription* languageDescr = (transactionDB)?transactionDB->getLanguageDescription():&defaultLanguageDescr;

	std::vector<std::pair<std::string,db::TransactionFunctionR> > funclist
		= db::loadTransactionProgramFile( filename, languageDescr, dbsource, embeddedStatementMap);

	try
	{
		if (transactionDB)
		{
			transactionDB->addProgram( dbsource);
			transactionDB->addStatements( embeddedStatementMap);
		}
		std::vector<std::pair<std::string,db::TransactionFunctionR> >::const_iterator fi = funclist.begin(), fe = funclist.end();
		for (; fi != fe; ++fi)
		{
			langbind::FormFunctionR func( new TransactionFunction( fi->second));
			library.defineFormFunction( fi->first, func);
		}
	}
	catch (const std::runtime_error& e)
	{
		std::runtime_error( std::string( "error loading transaction definition program '") + filename + "': " + e.what());
	}
}


