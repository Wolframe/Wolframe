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
///\brief Definition of a transaction
///\file tdl/transactionDefinition.hpp
#ifndef _DATABASE_TDL_TRANSACTION_DEFINITION_HPP_INCLUDED
#define _DATABASE_TDL_TRANSACTION_DEFINITION_HPP_INCLUDED
#include "database/databaseLanguage.hpp"
#include "tdl/preprocBlock.hpp"
#include <string>
#include <vector>
#include <stdexcept>

namespace _Wolframe {
namespace db {
namespace tdl {

struct ResultDefinition
{
	std::vector<std::string> path;
	std::string filter;

	ResultDefinition(){}
	ResultDefinition( const ResultDefinition& o)
		:path(o.path),filter(o.filter){}
	ResultDefinition( const std::vector<std::string>& path_, const std::string filter_)
		:path(path_),filter(filter_){}

	void clear();
	static ResultDefinition parse( const LanguageDescription* langdescr, std::string::const_iterator& si, const std::string::const_iterator& se);
};

struct TransactionDefinition
{
	ResultDefinition result;
	AuthorizeDefinition authorization;
	PreProcBlock preproc;

	TransactionDefinition(){}
	TransactionDefinition( const TransactionDefinition& o)
		:preproc(o.preproc),result(o.result),authorization(o.authorization){}
	TransactionDefinition( const ResultDefinition& r, const AuthorizeDefinition a, const PreProcBlock& p)
		:preproc(p),result(r){}
};
}}}//namespace
#endif

