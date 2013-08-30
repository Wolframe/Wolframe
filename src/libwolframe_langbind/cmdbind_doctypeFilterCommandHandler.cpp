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
	,m_inputidx(0)
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
	const char* inp = m_input.charptr();
	std::size_t startidx = (const char*)begin - m_input.charptr();
	std::size_t endidx = bytesTransferred + startidx;
	if (endidx > m_input.size())
	{
		throw std::logic_error( "illegal input range passed to DoctypeFilterCommandHandler");
	}
	m_input.setPos( endidx);
	m_inputidx = startidx;

	LOG_TRACE << "STATE DoctypeCommandHandler " << stateName( m_state) << " (put input)";
	try
	{
		for (; m_inputidx<endidx && m_state != Done; ++m_inputidx)
		{
			if (!inp[m_inputidx])
			{
				++m_nullcnt;
				if (m_nullcnt > 4) throw_error( "Unknown encoding");
			}
			else
			{
				if (m_lastchar == '\n' && inp[m_inputidx] == '.')
				{
					setState( Done);
					break;
				}
				m_lastchar = inp[m_inputidx];
				m_nullcnt = 0;

				switch (m_state)
				{
					case Init:
						if (inp[m_inputidx] == '<')
						{
							setState( ParseHeader0);
						}
						else if (inp[m_inputidx] < 0 || inp[m_inputidx] > 32)
						{
							throw_error( "expected '<?'");
						}
						break;

					case ParseHeader0:
						if (inp[m_inputidx] == '?')
						{
							setState( ParseHeader);
						}
						else
						{
							throw_error( "expected '<?'");
						}
						break;

					case ParseHeader:
						if (inp[m_inputidx] == '>')
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
							m_itembuf.push_back( inp[m_inputidx]);
							if (m_itembuf.size() > 128)
							{
								throw_error( "XML header not terminated");
							}
						}
						break;

					case SearchDoctypeTag:
						if (inp[m_inputidx] == '<')
						{
							setState( ParseDoctype0);
						}
						else if (inp[m_inputidx] < 0 || inp[m_inputidx] > 32)
						{
							throw_error( "expected '<!'");
						}
						break;

					case ParseDoctype0:
						if (inp[m_inputidx] == '!')
						{
							setState( ParseDoctype1);
						}
						else
						{
							throw_error( "expected '<!'");
						}
						break;

					case ParseDoctype1:
						if (inp[m_inputidx] == '-')
						{
							setState( SkipComment);
						}
						else if (inp[m_inputidx] == 'D')
						{
							m_itembuf.push_back( inp[m_inputidx]);
							setState( ParseDoctype2);
						}
						else
						{
							throw_error( "expected '<!DOCTYPE' or <!--");
						}
						break;

					case SkipComment:
						if (inp[m_inputidx] == '>')
						{
							setState( SearchDoctypeTag);
						}
						break;

					case ParseDoctype2:
						if (inp[m_inputidx] <= ' ' && inp[m_inputidx] > 0)
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
							m_itembuf.push_back( inp[m_inputidx]);
							if (m_itembuf.size() > 8)
							{
								throw_error( "expected '<!DOCTYPE'");
							}
						}
						break;

					case ParseDoctype:
						if (inp[m_inputidx] <= ' ' && inp[m_inputidx] > 0)
						{
							m_doctype.push_back( ' ');
						}
						else if (inp[m_inputidx] == '>')
						{
							m_doctypeid = types::getIdFromDoctype( m_doctype);
							setState( Done);
						}
						else
						{
							m_doctype.push_back( inp[m_inputidx]);
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
	m_inputbuffer.append( (const char*)begin, (m_inputidx - startidx));
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
	begin = (const void*)(m_input.charptr() + m_inputidx);
	nofBytes = m_input.pos() - m_inputidx;
}

void DoctypeFilterCommandHandler::getInputBuffer( void*& begin, std::size_t& nofBytes)
{
	begin = m_inputbuffer.base();
	nofBytes = m_inputbuffer.size();
}

