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
/// \brief Test for types/addressRestriction.hpp
#include "types/addressRestriction.hpp"
#include "protocol/parser.hpp"
#include "protocol/buffers.hpp"
#include "gtest/gtest.h"
#include "wtest/testReport.hpp"
#include <stdlib.h>

using namespace _Wolframe;

struct TestDescription
{
	types::AddressRestriction::Order order;
	const char* restriction1[8];
	const char* restriction2[8];
	const char* in;
	bool out;
};
static const TestDescription testDescription[] =
{
	{types::AddressRestriction::Deny_Allow, {"all",0}, {"192.168.0.0",0}, "192.168.0.0", true},
	{types::AddressRestriction::Deny_Allow, {"all",0}, {"192.168.0.1",0}, "192.168.0.0", false},
	{types::AddressRestriction::Deny_Allow, {"all",0}, {"192.168.0.1/24",0}, "192.168.0.0", true},
	{types::AddressRestriction::Deny_Allow, {"all",0}, {"192.168.0.1/31",0}, "192.168.0.0", true},
	{types::AddressRestriction::Deny_Allow, {"all",0}, {"192.168.0.1/16",0}, "192.168.123.123", true},
	{types::AddressRestriction::Deny_Allow, {"all",0}, {"2607:f0d0:1002:5100::/56",0}, "2607:f0d0:1002:5100::4",true},
	{types::AddressRestriction::Deny_Allow, {"all",0}, {"2607:f0d0:1002:5100::/56",0}, "2607:f0d0:1002:5200::1",false},

	{types::AddressRestriction::Deny_Allow, {"192.168.19.123/24",0}, {"192.168.19.124",0}, "192.168.19.123", false},
	{types::AddressRestriction::Deny_Allow, {"192.168.19.123/24",0}, {"192.168.19.124",0}, "192.168.19.124", true},
	{types::AddressRestriction::Deny_Allow, {"192.168.19.123/255.255.255.248",0}, {"192.168.19.122",0}, "192.168.19.122", true},

	{types::AddressRestriction::Allow_Deny, {"all",0}, {"192.168.0.0",0}, "192.168.0.0", false},
	{types::AddressRestriction::Allow_Deny, {"all",0}, {"192.168.0.1",0}, "192.168.0.0", true},
	{types::AddressRestriction::Allow_Deny, {"all",0}, {"192.168.0.1/24",0}, "192.168.0.0", false},
	{types::AddressRestriction::Allow_Deny, {"all",0}, {"192.168.0.1/31",0}, "192.168.0.0", false},
	{types::AddressRestriction::Allow_Deny, {"all",0}, {"192.168.0.1/16",0}, "192.168.123.123", false},

	{types::AddressRestriction::Allow_Deny, {"192.168.19.123/24",0}, {"192.168.19.124",0}, "192.168.19.123", true},
	{types::AddressRestriction::Allow_Deny, {"192.168.19.123/24",0}, {"192.168.19.124",0}, "192.168.19.124", false},
	{types::AddressRestriction::Allow_Deny, {"192.168.19.123/255.255.255.248",0}, {"192.168.19.122",0}, "192.168.19.122", false},
	{types::AddressRestriction::Allow_Deny, {0},{0},0,false}
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
			types::AddressRestriction restriction;
			restriction.defineOrder( testDescription[tt].order);

			const char* rstr = 0;
			for (unsigned int ii=0; 0!=(rstr=testDescription[tt].restriction1[ii]); ii++)
			{
				try
				{
					switch (testDescription[tt].order)
					{
						case types::AddressRestriction::Allow_Deny:
							if (0==std::strcmp( rstr, "all"))
							{
								restriction.defineAllowedAll();
							}
							else
							{
								restriction.defineAddressAllowed( rstr);
							}
							break;
						case types::AddressRestriction::Deny_Allow:
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
				catch (const std::runtime_error& e)
				{
					throw std::runtime_error(std::string("error in restriction '") + rstr + ": " + e.what());
				}
			}
			for (unsigned int ii=0; 0!=(rstr=testDescription[tt].restriction2[ii]); ii++)
			{
				try
				{
					switch (testDescription[tt].order)
					{
						case types::AddressRestriction::Deny_Allow:
							if (0==std::strcmp( rstr, "all"))
							{
								restriction.defineAllowedAll();
							}
							else
							{
								restriction.defineAddressAllowed( rstr);
							}
							break;
						case types::AddressRestriction::Allow_Deny:
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
				catch (const std::runtime_error& e)
				{
					throw std::runtime_error(std::string("error in restriction '") + rstr + ": " + e.what());
				}
			}
			expected.push_back( testDescription[ tt].out);
			types::AddressRestriction::IPAddress adr = types::AddressRestriction::IPAddress::from_string( testDescription[ tt].in);
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

