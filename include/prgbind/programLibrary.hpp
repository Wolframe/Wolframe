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
///\brief Program library interface
///\file programLibrary.hpp

#ifndef _PRGBIND_PROGRAM_LIBRARY_HPP_INCLUDED
#define _PRGBIND_PROGRAM_LIBRARY_HPP_INCLUDED
#include "filter/filter.hpp"
#include "module/filterBuilder.hpp"
#include "database/database.hpp"
#include "langbind/builtInFunction.hpp"
#include "ddl/compilerInterface.hpp"
#include "prnt/printFunction.hpp"
#include "module/printFunctionBuilder.hpp"
#include "langbind/normalizeFunction.hpp"
#include "prgbind/program.hpp"
/*[-]*/#include "prgbind/transactionProgram.hpp"
/*[-]*/#include "prgbind/program.hpp"
/*[-]*/#include "database/transactionFunction.hpp"
/*[-]*/#include "langbind/normalizeProgram.hpp"
/*[-]*/#include "module/normalizeFunctionBuilder.hpp"
/*[-]*/#include "langbind/formFunction.hpp"
#include <string>
#include <list>


namespace _Wolframe {
namespace prgbind {

class ProgramLibrary
{
public:
	ProgramLibrary();
	ProgramLibrary( const ProgramLibrary& o);

	virtual ~ProgramLibrary();
	virtual void defineBuiltInFunction( const std::string& name, const langbind::BuiltInFunction& f);
	virtual void defineFormFunction( const std::string& name, langbind::FormFunctionR f);
	virtual void defineForm( const std::string& name, const ddl::Form& f);
	virtual void defineNormalizeFunctionConstructor( const langbind::NormalizeFunctionConstructorR& f);
	virtual void defineNormalizeFunction( const std::string& name, const langbind::NormalizeFunctionR& f) const;
	virtual void defineFormDDL( const ddl::DDLCompilerR& c);
	virtual void definePrintLayoutType( const module::PrintFunctionConstructorR& f);
	virtual void defineFilterConstructor( const module::FilterConstructorR& f);

	virtual const ddl::TypeMap* formtypemap() const;
	virtual const types::keymap<langbind::NormalizeFunctionConstructorR>& normalizeFunctionConstructorMap() const;

	virtual const langbind::FormFunction* getFormFunction( const std::string& name) const;
	virtual const langbind::NormalizeFunction* getNormalizeFunction( const std::string& name) const;
	virtual langbind::Filter* createFilter( const std::string& name, const std::string& arg ) const;
	virtual bool existsFilter( const std::string& name) const;

	virtual void loadPrograms( db::Database* transactionDB, const std::list<std::string>& filenames);

private:
	class Impl;
	Impl* m_impl;
};

}} //namespace
#endif

