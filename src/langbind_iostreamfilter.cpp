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
		++pp;
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

bool _Wolframe::langbind::iostreamfilter( const std::string& proc, const std::string& ifl, std::size_t ib, const std::string& ofl, std::size_t ob, std::istream& is, std::ostream& os)
{
	langbind::GlobalContext* gc = langbind::getGlobalContext();
	PluginFunction pf;
	DDLFormR df;
	langbind::Filter flt = getFilter( gc, ifl, ofl);
	BufferStruct buf( ib, ob);
	flt.outputfilter().get()->init( buf.outbuf, buf.outsize);

#if WITH_LUA
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
	}
	else
#endif
	if (gc->getPluginFunction( proc.c_str(), pf))
	{
	}
	else if (gc->getForm( proc.c_str(), df))
	{
	}
	else if (gc->hasTransactionFunction( proc.c_str()))
	{
	}
	else
	{
		LOG_ERROR << "mapping command not found: '" << proc.c_str() << "'";
		return false;
	}
	return true;
}

bool _Wolframe::langbind::iostreamfilter( const std::string& ifl, std::size_t ib, const std::string& ofl, std::size_t ob, std::istream& is, std::ostream& os)
{
	langbind::GlobalContext* gc = langbind::getGlobalContext();
	langbind::FilterFactoryR tf( new langbind::TokenFilterFactory());
	gc->defineFilter( "token", tf);

	langbind::Filter flt = getFilter( gc, ifl, ofl);
	BufferStruct buf( ib, ob);
	flt.outputfilter().get()->init( buf.outbuf, buf.outsize);

	const void* element;
	std::size_t elementsize;
	protocol::InputFilter::ElementType elementType;
	std::size_t taglevel=0;

	READ_INPUT:
	{
		if (!readInput( buf.inbuf, buf.insize, is, flt.inputfilter())) goto TERMINATE;
		goto PROCESS_READ;
	}
	WRITE_OUTPUT:
	{
		writeOutput( buf.outbuf, buf.outsize, os, flt.outputfilter());
		goto PROCESS_WRITE;
	}
	PROCESS_READ:
	{
		if (!flt.inputfilter().get()->getNext( elementType, element, elementsize))
		{
			switch (flt.inputfilter().get()->state())
			{
				case protocol::InputFilter::EndOfMessage:
				{
					if (flt.inputfilter().get()->size())
					{
						flt.inputfilter().get()->setState( protocol::InputFilter::Error, "Buffer too small");
						goto ERROR_READ;
					}
					else
					{
						goto READ_INPUT;
					}
				}
				case protocol::InputFilter::Error: goto ERROR_READ;
				case protocol::InputFilter::Open:
				{
					protocol::InputFilter* follow = flt.inputfilter()->createFollow();
					if (follow)
					{
						flt.inputfilter().reset( follow);
						goto PROCESS_READ;
					}
					else
					{
						goto ERROR_READ;
					}
				}
			}
		}
	}
	PROCESS_WRITE:
	{
		protocol::OutputFilter::ElementType tt = (protocol::OutputFilter::ElementType) elementType;
		if (!flt.outputfilter().get()->print( tt, element, elementsize))
		{
			switch (flt.outputfilter().get()->state())
			{
				case protocol::OutputFilter::EndOfBuffer:
				{
					goto WRITE_OUTPUT;
				}
				case protocol::OutputFilter::Error: goto ERROR_WRITE;
				case protocol::OutputFilter::Open:
				{
					protocol::OutputFilter* follow = flt.outputfilter()->createFollow();
					if (follow)
					{
						flt.outputfilter().reset( follow);
						goto PROCESS_WRITE;
					}
					else
					{
						goto ERROR_WRITE;
					}
				}

			}
		}
		if (elementType == protocol::InputFilter::OpenTag)
		{
			++taglevel;
		}
		else if (elementType == protocol::InputFilter::CloseTag)
		{
			--taglevel;
			if (taglevel == 0) goto TERMINATE;
		}
		goto PROCESS_READ;
	}
	ERROR_READ:
	{
		LOG_ERROR << "Error in input: '" << flt.inputfilter().get()->getError() << "'";
		goto TERMINATE;
	}
	ERROR_WRITE:
	{
		LOG_ERROR << "Error in output: '" << flt.outputfilter().get()->getError() << "'";
		goto TERMINATE;
	}
	TERMINATE:
	{
		os.write( buf.outbuf, flt.outputfilter().get()->pos());
	}
	return true;
}


