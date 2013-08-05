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
#include "langbind/normalizeFunction.hpp"
#include "module/normalizeFunctionBuilder.hpp"
#include "langbind/formFunction.hpp"
#include "langbind/appObjects.hpp"
#include "logger/logger-v1.hpp"
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

class NormalizeFunctionMap
	:public types::NormalizeFunctionMap
{
public:
	virtual ~NormalizeFunctionMap(){}
	virtual const types::NormalizeFunction* get( const std::string& name) const
	{
		types::keymap<types::NormalizeFunctionR>::const_iterator fi = m_impl.find( name);
		if (fi == m_impl.end()) return 0;
		return fi->second.get();
	}
	void define( const std::string& name, types::NormalizeFunctionR f)
	{
		if (m_impl.find( name) != m_impl.end())
		{
			LOG_ERROR << std::string("duplicate definition of normalize function '") + name + "'";
			throw std::runtime_error( std::string("duplicate definition of normalize function '") + name + "'");
		}
		m_impl.insert( name, f);
	}
private:
	types::keymap<types::NormalizeFunctionR> m_impl;
};

class ProgramLibrary::Impl
{
public:
	types::keymap<module::NormalizeFunctionConstructorR> m_normalizeFunctionConstructorMap;
	NormalizeFunctionMap m_normalizeFunctionMap;
	types::keymap<langbind::FormFunctionR> m_formFunctionMap;
	types::keymap<module::FilterConstructorR> m_filterMap;
	types::keymap<types::FormDescriptionR> m_formMap;
	std::vector<types::FormDescriptionR> m_privateFormList;
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
		if (m_formFunctionMap.find( name) != m_formFunctionMap.end())
		{
			LOG_ERROR << std::string("duplicate definition of form function '") + name + "'";
			throw std::runtime_error( std::string("duplicate definition of form function '") + name + "'");
		}
		m_formFunctionMap.insert( name, langbind::FormFunctionR( new BuiltInFunction( f)));
	}

	void defineFormFunction( const std::string& name, langbind::FormFunctionR f)
	{
		if (m_formFunctionMap.find( name) != m_formFunctionMap.end())
		{
			LOG_ERROR << std::string("duplicate definition of form function '") + name + "'";
			throw std::runtime_error( std::string("duplicate definition of form function '") + name + "'");
		}
		m_formFunctionMap.insert( name, f);
	}

	void defineNormalizeFunction( const std::string& name, types::NormalizeFunctionR f)
	{
		m_normalizeFunctionMap.define( name, f);
	}

	void definePrivateForm( const types::FormDescriptionR& f)
	{
		m_privateFormList.push_back( f);
	}

	void defineForm( const std::string& name, const types::FormDescriptionR& f)
	{
		if (m_formMap.find( name) != m_formMap.end())
		{
			LOG_ERROR << std::string("duplicate definition of form '") + name + "'";
			throw std::runtime_error( std::string("duplicate definition of form '") + name + "'");
		}
		m_formMap.insert( name, f);
	}

	void defineFormDDL( const langbind::DDLCompilerR& c)
	{
		DDLProgram* prg = new DDLProgram( c);
		m_programTypes.push_back( ProgramR( prg));
	}

	void definePrintLayoutType( const module::PrintFunctionConstructorR& f)
	{
		PrintProgram* prg = new PrintProgram( f);
		m_programTypes.push_back( ProgramR( prg));
	}

	void defineNormalizeFunctionConstructor( const module::NormalizeFunctionConstructorR& f)
	{
		m_normalizeFunctionConstructorMap.insert( std::string(f->domain()), f);
	}

	void defineProgramType( const ProgramR& prg)
	{
		m_programTypes.push_back( prg);
	}

	void defineFilterConstructor( const module::FilterConstructorR& f)
	{
		if (m_filterMap.find( f->name()) != m_filterMap.end())
		{
			LOG_ERROR << std::string("duplicate definition of filter '") + f->name() + "'";
			throw std::runtime_error( std::string("duplicate definition of filter '") + f->name() + "'");
		}
		m_filterMap.insert( f->name(), f);
		if (!f->category().empty())
		{
			types::keymap<module::FilterConstructorR>::const_iterator fi = m_filterMap.find( f->category());
			if (fi == m_filterMap.end())
			{
				m_filterMap.insert( f->category(), f);
			}
		}
	}

	const types::FormDescription* getFormDescription( const std::string& name) const
	{
		types::keymap<types::FormDescriptionR>::const_iterator fi = m_formMap.find( name);
		if (fi == m_formMap.end()) return 0;
		return fi->second.get();
	}

	std::vector<std::string> getFormNames() const
	{
		return m_formMap.getkeys< std::vector<std::string> >();
	}

	const langbind::FormFunction* getFormFunction( const std::string& name) const
	{
		types::keymap<langbind::FormFunctionR>::const_iterator fi = m_formFunctionMap.find( name);
		if (fi == m_formFunctionMap.end()) return 0;
		return fi->second.get();
	}

	const types::NormalizeFunction* getNormalizeFunction( const std::string& name) const
	{
		return m_normalizeFunctionMap.get( name);
	}

	const types::NormalizeFunctionMap* formtypemap() const
	{
		return &m_normalizeFunctionMap;
	}

	langbind::Filter* createFilter( const std::string& name, const std::string& arg) const
	{
		types::keymap<module::FilterConstructorR>::const_iterator fi = m_filterMap.find( name);
		return (fi == m_filterMap.end())?0:fi->second->object( arg);
	}

	bool existsFilter( const std::string& name) const
	{
		types::keymap<module::FilterConstructorR>::const_iterator fi = m_filterMap.find( name);
		return (fi != m_filterMap.end());
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
				LOG_ERROR << std::string("unknown type of program '") + *fi + "'";
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

void ProgramLibrary::defineNormalizeFunctionConstructor( const module::NormalizeFunctionConstructorR& f)
{
	m_impl->defineNormalizeFunctionConstructor( f);
}

void ProgramLibrary::defineNormalizeFunction( const std::string& name, const types::NormalizeFunctionR& f) const
{
	m_impl->defineNormalizeFunction( name, f);
}

void ProgramLibrary::definePrivateForm( const types::FormDescriptionR& f)
{
	m_impl->definePrivateForm( f);
}

void ProgramLibrary::defineForm( const std::string& name, const types::FormDescriptionR& f)
{
	m_impl->defineForm( name, f);
}

void ProgramLibrary::defineFormDDL( const langbind::DDLCompilerR& c)
{
	m_impl->defineFormDDL( c);
}

void ProgramLibrary::definePrintLayoutType( const module::PrintFunctionConstructorR& f)
{
	m_impl->definePrintLayoutType( f);
}

void ProgramLibrary::defineFilterConstructor( const module::FilterConstructorR& f)
{
	return m_impl->defineFilterConstructor( f);
}

void ProgramLibrary::defineProgramType( const ProgramR& prg)
{
	m_impl->defineProgramType( prg);
}

const types::NormalizeFunctionMap* ProgramLibrary::formtypemap() const
{
	return m_impl->formtypemap();
}

const types::keymap<module::NormalizeFunctionConstructorR>& ProgramLibrary::normalizeFunctionConstructorMap() const
{
	return m_impl->m_normalizeFunctionConstructorMap;
}

const langbind::FormFunction* ProgramLibrary::getFormFunction( const std::string& name) const
{
	return m_impl->getFormFunction( name);
}

const types::FormDescription* ProgramLibrary::getFormDescription( const std::string& name) const
{
	return m_impl->getFormDescription( name);
}

std::vector<std::string> ProgramLibrary::getFormNames() const
{
	return m_impl->getFormNames();
}

const types::NormalizeFunction* ProgramLibrary::getNormalizeFunction( const std::string& name) const
{
	return m_impl->getNormalizeFunction( name);
}

langbind::Filter* ProgramLibrary::createFilter( const std::string& name, const std::string& arg) const
{
	return m_impl->createFilter( name, arg);
}

bool ProgramLibrary::existsFilter( const std::string& name) const
{
	return m_impl->existsFilter( name);
}

void ProgramLibrary::loadPrograms( db::Database* transactionDB, const std::list<std::string>& filenames)
{
	m_impl->loadPrograms( *this, transactionDB, filenames);
}

