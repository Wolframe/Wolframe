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
/// \file directmapParser.cpp
/// \brief test for the directmap serialization

#include "serialize/directmapDescription.hpp"
#include "serialize/directmapBase.hpp"
#include "langbind/appObjects.hpp"
#include "logger.hpp"
#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>
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
	static const DescriptionBase* getDescription();
};

struct Phone
{
	std::string number;
	std::string mobile;
	static const DescriptionBase* getDescription();
};

struct Address
{
	int country;
	std::string street;
	std::string city;
	Phone phone;

	Address() :country(0){}
	static const DescriptionBase* getDescription();
};

struct Garden
{
	std::string name;
	Address address;
	std::vector<Plant> plants;

	Garden() {}
	static const DescriptionBase* getDescription();
};

struct Places
{
	std::vector<Garden> gardens;
	static const DescriptionBase* getDescription();
};

const DescriptionBase* Plant::getDescription()
{
	struct ThisDescription :public Description<Plant>
	{
		ThisDescription()
		{
			(*this)
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


const DescriptionBase* Phone::getDescription()
{
	struct ThisDescription :public Description<Phone>
	{
		ThisDescription()
		{
			(*this)
			("NUMBER",		&Phone::number)
			("MOBILE",		&Phone::mobile)
			;
		}
	};
	static const ThisDescription rt;
	return &rt;
}

const DescriptionBase* Address::getDescription()
{
	struct ThisDescription :public Description<Address>
	{
		ThisDescription()
		{
			(*this)
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

const DescriptionBase* Garden::getDescription()
{
	struct ThisDescription :public Description<Garden>
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

const DescriptionBase* Places::getDescription()
{
	struct ThisDescription :public Description<Places>
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
static int run( const char* name, const char* input, std::string& output)
{
	app::Filter filter( "xml:textwolf", 1024, 1024);
	protocol::InputFilter* in = filter.m_inputfilter.get();
	protocol::FormatOutput* out = filter.m_formatoutput.get();

	if (!in)
	{
		LOG_ERROR << "error in serialization: no valid input filter defined";
		return 1;
	}
	if (!out)
	{
		LOG_ERROR << "error in serialization: no valid format output defined";
		return 2;
	}
	Struct object;
	ProcessingContext ctx;
	const DescriptionBase* ds = Struct::getDescription();

	in->protocolInput( (void*)input, strlen(input), true);
	if (!ds->parse( name, (void*)&object, *in, ctx))
	{
		LOG_ERROR << "error in serialization of " << name << ctx.getLastError();
		return 3;
	}
	if (!ds->print( name, (const void*)&object, *out, ctx))
	{
		LOG_ERROR << "error in deserialization of " << name << ": " << ctx.getLastError();
		return 4;
	}
	output = ctx.content();
	return 0;
}

typedef int (*runFunction)( const char* name, const char* input, std::string& output);

struct TestDescription
{
	const char* name;
	const char* input;
	const char* output;
	runFunction run;
};

static const TestDescription testDescription[2] = {
{
	"PLACES",

	"<?xml version='1.0' encoding='Isolatin-1' standalone='yes'>\r\n"
	"<PLACES>"
	"<GARDEN>"
	"<NAME>Botanischer Garten</NAME>"
	"<ADDRESS>"
	"<COUNTRY>41</COUNTRY>"
	"<STREET>Zollikerstrasse</STREET>"
	"<CITY>Zürich</CITY>"
	"<PHONE><NUMBER>01234567</NUMBER><MOBILE>01234567</MOBILE></PHONE>"
	"</ADDRESS>"
	"<PLANT>"
	"<COMMON>Bloodroot</COMMON>"
	"<BOTANICAL>Sanguinaria canadensis</BOTANICAL>"
	"<ZONE>4</ZONE>"
	"<LIGHT>Mostly Shady</LIGHT>"
	"<PRICE>$2.44</PRICE>"
	"<AVAILABILITY>31599</AVAILABILITY>"
	"</PLANT>"
	"<PLANT>"
	"<COMMON>Columbine</COMMON>"
	"<BOTANICAL>Aquilegia canadensis</BOTANICAL>"
	"<ZONE>3</ZONE>"
	"<LIGHT>Mostly Shady</LIGHT>"
	"<PRICE>$9.37</PRICE>"
	"<AVAILABILITY>3069</AVAILABILITY>"
	"</PLANT>"
	"<PLANT>"
	"<COMMON>Marsh Marigold</COMMON>"
	"<BOTANICAL>Caltha palustris</BOTANICAL>"
	"<ZONE>4</ZONE>"
	"<LIGHT>Mostly Sunny</LIGHT>"
	"<PRICE>$6.81</PRICE>"
	"<AVAILABILITY>51799</AVAILABILITY>"
	"</PLANT>"
	"</GARDEN>"
	"</PLACES>",

	"<?xml version='1.0' encoding='Isolatin-1' standalone='yes'>\r\n"
	"<PLACES>"
	"<GARDEN>"
	"<NAME>Botanischer Garten</NAME>"
	"<ADDRESS>"
	"<COUNTRY>41</COUNTRY>"
	"<STREET>Zollikerstrasse</STREET>"
	"<CITY>Zürich</CITY>"
	"<PHONE><NUMBER>01234567</NUMBER><MOBILE>01234567</MOBILE></PHONE>"
	"</ADDRESS>"
	"<PLANT>"
	"<COMMON>Bloodroot</COMMON>"
	"<BOTANICAL>Sanguinaria canadensis</BOTANICAL>"
	"<ZONE>4</ZONE>"
	"<LIGHT>Mostly Shady</LIGHT>"
	"<PRICE>$2.44</PRICE>"
	"<AVAILABILITY>31599</AVAILABILITY>"
	"</PLANT>"
	"<PLANT>"
	"<COMMON>Columbine</COMMON>"
	"<BOTANICAL>Aquilegia canadensis</BOTANICAL>"
	"<ZONE>3</ZONE>"
	"<LIGHT>Mostly Shady</LIGHT>"
	"<PRICE>$9.37</PRICE>"
	"<AVAILABILITY>3069</AVAILABILITY>"
	"</PLANT>"
	"<PLANT>"
	"<COMMON>Marsh Marigold</COMMON>"
	"<BOTANICAL>Caltha palustris</BOTANICAL>"
	"<ZONE>4</ZONE>"
	"<LIGHT>Mostly Sunny</LIGHT>"
	"<PRICE>$6.81</PRICE>"
	"<AVAILABILITY>51799</AVAILABILITY>"
	"</PLANT>"
	"</GARDEN>"
	"</PLACES>",

	&run<Places>,
},
{0,0,0,0}
};

class TestFixture : public ::testing::Test
{
protected:
	TestFixture() {}
	virtual ~TestFixture() {}
	virtual void SetUp() {}
	virtual void TearDown() {}
};

TEST_F( TestFixture, tests)
{
	unsigned int ti;
	for (ti=0; testDescription[ti].input; ti++)
	{
		std::string result;
		EXPECT_EQ( 0, testDescription[ti].run( testDescription[ti].name, testDescription[ti].input, result));
#define _Wolframe_LOWLEVEL_DEBUG
#ifdef _Wolframe_LOWLEVEL_DEBUG
		unsigned int ii=0,nn=result.size();
		for (;ii<nn && result[ii]==testDescription[ti].output[ii]; ii++);
		if (ii != nn)
		{
			std::string diff( testDescription[ti].output+ii, 10);
			std::string res( result.c_str()+ii, 10);
			std::size_t pp = ii - (ii<10?ii:10);
			std::string pred( result.c_str()+ii-pp, ii-pp);
			printf( "TEST %s DIFF AFTER '%s' RESULT='%s' EXPECTED='%s'\n", testDescription[ti].name, pred.c_str(), res.c_str(), diff.c_str());

			boost::this_thread::sleep( boost::posix_time::seconds( 5 ));
		}
#endif
		ASSERT_EQ( testDescription[ti].output, result);
	}
}

int main( int argc, char **argv )
{
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}


