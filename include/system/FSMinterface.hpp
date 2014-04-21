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
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
/// \file system/FSMinterface.hpp
/// Finite State Machine interface
///

#ifndef _FSM_INTERFACE_HPP_INCLUDED
#define _FSM_INTERFACE_HPP_INCLUDED

#include <cstddef>		// for std::size_t

namespace _Wolframe {

/// Finite State Machine interface
class FSM
{
public:
	/// Finite State Machine operation
	class Operation
	{
	public:
		enum FSMoperation	{
			READ,
			WRITE,
			CLOSE
		};
	private:
//		FSMoperation	m_op;
//		const void*	m_data;
//		std::size_t	m_dataSize;
	};

	/// Finite State Machine signals
	enum Signal	{
		TIMEOUT,
		TERMINATE,
		END_OF_FILE,
		CANCELLED,
		BROKEN_PIPE,
		UNKNOWN_ERROR
	};

	/// The input data.
	virtual void receiveData( const void* data, std::size_t size ) = 0;

	/// What operation the FSM expects next from the outside.
	virtual const Operation nextOperation() = 0;

	/// Signal the FSM.
	virtual void signal( Signal /*event*/ )	{}

	/// Data that has not been consumed by the FSM.
	virtual std::size_t dataLeft( const void*& begin ) = 0;
};

} // namespace _Wolframe

#endif // _FSM_INTERFACE_HPP_INCLUDED
