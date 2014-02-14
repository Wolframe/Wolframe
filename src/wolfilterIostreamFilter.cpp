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
///\file wolfilterIostreamFilter.hpp
///\brief Implementation of a kind of pipe (istream|ostream) through wolframe mappings like filters, forms, functions
#include "wolfilterIostreamFilter.hpp"
#include "langbind/appObjects.hpp"
#include "langbind/formFunction.hpp"
#include "serialize/ddl/filtermapDDLParse.hpp"
#include "serialize/ddl/filtermapDDLSerialize.hpp"
#include "filter/typingfilter.hpp"
#include "filter/null_filter.hpp"
#include "utils/stringUtils.hpp"
#include "logger-v1.hpp"
#include <boost/algorithm/string.hpp>
#include <string>
#include <fstream>
#include <iostream>

using namespace _Wolframe;
using namespace langbind;

std::pair<std::string, std::vector<langbind::FilterArgument> > filterIdentifier( const std::string& id)
{
	std::pair<std::string, std::vector<langbind::FilterArgument> > rt;
	const char* cc = id.c_str();
	const char* ee = cc;
	const char* end = id.c_str() + id.size();
	std::string argstr;

	while (*ee && *ee != ',' && *ee != '(') ++ee;
	if (*ee == '(')
	{
		while (end != ee)
		{
			--end;
			if ((unsigned char)*end == ')') break;
			if ((unsigned char)*end > 32) throw std::runtime_error(std::string("syntax error in filter expression '") + id + "'");
		}
		if (end == ee) throw std::runtime_error(std::string("syntax error in filter expression '") + id + "'");
		argstr = std::string( ee+1, end-ee-1);
		rt.first = std::string( cc, ee-cc);
	}
	else if (!*ee)
	{
		rt.first = std::string( id);
	}
	else
	{
		argstr = std::string( ee+1);
		rt.first = std::string( cc, ee-cc);
	}
	std::vector<std::string> spl;
	utils::splitString( spl, argstr, ",");
	std::vector<std::string>::const_iterator ai = spl.begin(), ae = spl.end();
	for (; ai != ae; ++ai)
	{
		std::vector<std::string> asg;
		utils::splitString( asg, *ai, "=");
		if (asg.size() == 1)
		{
			rt.second.push_back( langbind::FilterArgument("",*ai));
		}
		else if (asg.size() == 2)
		{
			rt.second.push_back( langbind::FilterArgument( asg.at(0), asg.at(1)));
		}
		else
		{
			throw std::runtime_error( "syntax error in filter arguments");
		}
	}
	return rt;
}

static std::string filterargAsString( const std::vector<langbind::FilterArgument>& arg)
{
	std::ostringstream out;
	std::vector<langbind::FilterArgument>::const_iterator ai = arg.begin(), ae = arg.end();
	for (; ai != ae; ++ai)
	{
		if (ai != arg.begin()) out << ", ";
		out << ai->first << "='" << ai->second << "'";
	}
	return out.str();
}

static Filter getFilter( proc::ProcessorProvider* provider, const std::string& ifl_, const std::string& ofl_)
{
	Filter rt;
	if (ifl_.empty() && ofl_.empty())
	{
		return langbind::createNullFilter( "", "");
	}
	std::pair<std::string,std::vector<langbind::FilterArgument> > ifl = filterIdentifier( ifl_);
	std::pair<std::string,std::vector<langbind::FilterArgument> > ofl = filterIdentifier( ofl_);
	if (boost::iequals( ofl_, ifl_))
	{
		Filter* fp = provider->filter( ifl.first, ifl.second);
		if (!fp)
		{
			std::ostringstream msg;
			msg << "unknown filter: name = '" << ifl.first << "' ,arguments = '" << filterargAsString(ofl.second) << "'";
			throw std::runtime_error( msg.str());
		}
		else
		{
			rt = *fp;
			delete fp;
		}
	}
	else
	{
		Filter* in = provider->filter( ifl.first, ifl.second);
		Filter* out = provider->filter( ofl.first, ofl.second);
		if (!in)
		{
			std::ostringstream msg;
			msg << "unknown input filter: name = '" << ifl.first << "' arguments = '" << filterargAsString( ifl.second) << "'";
			throw std::runtime_error( msg.str());
		}
		if (!out)
		{
			delete in;
			std::ostringstream msg;
			msg << "unknown output filter: name = '" << ofl.first << "' arguments = '" << filterargAsString( ofl.second) << "'";
			throw std::runtime_error( msg.str());
		}
		rt = Filter( in->inputfilter(), out->outputfilter());
		delete in;
		delete out;
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
		if ((unsigned char)data.ptr[ii] > 32)
		{
			throw std::runtime_error( "unconsumed input left");
		}
	}
	while (!end)
	{
		char ch = 0;
		is.read( &ch, sizeof(char));
		if ((unsigned char)ch > 32)
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
			msg << "error processing input: '" << iflt->getError() << "'";
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
			msg << "error writing output: '" << oflt->getError() << "'";
			throw std::runtime_error( msg.str());
		}
	}
}


void _Wolframe::langbind::iostreamfilter( proc::ProcessorProvider* provider, const std::string& proc, const std::string& ifl, std::size_t ib, const std::string& ofl, std::size_t ob, std::istream& is, std::ostream& os)
{
	Filter flt = getFilter( provider, ifl, ofl);
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
				LOG_DATA << "[iostream filter] print " << std::string( (const char*)elem, elemsize);
			}
		}
		checkUnconsumedInput( is, *flt.inputfilter());
		writeOutput( buf.outbuf, buf.outsize, os, *flt.outputfilter());
		if (taglevel != -1) throw std::runtime_error( "tags not balanced");
		return;
	}
	{
		cmdbind::IOFilterCommandHandler* hnd = provider->iofilterhandler( proc);
		if (hnd)
		{
			cmdbind::CommandHandlerR cmdhnd( hnd);
			hnd->passParameters( proc, 0, 0);
			// Take the input filter defined in the command handler if not passed as parameter:
			if (ifl.empty() && hnd->inputfilter().get())
			{
				flt.inputfilter() = hnd->inputfilter();
			}
			else
			{
				hnd->setFilter( flt.inputfilter());
			}
			// Take the output filter defined in the command handler if not passed as parameter:
			if (ofl.empty() && hnd->outputfilter().get())
			{
				flt.outputfilter() = hnd->outputfilter();
				hnd->outputfilter()->setOutputBuffer( buf.outbuf, buf.outsize);
			}
			else
			{
				hnd->setFilter( flt.outputfilter());
			}
			// Processing:
			const char* errmsg;
			cmdbind::IOFilterCommandHandler::CallResult res;

			while ((res = hnd->call( errmsg)) == cmdbind::IOFilterCommandHandler::Yield)
			{
				processIO( buf, flt.inputfilter().get(), flt.outputfilter().get(), is, os);
			}
			if (res == cmdbind::IOFilterCommandHandler::Error)
			{
				std::string emsg( errmsg?errmsg:"unknown");
				throw std::runtime_error( emsg);
			}
			else
			{
				writeOutput( buf.outbuf, buf.outsize, os, *flt.outputfilter());
			}
			checkUnconsumedInput( is, *flt.inputfilter());
			return;
		}
	}
	{
		const FormFunction* func = provider->formFunction( proc);
		if (func)
		{
			flt.inputfilter()->setValue( "empty", "false");
			TypedInputFilterR inp( new TypingInputFilter( flt.inputfilter()));
			TypedOutputFilterR outp( new TypingOutputFilter( flt.outputfilter()));
			FormFunctionClosureR closure( func->createClosure());
			closure->init( provider, inp, serialize::Context::ValidateAttributes);

			while (!closure->call()) processIO( buf, flt.inputfilter().get(), flt.outputfilter().get(), is, os);

			RedirectFilterClosure res( closure->result(), outp);
			while (!res.call()) processIO( buf, flt.inputfilter().get(), flt.outputfilter().get(), is, os);

			writeOutput( buf.outbuf, buf.outsize, os, *flt.outputfilter());
			checkUnconsumedInput( is, *flt.inputfilter());
			return;
		}
	}
	{
		const types::FormDescription* st = provider->formDescription( proc);
		if (st)
		{
			types::Form df( st);
			flt.inputfilter()->setValue( "empty", "false");
			TypedInputFilterR inp( new TypingInputFilter( flt.inputfilter()));
			TypedOutputFilterR outp( new TypingOutputFilter( flt.outputfilter()));
			serialize::DDLStructParser closure( &df);
			closure.init( inp, serialize::Context::ValidateAttributes);

			while (!closure.call()) processIO( buf, flt.inputfilter().get(), flt.outputfilter().get(), is, os);

			serialize::DDLStructSerializer res( &df);
			res.init( outp, serialize::Context::None);

			while (!res.call()) processIO( buf, flt.inputfilter().get(), flt.outputfilter().get(), is, os);

			writeOutput( buf.outbuf, buf.outsize, os, *flt.outputfilter());
			checkUnconsumedInput( is, *flt.inputfilter());
			return;
		}
	}
	throw std::runtime_error( "command not found");
}


