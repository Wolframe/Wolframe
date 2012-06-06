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
#include <stdexcept>


using namespace _Wolframe;
using namespace langbind;

bool SerializeInputFilter::getNext( ElementType& type, Element& element)
{
	if (!m_inputfilter) return false;
	element.type = TypedInputFilter::Element::string_;
	bool rt = m_inputfilter->getNext( type, element.value.blob_.ptr, element.value.blob_.size);
	if (!rt) setState( m_inputfilter->state(), m_inputfilter->getError());
	return rt;
}

bool SerializeOutputFilter::print( ElementType type, const Element& element)
{
	bool rt = true;

	if (!m_outputfilter) return false;
	try
	{
		std::string castelement;
		switch (element.type)
		{
			case TypedOutputFilter::Element::bool_:
				if (element.value.bool_)
				{
					rt = m_outputfilter->print( type, "true", 4);
					break;
				}
				else
				{
					rt = m_outputfilter->print( type, "false", 5);
					break;
				}

			case TypedOutputFilter::Element::double_:
				castelement = boost::lexical_cast<std::string>( element.value.double_);
				rt = m_outputfilter->print( type, castelement);
				break;

			case TypedOutputFilter::Element::int_:
				castelement = boost::lexical_cast<std::string>( element.value.int_);
				rt = m_outputfilter->print( type, castelement);
				break;

			case TypedOutputFilter::Element::uint_:
				castelement = boost::lexical_cast<std::string>( element.value.uint_);
				rt = m_outputfilter->print( type, castelement);
				break;

			case TypedOutputFilter::Element::string_:
				rt = m_outputfilter->print( type, element.value.string_.ptr, element.value.string_.size);
				break;
		}
		if (!rt) setState( m_outputfilter->state(), m_outputfilter->getError());
	}
	catch (boost::bad_lexical_cast& e)
	{
		setState( OutputFilter::Error, e.what());
		return false;
	}
	return rt;
}



