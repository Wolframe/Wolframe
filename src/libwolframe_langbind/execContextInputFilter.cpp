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
/// \file execContextInputFilter.cpp
/// \brief Implementation of execution context elements as input filter
#include "filter/execContextInputFilter.hpp"

using namespace _Wolframe;
using namespace _Wolframe::langbind;

ExecContextElement::Value ExecContextElement::valueFromId( const std::string& v)
{
	int rt = (int)UserName; //... do not accept reserved "Const"

	while (rt <= (int)MaxValue)
	{
		if (boost::algorithm::iequals( v, valueName((Value)rt)))
		{
			return (Value)rt;
		}
		++rt;
	}
	throw std::runtime_error( std::string("unknown identifier for value in expression '") + v + "'");
}

ExecContextInputFilter::ExecContextInputFilter( const std::vector<ExecContextElement>& params_, const proc::ExecContext& ctx_, const std::string& authorizationResource_)
	:utils::TypeSignature("langbind::ExecContextInputFilter", __LINE__)
	,langbind::TypedInputFilter("authzarg")
	,m_state(0)
	,m_paramidx(0)
	,m_params(&params_)
	,m_ctx(&ctx_)
	,m_authorizationResource(authorizationResource_)
	{}

ExecContextInputFilter::ExecContextInputFilter( const ExecContextInputFilter& o)
	:utils::TypeSignature("langbind::ExecContextInputFilter", __LINE__)
	,langbind::TypedInputFilter(o)
	,m_state(o.m_state)
	,m_paramidx(o.m_paramidx)
	,m_params(o.m_params)
	,m_ctx(o.m_ctx)
	,m_authorizationResource(o.m_authorizationResource)
	,m_elembuf(o.m_elembuf)
	{}

const std::string& ExecContextInputFilter::getElement( const std::string& ar, char delim, std::size_t idx)
{
	if (!delim)
	{
		m_elembuf.clear();
		m_elembuf.append( ar);
		return m_elembuf;
	}
	const char* cc = ar.c_str();
	const char* ee = std::strchr( cc, delim);
	for (; idx > 1 && ee != 0; --idx,cc=ee+1,ee=std::strchr(cc,delim)){}
	m_elembuf.clear();
	if (idx == 1)
	{
		if (ee == 0) ee = ar.c_str() + ar.size();
		m_elembuf.append( cc, ee-cc);
	}
	return m_elembuf;
}

bool ExecContextInputFilter::getNext( ElementType& type, types::VariantConst& element)
{
	for (;;)
	switch (m_state++)
	{
		case 0: if (m_paramidx >= m_params->size())
			{
				if (m_paramidx > m_params->size())
				{
					setState( langbind::InputFilter::Error, "internal: call of get next after close");
					return false;
				}
				m_state = 2/*3*/;
				continue;
			}
			else
			{
				type = langbind::FilterBase::OpenTag;
				element = m_params->at( m_paramidx).name;
				return true;
			}
		case 1:
		{
			const ExecContextElement& pp = m_params->at( m_paramidx);
			switch (pp.value)
			{
				case ExecContextElement::Const:
					element = getElement( pp.const_value, pp.value_delim, pp.value_idx);
					break;
				case ExecContextElement::UserName:
					if (!m_ctx->user()) continue;
					element = getElement( m_ctx->user()->uname(), pp.value_delim, pp.value_idx);
					break;
				case ExecContextElement::SocketId:
					if (!m_ctx->socketIdentifier()) continue;
					element = getElement( m_ctx->socketIdentifier(), pp.value_delim, pp.value_idx);
					break;
				case ExecContextElement::RemoteHost:
					if (!m_ctx->remoteEndpoint()) continue;
					element = getElement( m_ctx->remoteEndpoint()->host(), pp.value_delim, pp.value_idx);
					break;
				case ExecContextElement::ConnectionType:
					if (!m_ctx->remoteEndpoint()) continue;
					element = getElement( net::ConnectionEndpoint::connectionTypeName( m_ctx->remoteEndpoint()->type()), pp.value_delim, pp.value_idx);
					break;
				case ExecContextElement::Authenticator:
					if (!m_ctx->user()) continue;
					element = getElement( m_ctx->user()->authenticator(), pp.value_delim, pp.value_idx);
					break;
				case ExecContextElement::Resource:
					element = getElement( m_authorizationResource, pp.value_delim, pp.value_idx);
					break;
			}
			type = langbind::FilterBase::Value;
			return true;
		}
		case 2: type = langbind::FilterBase::CloseTag; element.init(); m_state -= 3; ++m_paramidx; return true;
		case 3: type = langbind::FilterBase::CloseTag; element.init(); return true;
		case 4: type = langbind::FilterBase::CloseTag; element.init(); return true;
		default: return false;
	}
}

void ExecContextInputFilter::resetIterator()
{
	m_state = 0;
	m_paramidx = 0;
}

