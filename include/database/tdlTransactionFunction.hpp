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
///\file database/tdlTransactionFunction.hpp
#ifndef _DATABASE_TDL_TRANSACTION_FUNCTION_HPP_INCLUDED
#define _DATABASE_TDL_TRANSACTION_FUNCTION_HPP_INCLUDED
#include "vm/program.hpp"
#include "tdl/preprocBlock.hpp"
#include <string>
#include <vector>

namespace _Wolframe {
namespace db {

class TdlTransactionFunction
{
public:

	TdlTransactionFunction(){}
	TdlTransactionFunction( const TdlTransactionFunction& o)
		:m_resultfilter(o.m_resultfilter),m_authfunction(o.m_authfunction),m_authresource(o.m_authresource),m_preproc(o.m_preproc),m_program(o.m_program){}
	TdlTransactionFunction( const std::string& rf, const std::string& af, const std::string& ar, const PreProcBlock& pb, const ProgramR& prg)
		:m_resultfilter(rf),m_authfunction(af),m_authresource(ar),m_preproc(pb),m_program(prg){}

private:
	std::string m_resultfilter;
	std::string m_authfunction;
	std::string m_authresource;
	PreProcBlock m_preproc;
	ProgramR m_program;
};

typedef boost::shared_ptr<TdlTransactionFunction> TdlTransactionFunctionR;

}}//namespace
#endif

