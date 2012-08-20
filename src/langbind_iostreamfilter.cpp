/************************************************************************

 Copyright (C) 2011, 2012 Project Wolframe.
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

#include "langbind/iostreamfilter.hpp"
#include "langbind/appGlobalContext.hpp"
#include "serialize/ddl/filtermapDDLParse.hpp"
#include "serialize/ddl/filtermapDDLSerialize.hpp"
#include "filter/token_filter.hpp"
#include "filter/typingfilter.hpp"
#if WITH_LUA
#include "cmdbind/luaCommandHandler.hpp"
#include "langbind/luaObjects.hpp"
#endif
#include <boost/algorithm/string.hpp>
#include <string>
#include <fstream>
#include <iostream>

using namespace _Wolframe;
using namespace langbind;

static Filter getFilter( GlobalContext* gc, const std::string& ifl, const std::string& ofl)
{
	Filter rt;
	if (boost::iequals( ofl, ifl))
	{
		if (!gc->getFilter( ifl.c_str(), rt))
		{
			std::ostringstream msg;
			msg << "unknown filter '" << ifl << "'";
			throw std::runtime_error( msg.str());
		}
	}
	else
	{
		Filter in;
		Filter out;
		if (!gc->getFilter( ifl.c_str(), in))
		{
			std::ostringstream msg;
			msg << "unknown input filter '" << ifl << "'";
			throw std::runtime_error( msg.str());
		}
		if (!gc->getFilter( ofl.c_str(), out))
		{
			std::ostringstream msg;
			msg << "unknown output filter '" << ofl << "'";
			throw std::runtime_error( msg.str());
		}
		rt = Filter( in.inputfilter(), out.outputfilter());
	}
	return rt;
}

static void readInput( char* buf, unsigned int bufsize, std::istream& is, InputFilter& iflt)
{
	std::size_t pp = 0;
	while (pp < bufsize && !is.eof())
	{
		is.read( buf+pp, sizeof(char));
		if (!is.eof()) ++pp;
	}
	iflt.putInput( buf, pp, is.eof());
	iflt.setState( InputFilter::Open);
}

static void checkUnconsumedInput( std::istream& is, InputFilter& iflt)
{
	union
	{
		const void* ptr_;
		const char* ptr;
	} data;
	std::size_t ii,size;
	bool end;
	iflt.getRest( data.ptr_, size, end);
	for (ii=0; ii<size; ++ii)
	{
		if (data.ptr[ii] < 0 || data.ptr[ii] > 32)
		{
			throw std::runtime_error( "unconsumed input left");
		}
	}
	while (!end)
	{
		char ch = 0;
		is.read( &ch, sizeof(char));
		if (ch < 0 || ch > 32)
		{
			throw std::runtime_error( "unconsumed input left");
		}
		end = is.eof();
	}
}

static void writeOutput( char* buf, unsigned int size, std::ostream& os, OutputFilter& oflt)
{
	os.write( buf, oflt.getPosition());
	oflt.setOutputBuffer( buf, size);
	oflt.setState( OutputFilter::Open);
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

static void processIO( BufferStruct& buf, InputFilter* iflt, OutputFilter* oflt, std::istream& is, std::ostream& os)
{
	if (!iflt || !oflt)
	{
		throw std::runtime_error( "lost filter");
	}
	switch (iflt->state())
	{
		case InputFilter::Open:
			break;

		case InputFilter::EndOfMessage:
			if (is.eof()) throw std::runtime_error( "unexpected end of input");
			readInput( buf.inbuf, buf.insize, is, *iflt);
			return;

		case InputFilter::Error:
		{
			std::ostringstream msg;
			msg << "error in input filter " << iflt->getError() << "'";
			throw std::runtime_error( msg.str());
		}
	}
	switch (oflt->state())
	{
		case OutputFilter::Open:
			throw std::runtime_error( "unknown error");

		case OutputFilter::EndOfBuffer:
			writeOutput( buf.outbuf, buf.outsize, os, *oflt);
			return;

		case OutputFilter::Error:
		{
			std::ostringstream msg;
			msg << "error in output filter " << oflt->getError() << "'";
			throw std::runtime_error( msg.str());
		}
	}
}


void _Wolframe::langbind::iostreamfilter( const std::string& proc, const std::string& ifl, std::size_t ib, const std::string& ofl, std::size_t ob, std::istream& is, std::ostream& os)
{
	GlobalContext* gc = getGlobalContext();
	Filter flt = getFilter( gc, ifl, ofl);
	if (!flt.inputfilter().get()) throw std::runtime_error( "input filter not found");
	if (!flt.outputfilter().get()) throw std::runtime_error( "output filter not found");

	BufferStruct buf( ib, ob);
	flt.outputfilter()->setOutputBuffer( buf.outbuf, buf.outsize);

	if (proc.size() == 0 || proc == "-")
	{
		const void* elem;
		int taglevel = 0;
		std::size_t elemsize;
		InputFilter::ElementType etype;

		while (taglevel >= 0)
		{
			if (!flt.inputfilter().get()->getNext( etype, elem, elemsize))
			{
				processIO( buf, flt.inputfilter().get(), flt.outputfilter().get(), is, os);
				continue;
			}
			if (etype == FilterBase::OpenTag)
			{
				taglevel++;
			}
			else if (etype == FilterBase::CloseTag)
			{
				taglevel--;
			}
			if (taglevel >= 0)
			{
				while (!flt.outputfilter().get()->print( etype, elem, elemsize))
				{
					processIO( buf, flt.inputfilter().get(), flt.outputfilter().get(), is, os);
				}
			}
		}
		checkUnconsumedInput( is, *flt.inputfilter());
		writeOutput( buf.outbuf, buf.outsize, os, *flt.outputfilter());
		if (taglevel != -1) throw std::runtime_error( "tags not balanced");
		return;
	}
#if WITH_LUA
	{
		LuaScriptInstanceR sc;
		if (gc->getLuaScriptInstance( proc, sc))
		{
			if (!gc->initLuaScriptInstance( sc.get(), Input(flt.inputfilter()), Output(flt.outputfilter())))
			{
				throw std::runtime_error( "error initializing lua script");
			}
			lua_getglobal( sc->thread(), proc.c_str());
			int rt = lua_resume( sc->thread(), NULL, 0);
			while (rt == LUA_YIELD)
			{
				processIO( buf, flt.inputfilter().get(), flt.outputfilter().get(), is, os);
				rt = lua_resume( sc->thread(), NULL, 0);
			}
			if (rt == LUA_OK)
			{
				writeOutput( buf.outbuf, buf.outsize, os, *flt.outputfilter());
			}
			else
			{
				throw std::runtime_error( lua_tostring( sc->thread(), -1));
			}
			checkUnconsumedInput( is, *flt.inputfilter());
			return;
		}
	}
#endif
	{
		FormFunction func;
		if (gc->getFormFunction( proc.c_str(), func))
		{
			flt.inputfilter()->setValue( "empty", "false");
			TypedInputFilterR inp( new TypingInputFilter( flt.inputfilter()));
			TypedOutputFilterR outp( new TypingOutputFilter( flt.outputfilter()));
			FormFunctionClosure closure( func);
			closure.init( inp, serialize::Context::ValidateAttributes);

			while (!closure.call()) processIO( buf, flt.inputfilter().get(), flt.outputfilter().get(), is, os);

			serialize::StructSerializer res = closure.result();
			res.init( outp);

			while (!res.call()) processIO( buf, flt.inputfilter().get(), flt.outputfilter().get(), is, os);

			writeOutput( buf.outbuf, buf.outsize, os, *flt.outputfilter());
			checkUnconsumedInput( is, *flt.inputfilter());
			return;
		}
	}
	{
		DDLForm df;
		if (gc->getForm( proc.c_str(), df))
		{
			flt.inputfilter()->setValue( "empty", "false");
			TypedInputFilterR inp( new TypingInputFilter( flt.inputfilter()));
			TypedOutputFilterR outp( new TypingOutputFilter( flt.outputfilter()));
			serialize::DDLStructParser closure( df.structure());
			closure.init( inp, serialize::Context::ValidateAttributes);

			while (!closure.call()) processIO( buf, flt.inputfilter().get(), flt.outputfilter().get(), is, os);

			serialize::DDLStructSerializer res( df.structure());
			res.init( outp, serialize::Context::None);

			while (!res.call()) processIO( buf, flt.inputfilter().get(), flt.outputfilter().get(), is, os);

			writeOutput( buf.outbuf, buf.outsize, os, *flt.outputfilter());
			checkUnconsumedInput( is, *flt.inputfilter());
			return;
		}
	}
	{
		PeerFormFunction func;
		if (gc->getPeerFormFunction( proc.c_str(), func))
		{
			flt.inputfilter()->setValue( "empty", "false");
			TypedInputFilterR inp( new TypingInputFilter( flt.inputfilter()));
			TypedOutputFilterR outp( new TypingOutputFilter( flt.outputfilter()));
			PeerFormFunctionClosure closure( func);
			closure.init( inp);

			while (!closure.call()) processIO( buf, flt.inputfilter().get(), flt.outputfilter().get(), is, os);

			serialize::DDLStructSerializer res( closure.result().structure());
			res.init( outp, serialize::Context::None);

			while (!res.call()) processIO( buf, flt.inputfilter().get(), flt.outputfilter().get(), is, os);

			writeOutput( buf.outbuf, buf.outsize, os, *flt.outputfilter());
			checkUnconsumedInput( is, *flt.inputfilter());
			return;
		}
	}
	throw std::runtime_error( "command not found");
}


