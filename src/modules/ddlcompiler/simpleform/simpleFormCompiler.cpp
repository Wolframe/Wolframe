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
#include "types/doctype.hpp"
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
		,m_isOptional(o.m_isOptional)
		,m_isMandatory(o.m_isMandatory)
		,m_isForm(o.m_isForm)
		,m_isFormReference(o.m_isFormReference)
		,m_isSelfReference(o.m_isSelfReference)
		,m_type(o.m_type)
		,m_subform(o.m_subform)
		,m_value(o.m_value){}

	explicit FRMAttribute( const std::string& item, const TypeMap* typemap, const types::keymap<StructType>& formmap, const std::string& selfname)
		:m_isVector(false)
		,m_isAttribute(false)
		,m_isOptional(false)
		,m_isMandatory(false)
		,m_isForm(false)
		,m_isFormReference(false)
		,m_isSelfReference(false)
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
						if (m_isAttribute) throw std::runtime_error( "Syntax error in Simple Form: duplicate attribute '@'");
						m_isAttribute = true;
						continue;
					}
					if (*ii == '!')
					{
						if (m_isMandatory) throw std::runtime_error( "Syntax error in Simple Form: duplicate attribute '*'");
						if (m_isOptional) throw std::runtime_error( "Syntax error in Simple Form: contradicting attributes '*' and '?' set");
						m_isMandatory = true;
						continue;
					}
					if (*ii == '?')
					{
						if (m_isOptional) throw std::runtime_error( "Syntax error in Simple Form: duplicate attribute '?'");
						if (m_isMandatory) throw std::runtime_error( "Syntax error in Simple Form: contradicting attributes '?' and '*' set");
						m_isOptional = true;
						continue;
					}
					st = ParseName;
					/* no break here !*/
				case ParseName:
					if (((*ii|32) >= 'a' && (*ii|32) <= 'z') || (*ii >= '0' && *ii <= '9') || *ii == '_')
					{
						vv.push_back( *ii);
						continue;
					}
					parseType( vv, typemap, formmap, selfname);
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
			parseType( vv, typemap, formmap, selfname);
		}
		else if (st != ParseEnd)
		{
			throw std::runtime_error( "Syntax error in Simple Form Attribute: Incomplete attribute");
		}
	}

	bool isFormReference() const		{return m_isFormReference;}
	bool isSelfReference() const		{return m_isSelfReference;}
	bool isVector() const			{return m_isVector;}
	bool isAttribute() const		{return m_isAttribute;}
	bool isOptional() const			{return m_isOptional;}
	bool isMandatory() const		{return m_isMandatory;}
	bool isForm() const			{return m_isForm;}
	const NormalizeFunction* type() const	{return m_type;}
	const std::string& value() const	{return m_value;}
	const StructType& subform() const	{return m_subform;}

private:
	void parseType( const std::string& typestr, const TypeMap* typemap, const types::keymap<StructType>& formmap, const std::string& selfname)
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
			m_isFormReference = true;
			m_subform = fmi->second;
		}
		else if (0!=(m_type = typemap->getType( typestr)))
		{
		}
		else if (boost::algorithm::iequals( typestr, selfname))
		{
			m_isSelfReference = true;
		}
		else
		{
			throw std::runtime_error( (std::string( "unknown type: '") += typestr) += "'");
		}
	}
private:
	bool m_isVector;
	bool m_isAttribute;
	bool m_isOptional;
	bool m_isMandatory;
	bool m_isForm;
	bool m_isFormReference;
	bool m_isSelfReference;
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

static void compile_ptree( const boost::property_tree::ptree& pt, StructType& result, const TypeMap* typemap, const types::keymap<StructType>& formmap, const std::string& selfname)
{
	boost::property_tree::ptree::const_iterator itr=pt.begin(),end=pt.end();
	for (;itr != end; ++itr)
	{
		std::string::const_iterator si = itr->first.begin(), se = itr->first.begin();
		std::string first;
		std::string second;
		for (; si != se; ++si)
		{
			if (*si == '[' || *si == '(' || *si == '@') break;
		}
		if (si == se)
		{
			first = itr->first;
			second = itr->second.data();
		}
		else
		{
			first = std::string( itr->first.begin(), si);
			second = std::string( si, se);
			second.push_back(' ');
			second.append( itr->second.data());
		}
		if (!isIdentifier( first))
		{
			throw std::runtime_error( "Semantic error: Identifier expected as variable name");
		}
		if (itr->second.begin() == itr->second.end() && second.size())
		{
			FRMAttribute fa( second, typemap, formmap, selfname);
			if (fa.isForm())
			{
				throw std::runtime_error( "Semantic error: illegal type specifier");
			}
			if (fa.isFormReference())
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
				if (fa.isOptional())
				{
					val.optional(true);
				}
				if (fa.isMandatory())
				{
					val.mandatory(true);
				}
				if (first == "_")
				{
					if (fa.isVector()) throw std::runtime_error( "Semantic error: try to inherit from an array ('[]')");
					if (fa.isOptional()) throw std::runtime_error( "Semantic error: optional ('?') declaration for inherited structure");
					if (fa.isMandatory()) throw std::runtime_error( "Semantic error: mandatory ('!') declaration for inherited structure");
					result.inheritContent( val);
				}
				else
				{
					result.defineContent( first, val);
				}
			}
			else if (fa.isSelfReference())
			{
				IndirectionConstructorR ind;
				StructType val;
				if (fa.isAttribute())
				{
					throw std::runtime_error( "Syntax error: Form declared as attribute");
				}
				if (fa.isVector())
				{
					StructType elem;
					elem.defineAsIndirection( ind);
					val.defineAsVector( elem);
				}
				else
				{
					val.defineAsIndirection( ind);
				}
				if (fa.isOptional())
				{
					throw std::runtime_error( "Syntax error: Self references are always optional");
				}
				if (fa.isMandatory())
				{
					throw std::runtime_error( "Syntax error: Self references are never mandatory");
				}
				if (first == "_")
				{
					throw std::runtime_error( "Syntax error: Self reference declared as untagged content element");
				}
				else
				{
					result.defineContent( first, val);
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
				if (fa.isOptional())
				{
					val.optional(true);
				}
				if (fa.isMandatory())
				{
					val.mandatory(true);
				}
				if (fa.isAttribute())
				{
					if (first == "_") throw std::runtime_error( "empty attribute name is illegal");
					result.defineAttribute( first, val);
				}
				else
				{
					if (first == "_")
					{
						result.defineContent( "", val);
					}
					else
					{
						result.defineContent( first, val);
					}
				}
			}
		}
		else
		{
			//...  Embedded substructure definition
			if (!second.empty())
			{
				FRMAttribute fa( second, typemap, formmap, selfname);
				if (!fa.isForm())
				{
					throw std::runtime_error( "Semantic error: Atomic type declared as structure");
				}
				if (fa.isFormReference())
				{
					throw std::runtime_error( "Syntax error: Form reference declared as structure");
				}
				else if (fa.isSelfReference())
				{
					throw std::runtime_error( "Syntax error: Form self indirection declared as structure");
				}
				if (fa.isAttribute())
				{
					throw std::runtime_error( "Syntax error: Form declared as attribute");
				}
				StructType st;
				if (fa.isVector())
				{
					StructType prototype;
					compile_ptree( itr->second, prototype, typemap, formmap, selfname);
					st.defineAsVector( prototype);
				}
				else
				{
					compile_ptree( itr->second, st, typemap, formmap, selfname);
				}
				if (fa.isOptional())
				{
					st.optional(true);
				}
				if (fa.isMandatory())
				{
					st.mandatory(true);
				}
				if (first == "_")
				{
					throw std::runtime_error( "Syntax error: Embedded substructure declared as untagged content element");
				}
				else
				{
					result.defineContent( first, st);
				}
			}
			else
			{
				StructType st;
				compile_ptree( itr->second, st, typemap, formmap, selfname);
				result.defineContent( first, st);
			}
		}
	}
}

static void compile_forms( const boost::property_tree::ptree& pt, std::vector<Form>& result, const TypeMap* typemap)
{
	types::keymap<StructType> formmap;
	boost::property_tree::ptree::const_iterator itr=pt.begin(),end=pt.end();
	if (itr != end && !boost::algorithm::iequals( itr->first, "FORM"))
	{
		// ... single form
		Form form( "simpleform");
		compile_ptree( pt, form, typemap, formmap, "");
		result.push_back( form);
	}
	else
	{
		for (;itr != end; ++itr)
		{
			if (boost::algorithm::iequals( itr->first, "FORM"))
			{
				if (!isIdentifier( itr->second.data())) throw std::runtime_error( "identifier expected after FORM");

				Form form( "simpleform");
				form.defineName( itr->second.data());
				compile_ptree( itr->second, form, typemap, formmap, itr->second.data());
				IndirectionConstructorR selfref( new StructIndirectionConstructor(form));
				StructIndirectionConstructor::substituteSelf( &form, selfref);
				formmap.insert( form.name(), form);
				result.push_back( form);
			}
			else if (boost::algorithm::iequals( itr->first, "STRUCT"))
			{
				if (!isIdentifier( itr->second.data())) throw std::runtime_error( "identifier expected after FORM");

				Form form( "simpleform");
				form.defineName( itr->second.data());
				compile_ptree( itr->second, form, typemap, formmap, itr->second.data());
				IndirectionConstructorR selfref( new StructIndirectionConstructor(form));
				StructIndirectionConstructor::substituteSelf( &form, selfref);
				formmap.insert( form.name(), form);
			}
			else
			{
				throw std::runtime_error( "FORM expected as top level node");
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


