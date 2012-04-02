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
///\file appObjects.cpp
///\brief implementation of scripting language objects
#include <algorithm>
#include <cctype>
#include "langbind/appObjects.hpp"
#include "logger-v1.hpp"
#include "protocol/inputfilter.hpp"
#include "protocol/outputfilter.hpp"
#include <boost/algorithm/string.hpp>
#include "filter/char_filter.hpp"
#include "filter/line_filter.hpp"
#include "filter/token_filter.hpp"
#ifdef WITH_LIBXML2
#include "filter/libxml2_filter.hpp"
#endif
#include "filter/textwolf_filter.hpp"
#ifdef WITH_XMLLITE
#include "filter/xmllite_filter.hpp"
#endif
#if WITH_MSXML
#include "filter/msxml_filter.hpp"
#endif

using namespace _Wolframe;
using namespace langbind;

void FilterMap::defineFilter( const char* name, const FilterFactoryR& f)
{
	std::string nam( name);
	std::transform( nam.begin(), nam.end(), nam.begin(), (int(*)(int)) std::tolower);
	m_map[ nam] = f;
}

bool FilterMap::getFilter( const char* arg, Filter& rt)
{
	std::size_t nn = std::strlen(arg);
	std::size_t ii = nn;
	std::string nam( arg);
	std::transform( nam.begin(), nam.end(), nam.begin(), (int(*)(int)) std::tolower);
	do
	{
		nam.resize(ii);
		std::map<std::string,FilterFactoryR>::const_iterator itr=m_map.find( nam),end=m_map.end();
		if (itr != end)
		{
			rt = itr->second->create( (ii==nn)?0:(arg+ii+1));
			return true;
		}
		for (ii=nn; ii>0 && arg[ii] != ':'; --ii);
	}
	while (ii>0);
	return false;
}

FilterMap::FilterMap()
{
	defineFilter( "char", FilterFactoryR( new CharFilterFactory()));
	defineFilter( "line", FilterFactoryR( new LineFilterFactory()));
	defineFilter( "xml:textwolf", FilterFactoryR( new TextwolfXmlFilterFactory()));
#ifdef WITH_LIBXML2
	defineFilter( "xml:libxml2", FilterFactoryR( new Libxml2FilterFactory()));
#endif
#ifdef WITH_XMLLITE
	defineFilter( "xml:xmllite", FilterFactoryR( new Libxml2FilterFactory()));
#endif
#ifdef WITH_MSXML
	defineFilter( "xml:msxml", FilterFactoryR( new Libxml2FilterFactory()));
#endif
}

void DDLFormMap::defineForm( const char* name, const DDLForm& f)
{
	std::string nam( name);
	std::transform( nam.begin(), nam.end(), nam.begin(), (int(*)(int)) std::tolower);
	m_map[ nam] = f;
}

bool DDLFormMap::getForm( const char* name, DDLForm& rt) const
{
	std::string nam( name);
	std::transform( nam.begin(), nam.end(), nam.begin(), (int(*)(int)) std::tolower);
	std::map<std::string,DDLForm>::const_iterator ii=m_map.find( nam),ee=m_map.end();
	if (ii == ee)
	{
		return false;
	}
	else
	{
		rt = ii->second;
		return true;
	}
}

bool TransactionFunctionMap::getTransactionFunction( const char* name, TransactionFunction& rt) const
{
	std::string nam( name);
	std::transform( nam.begin(), nam.end(), nam.begin(), (int(*)(int)) std::tolower);
	std::map<std::string,TransactionFunction>::const_iterator ii=m_map.find( nam),ee=m_map.end();
	if (ii == ee)
	{
		return false;
	}
	else
	{
		rt = ii->second;
		return true;
	}
}

void TransactionFunctionMap::defineTransactionFunction( const char* name, const TransactionFunction& f)
{
	std::string nam( name);
	std::transform( nam.begin(), nam.end(), nam.begin(), (int(*)(int)) std::tolower);
	m_map[ nam] = f;
}

void DDLCompilerMap::defineDDLCompiler( const char* name, const ddl::CompilerInterfaceR& ci)
{
	std::string nam( name);
	std::transform( nam.begin(), nam.end(), nam.begin(), (int(*)(int)) std::tolower);
	m_map[ nam] = ci;
}

bool DDLCompilerMap::getDDLCompiler( const char* name, ddl::CompilerInterfaceR& rt) const
{
	std::string nam( name);
	std::map<std::string,ddl::CompilerInterfaceR>::const_iterator ii=m_map.find( nam),ee=m_map.end();
	if (ii == ee)
	{
		return false;
	}
	else
	{
		rt = ii->second;
		return true;
	}
}

static GlobalContextR g_context;

void _Wolframe::langbind::defineGlobalContext( const GlobalContextR& context)
{
	g_context = context;
}

GlobalContext* _Wolframe::langbind::getGlobalContext()
{
	return g_context.get();
}

struct AutoCreateGlobalContext
{
	AutoCreateGlobalContext()
	{
		g_context.reset( new GlobalContext());
	}
};
AutoCreateGlobalContext g_autoCreateGlobalContext;


static InputFilterClosure::ItemType fetchFailureResult( const protocol::InputFilter& ff)
{
	const char* msg;
	switch (ff.state())
	{
		case protocol::InputFilter::EndOfMessage:
			return InputFilterClosure::DoYield;

		case protocol::InputFilter::Error:
			msg = ff.getError();
			LOG_ERROR << "error in input filter (" << (msg?msg:"unknown") << ")";
			return InputFilterClosure::Error;

		case protocol::InputFilter::Open:
			LOG_DATA << "end of input";
			return InputFilterClosure::EndOfData;
	}
	LOG_ERROR << "illegal state in iterator";
	return InputFilterClosure::Error;
}

InputFilterClosure::ItemType InputFilterClosure::fetch( const char*& tag, unsigned int& tagsize, const char*& val, unsigned int& valsize)
{
	const void* element;
	std::size_t elementsize;

AGAIN:
	if (!m_inputfilter.get())
	{
		return EndOfData;
	}
	if (!m_inputfilter->getNext( m_type, element, elementsize))
	{
		if (m_inputfilter->state() == protocol::InputFilter::Open)
		{
			// at end of data check if there is a follow filter (transformed filter) to continue with:
			protocol::InputFilter* follow = m_inputfilter->createFollow();
			if (follow)
			{
				m_inputfilter.reset( follow);
				goto AGAIN;
			}
		}
		return fetchFailureResult( *m_inputfilter);
	}
	else
	{
		switch (m_type)
		{
			case protocol::InputFilter::OpenTag:
				m_taglevel += 1;
				tag = (const char*)element;
				tagsize = elementsize;
				val = 0;
				valsize = 0;
				m_gotattr = false;
				return Data;

			case protocol::InputFilter::Value:
				if (m_gotattr)
				{
					tag = m_attrbuf.c_str();
					tagsize = m_attrbuf.size();
					m_gotattr = false;
				}
				else
				{
					tag = 0;
					tagsize = 0;
				}
				val = (const char*)element;
				valsize = elementsize;
				return Data;

			 case protocol::InputFilter::Attribute:
				m_attrbuf.clear();
				m_attrbuf.append( (const char*)element, elementsize);
				m_gotattr = true;
				goto AGAIN;

			 case protocol::InputFilter::CloseTag:
				tag = 0;
				tagsize = 0;
				val = 0;
				valsize = 0;
				m_gotattr = false;
				if (m_taglevel == 0)
				{
					return EndOfData;
				}
				else
				{
					m_taglevel -= 1;
					return Data;
				}
		}
	}
	LOG_ERROR << "illegal state in iterator";
	return Error;
}

Output::ItemType Output::print( const char* tag, unsigned int tagsize, const char* val, unsigned int valsize)
{
	if (!m_outputfilter.get())
	{
		LOG_ERROR << "no output sink defined (output ignored)";
		return Error;
	}
	try
	{
		if (tag)
		{
			if (val)
			{
				switch (m_state)
				{
					case 0:
						if (!m_outputfilter->print( protocol::OutputFilter::Attribute, tag, tagsize)) break;
						m_state ++;
					case 1:
						if (!m_outputfilter->print( protocol::OutputFilter::Value, val, valsize)) break;
						m_state ++;
					case 2:
						m_state = 0;
						return Data;
				}
				const char* err = m_outputfilter->getError();
				if (err)
				{
					LOG_ERROR << "error in output filter (" << err << ")";
					return Error;
				}
				else if (m_outputfilter->state() != protocol::OutputFilter::EndOfBuffer)
				{
					// in case of return false and no error check if there is a follow format ouptut filter to continue with:
					protocol::OutputFilter* follow = m_outputfilter->createFollow();
					if (follow)
					{
						m_outputfilter.reset( follow);
						return print( tag, tagsize, val, valsize);
					}

				}
				return DoYield;
			}
			else
			{
				if (!m_outputfilter->print( protocol::OutputFilter::OpenTag, tag, tagsize))
				{
					const char* err = m_outputfilter->getError();
					if (err)
					{
						LOG_ERROR << "error in output filter open tag (" << err << ")";
						return Error;
					}
					else  if (m_outputfilter->state() != protocol::OutputFilter::EndOfBuffer)
					{
						// in case of return false and no error check if there is a follow format ouptut filter to continue with:
						protocol::OutputFilter* follow = m_outputfilter->createFollow();
						if (follow)
						{
							m_outputfilter.reset( follow);
							return print( tag, tagsize, val, valsize);
						}
					}
					return DoYield;
				}
				return Data;
			}
		}
		else if (val)
		{
			if (!m_outputfilter->print( protocol::OutputFilter::Value, val, valsize))
			{
				const char* err = m_outputfilter->getError();
				if (err)
				{
					LOG_ERROR << "error in output filter value (" << err << ")";
					return Error;
				}
				else if (m_outputfilter->state() != protocol::OutputFilter::EndOfBuffer)
				{
					// in case of return false and no error check if there is a follow format ouptut filter to continue with:
					protocol::OutputFilter* follow = m_outputfilter->createFollow();
					if (follow)
					{
						m_outputfilter.reset( follow);
						return print( tag, tagsize, val, valsize);
					}
				}
				return DoYield;
			}
			return Data;
		}
		else
		{
			if (!m_outputfilter->print( protocol::OutputFilter::CloseTag, 0, 0))
			{
				const char* err = m_outputfilter->getError();
				if (err)
				{
					LOG_ERROR << "error in output filter close tag (" << err << ")";
					return Error;
				}
				else if (m_outputfilter->state() != protocol::OutputFilter::EndOfBuffer)
				{
					// in case of return false and no error check if there is a follow format ouptut filter to continue with:
					protocol::OutputFilter* follow = m_outputfilter->createFollow();
					if (follow)
					{
						m_outputfilter.reset( follow);
						return print( tag, tagsize, val, valsize);
					}
				}
				return DoYield;
			}
			return Data;
		}
	}
	catch (std::bad_alloc)
	{
		LOG_ERROR << "out of memory in output filter";
		return Error;
	}
}





