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
#include "langbind/luaCommandHandler.hpp"
#endif
#include <boost/algorithm/string.hpp>
#include <string>
#include <fstream>
#include <iostream>

using namespace _Wolframe;
using namespace langbind;

int _Wolframe::langbind::iostreamfilter( const std::string& ifl, std::size_t ib, const std::string& ofl, std::size_t ob, const std::string& proc, std::istream& is, std::ostream& os)
{
	langbind::GlobalContext* gc = langbind::getGlobalContext();
	PluginFunction pf;
	DDLForm df;
	TransactionFunction tf;
	CountedReference<protocol::CommandHandler> cmdhandler;

#if WITH_LUA
	LuaScriptInstanceR sc;
	if (gc->getLuaScriptInstance( proc.c_str(), sc))
	{
		cmdhandler.reset( new langbind::LuaCommandHandler());
	}
	else
#endif
	if (gc->getPluginFunction( proc.c_str(), pf))
	{
	}
	else if (gc->getForm( proc.c_str(), df))
	{
	}
	else if (gc->getTransactionFunction( proc.c_str(), tf))
	{
	}
	else
	{
		LOG_ERROR << "mapping command not found: '" << proc.c_str() << "'";
	}
	return _Wolframe::langbind::iostreamfilter( ifl, ib, ofl, ob, is, os);
}

int _Wolframe::langbind::iostreamfilter( const std::string& ifl, std::size_t ib, const std::string& ofl, std::size_t ob, std::istream& is, std::ostream& os)
{
	langbind::GlobalContext* gc = langbind::getGlobalContext();
	langbind::FilterFactoryR tf( new langbind::TokenFilterFactory());
	gc->defineFilter( "token", tf);

	langbind::Filter flt;
	if (boost::iequals( ofl, ifl))
	{
		if (!gc->getFilter( ifl.c_str(), flt))
		{
			LOG_ERROR << "unknown filter '" << ifl << "'";
			return 1;
		}
	}
	else
	{
		langbind::Filter in;
		langbind::Filter out;
		if (!gc->getFilter( ifl.c_str(), in))
		{
			LOG_ERROR << "unknown input filter '" << ofl << "'";
			return 1;
		}
		if (!gc->getFilter( ofl.c_str(), out))
		{
			LOG_ERROR << "unknown output filter '" << ofl << "'";
			return 2;
		}
		flt = langbind::Filter( in.inputfilter(), out.outputfilter());
	}
	char* inputBuffer = new char[ ib];
	char* outputBuffer = new char[ ob];
	const void* element;
	std::size_t elementsize;
	protocol::InputFilter::ElementType elementType;
	std::size_t taglevel=0;

	READ_INPUT:
	{
		if (flt.inputfilter().get()->gotEoD())
		{
			goto TERMINATE;
		}
		std::size_t pp = 0;
		while (pp < ib && !is.eof())
		{
			is.read( inputBuffer+pp, sizeof(char));
			++pp;
		}
		flt.inputfilter().get()->protocolInput( inputBuffer, pp, pp < ib);
		goto PROCESS_READ;
	}
	WRITE_OUTPUT:
	{
		os.write( outputBuffer, flt.outputfilter().get()->pos());
		flt.outputfilter().get()->init( outputBuffer, ib);
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
		os.write( outputBuffer, flt.outputfilter().get()->pos());
		delete [] inputBuffer;
		delete [] outputBuffer;
	}
	return 0;
}


