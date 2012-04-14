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
/// \file serialize/struct/structFiltermapTest.cpp
/// \brief test for the serialization/deserialization of filters

#include "serialize/struct/filtermapDescription.hpp"
#include "serialize/struct/filtermapBase.hpp"
#include "langbind/appObjects.hpp"
#include "langbind/appGlobalContext.hpp"
#include "logger-v1.hpp"
#include "tests/testUtils.hpp"
#ifdef _WIN32
#pragma warning(disable:4996)
#pragma warning(disable:4127)
#endif
#include <iostream>
#include <gtest/gtest.h>
#include <boost/thread/thread.hpp>

using namespace _Wolframe;
using namespace serialize;

struct Plant
{
	std::vector<std::string> common;
	std::string botanical;
	unsigned int zone;
	std::string light;
	std::string price;
	unsigned int availability;

	Plant() :zone(0),availability(0){}
	static const FiltermapDescriptionBase* getFiltermapDescription();
};

struct Phone
{
	std::string number;
	std::string mobile;
	static const FiltermapDescriptionBase* getFiltermapDescription();
};

struct Address
{
	int country;
	std::string street;
	std::string city;
	Phone phone;

	Address() :country(0){}
	static const FiltermapDescriptionBase* getFiltermapDescription();
};

struct Garden
{
	std::string name;
	Address address;
	std::vector<Plant> plants;

	Garden() {}
	static const FiltermapDescriptionBase* getFiltermapDescription();
};

struct Places
{
	std::vector<Garden> gardens;
	static const FiltermapDescriptionBase* getFiltermapDescription();
};

const FiltermapDescriptionBase* Plant::getFiltermapDescription()
{
	struct ThisDescription :public FiltermapDescription<Plant>
	{
		ThisDescription()
		{
			(*this)
			--
			("COMMON",		&Plant::common)
			("BOTANICAL",		&Plant::botanical)
			("ZONE",		&Plant::zone)
			("LIGHT",		&Plant::light)
			("PRICE",		&Plant::price)
			("AVAILABILITY",	&Plant::availability)
			;
		}
	};
	static const ThisDescription rt;
	return &rt;
}


const FiltermapDescriptionBase* Phone::getFiltermapDescription()
{
	struct ThisDescription :public FiltermapDescription<Phone>
	{
		ThisDescription()
		{
			(*this)
			--
			("NUMBER",		&Phone::number)
			("MOBILE",		&Phone::mobile)
			;
		}
	};
	static const ThisDescription rt;
	return &rt;
}

const FiltermapDescriptionBase* Address::getFiltermapDescription()
{
	struct ThisDescription :public FiltermapDescription<Address>
	{
		ThisDescription()
		{
			(*this)
			--
			("COUNTRY",		&Address::country)
			("STREET",		&Address::street)
			("CITY",		&Address::city)
			("PHONE",		&Address::phone)
			;
		}
	};
	static const ThisDescription rt;
	return &rt;
}

const FiltermapDescriptionBase* Garden::getFiltermapDescription()
{
	struct ThisDescription :public FiltermapDescription<Garden>
	{
		ThisDescription()
		{
			(*this)
			("NAME",		&Garden::name)
			("ADDRESS",		&Garden::address)
			("PLANT",		&Garden::plants)
			;
		}
	};
	static const ThisDescription rt;
	return &rt;
}

const FiltermapDescriptionBase* Places::getFiltermapDescription()
{
	struct ThisDescription :public FiltermapDescription<Places>
	{
		ThisDescription()
		{
			(*this)
			("GARDEN",		&Places::gardens)
			;
		}
	};
	static const ThisDescription rt;
	return &rt;
}

template <class Struct>
static int run( const char* root, const std::string& input, std::string& output)
{
	langbind::Filter filter;
	if (!langbind::GlobalContext().getFilter( "xml:textwolf", filter))
	{
		LOG_ERROR << "error in serialization: no valid filter defined";
		return 1;
	}
	protocol::InputFilter* in = filter.inputfilter().get();
	protocol::OutputFilter* out = filter.outputfilter().get();
	if (!in || !out)
	{
		std::cerr << "cannot load filter" << std::endl;
		return 1;
	}

	if (!in)
	{
		LOG_ERROR << "error in serialization: no valid input filter defined";
		return 1;
	}
	if (!out)
	{
		LOG_ERROR << "error in serialization: no valid output filter defined";
		return 2;
	}
	Struct object;
	Context ctx;
	const FiltermapDescriptionBase* ds = Struct::getFiltermapDescription();

	in->protocolInput( (void*)input.c_str(), input.size(), true);
	if (!ds->parse( root, (void*)&object, *in, ctx))
	{
		LOG_ERROR << "error in serialization of " << root << " " << ctx.getLastError();
		return 3;
	}
	if (!ds->print( root, (const void*)&object, *out, ctx))
	{
		LOG_ERROR << "error in deserialization of " << root << ": " << ctx.getLastError();
		return 4;
	}
	output = ctx.content();
	return 0;
}

typedef int (*runFunction)( const char* name, const std::string& input, std::string& output);

struct TestDescription
{
	const char* name;		///< determines the name of the result and of the expected result file
	const char* roottag;		///< name of the root tag of the xml file
	const char* datafile;		///< input to feed
	runFunction run;		///< the function to execute
};

static const TestDescription testDescription[2] = {
{
	"directmap_PLACES",
	"PLACES",
	"test_directmap_places_IsoLatin1.xml",
	&run<Places>
},
{0,0,0,0}
};

class StructFiltermapTest : public ::testing::Test
{
protected:
	StructFiltermapTest() {}
	virtual ~StructFiltermapTest() {}
	virtual void SetUp() {}
	virtual void TearDown() {}
};

TEST_F( StructFiltermapTest, tests)
{
	unsigned int ti;
	for (ti=0; testDescription[ti].name; ti++)
	{
		wtest::Data data( testDescription[ti].name, testDescription[ti].datafile);
		std::string testoutput;
		EXPECT_EQ( 0, testDescription[ti].run( testDescription[ti].roottag, data.input, testoutput));
		data.check( testoutput);
		ASSERT_EQ( data.expected, testoutput);
	}
}

int main( int argc, char **argv )
{
	wtest::Data::createDataDir( "result");
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}


