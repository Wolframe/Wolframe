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
///\brief Interface for programs loaded by the processor provider
///\file prgbind/program.hpp

#ifndef _PRGBIND_PROGRAM_HPP_INCLUDED
#define _PRGBIND_PROGRAM_HPP_INCLUDED
#include "database/database.hpp"
#include <string>
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace prgbind {

///\brief Forward declaration
class ProgramLibrary;

///\class Program
///\brief Interface for programs of a program library
struct Program
{
	///\enum Category
	///\brief Category of the program. Determines ascending load order (dependencies)
	enum Category
	{
		Core=10,
		FormType=20,
		Form=30,
		Function=40
	};

	explicit Program( Category category_)
		:m_category(category_){}

	virtual ~Program(){}

	virtual bool is_mine( const std::string& filename) const=0;
	virtual void loadProgram( ProgramLibrary& library, db::Database* transactionDB, const std::string& filename)=0;

	Category category() const		{return m_category;}

private:
	Category m_category;
};

typedef boost::shared_ptr<Program> ProgramR;

}}//namespace
#endif

