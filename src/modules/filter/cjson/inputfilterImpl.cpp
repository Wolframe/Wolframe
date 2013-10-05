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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
///\file inputfilterImpl.cpp
///\brief Implementation of input filter abstraction for the cjson library
#include "inputfilterImpl.hpp"
#include "langbind/charsetEncodings.hpp"

using namespace _Wolframe;
using namespace _Wolframe::langbind;

bool InputFilterImpl::getValue( const char* name, std::string& val)
{
	if (std::strcmp(name,"encoding") == 0 && m_state == Processing)
	{
		val = m_encoding;
		return true;
	}
	return false;
}

bool InputFilterImpl::getDocType( std::string& val)
{
}

bool InputFilterImpl::setValue( const char* name, const std::string& value)
{
}

void InputFilterImpl::putInput( const void* content, std::size_t contentsize, bool end)
{
	m_content.append( (const char*)content, contentsize);
	if (end)
	{
		if (m_root) throw std::logic_error(" bad operation on JSON input filter: put input after end");
		CharsetClass::Id cl = CharsetClass::guess( (const char*)content, contentsize);
		std::string m_encoding = "UTF-";
		if ((int)cl & (CharsetClass::U1) != 0) m_encoding += "8";
		if ((int)cl & (CharsetClass::U2) != 0) m_encoding += "16";
		if ((int)cl & (CharsetClass::U4) != 0) m_encoding += "32";
		if ((int)cl & (CharsetClass::BE) != 0) m_encoding += "BE";
		if ((int)cl & (CharsetClass::LE) != 0) m_encoding += "LE";
		CharsetEncoding enc = langbind::getCharsetEncoding( m_encoding);
		m_content = langbind::convertStringCharsetToUTF8( enc, m_content);
		m_state = Processing;
		m_root = cJSON_Parse( m_content.c_str());
		m_stk.push_back( m_root);
	}
}

bool InputFilterImpl::getDocType( types::DocType& doctype)
{
}

bool InputFilterImpl::getNext( InputFilter::ElementType& type, const void*& element, std::size_t& elementsize)
{
}

bool InputFilterImpl::setFlags( Flags f)
{
	if (0!=((int)f & (int)langbind::FilterBase::SerializeWithIndices))
	{
		return false;
	}
	return InputFilter::setFlags( f);
}


