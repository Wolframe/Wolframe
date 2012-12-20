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
///\brief Implements the list of all initially defined program types
///\file programLibrary.cpp

#include "prgbind/programLibrary.hpp"
#include "prgbind/program.hpp"
#include "prgbind/transactionProgram.hpp"
#include "prgbind/ddlProgram.hpp"
#include "prgbind/printProgram.hpp"
#include "prgbind/normalizeProgram.hpp"
#include "database/transactionFunction.hpp"
#include "prnt/printFunction.hpp"
#include "langbind/normalizeProgram.hpp"
#include "module/normalizeFunctionBuilder.hpp"
#include "langbind/formFunction.hpp"
#include "langbind/appObjects.hpp"
#include "logger-v1.hpp"
#include <algorithm>

using namespace _Wolframe;
using namespace _Wolframe::prgbind;

class BuiltInFunctionClosure
	:public virtual types::TypeSignature
	,public langbind::FormFunctionClosure
{
public:
	BuiltInFunctionClosure( const langbind::BuiltInFunction& f)
		:types::TypeSignature("prgbind::BuiltInFunctionClosure", __LINE__)
		,m_func(f)
		,m_state(0)
		,m_param_data(f.api_param())
		,m_result_data(f.api_result())
		,m_result(langbind::TypedInputFilterR( new serialize::StructSerializer( m_result_data.data(),m_result_data.descr())))
		,m_parser(m_param_data.data(),m_param_data.descr()){}

	BuiltInFunctionClosure( const BuiltInFunctionClosure& o)
		:types::TypeSignature(o)
		,m_func(o.m_func)
		,m_state(0)
		,m_param_data(o.m_param_data)
		,m_result_data(o.m_result_data)
		,m_result(o.m_result)
		,m_parser(o.m_parser)
		{}

	virtual bool call()
	{
		void* param_struct = m_param_data.get();
		void* result_struct = m_result_data.get();
		switch (m_state)
		{
			case 0:
				if (!m_parser.call()) return false;
				m_state = 1;
			case 1:
				int rt = m_func.call( result_struct, param_struct);
				if (rt != 0)
				{
					std::ostringstream msg;
					msg << "error in call of form function (return code " << rt << ")";
					throw std::runtime_error( msg.str());
				}
				m_state = 2;
		}
		return true;
	}

	virtual void init( const proc::ProcessorProvider*, const langbind::TypedInputFilterR& i, serialize::Context::Flags f)
	{
		m_parser.init( i, f);
	}

	virtual langbind::TypedInputFilterR result() const
	{
		return m_result;
	}

private:
	langbind::BuiltInFunction m_func;
	int m_state;
	langbind::ApiFormData m_param_data;
	langbind::ApiFormData m_result_data;
	langbind::TypedInputFilterR m_result;
	serialize::StructParser m_parser;
};

class BuiltInFunction
	:public langbind::FormFunction
{
public:
	BuiltInFunction( const langbind::BuiltInFunction& f)
		:m_impl(f){}

	virtual langbind::FormFunctionClosure* createClosure() const
	{
		return new BuiltInFunctionClosure( m_impl);
	}

private:
	const proc::ProcessorProvider* m_provider;
	langbind::BuiltInFunction m_impl;
};


class ProgramLibrary::Impl
	:public ddl::TypeMap
{
public:
	types::keymap<langbind::NormalizeFunctionConstructorR> m_normalizeFunctionConstructorMap;
	types::keymap<langbind::NormalizeFunctionR> m_normalizeFunctionMap;
	types::keymap<langbind::FormFunctionR> m_formFunctionMap;
	types::keymap<ddl::Form> m_formMap;
	std::vector<ProgramR> m_programTypes;

	Impl()
	{
		m_programTypes.push_back( ProgramR( new TransactionDefinitionProgram()));
		m_programTypes.push_back( ProgramR( new NormalizeProgram()));
	}

	Impl( const Impl& o)
		:m_programTypes(o.m_programTypes){}

	void defineBuiltInFunction( const std::string& name, const BuiltInFunction& f)
	{
		m_formFunctionMap.insert( name, langbind::FormFunctionR( new BuiltInFunction( f)));
	}

	void defineFormFunction( const std::string& name, langbind::FormFunctionR f)
	{
		m_formFunctionMap.insert( name, f);
	}

	void defineNormalizeFunction( const std::string& name, langbind::NormalizeFunctionR f)
	{
		m_normalizeFunctionMap.insert( name, f);
	}

	void defineForm( const std::string& name, const ddl::Form& f)
	{
		m_formMap.insert( name, f);
	}

	void defineFormDDL( const ddl::DDLCompilerR& constructor_)
	{
		DDLProgram* prg = new DDLProgram( constructor_);
		m_programTypes.push_back( ProgramR( prg));
	}

	void definePrintLayoutType( const module::PrintFunctionConstructorR& constructor_)
	{
		PrintProgram* prg = new PrintProgram( constructor_);
		m_programTypes.push_back( ProgramR( prg));
	}

	void defineNormalizeFunctionConstructor( const langbind::NormalizeFunctionConstructorR& f)
	{
		m_normalizeFunctionConstructorMap.insert( std::string(f->domain()), f);
	}

	const langbind::FormFunction* getFormFunction( const std::string& name) const
	{
		types::keymap<langbind::FormFunctionR>::const_iterator fi = m_formFunctionMap.find( name);
		if (fi == m_formFunctionMap.end()) return 0;
		return fi->second.get();
	}

	const langbind::NormalizeFunction* getNormalizeFunction( const std::string& name) const
	{
		types::keymap<langbind::NormalizeFunctionR>::const_iterator fi = m_normalizeFunctionMap.find( name);
		if (fi == m_normalizeFunctionMap.end()) return 0;
		return fi->second.get();
	}

	virtual const ddl::NormalizeFunction* getType( const std::string& name) const
	{
		return getNormalizeFunction( name);
	}

	static bool programOrderAsc( std::pair<Program*, std::string> const& a, std::pair<Program*, std::string> const& b)
	{
		return ((int)a.first->category()) < ((int)b.first->category());
	}

	void loadPrograms( ProgramLibrary& library, db::Database* transactionDB, const std::list<std::string>& filenames)
	{
		std::vector< std::pair<Program*, std::string> > typed_filenames;

		std::list<std::string>::const_iterator fi = filenames.begin(), fe = filenames.end();
		for (; fi != fe; ++fi)
		{
			std::vector<ProgramR>::const_iterator pi = m_programTypes.begin(), pe = m_programTypes.end();
			for (; pi != pe; ++pi)
			{
				if ((*pi)->is_mine( *fi))
				{
					typed_filenames.push_back( std::pair<Program*, std::string>(pi->get(), *fi));
					break;
				}
			}
			if (pi == pe)
			{
				throw std::runtime_error( std::string("unknown type of program '") + *fi + "'");
			}
		}
		std::sort( typed_filenames.begin(), typed_filenames.end(), programOrderAsc);

		std::vector< std::pair<Program*, std::string> >::const_iterator ti = typed_filenames.begin(), te = typed_filenames.end();
		for (; ti != te; ++ti)
		{
			LOG_TRACE << "Loading program '" << ti->second << "'";
			ti->first->loadProgram( library, transactionDB, ti->second);
		}
	}
};

ProgramLibrary::ProgramLibrary()
	:m_impl(new Impl())
{}

ProgramLibrary::ProgramLibrary( const ProgramLibrary& o)
	:m_impl(new Impl( *o.m_impl))
{}

ProgramLibrary::~ProgramLibrary()
{
	delete m_impl;
}

void ProgramLibrary::defineBuiltInFunction( const std::string& name, const langbind::BuiltInFunction& f)
{
	m_impl->defineBuiltInFunction( name, f);
}

void ProgramLibrary::defineFormFunction( const std::string& name, langbind::FormFunctionR f)
{
	m_impl->defineFormFunction( name, f);
}

void ProgramLibrary::defineNormalizeFunctionConstructor( const langbind::NormalizeFunctionConstructorR& f)
{
	m_impl->defineNormalizeFunctionConstructor( f);
}

void ProgramLibrary::defineNormalizeFunction( const std::string& name, const langbind::NormalizeFunctionR& f) const
{
	m_impl->defineNormalizeFunction( name, f);
}

void ProgramLibrary::defineForm( const std::string& name, const ddl::Form& f)
{
	m_impl->defineForm( name, f);
}

void ProgramLibrary::defineFormDDL( const ddl::DDLCompilerR& constructor_)
{
	m_impl->defineFormDDL( constructor_);
}

void ProgramLibrary::definePrintLayoutType( const module::PrintFunctionConstructorR& constructor_)
{
	m_impl->definePrintLayoutType( constructor_);
}

const ddl::TypeMap* ProgramLibrary::formtypemap() const
{
	return m_impl;
}

const types::keymap<langbind::NormalizeFunctionConstructorR>& ProgramLibrary::normalizeFunctionConstructorMap() const
{
	return m_impl->m_normalizeFunctionConstructorMap;
}

const langbind::FormFunction* ProgramLibrary::getFormFunction( const std::string& name) const
{
	return m_impl->getFormFunction( name);
}

const langbind::NormalizeFunction* ProgramLibrary::getNormalizeFunction( const std::string& name) const
{
	return m_impl->getNormalizeFunction( name);
}

void ProgramLibrary::loadPrograms( db::Database* transactionDB, const std::list<std::string>& filenames)
{
	m_impl->loadPrograms( *this, transactionDB, filenames);
}

