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
///\file ddl_compiler_SimpleFormCompiler.cpp
///\brief implementation of a compiler for a self defined form DDL called 'simple form DDL'

#include "ddl/compiler/simpleFormCompiler.hpp"
#include <string>
#include <vector>
#include <map>
#include <cstring>
#include <cstddef>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/info_parser.hpp>

using namespace _Wolframe;
using namespace ddl;

namespace
{

class FRMAttribute
{
public:
	enum Type
	{
		float_=AtomicType::float_,
		int_=AtomicType::int_,
		uint_=AtomicType::uint_,
		string_=AtomicType::string_,
		form_
	};

	static const char* typeName( Type tp)
	{
		static const char* ar[] = {"float","int","uint","string","form",0};
		return ar[ (int)tp];
	}

	static bool getType( const char* name, Type& tp)
	{
		const char* rt;
		unsigned int ii;
		for (ii=0,rt=typeName((Type)(ii)); rt!=0; ii++,rt=typeName((Type)(ii)))
		{
			if (boost::algorithm::iequals( rt, name))
			{
				tp = (Type)ii;
				return true;
			}
		}
		return false;
	}

public:
	FRMAttribute( const FRMAttribute& o)
		:m_isVector(o.m_isVector)
		,m_isAttribute(o.m_isAttribute)
		,m_type(o.m_type)
		,m_value(o.m_value){}

	FRMAttribute( const std::string& name, const std::string& item)
		:m_isVector(false),m_isAttribute(false),m_type(string_)
	{
		enum State
		{
			ParseStart,
			ParseName,
			ParseEndName,
			ParseArOpen,
			ParseDfOpen,
			ParseDfValue,
			ParseEnd
		};
		State st = ParseName;
		std::string::const_iterator ii=item.begin(), ee=item.end(), valbegin, valend;
		for (; ii != ee; ++ii)
		{
			switch (st)
			{
				case ParseStart:
					if (*ii == '@')
					{
						m_isAttribute = true;
						st = ParseName;
						break;
					}
					st = ParseName;
					/* no break here !*/
				case ParseName:
					if (((*ii|32) >= 'a' && (*ii|32) <= 'z') || (*ii >= '0' && *ii <= '9') || *ii == '_')
					{
						break;
					}
					parseType( item.substr( 0, ii-item.begin()));
					st = ParseEndName;
					/* no break here !*/
				case ParseEndName:
					if (*ii < ' ' && *ii >= 0)
					{
						break;
					}
					if (*ii == '[')
					{
						m_isVector = true;
						st = ParseArOpen;
						break;
					}
					if (*ii == '(')
					{
						if (m_type == form_)
						{
							throw std::runtime_error( (std::string("Form ") += name) += "declared with default value");
						}
						st = ParseDfOpen;
						break;
					}
					throw std::runtime_error( "Syntax error in Simple Form Attribute: '[' or '(' expected");

				case ParseArOpen:
					if (*ii < ' ' && *ii >= 0)
					{
						break;
					}
					if (*ii == ']')
					{
						st = ParseEnd;
						break;
					}
					throw std::runtime_error( "Syntax error in Simple Form Attribute: ']' expected");

				case ParseDfOpen:
					if (*ii < ' ' && *ii >= 0)
					{
						break;
					}
					if (*ii == ')')
					{
						st = ParseEnd;
						break;
					}
					st = ParseDfValue;
					valbegin = ii;
					valend = ii;
					/* no break here !*/

				case ParseDfValue:
					if (*ii < ' ' && *ii >= 0)
					{
						break;
					}
					if (*ii == ')')
					{
						m_value = item.substr( valbegin-item.begin(), valend-valbegin);
						st = ParseEnd;
						break;
					}
					else
					{
						valend = ii+1;
					}
				break;
				case ParseEnd:
					if (*ii < ' ' && *ii >= 0)
					{
						break;
					}
					throw std::runtime_error( "Syntax error in Simple Form Attribute: Illegal character after end of attribute");
			}
		}
		if (st == ParseName)
		{
			parseType( item.substr( 0, ii-item.begin()));
		}
		else if (st != ParseEnd)
		{
			throw std::runtime_error( "Syntax error in Simple Form Attribute: Incomplete attribute");
		}
	}

	bool isVector() const			{return m_isVector;}
	bool isAttribute() const		{return m_isAttribute;}
	Type type() const			{return m_type;}
	const std::string& value() const	{return m_value;}

private:
	void parseType( const std::string& typestr)
	{
		if (typestr.size() == 0)
		{
			m_type = form_;
		}
		else if (!getType( typestr.c_str(), m_type))
		{
			throw std::runtime_error( (std::string( "Unknown type: '") += typestr) += "'");
		}
		if (m_type == form_)
		{
			if (m_isAttribute)
			{
				throw std::runtime_error( "Syntax error: Form declared as atrribute");
			}
		}
	}
private:
	bool m_isVector;
	bool m_isAttribute;
	Type m_type;
	std::string m_value;
};

}///anonymous namespace


static void compile_ptree( const boost::property_tree::ptree& pt, StructType& result)
{
	boost::property_tree::ptree::const_iterator itr=pt.begin(),end=pt.end();
	for (;itr != end; ++itr)
	{
		if (itr->second.begin() == itr->second.end() && itr->second.data().size())
		{
			FRMAttribute fa( itr->first, itr->second.data());
			if (fa.type() == FRMAttribute::form_)
			{
				throw std::runtime_error( "Semantic error: Form declared as empty and with default value");
			}
			AtomicType at( (AtomicType::Type)fa.type());
			at.set( fa.value());
			StructType val;
			if (fa.isVector())
			{
				val.defineAsVector( at);
			}
			else
			{
				val = at;
			}
			if (fa.isAttribute())
			{
				result.defineAttribute( itr->first, val);
			}
			else
			{
				result.defineContent( itr->first, val);
			}
		}
		else
		{
			if (itr->second.data().size())
			{
				FRMAttribute fa( itr->first, itr->second.data());
				if (fa.type() != FRMAttribute::form_)
				{
					throw std::runtime_error( "Semantic error: Atomic type declared as structure");
				}
				StructType st;
				if (fa.isVector())
				{
					StructType prototype;
					compile_ptree( itr->second, prototype);
					st.defineAsVector( prototype);
				}
				else
				{
					compile_ptree( itr->second, st);
				}
				result.defineContent( itr->first, st);
			}
			else
			{
				StructType st;
				compile_ptree( itr->second, st);
				result.defineContent( itr->first, st);
			}
		}
	}
}


bool SimpleFormCompiler::compile( const std::string& srcstring, StructType& result_, std::string& error_) const
{
	try
	{
		std::istringstream src( srcstring);
		boost::property_tree::ptree pt;
		boost::property_tree::info_parser::read_info( src, pt);
		compile_ptree( pt, result_);
		return true;
	}
	catch (const std::runtime_error& e)
	{
		error_ = e.what();
	}
	return false;
}

