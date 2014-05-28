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
/// \file types/secureReference.hpp
/// \brief Smart pointer definition with the intention to hold sensitive data
#ifndef _WOLFRAME_TYPES_SECURE_REFERENCE_HPP_INCLUDED
#define _WOLFRAME_TYPES_SECURE_REFERENCE_HPP_INCLUDED

namespace _Wolframe {
namespace types {

/// \class SecureReference
/// \brief Smart pointer template with the intention to hold sensitive data
/// \note The implementation does not hold its promise
template <class P>
class SecureReference
{
public:
	/// \brief Constructor
	explicit SecureReference( P* ptr_=0)
		:m_ptr(ptr_){}

	/// \brief Destructor
	~SecureReference()
	{
		if (m_ptr) delete m_ptr;
	}

	/// \brief Access content
	const P* get() const
	{
		return m_ptr;
	}

	/// \brief Recreate or dispose reference
	void reset( P* ptr_=0)
	{
		if (m_ptr)
		{
			delete m_ptr;
		}
		m_ptr = ptr_;
	}

private:
	SecureReference( const SecureReference&) {}	//... non copyable
	void operator=( const SecureReference&) {}	//... non copyable
	P* m_ptr;					///< pointer to this instance
};

}}//namespace
#endif


