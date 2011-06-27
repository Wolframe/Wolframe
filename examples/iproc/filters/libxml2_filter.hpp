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

namespace _Wolframe {
namespace filter {
namespace libxml2 {

class Container
{
	Container( void* , std::size_t ){}

	class iterator
	{
		iterator( Container* c) 	:m_container(c){}
		iterator()			:m_container(0){}

		iterator& operator++()
		{
			m_element.clear();
			return *this;
		}

		iterator& operator++(int)
		{
			m_element.clear();
			return *this;
		}

		const std::string& operator*() const
		{
			return m_element;
		}
	private:
		Container* m_container;
		std::string m_element;
	};
};
}//namespace

typedef BufferingInputFilter<libxml2::Container,std::string> Libxml2InputFilter;

}}//namespace
#endif

