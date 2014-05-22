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
#include "serialize/ddlFormSerializer.hpp"
#include "serialize/ddlFormParser.hpp"
#include "langbind/formFunction.hpp"
#include "serialize/ddl/ddlStructParser.hpp"
#include "serialize/ddl/ddlStructSerializer.hpp"
#include "cmdbind/doctypeFilterCommandHandler.hpp"
#include "cmdbind/ioFilterCommandHandlerEscDLF.hpp"
#include "processor/execContext.hpp"
#include "filter/typingfilter.hpp"
#include "filter/null_filter.hpp"
#include "filter/redirectFilterClosure.hpp"
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

static Filter getFilter( const proc::ProcessorProviderInterface* provider, const std::string& ifl_, const std::string& ofl_)
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

static void readInput( char* buf, unsigned int bufsize, std::istream& is, cmdbind::CommandHandler* cmdh)
{
	std::size_t pp = 0;
	while (pp < bufsize && !is.eof())
	{
		is.read( buf+pp, sizeof(char));
		if (!is.eof()) ++pp;
	}
	if (pp == 0 && is.eof())
	{
		throw std::runtime_error("unexpected end of file");
	}
	cmdh->putInput( buf, pp);
}

static bool readInputEscLFdot( int& state, char* buf, unsigned int bufsize, std::istream& is, cmdbind::CommandHandler* cmdh)
{
	enum State {Init=0,ConsumedLF=1,ConsumedLFDot=2,PutEOF_LF=3,PutEOF_LFDot=4,PutEOF_LFDotLF=5};
	std::size_t pp = 0;
	while (pp < bufsize)
	{
		if (state == ConsumedLFDot)
		{
			state = Init;
			buf[ pp++] = '.';
		}
		else if (state == PutEOF_LF)
		{
			state = PutEOF_LFDot;
			buf[ pp++] = '\n';
		}
		else if (state == PutEOF_LFDot)
		{
			state = PutEOF_LFDotLF;
			buf[ pp++] = '.';
		}
		else if (state == PutEOF_LFDotLF)
		{
			buf[ pp++] = '\n';
			cmdh->putInput( buf, pp);
			state = Init;
			return true;
		}
		else if (is.eof())
		{
			state = PutEOF_LF;
		}
		else
		{
			is.read( buf + pp, sizeof(char));
			if (!is.eof())
			{
				switch ((State)state)
				{
					case Init:
						if (buf[pp] == '\n')
						{
							state = ConsumedLF;
						}
						break;
					case ConsumedLF:
						if (buf[pp] == '.')
						{
							state = ConsumedLFDot;
						}
						else if (buf[pp] != '\n')
						{
							state = Init;
						}
						break;
					case ConsumedLFDot:
						throw std::logic_error("illegal state");
					case PutEOF_LF:
					case PutEOF_LFDot:
					case PutEOF_LFDotLF:
						break;
				}
				++pp;
			}
		}
	}
	cmdh->putInput( buf, pp);
	return false;
}

static void writeOutputEscLFdot( int& state, const char* buf, unsigned int bufsize, std::ostream& os)
{
	enum State {
		Init=0,
		ConsumedCR,
		ConsumedLF,
		ConsumedCRLF,
		ConsumedLFDot,
		ConsumedCRLFDot,
		ConsumedLFDotCR,
		ConsumedCRLFDotCR
	};
	std::size_t pp = 0;
	std::string outstr;
	while (pp < bufsize)
	{
		switch ((State)state)
		{
			case Init:
				if (buf[pp] == '\n')
				{
					state = ConsumedLF;
				}
				else if (buf[pp] == '\r')
				{
					state = ConsumedCR;
				}
				else
				{
					outstr.push_back( buf[pp]);
				}
				break;
			case ConsumedCR:
				if (buf[pp] == '\n')
				{
					state = ConsumedCRLF;
				}
				else if (buf[pp] == '\r')
				{
					outstr.push_back( buf[pp]);
				}
				else
				{
					outstr.push_back( '\r');
					outstr.push_back( buf[pp]);
					state = Init;
				}
				break;
			case ConsumedLF:
				if (buf[pp] == '.')
				{
					state = ConsumedLFDot;
				}
				else if (buf[pp] == '\n')
				{
					outstr.push_back( '\n');
				}
				else if (buf[pp] == '\r')
				{
					outstr.push_back( '\n');
					state = ConsumedCR;
				}
				else
				{
					outstr.push_back( '\n');
					outstr.push_back( buf[pp]);
					state = Init;
				}
				break;
			case ConsumedCRLF:
				if (buf[pp] == '.')
				{
					state = ConsumedCRLFDot;
				}
				else if (buf[pp] == '\n')
				{
					outstr.push_back( '\r');
					outstr.push_back( '\n');
					state = ConsumedLF;
				}
				else if (buf[pp] == '\r')
				{
					outstr.push_back( '\r');
					outstr.push_back( '\n');
					state = ConsumedCR;
				}
				else
				{
					outstr.push_back( '\r');
					outstr.push_back( '\n');
					outstr.push_back( buf[pp]);
					state = Init;
				}
				break;
			case ConsumedLFDot:
				if (buf[pp] == '\n')
				{
					state = Init;
					//... EOF marker is swallowed
				}
				if (buf[pp] == '\r')
				{
					state = ConsumedLFDotCR;
				}
				else
				{
					outstr.push_back( '\n');
					outstr.push_back( buf[pp]);
					//... LF dot X -> LF X
					state = Init;
				}
				break;
			case ConsumedCRLFDot:
				if (buf[pp] == '\n')
				{
					state = Init;
					//... EOF marker is swallowed
				}
				if (buf[pp] == '\r')
				{
					state = ConsumedCRLFDotCR;
				}
				else
				{
					outstr.push_back( '\r');
					outstr.push_back( '\n');
					outstr.push_back( buf[pp]);
					//... CR LF dot X -> CR LF X
					state = Init;
				}
				break;
			case ConsumedLFDotCR:
			case ConsumedCRLFDotCR:
				if (buf[pp] == '\n')
				{
					state = Init;
					//... EOF marker is swallowed
				}
				else
				{
					throw std::runtime_error("expected LF after LF dot CR sequence in input (incomplete EOF marker)");
				}
				break;
		}
		++pp;
	}
	os << outstr;
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
		case InputFilter::Start:
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
		case OutputFilter::Start:
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

static bool redirectInput( BufferStruct& buf, int& stateEscOut, const void* data, std::size_t datasize, cmdbind::CommandHandler* toh, std::ostream& os)
{
	bool doEscapeLFdot = !!dynamic_cast<cmdbind::IOFilterCommandHandlerEscDLF*>( toh);
	const void* toh_output;
	std::size_t toh_outputsize;
	const char* error;
	toh->setInputBuffer( const_cast<char*>((const char*)data), datasize);
	toh->putInput( data, datasize);

	for (;;) switch (toh->nextOperation())
	{
		case cmdbind::CommandHandler::READ:
			toh->setInputBuffer( buf.inbuf, buf.insize);
			return true;
		case cmdbind::CommandHandler::WRITE:
			toh->getOutput( toh_output, toh_outputsize);
			if (doEscapeLFdot)
			{
				writeOutputEscLFdot( stateEscOut, (const char*)toh_output, toh_outputsize, os);
			}
			else
			{
				os << std::string( (const char*)toh_output, toh_outputsize);
			}
			continue;
		case cmdbind::CommandHandler::CLOSE:
			error = toh->lastError();
			if (error)
			{ 
				std::ostringstream msg;
				msg << "error redirect input: " << error;
				throw std::runtime_error( msg.str());
			}
			toh->setInputBuffer( buf.inbuf, buf.insize);
			return false;
	}
}

static void processCommandHandler( BufferStruct& buf, int& stateEscIn, int& stateEscOut, cmdbind::CommandHandler* cmdh, std::istream& is, std::ostream& os, bool doEscapeLFdot)
{
	const void* cmdh_output;
	std::size_t cmdh_outputsize;
	void* cmdh_input;
	std::size_t cmdh_inputsize;
	const char* error;
	cmdh->setInputBuffer( buf.inbuf, buf.insize);
	int eofCnt = 0;

	for (;;) switch (cmdh->nextOperation())
	{
		case cmdbind::CommandHandler::READ:
			if (doEscapeLFdot)
			{
				cmdh->getInputBlock( cmdh_input, cmdh_inputsize);
				if (readInputEscLFdot( stateEscIn, (char*)cmdh_input, cmdh_inputsize, is, cmdh))
				{
					if (++eofCnt > 2)
					{
						throw std::runtime_error("got READ after processing EoD");
					}
				}
			}
			else
			{
				readInput( buf.inbuf, buf.insize, is, cmdh);
			}
			continue;

		case cmdbind::CommandHandler::WRITE:
			cmdh->getOutput( cmdh_output, cmdh_outputsize);
			if (doEscapeLFdot)
			{
				writeOutputEscLFdot( stateEscOut, (const char*)cmdh_output, cmdh_outputsize, os);
			}
			else
			{
				os << std::string( (const char*)cmdh_output, cmdh_outputsize);
			}
			continue;

		case cmdbind::CommandHandler::CLOSE:
			error = cmdh->lastError();
			if (error)
			{ 
				std::ostringstream msg;
				msg << "error process command handler: " << error;
				throw std::runtime_error( msg.str());
			}
			return;
	}
}


void _Wolframe::langbind::iostreamfilter( proc::ExecContext* execContext, const std::string& proc, const std::string& ifl, std::size_t ib, const std::string& ofl, std::size_t ob, std::istream& is, std::ostream& os)
{
	BufferStruct buf( ib, ob);
	const proc::ProcessorProviderInterface* provider = execContext->provider();
		
	if (proc.size() == 0 || proc == "-")
	{
		Filter flt = getFilter( provider, ifl, ofl);
		if (!flt.inputfilter().get()) throw std::runtime_error( "input filter not found");
		if (!flt.outputfilter().get()) throw std::runtime_error( "output filter not found");
		flt.outputfilter()->setOutputBuffer( buf.outbuf, buf.outsize);

		// ... no command specified -> we simply map the input through the
		//	input/output filters specified:
		if (ifl.empty() && ofl.empty()) throw std::runtime_error( "argument for command, form or function not defined and no filter for processing specified");

		const void* elem;
		int taglevel = 0;
		std::size_t elemsize;
		InputFilter::ElementType etype;

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
			LOG_DATA << "[iostream filter] print " << std::string( (const char*)elem, elemsize);
		}
		checkUnconsumedInput( is, *flt.inputfilter());
		writeOutput( buf.outbuf, buf.outsize, os, *flt.outputfilter());
		if (taglevel != -1) throw std::runtime_error( "tags not balanced");
		return;
	}
	{
		if (proc[ proc.size()-1] == '~')
		{
			// ... command is handled by a command handler
			//	-> detect document type with the doctype detection,
			//	evaluate the command to execute (substitute '~' with doctype id)
			//	and initialize the command handler with the docformat
			//	as first parameter as the main protocol does.
			Filter flt = getFilter( provider, ifl, ofl);
			bool doEscapeLFdot = false;

			cmdbind::DoctypeFilterCommandHandler* dtfh = new cmdbind::DoctypeFilterCommandHandler();
			cmdbind::CommandHandlerR dtfh_scoped( dtfh);
			dtfh->setOutputBuffer( buf.outbuf, buf.outsize, 0);

			// Detect document format:
			int stateEscIn = 0;
			int stateEscOut = 0;
			processCommandHandler( buf, stateEscIn, stateEscOut, dtfh, is, os, doEscapeLFdot);

			std::string docformat = dtfh->docformatid();
			std::string doctype = dtfh->doctypeid();
			std::string cmdname = std::string( proc.c_str(), proc.size()-1) + doctype;

			cmdbind::CommandHandler* cmdh = provider->cmdhandler( cmdname);
			cmdbind::CommandHandlerR cmdh_scoped( cmdh);
			if (cmdh)
			{
				cmdh->setExecContext( execContext);
				cmdbind::IOFilterCommandHandlerEscDLF* ifch = dynamic_cast<cmdbind::IOFilterCommandHandlerEscDLF*>( cmdh);

				if (ifch) 
				{
					LOG_DEBUG << "command handler is processing CRLFdot escaped content";
					if (flt.inputfilter().get()) ifch->setFilter( flt.inputfilter());
					if (flt.outputfilter().get())
					{
						ifch->setFilter( flt.outputfilter());
						flt.outputfilter()->setOutputBuffer( buf.outbuf, buf.outsize);
					}
					doEscapeLFdot = true;
				}
				const char* cmd_argv = docformat.c_str();
				cmdh->passParameters( cmdname, 1, &cmd_argv);
				cmdh->setOutputBuffer( buf.outbuf, buf.outsize, 0);

				// Get data (consumed and rest = not processed) from 
				//	the doc type detection command handler 
				//	to redirect to the executing command handler:
				std::string databuf;
				void* comsumed_buffer;
				std::size_t comsumed_size;
				dtfh->getInputBuffer( comsumed_buffer, comsumed_size);
				const void* rest_buffer;
				std::size_t rest_size;
				dtfh->getDataLeft( rest_buffer, rest_size);
				databuf.append( (const char*)comsumed_buffer, comsumed_size);
				databuf.append( (const char*)rest_buffer, rest_size);
	
				// Redirect input processed and call command handler 
				//	to process if there is still input left:
				if (redirectInput( buf, stateEscOut, databuf.c_str(), databuf.size(), cmdh, os))
				{
					processCommandHandler( buf, stateEscIn, stateEscOut, cmdh, is, os, doEscapeLFdot);
				}
				// Check if there is unconsumed input left (must not happen):
				bool end = false;
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
				return;
			}
			else
			{
				throw std::runtime_error( std::string("cannot find command handler for '") + cmdname + "' (" + proc + ")");
			}
		}
	}
	{
		cmdbind::CommandHandler* cmdh = provider->cmdhandler( proc);
		if (cmdh)
		{
			// ... command is handled by a command handler
			//	-> detect document format with the doctype detection
			//	and initialize the command handler with the docformat
			//	as first parameter as the main protocol does.
			cmdh->setExecContext( execContext);

			Filter flt = getFilter( provider, ifl, ofl);
			bool doEscapeLFdot = false;

			cmdbind::CommandHandlerR cmdh_scoped( cmdh);
			cmdbind::IOFilterCommandHandlerEscDLF* ifch = dynamic_cast<cmdbind::IOFilterCommandHandlerEscDLF*>( cmdh);

			if (ifch) 
			{
				LOG_DEBUG << "command handler is processing CRLFdot escaped content";
				if (flt.inputfilter().get()) ifch->setFilter( flt.inputfilter());
				if (flt.outputfilter().get())
				{
					ifch->setFilter( flt.outputfilter());
					flt.outputfilter()->setOutputBuffer( buf.outbuf, buf.outsize);
				}
				doEscapeLFdot = true;
			}
			cmdbind::DoctypeFilterCommandHandler* dtfh = new cmdbind::DoctypeFilterCommandHandler();
			cmdbind::CommandHandlerR dtfh_scoped( dtfh);
			dtfh->setOutputBuffer( buf.outbuf, buf.outsize, 0);

			// Detect document format:
			int stateEscIn = 0;
			int stateEscOut = 0;
			processCommandHandler( buf, stateEscIn, stateEscOut, dtfh, is, os, doEscapeLFdot);

			std::string docformat = dtfh->docformatid();
			const char* cmd_argv = docformat.c_str();
			cmdh->passParameters( proc, 1, &cmd_argv);
			cmdh->setOutputBuffer( buf.outbuf, buf.outsize, 0);

			// Get data (consumed and rest = not processed) from 
			//	the doc type detection command handler 
			//	to redirect to the executing command handler:
			std::string databuf;
			void* comsumed_buffer;
			std::size_t comsumed_size;
			dtfh->getInputBuffer( comsumed_buffer, comsumed_size);
			const void* rest_buffer;
			std::size_t rest_size;
			dtfh->getDataLeft( rest_buffer, rest_size);
			databuf.append( (const char*)comsumed_buffer, comsumed_size);
			databuf.append( (const char*)rest_buffer, rest_size);

			// Redirect input processed and call command handler 
			//	to process if there is still input left:
			if (redirectInput( buf, stateEscOut, databuf.c_str(), databuf.size(), cmdh, os))
			{
				processCommandHandler( buf, stateEscIn, stateEscOut, cmdh, is, os, doEscapeLFdot);
			}
			// Check if there is unconsumed input left (must not happen):
			bool end = false;
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
			return;
		}
	}
	{
		const FormFunction* func = provider->formFunction( proc);
		if (func)
		{
			// ... command is the name of a form function we call directly
			//	with the filter specified:
			Filter flt = getFilter( provider, ifl, ofl);
			if (!flt.inputfilter().get()) throw std::runtime_error( "input filter not found");
			if (!flt.outputfilter().get()) throw std::runtime_error( "output filter not found");
			flt.outputfilter()->setOutputBuffer( buf.outbuf, buf.outsize);

			flt.inputfilter()->setValue( "empty", "false");
			TypedInputFilterR inp( new TypingInputFilter( flt.inputfilter()));
			TypedOutputFilterR outp( new TypingOutputFilter( flt.outputfilter()));
			FormFunctionClosureR closure( func->createClosure());
			closure->init( execContext, inp, serialize::Context::ValidateAttributes);

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
			// ... command is the name a form description -> we simply map 
			//	the input with the input/output filters specified 
			//	through the form:
			Filter flt = getFilter( provider, ifl, ofl);
			if (!flt.inputfilter().get()) throw std::runtime_error( "input filter not found");
			if (!flt.outputfilter().get()) throw std::runtime_error( "output filter not found");
			flt.outputfilter()->setOutputBuffer( buf.outbuf, buf.outsize);

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
	throw std::runtime_error( std::string("identifier '") + proc + "' not defined as command, form or function");
}


