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
///\file wolfilterIostreamFilter.cpp
///\brief Implementation of a kind of pipe (istream|ostream) through wolframe mappings like filters, forms, functions
#include "wolfilterIostreamFilter.hpp"
#include "contentOnlyProtocolHandler.hpp"
#include "serialize/ddlFormSerializer.hpp"
#include "serialize/ddlFormParser.hpp"
#include "serialize/ddl/ddlStructParser.hpp"
#include "serialize/ddl/ddlStructSerializer.hpp"
#include "processor/execContext.hpp"
#include "cmdbind/protocolHandler.hpp"
#include "filter/typingfilter.hpp"
#include "filter/null_filter.hpp"
#include "filter/redirectFilterClosure.hpp"
#include "langbind/formFunction.hpp"
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

static Filter getFilter( const proc::ProcessorProviderInterface* provider, const std::string& ifl_, const std::string& ofl_)
{
	Filter rt;
	if (ifl_.empty() && ofl_.empty())
	{
		return langbind::createNullFilter();
	}
	std::pair<std::string,std::vector<langbind::FilterArgument> > ifl = filterIdentifier( ifl_);
	std::pair<std::string,std::vector<langbind::FilterArgument> > ofl = filterIdentifier( ofl_);
	if (boost::iequals( ofl_, ifl_))
	{
		const FilterType* ft = provider->filterType( ifl.first);
		if (!ft)
		{
			std::ostringstream msg;
			msg << "unknown filter '" << ifl.first << "'";
			throw std::runtime_error( msg.str());
		}
		else
		{
			FilterR fp( ft->create( ifl.second));
			rt = *fp;
		}
	}
	else
	{
		const FilterType* inft = provider->filterType( ifl.first);
		const FilterType* outft = provider->filterType( ofl.first);
		if (!inft)
		{
			std::ostringstream msg;
			msg << "unknown input filter '" << ifl.first << "'";
			throw std::runtime_error( msg.str());
		}
		if (!outft)
		{
			std::ostringstream msg;
			msg << "unknown output filter '" << ofl.first << "'";
			throw std::runtime_error( msg.str());
		}
		FilterR in( inft->create( ifl.second));
		FilterR out( outft->create( ofl.second));
		
		out->outputfilter()->inheritMetaData( in->inputfilter()->getMetaDataRef());
		rt = Filter( in->inputfilter(), out->outputfilter());
	}
	return rt;
}

static void readFilterInput( char* buf, unsigned int bufsize, std::istream& is, InputFilter& iflt)
{
	std::size_t pp = 0;
	while (pp < bufsize && !is.eof())
	{
		is.read( buf+pp, sizeof(char));
		if (!is.eof()) ++pp;
	}
	iflt.putInput( buf, pp, is.eof());
	if (iflt.state() == InputFilter::Error)
	{
		std::ostringstream msg;
		msg << "error processing input: '" << iflt.getError() << "'";
		throw std::runtime_error( msg.str());
	}
}

static bool readProtocolInput( char* buf, unsigned int bufsize, std::istream& is, cmdbind::ProtocolHandler* cmdh)
{
	if (is.eof())
	{
		cmdh->putEOF();
		return false;
	}
	std::size_t pp = 0;
	while (pp < bufsize && !is.eof())
	{
		is.read( buf+pp, sizeof(char));
		if (!is.eof()) ++pp;
	}
	if (pp == 0 && is.eof())
	{
		cmdh->putEOF();
		return false;
	}
	else
	{
		cmdh->putInput( buf, pp);
		return true;
	}
}



static void checkUnconsumedInput( std::istream& is)
{
	bool end = 0;
	while (!end)
	{
		char ch = 0;
		is.read( &ch, sizeof(char));
		if ((unsigned char)ch > 32)
		{
			std::string str;
			str.push_back( (unsigned char)ch < 32?'.':ch);

			std::size_t pp = 0;
			while (pp < 20 && !is.eof())
			{
				is.read( &ch, sizeof(char));
				if (!is.eof())
				{
					str.push_back( (unsigned char)ch < 32?'.':ch);
					++pp;
				}
			}
			throw std::runtime_error( std::string("unconsumed input left [") + str + "...]");
		}
		end = is.eof();
	}
}

static void writeOutput( std::ostream& os, OutputFilter& oflt)
{
	const void* buf;
	std::size_t bufsize;

	oflt.getOutput( buf, bufsize);
	os.write( (const char*)buf, bufsize);
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
		case InputFilter::Start:
		case InputFilter::Open:
			break;

		case InputFilter::EndOfMessage:
			if (is.eof()) throw std::runtime_error( "unexpected end of input (EOF)");
			readFilterInput( buf.inbuf, buf.insize, is, *iflt);
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
		case OutputFilter::Start:
		case OutputFilter::Open:
			throw std::runtime_error( "unknown error");

		case OutputFilter::EndOfBuffer:
			writeOutput( os, *oflt);
			return;

		case OutputFilter::Error:
		{
			std::ostringstream msg;
			msg << "error writing output: '" << oflt->getError() << "'";
			throw std::runtime_error( msg.str());
		}
	}
}

static bool processProtocolHandler( BufferStruct& buf, cmdbind::ProtocolHandler* cmdh, std::istream& is, std::ostream& os, std::string& lasterr)
{
	bool eod = false;
	cmdh->setInputBuffer( buf.inbuf, buf.insize);

	for (;;) switch (cmdh->nextOperation())
	{
		case cmdbind::ProtocolHandler::READ:
			if (eod) throw std::runtime_error( "protocol handler trying to read after end of data");
			eod = !readProtocolInput( buf.inbuf, buf.insize, is, cmdh);
			continue;

		case cmdbind::ProtocolHandler::WRITE:
		{
			const void* cmdh_output;
			std::size_t cmdh_outputsize;
			cmdh->getOutput( cmdh_output, cmdh_outputsize);
			os << std::string( (const char*)cmdh_output, cmdh_outputsize);
			continue;
		}
		case cmdbind::ProtocolHandler::CLOSE:
		{
			const char* error = cmdh->lastError();
			if (error)
			{ 
				std::ostringstream msg;
				msg << "error process command handler: " << error;
				lasterr = msg.str();
				return false;
			}
			return true;
		}
	}
}


void langbind::iostreamfilter( proc::ExecContext* execContext, const std::string& protocol, const std::string& proc, const std::string& ifl, std::size_t ib, const std::string& ofl, std::size_t ob, std::istream& is, std::ostream& os)
{
	BufferStruct buf( ib, ob);
	const proc::ProcessorProviderInterface* provider = execContext->provider();
		
	if (proc.size() == 0 || proc == "-")
	{
		LOG_TRACE << "Start executing filter";
		Filter flt = getFilter( provider, ifl, ofl);
		if (!flt.inputfilter().get()) throw std::runtime_error( "input filter not found");
		if (!flt.outputfilter().get()) throw std::runtime_error( "output filter not found");
		flt.outputfilter()->setOutputChunkSize( buf.outsize);

		// ... no command specified -> we simply map the input through the
		//	input/output filters specified:
		if (ifl.empty() && ofl.empty()) throw std::runtime_error( "argument for command, form or function not defined and no filter for processing specified");

		const void* elem;
		int taglevel = 0;
		std::size_t elemsize;
		FilterBase::ElementType etype;

		while (taglevel >= 0)
		{
			if (!flt.inputfilter().get()->getNext( etype, elem, elemsize))
			{
				if (flt.inputfilter()->state() == InputFilter::Open || flt.inputfilter()->state() == InputFilter::Start)
				{
					throw std::runtime_error( "filter not delivering final close tag");
				}
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
			while (!flt.outputfilter().get()->print( etype, elem, elemsize))
			{
				processIO( buf, flt.inputfilter().get(), flt.outputfilter().get(), is, os);
			}
			LOG_DATA << "[iostream filter] print " << FilterBase::elementTypeName(etype) << " '" << std::string( (const char*)elem, elemsize) << "'"; 
		}
		while (!flt.outputfilter().get()->close())
		{
			processIO( buf, flt.inputfilter().get(), flt.outputfilter().get(), is, os);
		}
		checkUnconsumedInput( is);
		writeOutput( os, *flt.outputfilter());
		if (taglevel != -1) throw std::runtime_error( "tags not balanced");
		return;
	}
	{
		if (proc[ proc.size()-1] == '~' || provider->hasCommand( proc))
		{
			LOG_TRACE << "Start executing command '" << proc << "'";

			cmdbind::ProtocolHandlerR protocolhnd;
			if (protocol.empty())
			{
				protocolhnd.reset( new cmdbind::ContentOnlyProtocolHandler());
			}
			else
			{
				protocolhnd.reset( provider->protocolHandler( protocol));
				if (!protocolhnd.get()) throw std::runtime_error( std::string("protocol '") + protocol + "' is not defined");
			}
			protocolhnd->setExecContext( execContext);
			if (proc[ proc.size()-1] == '~')
			{
				protocolhnd->setArgumentString( std::string( proc.c_str(), proc.size()-1));
			}
			else
			{
				protocolhnd->setArgumentString( proc + "!");
			}
			protocolhnd->setOutputBuffer( buf.outbuf, buf.outsize, 0);

			std::string lasterr;
			if (!processProtocolHandler( buf, protocolhnd.get(), is, os, lasterr))
			{
				throw std::runtime_error( lasterr);
			}
			// Check if there is unconsumed input left (must not happen):
			checkUnconsumedInput( is);
			return;
		}
	}
	{
		const FormFunction* func = provider->formFunction( proc);
		if (func)
		{
			LOG_TRACE << "Start executing form function '" << proc << "'";

			// ... command is the name of a form function we call directly
			//	with the filter specified:
			Filter flt = getFilter( provider, ifl, ofl);
			if (!flt.inputfilter().get()) throw std::runtime_error( "input filter not found");
			if (!flt.outputfilter().get()) throw std::runtime_error( "output filter not found");
			flt.outputfilter()->setOutputChunkSize( buf.outsize);

			flt.inputfilter()->setValue( "empty", "false");
			TypedInputFilterR inp( new TypingInputFilter( flt.inputfilter()));
			TypedOutputFilterR outp( new TypingOutputFilter( flt.outputfilter()));
			FormFunctionClosureR closure( func->createClosure());
			closure->init( execContext, inp, serialize::Flags::ValidateAttributes);

			while (!closure->call()) processIO( buf, flt.inputfilter().get(), flt.outputfilter().get(), is, os);

			RedirectFilterClosure res( closure->result(), outp, true);
			while (!res.call()) processIO( buf, flt.inputfilter().get(), flt.outputfilter().get(), is, os);
			while (!flt.outputfilter()->close()) processIO( buf, flt.inputfilter().get(), flt.outputfilter().get(), is, os);

			writeOutput( os, *flt.outputfilter());
			checkUnconsumedInput( is);
			return;
		}
	}
	{
		const types::FormDescription* st = provider->formDescription( proc);
		if (st)
		{
			LOG_TRACE << "Start mapping through form '" << proc << "'";

			// ... command is the name a form description -> we simply map
			//	the input with the input/output filters specified 
			//	through the form:
			Filter flt = getFilter( provider, ifl, ofl);
			if (!flt.inputfilter().get()) throw std::runtime_error( "input filter not found");
			if (!flt.outputfilter().get()) throw std::runtime_error( "output filter not found");
			flt.outputfilter()->setOutputChunkSize( buf.outsize);

			types::Form df( st);
			flt.inputfilter()->setValue( "empty", "false");
			TypedInputFilterR inp( new TypingInputFilter( flt.inputfilter()));
			TypedOutputFilterR outp( new TypingOutputFilter( flt.outputfilter()));
			serialize::DDLStructParser closure( &df);
			closure.init( inp, serialize::Flags::ValidateAttributes);

			while (!closure.call()) processIO( buf, flt.inputfilter().get(), flt.outputfilter().get(), is, os);

			serialize::DDLStructSerializer res( &df);
			res.init( outp, serialize::Flags::None);

			while (!res.call()) processIO( buf, flt.inputfilter().get(), flt.outputfilter().get(), is, os);
			while (!flt.outputfilter()->close()) processIO( buf, flt.inputfilter().get(), flt.outputfilter().get(), is, os);

			writeOutput( os, *flt.outputfilter());
			checkUnconsumedInput( is);
			return;
		}
	}
	throw std::runtime_error( std::string("identifier '") + proc + "' not defined as command, form or function");
}


