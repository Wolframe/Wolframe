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
///\file serialize/struct/structLuamapTest.cpp
///\brief test for lua serialization/deserialization of lua tables

#include "serialize/struct/luamapDescription.hpp"
#include "serialize/struct/luamapBase.hpp"
#include "iprocHandlerConfig.hpp"
#include "langbind/luaCommandEnvironment.hpp"
#include "langbind/appObjects.hpp"
#include "langbind/luaCommandHandler.hpp"
#include "logger-v1.hpp"
#include "tests/testUtils.hpp"
extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}
#ifdef _WIN32
#pragma warning(disable:4996)
#pragma warning(disable:4127)
#endif
#include <iostream>
#include <gtest/gtest.h>
#include <boost/thread/thread.hpp>

using namespace _Wolframe;
using namespace _Wolframe::iproc;
using namespace _Wolframe::langbind;
using namespace _Wolframe::serialize;

struct Plant
{
	std::vector<std::string> common;
	std::string botanical;
	unsigned int zone;
	std::string light;
	std::string price;
	unsigned int availability;

	Plant() :zone(0),availability(0){}
	static const LuamapDescriptionBase* getLuamapDescription();
};

struct Phone
{
	std::string number;
	std::string mobile;
	static const LuamapDescriptionBase* getLuamapDescription();
};

struct Address
{
	int country;
	std::string street;
	std::string city;
	Phone phone;

	Address() :country(0){}
	static const LuamapDescriptionBase* getLuamapDescription();
};

struct Garden
{
	std::string name;
	Address address;
	std::vector<Plant> plants;

	Garden() {}
	static const LuamapDescriptionBase* getLuamapDescription();
};

struct Places
{
	std::vector<Garden> gardens;
	static const LuamapDescriptionBase* getLuamapDescription();
};

struct Document
{
	Places places;
	static const LuamapDescriptionBase* getLuamapDescription();
};

const LuamapDescriptionBase* Plant::getLuamapDescription()
{
	struct ThisDescription :public LuamapDescription<Plant>
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


const LuamapDescriptionBase* Phone::getLuamapDescription()
{
	struct ThisDescription :public LuamapDescription<Phone>
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

const LuamapDescriptionBase* Address::getLuamapDescription()
{
	struct ThisDescription :public LuamapDescription<Address>
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

const LuamapDescriptionBase* Garden::getLuamapDescription()
{
	struct ThisDescription :public LuamapDescription<Garden>
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

const LuamapDescriptionBase* Places::getLuamapDescription()
{
	struct ThisDescription :public LuamapDescription<Places>
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

const LuamapDescriptionBase* Document::getLuamapDescription()
{
	struct ThisDescription :public LuamapDescription<Document>
	{
		ThisDescription()
		{
			(*this)
			("PLACES",		&Document::places)
			;
		}
	};
	static const ThisDescription rt;
	return &rt;
}

class IProcTestConfiguration :public LuaCommandEnvironment
{
public:
	IProcTestConfiguration ( const std::string& scriptpath) :LuaCommandEnvironment( "run",  scriptpath){}
};

template <class Struct>
static int luaSerializationTest( lua_State* ls)
{
	Struct obj;
	const LuamapDescriptionBase* ds = Struct::getLuamapDescription();
	lua_pushvalue( ls, 1);
	ds->parse( &obj, ls);
	ds->print( &obj, ls);
	return 1;
}

template <class Struct>
static int run( const IProcTestConfiguration& cfg, const std::string& input, std::string& output)
{
	char outputbuf[ 8192];
	langbind::Filter filter;
	filter = langbind::GlobalContext().getFilter( "xml:textwolf");
	protocol::InputFilter* in = filter.inputfilter().get();
	protocol::OutputFilter* out = filter.outputfilter().get();
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
	LuaCommandHandler processor( &cfg);
	lua_State* ls = processor.getLuaState();
	lua_pushcfunction( ls, &luaSerializationTest<Struct>);
	lua_setglobal( ls, "transform");

	in->protocolInput( (void*)input.c_str(), input.size(), true);
	processor.setFilter( filter.inputfilter());
	processor.setFilter( filter.outputfilter());
	for (;;)
	{
		const char* errorCode = 0;
		switch (processor.call( errorCode))
		{
			case LuaCommandHandler::Yield:
			{
				void* content = filter.outputfilter()->ptr();
				unsigned int contentsize = filter.outputfilter()->pos();
				output.append( (char*)content, contentsize);
				filter.outputfilter()->release();
				filter.outputfilter()->init( outputbuf, sizeof(outputbuf));
				break;
			}

			case LuaCommandHandler::Ok:
			{
				void* content = filter.outputfilter()->ptr();
				unsigned int contentsize = filter.outputfilter()->pos();
				output.append( (char*)content, contentsize);
				filter.outputfilter()->release();
				return 0;
			}

			case LuaCommandHandler::Error:
			{
				LOG_ERROR << "error processing " << (errorCode?errorCode:"unknown");
				return 2;
			}
		}
	}
	return 0;
}

typedef int (*runFunction)( const IProcTestConfiguration& cfg, const std::string& input, std::string& output);

struct TestDescription
{
	const char* name;		///< determines the name of the result and of the expected result file
	const char* datafile;		///< input to feed
	const char* scriptname;		///< input to feed
	runFunction run;		///< the function to execute
};

static const TestDescription testDescription[2] = {
{
	"luamap_PLACES",
	"test_luamap_places_IsoLatin1.xml",
	"test_serialization.lua",
	&run<Document>
},
{0,0,0,0}
};

class StructLuamapTest : public ::testing::Test
{
protected:
	StructLuamapTest() {}
	virtual ~StructLuamapTest() {}
	virtual void SetUp() {}
	virtual void TearDown() {}
};

TEST_F( StructLuamapTest, tests)
{
	unsigned int ti;
	for (ti=0; testDescription[ti].name; ti++)
	{
		wtest::Data data( testDescription[ti].name, testDescription[ti].datafile);
		std::string testoutput;
		IProcTestConfiguration cfg( (boost::filesystem::current_path() / "scripts"/ testDescription[ti].scriptname).string());
		EXPECT_EQ( 0, testDescription[ti].run( cfg, data.input, testoutput));
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


