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
///\brief Implementation of printing programs
///\file prgbind_printProgram.cpp

#include "prgbind/printProgram.hpp"
#include "langbind/appObjects.hpp"
#include "utils/miscUtils.hpp"
#include "filter/singlefilter.hpp"
#include <boost/algorithm/string.hpp>

using namespace _Wolframe;
using namespace _Wolframe::prgbind;

class PrintFunctionClosure
	:public langbind::FormFunctionClosure
	,public langbind::PrintFunctionClosure
{
public:
	PrintFunctionClosure( const prnt::PrintFunction* f)
		:types::TypeSignature("prgbind::PrintFunctionClosure", __LINE__)
		,langbind::PrintFunctionClosure(f){}

	virtual bool call()
	{
		return langbind::PrintFunctionClosure::call();
	}

	virtual void init( const proc::ProcessorProvider*, const langbind::TypedInputFilterR& i, serialize::Context::Flags)
	{
		langbind::PrintFunctionClosure::init(i);
	}

	virtual langbind::TypedInputFilterR result() const
	{
		langbind::TypedInputFilter::Element elem( langbind::PrintFunctionClosure::result());
		return langbind::TypedInputFilterR( new langbind::SingleElementInputFilter( elem));
	}
};

class PrintFunction
	:public langbind::FormFunction
{
public:
	PrintFunction( const prnt::PrintFunctionR& f)
		:m_impl(f){}

	virtual PrintFunctionClosure* createClosure() const
	{
		return new PrintFunctionClosure( m_impl.get());
	}

private:
	const proc::ProcessorProvider* m_provider;
	prnt::PrintFunctionR m_impl;
};


bool PrintProgram::is_mine( const std::string& filename) const
{
	std::string ext = utils::getFileExtension( filename);
	if (ext.empty()) return false;
	return boost::iequals( ext.c_str()+1, m_constructor->name());
}

void PrintProgram::loadProgram( ProgramLibrary& library, db::Database*, const std::string& filename)
{
	prnt::PrintFunctionR function( m_constructor->object( utils::readSourceFileContent( filename)));
	std::string name = function->name();
	if (name.empty())
	{
		name = utils::getFileStem( filename);
	}
	library.defineFormFunction( name, langbind::FormFunctionR( new PrintFunction( function)));
}


