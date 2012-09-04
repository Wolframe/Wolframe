/************************************************************************
Copyright (C) 2011, 2012 Project Wolframe.
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
///\file prnt_pdfPrinterDocument_testtrace.cpp
#include "prnt/pdfPrinterDocument.hpp"
#include <iostream>
#include <sstream>
#include <stdexcept>

using namespace _Wolframe;
using namespace _Wolframe::prnt;

namespace
{
class DocumentImpl :public Document
{
public:
	DocumentImpl(){}
	virtual ~DocumentImpl(){}

	virtual void execute_enter( Method method, const VariableScope& vars);
	virtual void execute_leave( Method method);

	virtual std::string tostring() const;

private:
	std::ostringstream m_out;
	std::vector< std::size_t> m_stk;
	std::size_t m_cnt;
};
}// anonymous namespace

void DocumentImpl::execute_enter( Method method, const VariableScope& vscope)
{
/*[-]*/std::cout << "ENTER " << methodName(method) << ":" << std::endl;
	m_out << "ENTER " << methodName(method) << ":" << std::endl;
	VariableScope::const_iterator vi = vscope.begin(), ve = vscope.end();
	m_stk.push_back( ++m_cnt);
/*[-]*/std::cout << m_cnt << "$ ";
	m_out << m_cnt << "$ ";
	while (vi != ve)
	{
/*[-]*/std::cout << vi.name() << " = '" << vi.value() << "', ";
		m_out << vi.name() << " = '" << vi.value() << "'";
		++vi;
		if (vi != ve) m_out << ", ";
	}
/*[-]*/std::cout << std::endl;
	m_out << std::endl;
}

void DocumentImpl::execute_leave( Method method)
{
	std::size_t idx = m_stk.back();
	m_stk.pop_back();
	m_out << "LEAVE " << methodName(method) << " " << idx << std::endl;
}

std::string DocumentImpl::tostring() const
{
	return m_out.str();
}

Document* _Wolframe::prnt::createTestTraceDocument()
{
	return new DocumentImpl();
}


