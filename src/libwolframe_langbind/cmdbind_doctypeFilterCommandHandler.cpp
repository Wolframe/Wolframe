/************************************************************************

 Copyright (C) 2011 - 2013 Project Wolframe.
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
///\file cmdbind_doctypeFilterCommandHandler.cpp

#include "cmdbind/doctypeFilterCommandHandler.hpp"
#include "logger-v1.hpp"
#include "types/doctype.hpp"
#include <cstring>

using namespace _Wolframe;
using namespace _Wolframe::cmdbind;

DoctypeFilterCommandHandler::DoctypeFilterCommandHandler()
	:m_state(Init)
	,m_lastchar('\n')
	,m_nullcnt(0)
{}

DoctypeFilterCommandHandler::~DoctypeFilterCommandHandler()
{}

void DoctypeFilterCommandHandler::setInputBuffer( void* buf, std::size_t allocsize)
{
	m_input = protocol::InputBlock( (char*)buf, allocsize);
}

void DoctypeFilterCommandHandler::setOutputBuffer( void*, std::size_t, std::size_t)
{}

std::string DoctypeFilterCommandHandler::doctypeid() const
{
	if (m_state != Done) throw std::logic_error( "illegal call of get doctypeid in this state");
	return m_doctypeid;
}

CommandHandler::Operation DoctypeFilterCommandHandler::nextOperation()
{
	if (m_state == Done) return CLOSE;
	return READ;
}

void DoctypeFilterCommandHandler::throw_error( const char* msg) const
{
	std::string ex( "failed to parse doctype from XML (state ");
	ex = ex + stateName( m_state) + ")";
	if (msg) ex = ex + ": " + msg;
	throw std::runtime_error( ex);
}

void DoctypeFilterCommandHandler::setState( State state_)
{
	m_state = state_;
	LOG_TRACE << "STATE DoctypeCommandHandler " << stateName( m_state);
}

void DoctypeFilterCommandHandler::putInput( const void *begin, std::size_t bytesTransferred)
{
	const char* inp = (const char*) begin;
	std::size_t ii = 0;
	m_inputbuffer.append( inp, bytesTransferred);

	LOG_TRACE << "STATE DoctypeCommandHandler " << stateName( m_state) << " (put input)";
	try
	{
		for (; ii<bytesTransferred && m_state != Done; ++ii)
		{
			if (!inp[ii])
			{
				++m_nullcnt;
				if (m_nullcnt > 4) throw_error( "Unknown encoding");
			}
			else
			{
				if (m_lastchar == '\n' && inp[ii] == '.')
				{
					setState( Done);
					break;
				}
				m_lastchar = inp[ii];
				m_nullcnt = 0;

				switch (m_state)
				{
					case Init:
						if (inp[ii] == '<')
						{
							setState( ParseHeader0);
						}
						else if (inp[ii] < 0 || inp[ii] > 32)
						{
							throw_error( "expected '<?'");
						}
						break;

					case ParseHeader0:
						if (inp[ii] == '?')
						{
							setState( ParseHeader);
						}
						else
						{
							throw_error( "expected '<?'");
						}
						break;

					case ParseHeader:
						if (inp[ii] == '>')
						{
							const char* cc = std::strstr( m_itembuf.c_str(), "standalone");
							if (cc)
							{
								cc = std::strchr( cc, '=');
								if (cc) cc = std::strstr( cc, "yes");
								if (cc)
								{
									setState( Done);
									break;
								}
							}
							setState( SearchDoctypeTag);
							m_itembuf.clear();
						}
						else
						{
							m_itembuf.push_back( inp[ii]);
							if (m_itembuf.size() > 128)
							{
								throw_error( "XML header not terminated");
							}
						}
						break;

					case SearchDoctypeTag:
						if (inp[ii] == '<')
						{
							setState( ParseDoctype0);
						}
						else if (inp[ii] < 0 || inp[ii] > 32)
						{
							throw_error( "expected '<!'");
						}
						break;

					case ParseDoctype0:
						if (inp[ii] == '!')
						{
							setState( ParseDoctype1);
						}
						else
						{
							throw_error( "expected '<!'");
						}
						break;

					case ParseDoctype1:
						if (inp[ii] == '-')
						{
							setState( SkipComment);
						}
						else if (inp[ii] == 'D')
						{
							m_itembuf.push_back( inp[ii]);
							setState( ParseDoctype2);
						}
						else
						{
							throw_error( "expected '<!DOCTYPE' or <!--");
						}
						break;

					case SkipComment:
						if (inp[ii] == '>')
						{
							setState( SearchDoctypeTag);
						}
						break;

					case ParseDoctype2:
						if (inp[ii] <= ' ' && inp[ii] > 0)
						{
							if (m_itembuf != "DOCTYPE")
							{
								throw_error( "expected '<!DOCTYPE'");
							}
							setState( ParseDoctype);
							m_itembuf.clear();
						}
						else
						{
							m_itembuf.push_back( inp[ii]);
							if (m_itembuf.size() > 8)
							{
								throw_error( "expected '<!DOCTYPE'");
							}
						}
						break;

					case ParseDoctype:
						if (inp[ii] <= ' ' && inp[ii] > 0)
						{
							m_doctype.push_back( ' ');
						}
						else if (inp[ii] == '>')
						{
							m_doctypeid = types::getIdFromDoctype( m_doctype);
							setState( Done);
						}
						else
						{
							m_doctype.push_back( inp[ii]);
						}
						break;

					case Done:
						break;
				}
			}
		}
	}
	catch (const std::runtime_error& err)
	{
		m_lastError = "failed to extract document type from xml";
		LOG_ERROR << "error in document type recognition: " << err.what();
		m_state = Done;
	}
}

void DoctypeFilterCommandHandler::getInputBlock( void*& begin, std::size_t& maxBlockSize)
{
	begin = m_input.ptr();
	maxBlockSize = m_input.size();
}

void DoctypeFilterCommandHandler::getOutput( const void*& begin, std::size_t& bytesToTransfer)
{
	begin = 0;
	bytesToTransfer = 0;
}

void DoctypeFilterCommandHandler::getDataLeft( const void*& begin, std::size_t& nofBytes)
{
	begin = m_input.ptr();
	nofBytes = 0;
}

void DoctypeFilterCommandHandler::getInputBuffer( void*& begin, std::size_t& nofBytes)
{
	begin = m_inputbuffer.base();
	nofBytes = m_inputbuffer.size();
}

