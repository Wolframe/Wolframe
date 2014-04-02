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
///\brief Definition of a block of preprocessing calls
///\file tdl/preprocBlock.hpp
#ifndef _DATABASE_TDL_PREPROC_BLOCK_HPP_INCLUDED
#define _DATABASE_TDL_PREPROC_BLOCK_HPP_INCLUDED
#include "database/databaseLanguage.hpp"
#include "database/tdlTransactionPreprocStep.hpp"
#include "database/vm/program.hpp"
#include "tdl/preprocCallStatement.hpp"
#include <string>
#include <vector>

namespace _Wolframe {
namespace db {
namespace tdl {

struct PreProcStep
{
	std::string selector;
	std::vector<std::string> resultpath;
	PreProcCallStatement statement;

	PreProcStep(){}
	PreProcStep( const PreProcStep& o)
		:selector(o.selector),resultpath(o.resultpath),statement(o.statement){}
	PreProcStep( const std::string& selector_, const std::vector<std::string>& resultpath_, const PreProcCallStatement& statement_)
		:selector(selector_),resultpath(resultpath_),statement(statement_){}

	void clear();
};

struct PreProcBlock
{
	std::vector<PreProcStep> steps;

	PreProcBlock(){}
	PreProcBlock( const PreProcBlock& o)
		:steps(o.steps){}
	explicit PreProcBlock( const std::vector<PreProcStep>& steps_)
		:steps(steps_){}

	static PreProcBlock parse( const LanguageDescription* langdescr, std::string::const_iterator& si, const std::string::const_iterator& se);

	std::vector<TdlTransactionPreprocStep> build( vm::Program* prg) const;
};

}}}//namespace
#endif

