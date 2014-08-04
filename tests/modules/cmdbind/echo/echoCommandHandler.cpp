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
///\file echoCommandHandler.hpp
///\brief Implementation of the echo command handler for protocol tests
#include "echoCommandHandler.hpp"
#include "processor/execContext.hpp"

using namespace _Wolframe;
using namespace _Wolframe::cmdbind;

IOFilterCommandHandler::CallResult EchoCommandHandler::call( const char*& err)
{
	for (;;) switch (m_state)
	{
		case 0:
		{
			const langbind::FilterType* filtertype = execContext()->provider()->filterType( "char");
			if (!filtertype)
			{
				throw std::runtime_error("filter type 'char' not defined");
			}
			langbind::FilterR filter( filtertype->create());
			if (!filter.get() || !filter->inputfilter().get() || !filter->outputfilter().get())
			{
				throw std::runtime_error("filter 'char' with input and output cannot be created");
			}
			setInputFilter( filter->inputfilter());
			setOutputFilter( filter->outputfilter());
			m_state = 1;
			/*no break here!*/
		}
		case 1:
		{
			const void* elemptr;
			std::size_t elemsize;
			langbind::InputFilter::ElementType elemtype;
			if (inputfilter()->getNext( elemtype, elemptr, elemsize))
			{
				m_content.append( (const char*)elemptr, elemsize);
			}
			else
			{
				if (inputfilter()->state() == langbind::InputFilter::EndOfMessage)
				{
					m_state = 2;
					return Yield;
				}
				else if (inputfilter()->state() == langbind::InputFilter::Error)
				{
					err = inputfilter()->getError();
					return Error;
				}
				else
				{
					m_eof = true;
					m_state = 2;
					continue;
				}
			}
			break;
		}
		case 2:
		{
			if (m_contentitr == m_content.size())
			{
				m_contentitr = 0;
				m_content.clear();
				m_state = m_eof?3:1;
				continue;
			}
			else
			{
				const void* elemptr = m_content.c_str() + m_contentitr;
				std::size_t elemsize = 1;
				langbind::InputFilter::ElementType elemtype = langbind::InputFilter::Value;
				if (outputfilter()->print( elemtype, elemptr, elemsize))
				{
					++m_contentitr;
					continue;
				}
				else
				{
					if (outputfilter()->state() == langbind::OutputFilter::Error)
					{
						err = outputfilter()->getError();
						return Error;
					}
					else
					{
						return Yield;
					}
				}
			}
			break;
		}
		case 3:
			return Ok;
	}
}


