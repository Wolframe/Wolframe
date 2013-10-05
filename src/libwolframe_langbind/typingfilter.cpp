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
///\file typingfilter.cpp
///\brief Implementation of filters that wrap untyped to typed input filters
#include "filter/typingfilter.hpp"
#include <boost/lexical_cast.hpp>
#include <stdexcept>


using namespace _Wolframe;
using namespace langbind;

bool TypingInputFilter::getNext( ElementType& type, types::VariantConst& element)
{
	if (!m_inputfilter.get()) return false;
	const void* charptr;
	std::size_t charsize;
	bool rt = m_inputfilter->getNext( type, charptr, charsize);
	if (!rt)
	{
		setState( m_inputfilter->state(), m_inputfilter->getError());
	}
	else
	{
		element.init( (const char*)charptr, charsize);
	}
	return rt;
}

bool TypingInputFilter::setFlags( Flags f)
{
	if (m_inputfilter.get()->setFlags( f))
	{
		return langbind::TypedInputFilter::setFlags( f);
	}
	return false;
}

bool TypingOutputFilter::print( ElementType type, const types::VariantConst& element)
{
	bool rt = true;

	if (!m_outputfilter.get()) return false;
	try
	{
		std::string castelement;
		switch (element.type())
		{
			case types::Variant::Null:
				rt = m_outputfilter->print( type, 0, 0);
				break;

			case types::Variant::Bool:
				if (element.tobool())
				{
					rt = m_outputfilter->print( type, "true", 4);
					break;
				}
				else
				{
					rt = m_outputfilter->print( type, "false", 5);
					break;
				}

			case types::Variant::Double:
				castelement = boost::lexical_cast<std::string>( element.todouble());
				rt = m_outputfilter->print( type, castelement);
				break;

			case types::Variant::Int:
				castelement = boost::lexical_cast<std::string>( element.toint());
				rt = m_outputfilter->print( type, castelement);
				break;

			case types::Variant::UInt:
				castelement = boost::lexical_cast<std::string>( element.touint());
				rt = m_outputfilter->print( type, castelement);
				break;

			case types::Variant::String:
				rt = m_outputfilter->print( type, element.charptr(), element.charsize());
				break;
		}
		setState( m_outputfilter->state(), m_outputfilter->getError());
	}
	catch (boost::bad_lexical_cast& e)
	{
		setState( OutputFilter::Error, e.what());
		return false;
	}
	return rt;
}



