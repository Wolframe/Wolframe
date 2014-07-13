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
/// \file addressRestriction.cpp
/// \brief Implementation of configured IP address restrictions (deny,allow)
#include "system/addressRestriction.hpp"
#include <vector>
#include <sstream>
#include <iostream>
#include <cstring>
#include <boost/asio/ip/address.hpp>
#include <boost/lexical_cast.hpp>

using namespace _Wolframe;
using namespace _Wolframe::net;

boost::uint32_t AddressRestriction::parseNetworkMask( const char* pp)
{
	boost::uint32_t rt;
	if (std::strchr( pp, '.'))
	{
		IPAddress maskadr = IPAddress::from_string( pp);
		if (!maskadr.is_v4())
		{
			throw std::runtime_error( "not a valid IPv4 network mask after the slash '/'");
		}
		rt = maskadr.to_v4().to_ulong();
		if (rt != 0)
		{
			boost::uint32_t mm = 0;
			unsigned int cnt = 0;
			while (cnt <= 32 && rt + mm + 1 != 0) 
			{
				++cnt;
				mm = (mm << 1) + 1;
			}
			if (rt + mm + 1 != 0)
			{
				throw std::runtime_error( "invalid IPv4 network mask");
			}
		}
	}
	else
	{
		unsigned int shift;
		try
		{
			shift = boost::lexical_cast<unsigned int>( std::string(pp));
		}
		catch (const std::bad_cast& e)
		{
			throw std::runtime_error( "not a valid IPv4 network address mask");
		}
		if (shift > 32)
		{
			throw std::runtime_error( "number of bits in IPv4 network mask out of range");
		}
		rt = (0xFFFFffff << (32 - shift));
	}
	return rt;
}

AddressRestriction::Element AddressRestriction::parseAddress( const std::string& adr)
{
	Element rt;
	const char* pp = std::strchr( adr.c_str(), '/');
	if (pp)
	{
		rt.ip = IPAddress::from_string( std::string( adr.c_str(), pp - adr.c_str()));
		if (!rt.ip.is_v4())
		{
			throw std::runtime_error( "address with network mask is not an IPv4 address");
		}
		rt.ipv4NetMask = parseNetworkMask( pp+1);
	}
	else
	{
		//IPv6
		rt.ip = IPAddress::from_string( adr);
		rt.ipv4NetMask = 0;
	}
	return rt;
}

void AddressRestriction::defineAddressAllowed( const std::string& adr)
{
	m_allowedar.push_back( parseAddress( adr));
}

void AddressRestriction::defineAddressDenied( const std::string& adr)
{
	m_deniedar.push_back( parseAddress( adr));
}

void AddressRestriction::defineAllowedAll()
{
	Element elem;
	elem.ip = boost::asio::ip::address_v4(0);
	m_allowedar.push_back( elem);
}

void AddressRestriction::defineDeniedAll()
{
	Element elem;
	elem.ip = boost::asio::ip::address_v4(0);
	m_deniedar.push_back( elem);
}

static void printElementVector( std::ostringstream& out, const std::vector<AddressRestriction::Element>& ar)
{
	out << "{";
	std::vector<AddressRestriction::Element>::const_iterator ai = ar.begin(), ae = ar.end();
	for (int idx=0; ai != ae; ++ai,++idx)
	{
		if (idx) out << ", ";
		if (ai->ipv4NetMask)
		{
			out << ai->ip.to_v4().to_string()
				<< "/" << boost::asio::ip::address_v4(ai->ipv4NetMask).to_string();
		}
		else if (ai->ip.to_v4().to_ulong() == 0)
		{
			out << "*";
		}
		else
		{
			out << ai->ip.to_string();
		}
	}
	out << "}";
}

bool AddressRestriction::matches( const std::vector<Element>& ar, const IPAddress& addr)
{
	std::vector<Element>::const_iterator ai = ar.begin(), ae = ar.end();
	for (; ai != ae; ++ai)
	{
		if (ai->ipv4NetMask)
		{
			if ((ai->ip.to_v4().to_ulong() & ai->ipv4NetMask)
				== (addr.to_v4().to_ulong() & ai->ipv4NetMask))
			{
				return true;
			}
		}
		else if (ai->ip == addr)
		{
			return true;
		}
		else if (ai->ip.to_v4().to_ulong() == 0)
		{
			return true;
		}
	}
	return false;
}

void AddressRestriction::defineOrder( const Order& order_)
{
	m_order = order_;
}

bool AddressRestriction::isAllowed( const boost::asio::ip::address& addr) const
{
	switch (m_order)
	{
		case Deny_Allow:
			if (matches( m_deniedar, addr))
			{
				if (matches( m_allowedar, addr))
				{
					return true;
				}
				return false;
			}
			return true;

		case Allow_Deny:
			if (matches( m_allowedar, addr))
			{
				if (matches( m_deniedar, addr))
				{
					return false;
				}
				return true;
			}
			return false;
	}
	return false;
}

std::string AddressRestriction::tostring() const
{
	std::ostringstream rt;
	switch (m_order)
	{
		case Deny_Allow:
			rt << "denied="; printElementVector( rt, m_deniedar);
			rt << ", allowed="; printElementVector( rt, m_allowedar);
			break;
		case Allow_Deny:
			rt << "allowed="; printElementVector( rt, m_allowedar);
			rt << ", denied="; printElementVector( rt, m_deniedar);
			break;
	}
	return rt.str();
}


