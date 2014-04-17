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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
///\file vmTransactionOutput.cpp
#include "database/vmTransactionOutput.hpp"
#include "utils/printFormats.hpp"
#include "utils/typeSignature.hpp"
#include <iostream>
#include <sstream>

using namespace _Wolframe;
using namespace _Wolframe::db;

void VmTransactionOutput::print( std::ostream& out, const utils::PrintFormat* pformat) const
{
	if (!pformat)
	{
		print( out, utils::logPrintFormat());
		return;
	}
	std::size_t indent = 0;
	std::size_t indentsize = (pformat->indent?std::strlen(pformat->indent):0);
	std::string indentstr;
	std::vector<types::Variant> stack;

	vm::Output::const_iterator oi = m_impl->begin(), oe = m_impl->end();
	for (; oi != oe; ++oi)
	{
		switch (oi->op())
		{
			case vm::Output::Element::OpenArray:
				stack.push_back( oi->arg());
				break;

			case vm::Output::Element::OpenArrayElement:
				if (stack.empty()) throw std::runtime_error( "tags in transaction output not balanced");
				out << pformat->newitem << indentstr << stack.back().tostring() << pformat->openstruct;
				indent += 1;
				indentstr.append( pformat->indent);
				break;

			case vm::Output::Element::Open:
				out << pformat->newitem << indentstr << oi->arg().tostring() << pformat->openstruct;
				indent += 1;
				indentstr.append( pformat->indent);
				break;

			case vm::Output::Element::CloseArray:
				if (stack.empty()) throw std::runtime_error( "tags in transaction output not balanced");
				stack.pop_back();
				break;

			case vm::Output::Element::CloseArrayElement:
			case vm::Output::Element::Close:
				if (indent == 0)
				{
					++oi;
					if (oi != oe) throw std::runtime_error("transaction function output tags not balanced");
					return;
				}
				out << pformat->newitem << indentstr << pformat->closestruct;
				indent -= 1;
				indentstr.resize( indentstr.size() - indentsize);
				break;

			case vm::Output::Element::Value:
				out << pformat->newitem << indentstr << pformat->startvalue << oi->arg().tostring() << pformat->endvalue; 
				break;
		}
	}
	if (indent != 0)
	{
		throw std::runtime_error("transaction function output tags not balanced");
	}
}

std::string VmTransactionOutput::tostring( const utils::PrintFormat* pformat) const
{
	std::ostringstream out;
	print( out, pformat);
	return out.str();
}

class VmTransactionInputFilter
	:public langbind::TypedInputFilter
{
public:
	explicit VmTransactionInputFilter( const vm::OutputR& output_)
		:utils::TypeSignature("db::VmTransactionInputFilter", __LINE__)
		,langbind::TypedInputFilter("vmTransactionOutput")
		,m_done(false),m_output(output_),m_itr(output_->begin()),m_end(output_->end())
	{}
	VmTransactionInputFilter( const VmTransactionInputFilter& o)
		:utils::TypeSignature("db::VmTransactionInputFilter", __LINE__)
		,langbind::TypedInputFilter(o)
		,m_done(o.m_done),m_output(o.m_output),m_itr(o.m_itr),m_end(o.m_end)
	{}
	virtual ~VmTransactionInputFilter(){}

	virtual TypedInputFilter* copy() const
	{
		return new VmTransactionInputFilter( *this);
	}

	virtual bool getNext( ElementType& type, types::VariantConst& element)
	{
	AGAIN:
		if (m_itr == m_end)
		{
			if (m_done) return false;
			type = langbind::FilterBase::CloseTag;
			element = types::VariantConst();
			m_done = true;
			return true;
		}
		switch (m_itr->op())
		{
			case vm::Output::Element::OpenArray:
				if (langbind::FilterBase::flag(SerializeWithIndices))
				{
					m_stack.push_back( (types::Variant::Data::Int)0L);
					type = langbind::FilterBase::OpenTag;
					element = m_itr->arg();
				}
				else
				{
					m_stack.push_back( m_itr->arg());
					++m_itr;
					goto AGAIN;
				}
				break;

			case vm::Output::Element::OpenArrayElement:
				if (m_stack.empty()) throw std::runtime_error("tags in output not balanced");
				if (langbind::FilterBase::flag(SerializeWithIndices))
				{
					++m_stack.back().data().value.Int;
				}
				type = langbind::FilterBase::OpenTag;
				element = m_stack.back();
				break;

			case vm::Output::Element::Open:
				type = langbind::FilterBase::OpenTag;
				element = m_itr->arg();
				break;

			case vm::Output::Element::CloseArray:
				if (m_stack.empty()) throw std::runtime_error("tags in output not balanced");
				if (langbind::FilterBase::flag(SerializeWithIndices))
				{
					type = langbind::FilterBase::CloseTag;
					element = m_itr->arg();
					m_stack.pop_back();
					break;
				}
				else
				{
					m_stack.pop_back();
					++m_itr;
					goto AGAIN;
				}
				break;

			case vm::Output::Element::CloseArrayElement:
			case vm::Output::Element::Close:
				type = langbind::FilterBase::CloseTag;
				element = m_itr->arg();
				break;

			case vm::Output::Element::Value:
				type = langbind::FilterBase::Value;
				element = m_itr->arg();
				break;
		}
		++m_itr;
		return true;
	}

private:
	bool m_done;
	vm::OutputR m_output;
	vm::Output::const_iterator m_itr;
	vm::Output::const_iterator m_end;
	std::vector<types::Variant> m_stack;
};

langbind::TypedInputFilterR VmTransactionOutput::get() const
{
	return langbind::TypedInputFilterR( new VmTransactionInputFilter( m_impl));
}


