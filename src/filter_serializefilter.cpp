/************************************************************************
Copyright (C) 2011 Project Wolframe.
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
///\file filter_serializefilter.cpp
///\brief implementation of serialization filters
#include "filter/serializefilter.hpp"
#include <boost/lexical_cast.hpp>


using namespace _Wolframe;
using namespace langbind;

bool SerializeInputFilter::getNext( ElementType& type, Element& element)
{
	if (!m_inputfilter) return false;
	element.type = TypedInputFilter::Element::string_;
	return m_inputfilter->getNext( type, element.value.string_.ptr, element.value.string_.size);
}


bool SerializeOutputFilter::print( ElementType type, const Element& element)
{
	std::string castelement;

	try
	{
		if (!m_outputfilter) return false;
		switch (element.type)
		{
			case TypedOutputFilter::Element::bool_:
				if (element.value.bool_)
				{
					return m_outputfilter->print( type, "true", 4);
				}
				else
				{
					return m_outputfilter->print( type, "false", 5);
				}

			case TypedOutputFilter::Element::double_:
				castelement = boost::lexical_cast<std::string>( element.value.double_);
				return m_outputfilter->print( type, castelement);

			case TypedOutputFilter::Element::int_:
				castelement = boost::lexical_cast<std::string>( element.value.int_);
				return m_outputfilter->print( type, castelement);

			case TypedOutputFilter::Element::uint_:
				castelement = boost::lexical_cast<std::string>( element.value.uint_);
				return m_outputfilter->print( type, castelement);

			case TypedOutputFilter::Element::string_:
				return m_outputfilter->print( type, element.value.string_.ptr, element.value.string_.size);
		}
	}
	catch (boost::bad_lexical_cast&){}
	return false;
}



