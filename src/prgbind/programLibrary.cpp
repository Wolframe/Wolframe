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
//\brief Implements the list of all initially defined program types
//\file programLibrary.cpp

#include "types/customDataNormalizer.hpp"
#include "prgbind/programLibrary.hpp"
#include "prgbind/program.hpp"
#include "prgbind/transactionProgram.hpp"
#include "prgbind/ddlProgram.hpp"
#include "prgbind/normalizeProgram.hpp"
#include "filter/null_filter.hpp"
#include "filter/filter.hpp"
#include "types/normalizeFunction.hpp"
#include "langbind/formFunction.hpp"
#include "logger-v1.hpp"
#include "utils/fileUtils.hpp"
#include <algorithm>

using namespace _Wolframe;
using namespace _Wolframe::prgbind;

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
		m_impl.insert( name, f);
	}
private:
	types::keymap<types::NormalizeFunctionR> m_impl;
};

class ProgramLibrary::Impl
{
public:
	types::keymap<langbind::AuthorizationFunctionR> m_authorizationFunctionMap;
	types::keymap<langbind::AuditFunctionR> m_auditFunctionMap;
	types::keymap<types::NormalizeFunctionType> m_normalizeFunctionTypeMap;
	types::keymap<types::CustomDataTypeR> m_customDataTypeMap;
	NormalizeFunctionMap m_normalizeFunctionMap;
	types::keymap<langbind::FormFunctionR> m_formFunctionMap;
	std::vector<langbind::RuntimeEnvironmentR> m_runtimeEnvironmentList;
	types::keymap<langbind::FilterTypeR> m_filterTypeMap;
	types::keymap<types::FormDescriptionR> m_formMap;
	std::vector<types::FormDescriptionR> m_privateFormList;
	std::vector<ProgramR> m_programTypes;
	std::string m_curfile;

	virtual ~Impl(){}
	Impl()
	{
		m_programTypes.push_back( ProgramR( new TransactionDefinitionProgram()));
		m_programTypes.push_back( ProgramR( new NormalizeProgram()));
		m_filterTypeMap.insert( "null", langbind::FilterTypeR( new langbind::NullFilterType()));
	}

	Impl( const Impl& o)
		:m_authorizationFunctionMap(o.m_authorizationFunctionMap)
		,m_auditFunctionMap(o.m_auditFunctionMap)
		,m_normalizeFunctionTypeMap(o.m_normalizeFunctionTypeMap)
		,m_customDataTypeMap(o.m_customDataTypeMap)
		,m_normalizeFunctionMap(o.m_normalizeFunctionMap)
		,m_formFunctionMap(o.m_formFunctionMap)
		,m_runtimeEnvironmentList(o.m_runtimeEnvironmentList)
		,m_filterTypeMap(o.m_filterTypeMap)
		,m_formMap(o.m_formMap)
		,m_privateFormList(o.m_privateFormList)
		,m_programTypes(o.m_programTypes)
		,m_curfile(o.m_curfile)
		{}

	void defineAuthorizationFunction( const std::string& name, const langbind::AuthorizationFunctionR& f)
	{
		m_authorizationFunctionMap.insert( name, f);
	}
	
	void defineAuditFunction( const std::string& name, const langbind::AuditFunctionR& f)
	{
		m_auditFunctionMap.insert( name, f);
	}
	
	void defineCppFormFunction( const std::string& name, const serialize::CppFormFunction& f)
	{
		m_formFunctionMap.insert( name, langbind::FormFunctionR( new serialize::CppFormFunction( f)));
	}

	void defineFormFunction( const std::string& name, const langbind::FormFunctionR& f)
	{
		m_formFunctionMap.insert( name, f);
	}

	void defineRuntimeEnvironment( const langbind::RuntimeEnvironmentR& env)
	{
		m_runtimeEnvironmentList.push_back( env);
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
		m_formMap.insert( name, f);
	}

	void defineFormDDL( const langbind::DDLCompilerR& c)
	{
		DDLProgram* prg = new DDLProgram( c);
		m_programTypes.push_back( ProgramR( prg));
	}

	void defineNormalizeFunctionType( const std::string& name, const types::NormalizeFunctionType& f)
	{
		m_normalizeFunctionTypeMap.insert( name, f);
	}

	void defineCustomDataType( const std::string& name, const types::CustomDataTypeR& t)
	{
		m_customDataTypeMap.insert( name, t);
	}

	void defineProgramType( const ProgramR& prg)
	{
		m_programTypes.push_back( prg);
	}

	void defineFilterType( const std::string& name, const langbind::FilterTypeR& f)
	{
		m_filterTypeMap.insert( name, f);
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

	const langbind::AuthorizationFunction* getAuthorizationFunction( const std::string& name) const
	{
		types::keymap<langbind::AuthorizationFunctionR>::const_iterator fi = m_authorizationFunctionMap.find( name);
		if (fi == m_authorizationFunctionMap.end()) return 0;
		return fi->second.get();
	}
	
	const langbind::AuditFunction* getAuditFunction( const std::string& name) const
	{
		types::keymap<langbind::AuditFunctionR>::const_iterator fi = m_auditFunctionMap.find( name);
		if (fi == m_auditFunctionMap.end()) return 0;
		return fi->second.get();
	}
	
	const langbind::FormFunction* getFormFunction( const std::string& name) const
	{
		types::keymap<langbind::FormFunctionR>::const_iterator fi = m_formFunctionMap.find( name);
		if (fi == m_formFunctionMap.end()) return 0;
		return fi->second.get();
	}

	const types::CustomDataType* getCustomDataType( const std::string& name) const
	{
		types::keymap<types::CustomDataTypeR>::const_iterator ti = m_customDataTypeMap.find( name);
		if (ti == m_customDataTypeMap.end()) return 0;
		return ti->second.get();
	}

	const types::NormalizeFunction* getNormalizeFunction( const std::string& name) const
	{
		return m_normalizeFunctionMap.get( name);
	}

	const types::NormalizeFunctionType* getNormalizeFunctionType( const std::string& name)
	{
		types::keymap<types::NormalizeFunctionType>::const_iterator fi = m_normalizeFunctionTypeMap.find( name);
		if  (fi == m_normalizeFunctionTypeMap.end()) return 0;
		return &fi->second;
	}

	const types::NormalizeFunctionMap* formtypemap() const
	{
		return &m_normalizeFunctionMap;
	}

	const langbind::FilterType* getFilterType( const std::string& name) const
	{
		types::keymap<langbind::FilterTypeR>::const_iterator fi = m_filterTypeMap.find( name);
		return (fi == m_filterTypeMap.end())?0:fi->second.get();
	}

	static bool programOrderAsc( std::pair<Program*, std::string> const& a, std::pair<Program*, std::string> const& b)
	{
		return ((int)a.first->category()) < ((int)b.first->category());
	}

	void loadPrograms( ProgramLibrary& library, db::Database* transactionDB, const std::vector<std::string>& filenames)
	{
		LOG_DEBUG << "Loading programs";

		// Loading programs enclosed in a runtime environment
		std::vector<langbind::RuntimeEnvironmentR>::const_iterator ri = m_runtimeEnvironmentList.begin(), re = m_runtimeEnvironmentList.end();
		for (; ri != re; ++ri)
		{
			std::vector<std::string> functions = (*ri)->functions();
			std::vector<std::string>::const_iterator fi = functions.begin(), fe = functions.end();
			for (; fi != fe; ++fi)
			{
				LOG_TRACE << "Function '" << *fi << "' registered in '" << (*ri)->name() << "' environment";
				m_formFunctionMap.insert( *fi, langbind::FormFunctionR( new langbind::RuntimeEnvironmentFormFunction( *fi, ri->get())));
			}
		}

		// Loading scripts
		std::vector< std::pair<Program*, std::string> > typed_filenames;

		std::vector<std::string>::const_iterator fi = filenames.begin(), fe = filenames.end();
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
			LOG_DEBUG << "Loading program '" << ti->second << "'";
			ti->first->loadProgram( library, transactionDB, m_curfile=ti->second);
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

void ProgramLibrary::defineAuthorizationFunction( const std::string& name, const langbind::AuthorizationFunctionR& f)
{
	m_impl->defineAuthorizationFunction( name, f);
}

void ProgramLibrary::defineAuditFunction( const std::string& name, const langbind::AuditFunctionR& f)
{
	m_impl->defineAuditFunction( name, f);
}

void ProgramLibrary::defineCppFormFunction( const std::string& name, const serialize::CppFormFunction& f)
{
	m_impl->defineCppFormFunction( name, f);
}

void ProgramLibrary::defineFormFunction( const std::string& name, const langbind::FormFunctionR& f)
{
	m_impl->defineFormFunction( name, f);
}

void ProgramLibrary::defineRuntimeEnvironment( const langbind::RuntimeEnvironmentR& env)
{
	m_impl->defineRuntimeEnvironment( env);
}

void ProgramLibrary::defineNormalizeFunctionType( const std::string& name, const types::NormalizeFunctionType& f)
{
	m_impl->defineNormalizeFunctionType( name, f);
}

void ProgramLibrary::defineCustomDataType( const std::string& name, const types::CustomDataTypeR& t)
{
	m_impl->defineCustomDataType( name, t);
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

void ProgramLibrary::defineFilterType( const std::string& name, const langbind::FilterTypeR& f)
{
	return m_impl->defineFilterType( name, f);
}

void ProgramLibrary::defineProgramType( const ProgramR& prg)
{
	m_impl->defineProgramType( prg);
}

const types::NormalizeFunctionMap* ProgramLibrary::formtypemap() const
{
	return m_impl->formtypemap();
}

const langbind::AuthorizationFunction* ProgramLibrary::getAuthorizationFunction( const std::string& name) const
{
	return m_impl->getAuthorizationFunction( name);
}

const langbind::AuditFunction* ProgramLibrary::getAuditFunction( const std::string& name) const
{
	return m_impl->getAuditFunction( name);
}

const types::CustomDataType* ProgramLibrary::getCustomDataType( const std::string& name) const
{
	return m_impl->getCustomDataType( name);
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

const types::NormalizeFunctionType* ProgramLibrary::getNormalizeFunctionType( const std::string& name) const
{
	return m_impl->getNormalizeFunctionType( name);
}

const langbind::FilterType* ProgramLibrary::getFilterType( const std::string& name) const
{
	return m_impl->getFilterType( name);
}

void ProgramLibrary::loadPrograms( db::Database* transactionDB, const std::vector<std::string>& filenames)
{
	m_impl->loadPrograms( *this, transactionDB, filenames);
}

