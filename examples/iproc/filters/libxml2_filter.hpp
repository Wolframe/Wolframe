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
///\file libxml2_filter.hpp
///\brief Filter using the libxml2 library for input and output
#include "bufferingFilterBase.hpp"
#include <cstddef>
#include <libxml++/libxml++.h>
#include <libxml++/parsers/textreader.h>

namespace _Wolframe {
namespace filter {
namespace libxml2 {

class Container
{
public:
	///\brief Enumeration of states of the libxml2 parser
	enum State
	{
		Init,		///< started
		Tag,		///< reading tag
		Attribute,	///< reading attribute name
		AttributeValue,	///< reading attribute value
		EndOfData	///< reached end of data
	};

	struct Element
	{
		std::string& value() const			{return m_value;}
		protocol::InputFilter::ElementType type() const {return m_type;}
	private:
		friend class Container;
	};

	Container( void* src, std::size_t srcsize)
		:m_reader( src, srcsize),m_state(Init),m_type(protocol::InputFilter::Value),m_depth(0){}

	void skip()
	{
		m_element.m_value.clear();
		for (;;)
		{
			while (m_depth > m_reader.get_depth())
			{
				m_element.m_type = protocol::InputFilter::CloseTag;
				return;
			}

			switch (m_state)
			{
				case Init:
					if (m_reader.read())
					{
						m_state = Tag;
						m_element.m_value.append( m_reader.get_name());
						m_element.m_type = protocol::InputFilter::OpenTag;
						return;
					}
					else
					{
						m_element.m_type = protocol::InputFilter::CloseTag;
						m_state = EndOfData;
						return;
					}

				case Tag:
					if (m_reader.has_attributes())
					{
						m_reader.move_to_first_attribute();
						m_element.m_value.append( m_reader.get_name());
						m_element.m_type = protocol::InputFilter::Attribute;
						m_state = AttributeValue;
						return;
					}
					else if (reader.has_value())
					{
						m_element.m_value.append( m_reader.reader.get_value());
						m_element.m_type = protocol::InputFilter::Value;
						m_state = Init;
						return;
					}
					else
					{
						m_state = Init;
						continue;
					}

				case Attribute:
					if (reader.move_to_next_attribute())
					{
						m_element.m_value.append( m_reader.get_name());
						m_element.m_type = protocol::InputFilter::Attribute;
						m_state = AttributeValue;
						return;
					}
					else if (reader.has_value())
					{
						m_element.m_value.append( m_reader.reader.get_value());
						m_element.m_type = protocol::InputFilter::Value;
						m_state = Init;
						return;
					}

				case AttributeValue:
					m_element.m_value.append( m_reader.reader.get_value());
					m_element.m_type = protocol::InputFilter::Value;
					m_state = Attribute;
					return;

				case EndOfData:
					m_element.m_type = protocol::InputFilter::CloseTag;
					return;

			}
		}
	}


	class iterator
	{
		iterator( Container* c) 	:m_container(c){}
		iterator()			:m_container(0){}

		iterator& operator++()
		{
			skip();
			return *this;
		}

		const Element& operator*() const
		{
			return m_container->m_element;
		}

		const Element* operator->() const
		{
			return &m_container->m_element;
		}
	private:
		Container* m_container;
	};
private:
	xmlpp::TextReader m_reader;
	State m_state;
	Element m_element;
	protocol::InputFilter::ElementType m_type;
	int m_depth;
};
}//namespace

typedef BufferingInputFilter<libxml2::Container,std::string> Libxml2InputFilter;

}}//namespace
#endif

