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
#include <boost/cstdint.hpp>

namespace _Wolframe {
namespace net {

/// \class AddressRestriction
/// \brief Structure to define and check configured ip based authorization
class AddressRestriction
{
public:
	/// \brief Default constructor
	AddressRestriction()
		:m_order(Allow_Deny){}
	/// \brief Copy constructor
	AddressRestriction( const AddressRestriction& o)
		:m_order(o.m_order),m_allowedar(o.m_allowedar),m_deniedar(o.m_deniedar){}

	enum Order
	{
		Deny_Allow,
		Allow_Deny
	};
	void defineOrder( const Order& order_);

	/// \brief Define an explicitely allowed address
	void defineAddressAllowed( const std::string& adr);
	/// \brief Define an explicitely denied address
	void defineAddressDenied( const std::string& adr);
	/// \brief Define all addresses as allowed
	void defineAllowedAll();
	/// \brief Define all addresses as denied
	void defineDeniedAll();

	/// \brief Check if an address belongs to the set of allowed
	bool isAllowed( const boost::asio::ip::address& adr) const;

private:
	typedef boost::asio::ip::address IPAddress;

	struct Element
	{
		IPAddress ip;
		boost::uint32_t ipv4NetMask;

		Element( const Element& o)
			:ip(o.ip),ipv4NetMask(o.ipv4NetMask){}
		Element()
			:ip(),ipv4NetMask(0){}
	};

	static Element parseAddress( const std::string& adr);
	static boost::uint32_t parseNetworkMask( const char* str);
	static bool matches( const std::vector<Element>& ar, const IPAddress& addr);

private:
	Order m_order;
	std::vector<Element> m_allowedar;
	std::vector<Element> m_deniedar;
};

}}//namespace
#endif
