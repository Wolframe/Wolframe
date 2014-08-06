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
/// \file doctypeDetectorTest.cpp
/// \brief Implementation of fake document type recognition for testing
#include "doctypeDetectorTest.hpp"
#include "utils/parseUtils.hpp"
#include "utils/fileUtils.hpp"
#include "logger-v1.hpp"
#include <cstring>
#include <stdexcept>

using namespace _Wolframe;
using namespace _Wolframe::cmdbind;

static bool isAlpha( char ch)
{
	return (((ch|32) >= 'a' && (ch|32) <= 'z') || (ch >= '0' && ch <= '9') || ch == '_');
}

bool DoctypeDetectorTest::run()
{
	unsigned char ch = m_charparser.getNext();
	for (; ch != 0; ch = m_charparser.getNext())
	{
		if (ch == '\n')
		{
			if (m_itembuf.size())
			{
				m_info.reset( new types::DoctypeInfo( "TEST", m_itembuf));
			}
			return true;
		}
		else if (!isAlpha(ch))
		{
			return false;
		}
		else
		{
			m_itembuf.push_back(ch);
		}
	}
	return false;
}





