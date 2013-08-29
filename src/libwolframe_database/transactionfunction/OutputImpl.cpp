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
///\brief Implementation of transaction function output filter
///\file transactionfunction/OutputImpl.cpp
#include "transactionfunction/OutputImpl.hpp"
#include "logger-v1.hpp"

using namespace _Wolframe;
using namespace _Wolframe::db;

TransactionFunctionOutput::Impl::Impl( const ResultStructureR& resultstruct_, const db::TransactionOutputR& data_)
	:m_valuestate(0)
	,m_colidx(0)
	,m_colend(0)
	,m_endofoutput(false)
	,m_resultstruct(resultstruct_)
	,m_data(data_)
{
	resetIterator();
}

TransactionFunctionOutput::Impl::Impl( const Impl& o)
	:m_valuestate(o.m_valuestate)
	,m_colidx(o.m_colidx)
	,m_colend(o.m_colend)
	,m_endofoutput(o.m_endofoutput)
	,m_started(o.m_started)
	,m_resultstruct(o.m_resultstruct)
	,m_structitr(o.m_structitr)
	,m_structend(o.m_structend)
	,m_stack(o.m_stack)
	,m_resitr(o.m_resitr)
	,m_resend(o.m_resend)
	,m_rowitr(o.m_rowitr)
	,m_rowend(o.m_rowend)
	,m_data(o.m_data)
{}

void TransactionFunctionOutput::Impl::resetIterator()
{
	m_valuestate = 0;
	m_colidx = 0;
	m_colend = 0;
	m_endofoutput = false;
	m_started = false;
	m_structitr = m_resultstruct->begin();
	m_structend = m_resultstruct->end();
	m_stack.clear();
	m_resitr = m_data->begin();
	m_resend = m_data->end();
}

std::string TransactionFunctionOutput::Impl::getResultGroup( const std::string& columnName)
{
	const char* cc = std::strchr( columnName.c_str(), '/');
	if (cc != 0)
	{
		std::string rt( columnName.c_str(), cc-columnName.c_str()+1);
		if (std::strchr( columnName.c_str()+rt.size(), '/') != 0)
		{
			throw std::runtime_error( "too complex grouping of result elements");
		}
		return rt;
	}
	return std::string();
}

bool TransactionFunctionOutput::Impl::hasResultGroup( const std::string& columnName)
{
	return (std::strchr( columnName.c_str(), '/') != 0);
}

bool TransactionFunctionOutput::Impl::endOfGroup()
{
	if (!m_group.empty())
	{
		if (m_colidx == m_colend || !boost::algorithm::istarts_with( m_resitr->columnName( m_colidx), m_group))
		{
			return true;
		}
	}
	return false;
}

bool TransactionFunctionOutput::Impl::getNext( ElementType& type, types::VariantConst& element, bool doSerializeWithIndices)
{
	if (!m_started)
	{
		LOG_DATA << "[transaction result structure] " << m_resultstruct->tostring();
		m_started = true;
	}
	while (m_structitr != m_structend)
	{
		if (!doSerializeWithIndices)
		{
			if (m_structitr->type == ResultElement::IndexStart || m_structitr->type == ResultElement::IndexEnd)
			{
				++m_structitr;
				continue;
			}
		}
		LOG_DATA << "[transaction result stm] State " << ResultElement::typeName( m_structitr->type) << " " << m_structitr->idx << " '" << (m_structitr->value?m_structitr->value:"") << "'";
		switch (m_structitr->type)
		{
			case ResultElement::OpenTag:
				type = TypedInputFilter::OpenTag;
				element = m_structitr->value;
				++m_structitr;
				return true;

			case ResultElement::CloseTag:
				type = TypedInputFilter::CloseTag;
				element.init();
				++m_structitr;
				return true;

			case ResultElement::OperationStart:
				m_stack.push_back( StackElement( ResultElement::OperationEnd, m_structitr));
				++m_structitr;
				continue;

			case ResultElement::FunctionStart:
				if (m_resitr == m_resend || m_resitr->functionidx() > m_structitr->idx)
				{
					for (++m_structitr; m_structitr != m_structend && m_structitr->type != ResultElement::FunctionEnd; ++m_structitr);
					if (m_structitr == m_structend) throw std::logic_error("illegal stack in transaction result iterator");
					++m_structitr;
					continue;
				}
				else if (m_resitr->functionidx() < m_structitr->idx || m_resitr->begin() == m_resitr->end())
				{
					++m_resitr;
					continue;
				}
				m_stack.push_back( StackElement( ResultElement::FunctionEnd, m_structitr));
				++m_structitr;
				m_rowitr = m_resitr->begin();
				m_rowend = m_resitr->end();
				m_colidx = 0;
				m_colend = m_resitr->nofColumns();
				if (hasResultGroup( m_resitr->columnName( m_colidx)))
				{
					m_group = getResultGroup( m_resitr->columnName( m_colidx));
					type = TypedInputFilter::OpenTag;
					element = types::VariantConst( m_group.c_str(), m_group.size()-1);
					return true;
				}
				continue;

			case ResultElement::IndexStart:
				m_stack.push_back( StackElement( ResultElement::IndexEnd, m_structitr));
				++m_structitr;
				type = TypedInputFilter::OpenTag;
				element = types::VariantConst( (unsigned int)++m_stack.back().m_cnt);
				return true;

			case ResultElement::IndexEnd:
				if (m_valuestate == StateIndexCloseTag)
				{
					m_valuestate = StateIndexNext;
					type = TypedInputFilter::CloseTag;
					element.init();
					return true;
				}
				m_valuestate = StateIndexCloseTag;
				if (m_stack.back().m_type != m_structitr->type)
				{
					throw std::logic_error( "illegal state in transaction result construction");
				}
				if (m_resitr != m_resend && m_resitr->functionidx() <= m_stack.back().m_structitr->idx)
				{
					m_structitr = m_stack.back().m_structitr;
					type = TypedInputFilter::OpenTag;
					element = types::VariantConst( (unsigned int)++m_stack.back().m_cnt);
					++m_structitr;
					return true;
				}
				else
				{
					m_stack.pop_back();
				}
				++m_structitr;
				continue;

			case ResultElement::OperationEnd:
				if (m_stack.back().m_type != m_structitr->type)
				{
					throw std::logic_error( "illegal state in transaction result construction (OperationEnd)");
				}
				if (m_resitr != m_resend && m_resitr->functionidx() <= m_stack.back().m_structitr->idx)
				{
					m_structitr = m_stack.back().m_structitr;
				}
				else
				{
					m_stack.pop_back();
				}
				++m_structitr;
				continue;

			case ResultElement::FunctionEnd:
				if (m_stack.back().m_type != m_structitr->type)
				{
					throw std::logic_error( "illegal state in transaction result construction (FunctionEnd)");
				}
				if (m_resitr != m_resend && m_resitr->functionidx() <= m_stack.back().m_structitr->idx)
				{
					m_structitr = m_stack.back().m_structitr;
				}
				else
				{
					m_stack.pop_back();
				}
				++m_structitr;
				continue;

			case ResultElement::Value:
				if (m_valuestate == StateColumnEndGroup)
				{
					m_group.clear();
					type = TypedInputFilter::CloseTag;
					element.init();
					m_valuestate = StateColumnOpenTag;
					return true;
				}
				if (m_valuestate == StateColumnOpenTag)
				{
					if (m_rowitr == m_rowend)
					{
						++m_structitr;
						++m_resitr;
						if (m_resitr != m_resend)
						{
							m_rowitr = m_resitr->begin();
							m_rowend = m_resitr->end();
							m_colidx = 0;
							m_colend = m_resitr->nofColumns();

							if (hasResultGroup( m_resitr->columnName( m_colidx)))
							{
								m_group = getResultGroup( m_resitr->columnName( m_colidx));
								type = TypedInputFilter::OpenTag;
								element = types::VariantConst( m_group.c_str(), m_group.size()-1);
								return true;
							}
						}
						continue;
					}
					if (m_colidx == m_colend)
					{
						++m_structitr;
						++m_rowitr;
						if (m_rowitr == m_rowend)
						{
							++m_resitr;
							if (m_resitr != m_resend)
							{
								m_rowitr = m_resitr->begin();
								m_rowend = m_resitr->end();
								m_colidx = 0;
								m_colend = m_resitr->nofColumns();
							}
						}
						else
						{
							m_colidx = 0;
						}
						continue;
					}
					if (m_group.empty() && hasResultGroup( m_resitr->columnName( m_colidx)))
					{
						m_group = getResultGroup( m_resitr->columnName( m_colidx));
						type = TypedInputFilter::OpenTag;
						element = types::VariantConst( m_group.c_str(), m_group.size()-1);
						return true;
					}
					if (!m_rowitr->at(m_colidx).defined())
					{
						++m_colidx;
						if (endOfGroup())
						{
							m_group.clear();
							type = TypedInputFilter::CloseTag;
							element.init();
							return true;
						}
						continue;
					}
					type = TypedInputFilter::OpenTag;
					if (m_group.empty())
					{
						element = m_resitr->columnName( m_colidx);
					}
					else
					{
						element = m_resitr->columnName( m_colidx).c_str() + m_group.size();
					}
					if (element.type() == types::Variant::string_ && element.charsize() == 0)
					{
						//... untagged content value (column name '_')
						type = langbind::TypedInputFilter::Value;
						element = m_rowitr->at(m_colidx);
						++m_colidx;
						if (endOfGroup())
						{
							m_valuestate = StateColumnEndGroup;
							return true;
						}
						m_valuestate = StateColumnOpenTag;
						return true;
					}
					else
					{
						m_valuestate = StateColumnValue;
						return true;
					}
				}
				if (m_valuestate == StateColumnValue)
				{
					type = langbind::TypedInputFilter::Value;
					element = m_rowitr->at(m_colidx);
					m_valuestate = StateColumnCloseTag;
					return true;
				}
				if (m_valuestate == StateColumnCloseTag)
				{
					type = langbind::TypedInputFilter::CloseTag;
					element.init();
					++m_colidx;
					if (endOfGroup())
					{
						m_valuestate = StateColumnEndGroup;
					}
					else
					{
						m_valuestate = StateColumnOpenTag;
					}
					return true;
				}
				throw std::logic_error( "illegal state (transaction result iterator)");
		}
	}
	if (!m_stack.empty())
	{
		throw std::logic_error( "illegal state (stack not empty after iterating transaction result)");
	}
	if (!m_endofoutput)
	{
		type = TypedInputFilter::CloseTag;
		element.init();
		m_endofoutput = true;
		return true;
	}
	return false;
}


