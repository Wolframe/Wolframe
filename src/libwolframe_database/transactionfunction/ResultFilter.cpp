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
///\brief Implementation for transaction function result result filter
///\file transactionfunction/ResultFilter.cpp
#include "transactionfunction/ResultFilter.hpp"

using namespace _Wolframe;
using namespace _Wolframe::db;

namespace {
struct FilterData
	:public langbind::TypedInputFilter::Data
{
	FilterData( const langbind::FormFunction* f,
		const langbind::TypedInputFilterR& dr,
		const langbind::FormFunctionClosureR& fr)	:func(f),dbres(dr),filterres(fr){}

	FilterData( const FilterData& o)			:func(o.func),dbres(o.dbres),filterres(o.filterres){}

	virtual ~FilterData(){}
	virtual Data* copy() const				{return new FilterData(*this);}
private:
	const langbind::FormFunction* func;
	langbind::TypedInputFilterR dbres;
	langbind::FormFunctionClosureR filterres;
};
}

ResultFilter::ResultFilter( const proc::ProcessorProvider* provider_, const std::string& filtername_, const ResultStructureR& resultstruct_, const TransactionOutputR& data_)
	:m_func(0)
{
	m_dbres.reset( new ResultIterator( resultstruct_,data_));
	m_func = provider_->formFunction( filtername_);
	if (!m_func) throw std::runtime_error( std::string( "transaction result filter function '") + filtername_ + "' not found (must be defined as form function)");
	langbind::FormFunctionClosureR clos( m_func->createClosure());
	m_filterres = clos;
	m_filterres->init( provider_, m_dbres);

	if (!m_filterres->call())
	{
		throw std::runtime_error( std::string( "failed to call filter function '") + filtername_ + "' with result of transaction (input not complete)");
	}
	m_result = m_filterres->result();
	m_result->setData( new FilterData( m_func, m_dbres, m_filterres));
	if (m_dbres->flag( langbind::TypedInputFilter::PropagateNoCase))
	{
		m_result->setFlags( langbind::TypedInputFilter::PropagateNoCase);
	}
}

ResultFilter::ResultFilter( const ResultFilter& o)
	:m_func(o.m_func),m_dbres(o.m_dbres),m_filterres(o.m_filterres),m_result(o.m_result){}


