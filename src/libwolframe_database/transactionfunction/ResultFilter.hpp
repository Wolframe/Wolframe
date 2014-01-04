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
///\brief Post Filter for result output of transaction function
///\file transactionfunction/ResultFilter.hpp
#ifndef _DATABASE_TRANSACTION_FUNCTION_RESULT_FILTER_HPP_INCLUDED
#define _DATABASE_TRANSACTION_FUNCTION_RESULT_FILTER_HPP_INCLUDED
#include "filter/typedfilter.hpp"
#include "langbind/formFunction.hpp"
#include "transactionfunction/ResultIterator.hpp"
#include "processor/procProvider.hpp"
#include <string>
#include <vector>

namespace _Wolframe {
namespace db {

class ResultFilter
{
public:
	ResultFilter( const proc::ProcessorProvider* provider_, const std::string& filtername_, const ResultStructureR& resultstruct_, const TransactionOutputR& data_);
	ResultFilter( const ResultFilter& o);
	virtual ~ResultFilter(){}

	langbind::TypedInputFilterR getOutput() const		{return m_result;}

private:
	const langbind::FormFunction* m_func;
	langbind::TypedInputFilterR m_dbres;
	langbind::FormFunctionClosureR m_filterres;
	langbind::TypedInputFilterR m_result;
};

}}//namespace
#endif



