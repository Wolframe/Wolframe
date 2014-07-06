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
/// \file aaMapProgram.hpp
/// \brief Interface of the AaMap program type

#ifndef _Wolframe_PRGBIND_AAMAP_PROGRAM_HPP_INCLUDED
#define _Wolframe_PRGBIND_AAMAP_PROGRAM_HPP_INCLUDED
#include "prgbind/program.hpp"
#include "prgbind/programLibrary.hpp"
#include <string>

namespace _Wolframe {
namespace prgbind {

/// \class AaMapProgram
/// \brief Program type for AaMap function descriptions.
class AaMapProgram
	:public Program
{
public:
	/// \brief Default constructor
	AaMapProgram()
		:Program( SuperFunction){}

	/// \brief Destructor
	virtual ~AaMapProgram(){}

	/// \brief Implementation of Program::is_mine( const std::string&) const;
	virtual bool is_mine( const std::string& filename) const;

	/// \brief Implementation of Program::loadProgram( ProgramLibrary&, db::Database*, const std::string&);
	virtual void loadProgram( ProgramLibrary& library, db::Database* transactionDB, const std::string& filename);
};

}}//namespace
#endif

