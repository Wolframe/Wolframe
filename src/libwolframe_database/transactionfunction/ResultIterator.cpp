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
///\brief Implementation of iterator on transaction function result
///\file transactionfunction/ResultIterator.cpp
#include "transactionfunction/ResultIterator.hpp"
#include "logger-v1.hpp"

using namespace _Wolframe;
using namespace _Wolframe::db;

ResultIteratorImpl::ResultIteratorImpl( const ResultStructureR& resultstruct_, const db::TransactionOutputR& data_)
	:m_resultstruct(resultstruct_)
	,m_data(data_)
{
	reset();
}

ResultIteratorImpl::ResultIteratorImpl( const ResultIteratorImpl& o)
	:m_valuestate(o.m_valuestate)
	,m_colidx(o.m_colidx)
	,m_colend(o.m_colend)
	,m_endofoutput(o.m_endofoutput)
	,m_started(o.m_started)
	,m_nextresult(o.m_nextresult)
	,m_serializeWithIndices(o.m_serializeWithIndices)
	,m_group(o.m_group)
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

void ResultIteratorImpl::reset()
{
	m_valuestate = 0;
	m_colidx = 0;
	m_colend = 0;
	m_endofoutput = false;
	m_started = false;
	m_nextresult = false;
	m_serializeWithIndices = false;
	m_group.clear();
	m_structitr = m_resultstruct->begin();
	m_structend = m_resultstruct->end();
	m_stack.clear();
	m_resitr = m_data->begin();
	m_resend = m_data->end();
}

std::string ResultIteratorImpl::getResultGroup( const std::string& columnName)
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

bool ResultIteratorImpl::hasResultGroup( const std::string& columnName)
{
	return (std::strchr( columnName.c_str(), '/') != 0);
}

bool ResultIteratorImpl::endOfGroup()
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

static std::size_t resolveColumnName( const TransactionOutput::CommandResult& cmdres, const std::string& name)
{
	std::size_t ci = 0, ce = cmdres.nofColumns();
	for (; ci != ce; ++ci)
	{
		if (boost::algorithm::iequals( cmdres.columnName( ci), name))
		{
			return ci+1;
		}
	}
	throw std::runtime_error( std::string( "column name '") + name + "' not found in result");
}

static types::VariantConst resolveScopedReference( const TransactionOutput& output, const TransactionOutput::result_const_iterator& resitr, std::size_t scope_functionidx, const types::Variant& reference, const char* cmdname)
{
	types::VariantConst rt;
	TransactionOutput::result_const_iterator fi = output.end();
	std::size_t prev_functionidx = scope_functionidx;
	bool found = false;

	if (resitr == output.end()) return rt;
	TransactionOutput::result_const_iterator ri = resitr;
	for (;ri != output.begin(); --ri)
	{
		if (prev_functionidx < ri->functionidx()) break; //... crossing operation scope border
		prev_functionidx = ri->functionidx();

		if (scope_functionidx >= ri->functionidx())
		{
			fi = ri;
			found = true;
			break;
		}
	}
	if (found)
	{
		std::size_t colidx;
		if (reference.type() == types::Variant::String)
		{
			colidx = resolveColumnName( *fi, reference.tostring());
		}
		else
		{
			colidx = reference.touint();
		}
		if (colidx == 0 || colidx > fi->nofColumns())
		{
			throw std::runtime_error( std::string("referencing non existing result in '") + cmdname + "'");
		}
		if (ri->nofRows() > 1)
		{
			throw std::runtime_error( std::string("referencing set of elements in '") + cmdname + "' (only reference of single element allowed)");
		}
		if (ri->nofRows() > 0)
		{
			rt = fi->begin()->at( colidx-1);
		}
	}
	return rt;
}

TransactionOutput::result_const_iterator ResultIteratorImpl::endOfOperation()
{
	std::size_t si = m_stack.size();
	for (;si > 0; --si)
	{
		const StackElement& se = m_stack.at(si-1);
		if (se.m_type == ResultElement::OperationEnd)
		{
			TransactionOutput::result_const_iterator ri = se.m_resitr, re = m_resend;
			TransactionOutput::result_const_iterator prev = ri;
			if (ri == re) return ri;
			for (++ri; ri != re; ++ri)
			{
				if (prev->functionidx() > ri->functionidx()) return prev;
			}
			return prev;
		}
	}
	return m_resend;
}

bool ResultIteratorImpl::skipStruct( int start, int end)
{
	bool doSkip;
	if (m_resitr == m_resend)
	{
		doSkip = true;
	}
	else if (m_nextresult)
	{
		doSkip = (int)m_resitr->functionidx() < m_structitr->idx;
	}
	else
	{
		doSkip = (int)m_resitr->functionidx() > m_structitr->idx;
	}
	if (doSkip)
	{
		int taglevel = 1;
		for (++m_structitr; m_structitr != m_structend && taglevel; ++m_structitr)
		{
			if (m_structitr->type == (ResultElement::Type)start) taglevel++;
			if (m_structitr->type == (ResultElement::Type)  end) taglevel--;
		}
		if (taglevel) throw std::runtime_error("illegal stack in transaction result iterator (operation)");
		return true;
	}
	return false;
}

void ResultIteratorImpl::nextResult()
{
	std::size_t prevfidx = m_resitr->functionidx();
	++m_resitr;
	m_nextresult |= prevfidx > m_resitr->functionidx();
}

bool ResultIteratorImpl::skipResult()
{
	if (m_resitr == m_resend) return false;
	if (m_nextresult) throw std::logic_error("illegal state in transaction function result building statemachine");
	if ((int)m_resitr->functionidx() < m_structitr->idx)
	{
		nextResult();
		return true;
	}
	return false;
}

bool ResultIteratorImpl::getNext( langbind::TypedInputFilter::ElementType& type, types::VariantConst& element)
{
	if (!m_started)
	{
		LOG_DATA << "[transaction result structure] " << m_resultstruct->tostring();
		LOG_DATA << "[transaction result data] " << m_data->tostring();
		m_started = true;
	}
	while (m_structitr != m_structend)
	{
		if (!m_serializeWithIndices)
		{
			if (m_structitr->type == ResultElement::IndexStart || m_structitr->type == ResultElement::IndexEnd)
			{
				++m_structitr;
				continue;
			}
		}
		LOG_DATA << "[transaction result stm] State " << ResultElement::typeName( m_structitr->type) << " " << m_structitr->idx << " '" << (m_structitr->value?m_structitr->value:"") << "' f=" << ((m_resitr==m_resend)?std::string("NULL"):boost::lexical_cast<std::string>(m_resitr->functionidx())) << " s=" << m_structitr->idx;
		switch (m_structitr->type)
		{
			case ResultElement::OpenTag:
				type = langbind::TypedInputFilter::OpenTag;
				element = m_structitr->value;
				++m_structitr;
				return true;

			case ResultElement::CloseTag:
				type = langbind::TypedInputFilter::CloseTag;
				element.init();
				++m_structitr;
				return true;

			case ResultElement::OperationStart:
				LOG_DATA << "[transaction result stm] Operation start instruction index:" << m_structitr->idx << " result index:" << m_resitr->functionidx();
				if (skipStruct( ResultElement::OperationStart, ResultElement::OperationEnd)) continue;
				if (skipResult()) continue;
				if (m_resitr != m_resend && (int)m_resitr->functionidx() == m_structitr->idx)
				{
					m_nextresult = false;
					m_stack.push_back( StackElement( ResultElement::OperationEnd, m_structitr, m_resitr->functionidx(), m_resitr));
					++m_structitr;
					continue;
				}
				else
				{
					throw std::logic_error( "error in transaction function result building statemachine (OperationStart)");
				}

			case ResultElement::FunctionStart:
				LOG_DATA << "[transaction result stm] Function start instruction index:" << m_structitr->idx << " result index:" << m_resitr->functionidx();
				if (skipStruct( ResultElement::FunctionStart, ResultElement::FunctionEnd)) continue;
				if (skipResult()) continue;
				if (m_resitr != m_resend && (int)m_resitr->functionidx() == m_structitr->idx)
				{
					if (m_resitr->begin() == m_resitr->end())
					{
						nextResult();
						continue;
					}
					m_nextresult = false;
					m_stack.push_back( StackElement( ResultElement::FunctionEnd, m_structitr, m_resitr->functionidx(), m_resitr));
					++m_structitr;

					m_rowitr = m_resitr->begin();
					m_rowend = m_resitr->end();
					m_colidx = 0;
					m_colend = m_resitr->nofColumns();
					if (hasResultGroup( m_resitr->columnName( m_colidx)))
					{
						m_group = getResultGroup( m_resitr->columnName( m_colidx));
						type = langbind::TypedInputFilter::OpenTag;
						element = types::VariantConst( m_group.c_str(), m_group.size()-1);
						return true;
					}
					continue;
				}
				else
				{
					throw std::logic_error( "error in transaction function result building statemachine (FunctionStart)");
				}

			case ResultElement::IndexStart:
				if (skipStruct( ResultElement::IndexStart, ResultElement::IndexEnd)) continue;
				if (skipResult()) continue;
				m_nextresult = false;
				if (m_resitr != m_resend && (int)m_resitr->functionidx() == m_structitr->idx)
				{
					type = langbind::TypedInputFilter::OpenTag;
					element = types::VariantConst( (unsigned int)++m_stack.back().m_cnt);
					m_stack.push_back( StackElement( ResultElement::IndexEnd, m_structitr, m_resitr->functionidx(), m_resitr));
					++m_structitr;
					return true;
				}
				else
				{
					throw std::logic_error( "error in transaction function result building statemachine (IndexStart)");
				}

			case ResultElement::IndexEnd:
				if (m_valuestate == StateIndexCloseTag)
				{
					m_valuestate = StateIndexNext;
					type = langbind::TypedInputFilter::CloseTag;
					element.init();
					return true;
				}
				m_valuestate = StateIndexCloseTag;
				if (m_stack.size() < 2 || m_stack.back().m_type != m_structitr->type)
				{
					throw std::logic_error( "illegal state in transaction result construction");
				}
				if (m_resitr != m_resend && m_resitr->functionidx() == m_stack.back().m_functionidx)
				{
					unsigned int indexcnt = ++m_stack.at( m_stack.size()-2).m_cnt;
					m_structitr = m_stack.back().m_structitr;
					m_stack.back().m_resitr = m_resitr;
					if (m_nextresult && m_structitr->idx <= (int)m_resitr->functionidx())
					{
						m_nextresult = false;
					}
					type = langbind::TypedInputFilter::OpenTag;
					element = types::VariantConst( indexcnt);
					++m_structitr;
					return true;
				}
				else
				{
					m_stack.pop_back();
					++m_structitr;
					continue;
				}

			case ResultElement::OperationEnd:
				if (m_stack.back().m_type != m_structitr->type)
				{
					throw std::logic_error( "illegal state in transaction result construction (OperationEnd)");
				}
				if (m_resitr != m_resend && m_resitr->functionidx() == m_stack.back().m_functionidx)
				{
					m_structitr = m_stack.back().m_structitr;
					m_stack.back().m_resitr = m_resitr;
					if (m_nextresult && m_structitr->idx <= (int)m_resitr->functionidx())
					{
						m_nextresult = false;
					}
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
				if (m_resitr != m_resend && m_resitr->functionidx() == m_stack.back().m_functionidx)
				{
					m_structitr = m_stack.back().m_structitr;
					m_stack.back().m_resitr = m_resitr;
					if (m_nextresult && m_structitr->idx <= (int)m_resitr->functionidx())
					{
						m_nextresult = false;
					}
				}
				else
				{
					m_stack.pop_back();
				}
				++m_structitr;
				continue;

			case ResultElement::IgnoreResult:
				if (skipResult()) continue;
				if (m_resitr != m_resend)
				{
					while (m_resitr != m_resend && (int)m_resitr->functionidx() == m_structitr->idx)
					{
						nextResult();
					}
					if (m_nextresult || m_resitr == m_resend || (int)m_resitr->functionidx() > m_structitr->idx)
					{
						++m_structitr;
						continue;
					}
				}
				else
				{
					++m_structitr;
				}
				continue;

			case ResultElement::Constant:
				element = types::VariantConst( m_structitr->value);
				type = langbind::TypedInputFilter::Value;
				++m_structitr;
				return true;

			case ResultElement::Value:
				if (m_valuestate == StateColumnEndGroup)
				{
					m_group.clear();
					type = langbind::TypedInputFilter::CloseTag;
					element.init();
					m_valuestate = StateColumnOpenTag;
					return true;
				}
				if (m_valuestate == StateColumnOpenTag)
				{
					if (m_resitr != m_resend && (int)m_resitr->functionidx() == m_structitr->idx)
					{
						LOG_DATA << "[transaction result stm] Value " << m_resitr->functionidx();
						if (m_stack.back().m_structitr->idx != (int)m_resitr->functionidx())
						{
							throw std::runtime_error("internal error in transaction function result building statemachine");
						}
						if (m_rowitr == m_rowend)
						{
							++m_structitr;
							nextResult();
							if (m_resitr != m_resend)
							{
								m_rowitr = m_resitr->begin();
								m_rowend = m_resitr->end();
								m_colidx = 0;
								m_colend = m_resitr->nofColumns();

								if (hasResultGroup( m_resitr->columnName( m_colidx)))
								{
									m_group = getResultGroup( m_resitr->columnName( m_colidx));
									type = langbind::TypedInputFilter::OpenTag;
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
								nextResult();
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
							type = langbind::TypedInputFilter::OpenTag;
							element = types::VariantConst( m_group.c_str(), m_group.size()-1);
							return true;
						}
						if (!m_rowitr->at(m_colidx).defined())
						{
							++m_colidx;
							if (endOfGroup())
							{
								m_group.clear();
								type = langbind::TypedInputFilter::CloseTag;
								element.init();
								return true;
							}
							continue;
						}
						type = langbind::TypedInputFilter::OpenTag;
						if (m_group.empty())
						{
							element = m_resitr->columnName( m_colidx);
						}
						else
						{
							element = m_resitr->columnName( m_colidx).c_str() + m_group.size();
						}
						if (element.type() == types::Variant::String && element.charsize() == 0)
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
					else if (m_nextresult)
					{
						++m_structitr;
						continue;
					}
					else
					{
						throw std::runtime_error( "illegal state in transaction function result building statemachine (Value)");
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
			case ResultElement::SelectResultColumn:
				element = resolveScopedReference( *m_data, m_resitr, m_resitr->functionidx(), types::VariantConst(m_structitr->idx), "PRINT");
				++m_structitr;
				if (element.defined())
				{
					type = langbind::TypedInputFilter::Value;
					return true;
				}
				continue;
			case ResultElement::SelectResultColumnName:
				element = resolveScopedReference( *m_data, m_resitr, m_resitr->functionidx(), types::VariantConst(m_structitr->value), "PRINT");
				++m_structitr;
				if (element.defined())
				{
					type = langbind::TypedInputFilter::Value;
					return true;
				}
				continue;
			case ResultElement::SelectResultFunction:
			{
				int scope_fidx = m_structitr->idx+1;
				++m_structitr;
				if (m_structitr == m_structend)
				{
					throw std::logic_error( "illegal state (unexpected end of result structure)");
				}
				if (m_structitr->type == ResultElement::SelectResultColumn)
				{
					element = resolveScopedReference( *m_data, endOfOperation(), scope_fidx, types::VariantConst(m_structitr->idx), "PRINT");
				}
				else if (m_structitr->type == ResultElement::SelectResultColumnName)
				{
					element = resolveScopedReference( *m_data, endOfOperation(), scope_fidx, types::VariantConst(m_structitr->value), "PRINT");
				}
				else
				{
					throw std::logic_error( "illegal state (unexpected element in result structure)");
				}
				++m_structitr;
				if (element.defined())
				{
					type = langbind::TypedInputFilter::Value;
					return true;
				}
				break;
			}
		}
	}
	if (!m_stack.empty())
	{
		throw std::logic_error( "illegal state (stack not empty after iterating transaction result)");
	}
	if (!m_endofoutput)
	{
		type = langbind::TypedInputFilter::CloseTag;
		element.init();
		m_endofoutput = true;
		return true;
	}
	return false;
}

langbind::TypedInputFilter* ResultIterator::copy() const
{
	return new ResultIterator( *this);
}

bool ResultIterator::getNext( langbind::TypedInputFilter::ElementType& type, types::VariantConst& element)
{
	return m_resitr.getNext( type, element);
}

void ResultIterator::resetIterator()
{
	m_resitr.reset();
}



