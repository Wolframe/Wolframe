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
/// \file system/addressRestriction.hpp
/// \brief Interface to configured ip based authorization checks

#ifndef _Wolframe_SYSTEM_ADDRESS_RESTRICTION_HPP_INCLUDED
#define _Wolframe_SYSTEM_ADDRESS_RESTRICTION_HPP_INCLUDED
#include <vector>
#include <boost/asio/ip/address.hpp>

namespace _Wolframe {
namespace net {

/// \class AddressRestriction
/// \brief Structure to define and check configured ip based authorization
class AddressRestriction
{
public:
	/// \brief Default constructor
	AddressRestriction(){}
	/// \brief Copy constructor
	AddressRestriction( const AddressRestriction& o)
		:m_allowedar(o.m_allowedar){}

	/// \brief Define an explicitely allowed address
	void defineAddressAllowed( const std::string& adr)
	{
		m_allowedar.push_back( boost::asio::ip::address::from_string( adr));
	}

	/// \brief Check if an address belongs to the set of allowed
	bool isAllowed( const boost::asio::ip::address& adr) const
	{
		if (m_allowedar.empty()) return true;
		std::vector<boost::asio::ip::address>::const_iterator ai = m_allowedar.begin(), ae = m_allowedar.end();
		for (; ai != ae; ++ai)
		{
			if (*ai == adr) return true;
		}
		return false;
	}

private:
	std::vector<boost::asio::ip::address> m_allowedar;
};

}}//namespace
#endif
