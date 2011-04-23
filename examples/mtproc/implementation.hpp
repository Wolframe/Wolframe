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
///
/// \file methodtable.hpp
/// \brief definition of method table for calls to to the application processor
///
#ifndef _Wolframe_MTPROC_IMPLEMENTATION_HPP_INCLUDED
#define _Wolframe_MTPROC_IMPLEMENTATION_HPP_INCLUDED
#include "methodtable.hpp"
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace mtproc {

struct Implementation :public Instance
{
	//* constructor/destructor of data
	static Method::Data* createData();
	static void destroyData( Method::Data* data);

	//* methods (context->data points to this)

	//echo the content
	static int echo( Method::Context* context, unsigned int argc, const char** argv);

	//echo the arguments without referencing content
	static int printarg( Method::Context* context, unsigned int argc, const char** argv);

	Implementation()
	{
		static Method mt[3] = {{"echo",&echo,true},{"parg",&printarg,false},{0,0,false}};
		m_mt = mt;
		m_data = 0;
		m_createData = &createData;
		m_destroyData = &destroyData;
	}

	bool init()
	{
		if (m_data) destroyData( m_data);
		m_data = createData();
		return (!!m_data);
	}

	void done()
	{
		if (m_data)
		{
			destroyData( m_data);
			m_data = 0;
		}
	}
}; 

}}//namespace
#endif


