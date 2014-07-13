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
/// \file testAddressRestriction.cpp
/// \brief Test for system/addressRestriction.hpp
#include "addressRestriction.cpp"
#include "protocol/parser.hpp"
#include "protocol/buffers.hpp"
#include "gtest/gtest.h"
#include "wtest/testReport.hpp"
#include <stdlib.h>

using namespace _Wolframe;

struct TestDescription
{
	net::AddressRestriction::Order order;
	const char* restriction1[8];
	const char* restriction2[8];
	const char* in;
	bool out;
};
static const TestDescription testDescription[] =
{
	{net::AddressRestriction::Deny_Allow, {"all",0}, {"192.168.0.0",0}, "192.168.0.0", true},
	{net::AddressRestriction::Deny_Allow, {"all",0}, {"192.168.0.1",0}, "192.168.0.0", false},
	{net::AddressRestriction::Deny_Allow, {"all",0}, {"192.168.0.1/24",0}, "192.168.0.0", true},
	{net::AddressRestriction::Deny_Allow, {"all",0}, {"192.168.0.1/31",0}, "192.168.0.0", true},
	{net::AddressRestriction::Deny_Allow, {"all",0}, {"192.168.0.1/16",0}, "192.168.123.123", true},

	{net::AddressRestriction::Deny_Allow, {"192.168.19.123/24",0}, {"192.168.19.124",0}, "192.168.19.123", false},
	{net::AddressRestriction::Deny_Allow, {"192.168.19.123/24",0}, {"192.168.19.124",0}, "192.168.19.124", true},
	{net::AddressRestriction::Deny_Allow, {"192.168.19.123/255.255.255.248",0}, {"192.168.19.122",0}, "192.168.19.122", true},

	{net::AddressRestriction::Allow_Deny, {"all",0}, {"192.168.0.0",0}, "192.168.0.0", false},
	{net::AddressRestriction::Allow_Deny, {"all",0}, {"192.168.0.1",0}, "192.168.0.0", true},
	{net::AddressRestriction::Allow_Deny, {"all",0}, {"192.168.0.1/24",0}, "192.168.0.0", false},
	{net::AddressRestriction::Allow_Deny, {"all",0}, {"192.168.0.1/31",0}, "192.168.0.0", false},
	{net::AddressRestriction::Allow_Deny, {"all",0}, {"192.168.0.1/16",0}, "192.168.123.123", false},

	{net::AddressRestriction::Allow_Deny, {"192.168.19.123/24",0}, {"192.168.19.124",0}, "192.168.19.123", true},
	{net::AddressRestriction::Allow_Deny, {"192.168.19.123/24",0}, {"192.168.19.124",0}, "192.168.19.124", false},
	{net::AddressRestriction::Allow_Deny, {"192.168.19.123/255.255.255.248",0}, {"192.168.19.122",0}, "192.168.19.122", false},
	{net::AddressRestriction::Allow_Deny, {0},{0},0,false}
};


class AddressRestrictionFixture : public ::testing::Test
{
public:
	std::vector<std::string> config;
	std::vector<bool> output;
	std::vector<bool> expected;

protected:
	AddressRestrictionFixture() {}

	virtual void SetUp()
	{
		for (unsigned int tt=0; testDescription[tt].in; tt++)
		{
			net::AddressRestriction restriction;
			restriction.defineOrder( testDescription[tt].order);

			const char* rstr = 0;
			for (unsigned int ii=0; 0!=(rstr=testDescription[tt].restriction1[ii]); ii++)
			{
				switch (testDescription[tt].order)
				{
					case net::AddressRestriction::Allow_Deny:
						if (0==std::strcmp( rstr, "all"))
						{
							restriction.defineAllowedAll();
						}
						else
						{
							restriction.defineAddressAllowed( rstr);
						}
						break;
					case net::AddressRestriction::Deny_Allow:
						if (0==std::strcmp( rstr, "all"))
						{
							restriction.defineDeniedAll();
						}
						else
						{
							restriction.defineAddressDenied( rstr);
						}
						break;
				}
			}
			for (unsigned int ii=0; 0!=(rstr=testDescription[tt].restriction2[ii]); ii++)
			{
				switch (testDescription[tt].order)
				{
					case net::AddressRestriction::Deny_Allow:
						if (0==std::strcmp( rstr, "all"))
						{
							restriction.defineAllowedAll();
						}
						else
						{
							restriction.defineAddressAllowed( rstr);
						}
						break;
					case net::AddressRestriction::Allow_Deny:
						if (0==std::strcmp( rstr, "all"))
						{
							restriction.defineDeniedAll();
						}
						else
						{
							restriction.defineAddressDenied( rstr);
						}
						break;
				}
			}
			expected.push_back( testDescription[ tt].out);
			net::AddressRestriction::IPAddress adr = net::AddressRestriction::IPAddress::from_string( testDescription[ tt].in);
			config.push_back( restriction.tostring() + " ? " + testDescription[ tt].in);
			output.push_back( restriction.isAllowed( adr));
		}
	}
	virtual void TearDown()
	{}
};


// Tests the Version constructors and members
TEST_F( AddressRestrictionFixture, Members )
{
	for (unsigned int tt=0; tt<output.size(); tt++)
	{
		std::cerr << "TEST " << config[tt] << " : " << (output[tt]?"allow":"deny") << std::endl;
		ASSERT_EQ( output[tt], expected[tt]);
	}
}

int main( int argc, char **argv )
{
	WOLFRAME_GTEST_REPORT( argv[0], refpath.string());
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}

