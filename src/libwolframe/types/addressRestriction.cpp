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
/// \file types/addressRestriction.cpp
/// \brief Implementation of configured IP address restrictions (deny,allow)
#include "types/addressRestriction.hpp"
#include <vector>
#include <sstream>
#include <iostream>
#include <cstring>
#include <boost/asio/ip/address.hpp>
#include <boost/lexical_cast.hpp>

using namespace _Wolframe;
using namespace _Wolframe::types;

boost::uint8_t AddressRestriction::parseNetworkMask( bool isIPv6, const char* pp)
{
	if (std::strchr( pp, '.'))
	{
		if (isIPv6)
		{
			throw std::runtime_error( "IPv6 cannot have an IPv4 network mask");
		}
		IPAddress maskadr = IPAddress::from_string( pp);
		if (!maskadr.is_v4())
		{
			throw std::runtime_error( "invalid IPv4 network mask");
		}
		boost::uint32_t netmask = maskadr.to_v4().to_ulong();
		if (netmask == 0)
		{
			throw std::runtime_error( "invalid IPv4 network mask");
		}
		boost::uint32_t mm = 0;
		unsigned int cnt = 0;
		while (cnt <= 32 && netmask + mm + 1 != 0) 
		{
			++cnt;
			mm = (mm << 1) + 1;
		}
		if (netmask + mm + 1 != 0)
		{
			throw std::runtime_error( "invalid IPv4 network mask");
		}
		return (boost::uint8_t)(32-cnt);
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
			throw std::runtime_error( "not a valid IP network address mask (number of bits)");
		}
		
		if (isIPv6)
		{
			if (shift > 128)
			{
				throw std::runtime_error( "number of bits in IPv6 network mask out of range");
			}
		}
		else
		{
			if (shift > 32)
			{
				throw std::runtime_error( "number of bits in IPv4 network mask out of range");
			}
		}
		return (boost::uint8_t)shift;
	}
}

AddressRestriction::Element AddressRestriction::parseAddress( const std::string& adr)
{
	Element rt;
	const char* pp = std::strchr( adr.c_str(), '/');
	if (pp)
	{
		rt.ip = IPAddress::from_string( std::string( adr.c_str(), pp - adr.c_str()));
		rt.netMask = parseNetworkMask( rt.ip.is_v6(), pp+1);
	}
	else
	{
		rt.ip = IPAddress::from_string( adr);
		rt.netMask = 0;
	}
	if (rt.ip.is_v6())
	{
		boost::asio::ip::address_v6 aa = rt.ip.to_v6();
		if (aa.is_multicast())
		{
			throw std::runtime_error("multicast address not allowed");
		}
	}
	else
	{
		boost::asio::ip::address_v4 aa = rt.ip.to_v4();
		if (aa.is_multicast())
		{
			throw std::runtime_error("multicast address not allowed");
		}
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
		if (ai->ip.is_v6())
		{
			if (ai->netMask)
			{
				out << ai->ip.to_v6().to_string() << "/" << (unsigned int)ai->netMask;
			}
			else
			{
				out << ai->ip.to_v6().to_string();
			}
		}
		else
		{
			if (ai->ip.to_v4().to_ulong() == 0)
			{
				out << "*";
			}
			else if (ai->netMask)
			{
				boost::uint32_t netMaskAddr = 0xFFFFffff;
				netMaskAddr <<= (32 - ai->netMask);
	
				out << ai->ip.to_v4().to_string()
					<< "/" << boost::asio::ip::address_v4( netMaskAddr).to_string();
			}
			else
			{
				out << ai->ip.to_v4().to_string();
			}
		}
	}
	out << "}";
}


static bool match_ipv6( const boost::asio::ip::address_v6& netAddr, boost::uint8_t netMask, const boost::asio::ip::address_v6& addr)
{
	if (!netMask)
	{
		return (netAddr == addr);
	}
	boost::asio::ip::address_v6::bytes_type netAddr_bytes = netAddr.to_bytes();
	boost::asio::ip::address_v6::bytes_type addr_bytes = addr.to_bytes();

	if (netMask > 128)
	{
		throw std::runtime_error( "illegal net mask");
	}
	boost::uint8_t routing_prefix = netMask;
	for(unsigned int ii=0; ii<16; ii++)
	{
		boost::uint8_t byte_netmask = 0xff;
		if(routing_prefix >= 8)
		{
			routing_prefix -= 8;
		}
		else if (routing_prefix == 0)
		{
			byte_netmask = 0;
		}
		else
		{
			byte_netmask <<= (8 - routing_prefix);
			routing_prefix = 0;
		}
		if ((netAddr_bytes[ii] & byte_netmask) != (addr_bytes[ii] & byte_netmask))
		{
			return false;
		}
		if (byte_netmask == 0) break;
	}
	return true;
}


bool AddressRestriction::matches( const std::vector<Element>& ar, const IPAddress& addr)
{
	std::vector<Element>::const_iterator ai = ar.begin(), ae = ar.end();
	for (; ai != ae; ++ai)
	{
		if (ai->netMask)
		{
			if (ai->ip.is_v6())
			{
				return match_ipv6( ai->ip.to_v6(), ai->netMask, addr.to_v6());
			}
			else
			{
				boost::uint32_t netMaskAddr = 0xFFFFffff;
				netMaskAddr <<= (32 - ai->netMask);

				if ((ai->ip.to_v4().to_ulong() & netMaskAddr)
					== (addr.to_v4().to_ulong() & netMaskAddr))
				{
					return true;
				}
				else
				{
					return false;
				}
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

bool AddressRestriction::isAllowed( const boost::asio::ip::address& addr) const
{
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

std::string AddressRestriction::tostring() const
{
	std::ostringstream rt;
	rt << "allowed="; printElementVector( rt, m_allowedar);
	rt << ", denied="; printElementVector( rt, m_deniedar);
	return rt.str();
}


