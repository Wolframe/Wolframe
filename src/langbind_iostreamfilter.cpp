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
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
///\file langbind/pipe.hpp
///\brief Implementation for a pipe (istream|ostream) through wolframe mappings like filters, forms, functions

#include "logger-v1.hpp"
#include "langbind/appObjects.hpp"
#include "langbind/appGlobalContext.hpp"
#include "langbind/iostreamfilter.hpp"
#include "serialize/ddl/filtermapSerialize.hpp"
#include "filter/token_filter.hpp"
#if WITH_LUA
#include "cmdbind/luaCommandHandler.hpp"
#endif
#include <boost/algorithm/string.hpp>
#include <string>
#include <fstream>
#include <iostream>

using namespace _Wolframe;
using namespace langbind;


static langbind::Filter getFilter( langbind::GlobalContext* gc, const std::string& ifl, const std::string& ofl)
{
	langbind::Filter rt;
	if (boost::iequals( ofl, ifl))
	{
		if (!gc->getFilter( ifl.c_str(), rt))
		{
			LOG_ERROR << "unknown filter '" << ifl << "'";
			return rt;
		}
	}
	else
	{
		langbind::Filter in;
		langbind::Filter out;
		if (!gc->getFilter( ifl.c_str(), in))
		{
			LOG_ERROR << "unknown input filter '" << ofl << "'";
			return rt;
		}
		if (!gc->getFilter( ofl.c_str(), out))
		{
			LOG_ERROR << "unknown output filter '" << ofl << "'";
			return rt;
		}
		rt = langbind::Filter( in.inputfilter(), out.outputfilter());
	}
	return rt;
}

static bool readInput( char* buf, unsigned int bufsize, std::istream& is, protocol::InputFilterR& iflt)
{
	if (iflt->gotEoD()) return false;
	std::size_t pp = 0;
	while (pp < bufsize && !is.eof())
	{
		is.read( buf+pp, sizeof(char));
		if (!is.eof()) ++pp;
	}
	iflt->protocolInput( buf, pp, is.eof());
	return true;
}

static void writeOutput( char* buf, unsigned int size, std::ostream& os, protocol::OutputFilterR& oflt)
{
	os.write( buf, oflt->pos());
	oflt->init( buf, size);
}

static bool followInput( protocol::InputFilterR& iflt)
{
	protocol::InputFilter* follow = iflt->createFollow();
	if (follow)
	{
		iflt.reset( follow);
		return true;
	}
	else
	{
		return false;
	}
}

static bool followOutput( protocol::OutputFilterR& oflt)
{
	protocol::OutputFilter* follow = oflt->createFollow();
	if (follow)
	{
		oflt.reset( follow);
		return true;
	}
	else
	{
		return false;
	}
}

struct BufferStruct
{
	boost::shared_ptr<char> mem;
	char* inbuf;
	char* outbuf;
	std::size_t insize;
	std::size_t outsize;

	BufferStruct( std::size_t ib, std::size_t ob)
		:mem( (char*)std::calloc( ib+ob, 1), std::free),insize(ib),outsize(ob)
	{
		inbuf = mem.get();
		outbuf = mem.get()+ib;
	}
};

static bool processIO( BufferStruct& buf, langbind::Filter& flt, std::istream& is, std::ostream& os)
{
	const char* errmsg;
	if (!flt.inputfilter().get() || !flt.outputfilter().get())
	{
		LOG_ERROR << "Error lost filter";
		return false;
	}
	switch (flt.inputfilter()->state())
	{
		case protocol::InputFilter::Open:
			break;

		case protocol::InputFilter::EndOfMessage:
			return readInput( buf.inbuf, buf.insize, is, flt.inputfilter());

		case protocol::InputFilter::Error:
			errmsg = flt.inputfilter()->getError();
			LOG_ERROR << "Error in input filter: " << (errmsg?errmsg:"unknown");
			return false;
	}
	switch (flt.outputfilter()->state())
	{
		case protocol::OutputFilter::Open:
			if (followOutput( flt.outputfilter())) return true;
			if (followInput( flt.inputfilter())) return true;
			return false;

		case protocol::OutputFilter::EndOfBuffer:
			writeOutput( buf.outbuf, buf.outsize, os, flt.outputfilter());
			return true;

		case protocol::OutputFilter::Error:
			errmsg = flt.outputfilter()->getError();
			LOG_ERROR << "Error in output filter: " << (errmsg?errmsg:"unknown");
			return false;
	}
	return true;
}

bool finalIOState( const langbind::Filter& flt)
{
	return	flt.inputfilter()->state() != protocol::InputFilter::Error
		&& flt.inputfilter()->gotEoD()
		&& !flt.inputfilter()->hasLeft()
		&& flt.outputfilter()->state() == protocol::OutputFilter::Open;
}


bool _Wolframe::langbind::iostreamfilter( const std::string& proc, const std::string& ifl, std::size_t ib, const std::string& ofl, std::size_t ob, std::istream& is, std::ostream& os)
{
	langbind::GlobalContext* gc = langbind::getGlobalContext();
	langbind::FilterFactoryR tf( new langbind::TokenFilterFactory());
	gc->defineFilter( "token", tf);

	langbind::Filter flt = getFilter( gc, ifl, ofl);
	if (!flt.inputfilter().get() || !flt.outputfilter().get()) return false;

	BufferStruct buf( ib, ob);
	flt.outputfilter().get()->init( buf.outbuf, buf.outsize);

	if (proc.size() == 0 || proc == "-")
	{
		const void* elem;
		std::size_t elemsize;
		protocol::InputFilter::ElementType ietype;
		protocol::OutputFilter::ElementType oetype;

		while (!flt.inputfilter()->gotEoD() || flt.inputfilter()->hasLeft())
		{
			if (!flt.inputfilter().get()->getNext( ietype, elem, elemsize))
			{
				if (!processIO( buf, flt, is, os)) goto _END_FILTER_LOOP;
				continue;
			}
			oetype = (protocol::OutputFilter::ElementType) ietype;
			while (!flt.outputfilter().get()->print( oetype, elem, elemsize))
			{
				if (!processIO( buf, flt, is, os)) goto _END_FILTER_LOOP;
			}
		}
		_END_FILTER_LOOP:
		writeOutput( buf.outbuf, buf.outsize, os, flt.outputfilter());
		return finalIOState( flt);
	}
#if WITH_LUA
	{
		LuaScriptInstanceR sc = createLuaScriptInstance( proc, flt.inputfilter(), flt.outputfilter());
		if (sc.get())
		{
			lua_getglobal( sc->thread(), proc.c_str());
			int rt = lua_resume( sc->thread(), NULL, 0);
			while (rt == LUA_YIELD)
			{
				if (!processIO( buf, flt, is, os)) break;
				rt = lua_resume( sc->thread(), NULL, 0);
			}
			if (rt)
			{
				const char* msg = lua_tostring( sc->thread(), -1);
				LOG_ERROR << "Error in lua script: '" << msg << "'";
				return false;
			}
			return true;
		}
	}
#endif
	{
		PluginFunction pf;
		if (gc->getPluginFunction( proc.c_str(), pf))
		{
			langbind::PluginFunction::CallResult rt = pf.call( *flt.inputfilter(), *flt.outputfilter());
			while (rt == langbind::PluginFunction::Yield)
			{
				if (!processIO( buf, flt, is, os)) break;
				rt = pf.call( *flt.inputfilter(), *flt.outputfilter());
			}
			return (rt == langbind::PluginFunction::Ok);
		}
	}
	{
		DDLFormR df;
		if (gc->getForm( proc.c_str(), df))
		{
			serialize::Context ictx;
			if (!serialize::parse( df->m_struct, *flt.inputfilter(), ictx))
			{
				LOG_ERROR << "Error in form serialization: '" << ictx.getLastError() << "'";
				return false;
			}
			serialize::Context octx;
			if (!serialize::print( df->m_struct, flt.outputfilter(), octx))
			{
				LOG_ERROR << "Error in form printing: '" << octx.getLastError() << "'";
				return false;
			}
			os.write( octx.content().c_str(), octx.content().size());
			return true;
		}
	}
	{
		if (gc->hasTransactionFunction( proc.c_str()))
		{

			return true;
		}
	}
	LOG_ERROR << "mapping command not found: '" << proc.c_str() << "'";
	return false;
}


