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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
///\file simpleFormCompiler.cpp
///\brief Implementation of a compiler for a self defined form DDL called 'simple form DDL'

#include "simpleFormCompiler.hpp"
#include "types/keymap.hpp"
#include "utils/doctype.hpp"
#include <string>
#include <vector>
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
	FRMAttribute( const FRMAttribute& o)
		:m_isVector(o.m_isVector)
		,m_isAttribute(o.m_isAttribute)
		,m_isForm(o.m_isForm)
		,m_isSubForm(o.m_isSubForm)
		,m_type(o.m_type)
		,m_subform(o.m_subform)
		,m_value(o.m_value){}

	explicit FRMAttribute( const std::string& item, const TypeMap* typemap, const types::keymap<StructType>& formmap)
		:m_isVector(false)
		,m_isAttribute(false)
		,m_isForm(false)
		,m_isSubForm(false)
		,m_type(0)
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
		State st = ParseStart;
		std::string vv;

		std::string::const_iterator ii=item.begin(), ee=item.end();
		for (; ii != ee; ++ii)
		{
			switch (st)
			{
				case ParseStart:
					if (*ii == '@')
					{
						++ii;
						m_isAttribute = true;
					}
					st = ParseName;
					/* no break here !*/
				case ParseName:
					if (((*ii|32) >= 'a' && (*ii|32) <= 'z') || (*ii >= '0' && *ii <= '9') || *ii == '_')
					{
						vv.push_back( *ii);
						continue;
					}
					parseType( vv, typemap, formmap);
					st = ParseEndName;
					/* no break here !*/
				case ParseEndName:
					if (*ii <= ' ' && *ii >= 0)
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
						st = ParseDfOpen;
						break;
					}
					throw std::runtime_error( "Syntax error in Simple Form Attribute: '[' or '(' expected");

				case ParseArOpen:
					if (*ii <= ' ' && *ii > 0)
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
					if (*ii <= ' ' && *ii > 0)
					{
						break;
					}
					if (*ii == ')')
					{
						st = ParseEnd;
						break;
					}
					st = ParseDfValue;
					vv.clear();
					/* no break here !*/

				case ParseDfValue:
					if (*ii == ')')
					{
						m_value = vv;
						st = ParseEnd;
						break;
					}
					else
					{
						vv.push_back( *ii);
					}
				break;
				case ParseEnd:
					if (*ii <= ' ' && *ii > 0)
					{
						break;
					}
					throw std::runtime_error( "Syntax error in simple form attribute: Illegal character after end of attribute");
			}
		}
		if (st == ParseName)
		{
			parseType( vv, typemap, formmap);
		}
		else if (st != ParseEnd)
		{
			throw std::runtime_error( "Syntax error in Simple Form Attribute: Incomplete attribute");
		}
	}

	bool isSubForm() const			{return m_isSubForm;}
	bool isVector() const			{return m_isVector;}
	bool isAttribute() const		{return m_isAttribute;}
	bool isForm() const			{return m_isForm;}
	const NormalizeFunction* type() const	{return m_type;}
	const std::string& value() const	{return m_value;}
	const StructType& subform() const	{return m_subform;}

private:
	void parseType( const std::string& typestr, const TypeMap* typemap, const types::keymap<StructType>& formmap)
	{
		types::keymap<StructType>::const_iterator fmi;
		if (typestr.size() == 0)
		{
			m_isForm = true;
		}
		else if (boost::algorithm::iequals( typestr, "string"))
		{
			m_type = 0;
		}
		else if ((fmi=formmap.find( typestr)) != formmap.end())
		{
			m_isSubForm = true;
			m_subform = fmi->second;
		}
		else
		{
			m_type = typemap->getType( typestr);
			if (!m_type)
			{
				throw std::runtime_error( (std::string( "unknown type: '") += typestr) += "'");
			}
		}
	}
private:
	bool m_isVector;
	bool m_isAttribute;
	bool m_isForm;
	bool m_isSubForm;
	const NormalizeFunction* m_type;
	StructType m_subform;
	std::string m_value;
};

}///anonymous namespace

static bool isIdentifier( const std::string& name)
{
	std::string::const_iterator ii=name.begin(), ee=name.end();
	for (;ii!=ee; ++ii)
	{
		if (!(((*ii|32) >= 'a' && (*ii|32) <= 'z') || (*ii >= '0' && *ii <= '9') || *ii == '_'))
		{
			break;
		}
	}
	return (ii==ee);
}

static void compile_ptree( const boost::property_tree::ptree& pt, StructType& result, const TypeMap* typemap, const types::keymap<StructType>& formmap)
{
	boost::property_tree::ptree::const_iterator itr=pt.begin(),end=pt.end();
	for (;itr != end; ++itr)
	{
		if (!isIdentifier( itr->first))
		{
			throw std::runtime_error( "Semantic error: Identifier expected as variable name");
		}
		if (itr->second.begin() == itr->second.end() && itr->second.data().size())
		{
			FRMAttribute fa( itr->second.data(), typemap, formmap);
			if (fa.isForm())
			{
				throw std::runtime_error( "Semantic error: illegal type specifier");
			}
			if (fa.isSubForm())
			{
				StructType val;
				if (fa.isAttribute())
				{
					throw std::runtime_error( "Syntax error: Form declared as attribute");
				}
				if (fa.isVector())
				{
					val.defineAsVector( fa.subform());
				}
				else
				{
					val = fa.subform();
				}
				if (itr->first == "_")
				{
					result.defineContent( "", val);
				}
				else
				{
					result.defineContent( itr->first, val);
				}
			}
			else
			{
				AtomicType at( fa.type());
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
					if (itr->first == "_") throw std::runtime_error( "empty attribute name is illegal");
					result.defineAttribute( itr->first, val);
				}
				else
				{
					if (itr->first == "_")
					{
						result.defineContent( "", val);
					}
					else
					{
						result.defineContent( itr->first, val);
					}
				}
			}
		}
		else
		{
			if (itr->second.data().size())
			{
				FRMAttribute fa( itr->second.data(), typemap, formmap);
				if (!fa.isForm())
				{
					throw std::runtime_error( "Semantic error: Atomic type declared as structure");
				}
				if (fa.isSubForm())
				{
					throw std::runtime_error( "Syntax error: Form reference declared as structure");
				}
				if (fa.isAttribute())
				{
					throw std::runtime_error( "Syntax error: Form declared as attribute");
				}
				StructType st;
				if (fa.isVector())
				{
					StructType prototype;
					compile_ptree( itr->second, prototype, typemap, formmap);
					st.defineAsVector( prototype);
				}
				else
				{
					if (fa.isForm())
					{
						throw std::runtime_error( "Semantic error: Form declared with default value");
					}
					compile_ptree( itr->second, st, typemap, formmap);
				}
				result.defineContent( itr->first, st);
			}
			else
			{
				StructType st;
				compile_ptree( itr->second, st, typemap, formmap);
				result.defineContent( itr->first, st);
			}
		}
	}
}

static void compile_forms( const boost::property_tree::ptree& pt, std::vector<Form>& result, const TypeMap* typemap)
{
	types::keymap<StructType> formmap;
	boost::property_tree::ptree::const_iterator itr=pt.begin(),end=pt.end();
	if (itr != end
	&&  !boost::algorithm::iequals( itr->first, "DOCTYPE")
	&&  !boost::algorithm::iequals( itr->first, "FORM"))
	{
		// ... single form
		Form form;
		compile_ptree( pt, form, typemap, formmap);
		result.push_back( form);
	}
	else
	{
		for (;itr != end; ++itr)
		{
			Form form;
			if (boost::algorithm::iequals( itr->first, "DOCTYPE"))
			{
				form.defineDoctype( itr->second.data());
				compile_ptree( itr->second, form, typemap, formmap);
				std::string doctypeid = utils::getIdFromDoctype( form.doctype());
				formmap.insert( doctypeid, form);
				result.push_back( form);
			}
			else if (boost::algorithm::iequals( itr->first, "FORM"))
			{
				if (!isIdentifier( itr->second.data())) throw std::runtime_error( "identifier expected after FORM");
				form.defineDoctype( std::string( "_ \"") + itr->second.data() + "\"");
				compile_ptree( itr->second, form, typemap, formmap);
				std::string doctypeid = utils::getIdFromDoctype( form.doctype());
				formmap.insert( doctypeid, form);
				result.push_back( form);
			}
			else
			{
				throw std::runtime_error( "DOCTYPE or FORM expected as top level node");
			}
		}
	}
}

std::vector<Form> SimpleFormCompiler::compile( const std::string& srcstring, const TypeMap* typemap) const
{
	std::vector<Form> rt;
	std::istringstream src( srcstring);
	boost::property_tree::ptree pt;
	boost::property_tree::info_parser::read_info( src, pt);
	compile_forms( pt, rt, typemap);
	return rt;
}

DDLCompiler* _Wolframe::ddl::createSimpleFormCompilerFunc()
{
	return new SimpleFormCompiler();
}


