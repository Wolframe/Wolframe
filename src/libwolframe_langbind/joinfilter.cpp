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
/// \file filter/joinfilter.hpp
/// \brief Implementation of a filter created from the join of two filters
#include "filter/joinfilter.hpp"

using namespace _Wolframe;
using namespace _Wolframe::langbind;

bool JoinInputFilter::getNextImpl( TypedInputFilter* flt, ElementType& type, types::VariantConst& element)
{
	if (!flt->getNext( type, element)) return false;
	switch (type)
	{
		case FilterBase::OpenTag:
			m_taglevel += 1;
			break;
		case FilterBase::CloseTag:
			m_taglevel -= 1;
			break;
		case FilterBase::Attribute:
		case FilterBase::Value:
			break;
	}
	return true;
}

bool JoinInputFilter::getNext( ElementType& type, types::VariantConst& element)
{
	bool rt;
	switch (m_joinstate)
	{
		case Init:
			m_joinstate = ProcessFilter1;
			setState( InputFilter::Open);

		case ProcessFilter1:
			rt = getNextImpl( m_inputfilter1.get(), type, element);
			if (rt)
			{
				if (m_taglevel >= 0)
				{
					setState( m_inputfilter1->state());
					return true;
				}
				else
				{
					m_taglevel = 0;
					m_joinstate = ProcessFilter2;
					/*no break here!*/
				}
			}
			else
			{
				setState( m_inputfilter1->state(), m_inputfilter1->getError());
				return false;
			}

		case ProcessFilter2:
			rt = getNextImpl( m_inputfilter2.get(), type, element);
			if (rt)
			{
				if (m_taglevel >= 0)
				{
					setState( m_inputfilter2->state());
					return true;
				}
				else
				{
					m_joinstate = FinalClose;
					/*no break here!*/
				}
			}
			else
			{
				setState( m_inputfilter2->state(), m_inputfilter2->getError());
				return false;
			}

		case FinalClose:
			setState( InputFilter::Open);
			m_joinstate = Done;
			if (type != FilterBase::CloseTag)
			{
				throw std::logic_error("internal: implementation error join filter");
			}
			return true;

		case Done:
			return false;
	}
	throw std::logic_error("internal: illegal state in join filter");
}

bool JoinInputFilter::setFlags( Flags f)
{
	switch (m_joinstate)
	{
		case Init:
		case ProcessFilter1:
		case ProcessFilter2:
			TypedInputFilter::setFlags(f);
			m_inputfilter1->setFlags(f);
			m_inputfilter2->setFlags(f);
			return true;
		case FinalClose:
		case Done:
			break;
	}
	return false;
}

bool JoinInputFilter::checkSetFlags( Flags f) const
{
	return m_inputfilter1->setFlags(f) && m_inputfilter2->setFlags(f);
}


