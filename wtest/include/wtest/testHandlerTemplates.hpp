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
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
//
// testHandlerTemplates.hpp - some test helpers
//

#ifndef _Wolframe_TEST_HANDLER_TEMPLATES_HPP_INCLUDED
#define _Wolframe_TEST_HANDLER_TEMPLATES_HPP_INCLUDED
#include <cstring>
#include <iostream>

namespace _Wolframe {
namespace test {

/// \brief Identical definition as net::NetworkOperation with non private members
class NetworkOperation
{
public:
	enum Operation	{
		READ,
		WRITE,
		CLOSE
	};

	explicit NetworkOperation( const net::NetworkOperation& o)
	{
		std::memcpy( this, &o, sizeof(*this));
	}

	unsigned timeout() const	{ return timeout_; }
	void* data() const		{ return data_; }
	void* buffer()			{ return data_; }
	std::size_t size() const	{ return size_; }
	Operation operation() const	{ return operation_; }

private:
	Operation	operation_;
	unsigned	timeout_;
	void*		data_;
	std::size_t	size_;
};


template <class Connection>
int runTestIO( const std::string& inp, std::string& out, Connection& connection)
{
	std::string::const_iterator in=inp.begin(),en=inp.end();
	for (;;)
	{
		NetworkOperation netop( connection.nextOperation());
		switch (netop.operation())
		{
			case NetworkOperation::READ:
			{
				char* data = (char*)netop.data();
				unsigned int size = netop.size();
				unsigned int ii;
				for (ii=0; ii<size && in<en; ii++,in++)
				{
					data[ii]=*in;
				}
				if (ii==0) return -1;
				connection.networkInput( (void*)data, ii);
			}
			break;

			case NetworkOperation::WRITE:
			{
				char* data = (char*)netop.data();
				std::size_t ii,size = netop.size();
				for (ii=0; ii<size; ii++)
				{
					out.push_back( data[ ii]);
				}
			}
			break;

			case NetworkOperation::CLOSE:
				return 0;

			default:
				continue;
		}
	}
	return 1;
}
}}//namespace _Wolframe::test
#endif

