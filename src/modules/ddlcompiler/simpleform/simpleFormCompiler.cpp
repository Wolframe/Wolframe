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
using namespace _Wolframe::langbind;

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
		,m_isIndirection(o.m_isIndirection)
		,m_type(o.m_type)
		,m_subform(o.m_subform)
		,m_value(o.m_value){}

	explicit FRMAttribute( const std::string& item, const types::NormalizeFunctionMap* typemap, const types::keymap<types::FormDescriptionR>& formmap, const std::string& selfname)
		:m_isVector(false)
		,m_isAttribute(false)
		,m_isOptional(false)
		,m_isMandatory(false)
		,m_isForm(false)
		,m_isFormReference(false)
		,m_isIndirection(false)
		,m_type(0)
		,m_subform(0)
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
		std::string::const_iterator ii=item.begin(), ee=item.end();
		for (; ii != ee; ++ii)
		{
			switch (st)
			{
				case ParseStart:
					if (*ii == '@')
					{
						if (m_isIndirection) throw std::runtime_error( "Syntax error in Simple Form: indirection cannot be an attribute('@')");
						if (m_isAttribute) throw std::runtime_error( "Syntax error in Simple Form: duplicate attribute '@'");
						m_isAttribute = true;
						continue;
					}
					if (*ii == '!')
					{
						if (m_isIndirection) throw std::runtime_error( "Syntax error in Simple Form: indirection cannot be mandatory('!')");
						if (m_isMandatory) throw std::runtime_error( "Syntax error in Simple Form: duplicate attribute '*'");
						if (m_isOptional) throw std::runtime_error( "Syntax error in Simple Form: contradicting attributes '*' and '?' set");
						m_isMandatory = true;
						continue;
					}
					if (*ii == '?')
					{
						if (m_isIndirection) throw std::runtime_error( "Syntax error in Simple Form: indirection cannot be optional('?')");
						if (m_isOptional) throw std::runtime_error( "Syntax error in Simple Form: duplicate attribute '?'");
						if (m_isMandatory) throw std::runtime_error( "Syntax error in Simple Form: contradicting attributes '?' and '*' set");
						m_isOptional = true;
						continue;
					}
					if (*ii == '-')
					{
						++ii;
						if (ii == ee)
						{
							throw std::runtime_error( "Syntax error in Simple Form: unexpected end of expression");
						}
						if (*ii != '>')
						{
							throw std::runtime_error( "Syntax error in Simple Form: unexpected operator '-'");
						}
						++ii;
						m_isIndirection = true;
						if (m_isOptional || m_isMandatory || m_isAttribute)
						{
							throw std::runtime_error( "Syntax error in Simple Form: indirection cannot be optional('?'), mandatory ('!') or defined as attribute ('@')");
						}
						continue;
					}
					st = ParseName;
					/* no break here !*/
				case ParseName:
					if (((*ii|32) >= 'a' && (*ii|32) <= 'z') || (*ii >= '0' && *ii <= '9') || *ii == '_')
					{
						m_symbol.push_back( *ii);
						continue;
					}
					if (!m_isIndirection)
					{
						resolveType( m_symbol, typemap, formmap, selfname);
					}
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
						if (m_isIndirection)
						{
							throw std::runtime_error( "Syntax error in Simple Form Attribute: Indirection cannon have a default value");
						}
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
					/* no break here !*/

				case ParseDfValue:
					if (*ii == ')')
					{
						st = ParseEnd;
						break;
					}
					else
					{
						m_value.push_back( *ii);
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
			if (!m_isIndirection)
			{
				resolveType( m_symbol, typemap, formmap);
			}
		}
		else if (st != ParseEnd)
		{
			throw std::runtime_error( "Syntax error in Simple Form Attribute: Incomplete attribute");
		}
	}

	bool isFormReference() const				{return m_isFormReference;}
	bool isIndirection() const				{return m_isIndirection;}
	bool isVector() const					{return m_isVector;}
	bool isAttribute() const				{return m_isAttribute;}
	bool isOptional() const					{return m_isOptional;}
	bool isMandatory() const				{return m_isMandatory;}
	bool isForm() const					{return m_isForm;}
	const types::NormalizeFunction* type() const		{return m_type;}
	const std::string& value() const			{return m_value;}
	const std::string& symbol() const			{return m_symbol;}
	const types::VariantStructDescription* subform() const	{return m_subform;}

private:
	void resolveType( const std::string& typestr, const types::NormalizeFunctionMap* typemap, const types::keymap<types::FormDescriptionR>& formmap)
	{
		types::keymap<types::FormDescriptionR>::const_iterator fmi;
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
			m_subform = fmi->second.get();
		}
		else if (0!=(m_type = typemap->get( typestr)))
		{
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
	bool m_isIndirection;
	const types::NormalizeFunction* m_type;
	const types::VariantStructDescription* m_subform;
	std::string m_symbol;
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

static void compile_ptree( const boost::property_tree::ptree& pt, types::VariantStructDescription& result, const types::NormalizeFunctionMap* typemap, const types::keymap<types::FormDescriptionR>& formmap)
{
	boost::property_tree::ptree::const_iterator itr=pt.begin(),end=pt.end();
	for (;itr != end; ++itr)
	{
		std::string::const_iterator si = itr->first.begin(), se = itr->first.end();
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
			throw std::runtime_error( std::string("Semantic error: Identifier expected as variable name (") + first + ")");
		}
		if (itr->second.begin() == itr->second.end() && second.size())
		{
			FRMAttribute fa( second, typemap, formmap, selfname);
			if (fa.isForm())
			{
				throw std::runtime_error( "Semantic error: illegal type specifier");
			}
			else if (fa.isIndirection())
			{
				result.addUnresolved( fa.symbol());
				if (fa.isVector())
				{
					result.back().makeArray();
				}
				if (first == "_")
				{
					throw std::runtime_error( "Syntax error: Reference declared as inherited (untagged content element '_')");
				}
			}
			else if (fa.isFormReference())
			{
				if (fa.isAttribute())
				{
					throw std::runtime_error( "Syntax error: Form declared as attribute");
				}
				const types::VariantStructDescription* val = fa.subform();
				if (first == "_")
				{
					if (fa.isVector()) throw std::runtime_error( "Semantic error: try to inherit from an array ('[]')");
					if (fa.isOptional()) throw std::runtime_error( "Semantic error: optional ('?') declaration for inherited structure");
					if (fa.isMandatory()) throw std::runtime_error( "Semantic error: mandatory ('!') declaration for inherited structure");
					result.inherit( *val);
				}
				else
				{
					result.addStructure( first, *val);
					if (fa.isVector())
					{
						if (fa.isOptional()) throw std::runtime_error( "Semantic error: try to define an optional array");
						if (fa.isMandatory()) throw std::runtime_error( "Semantic error: try to define an mandatory array");
						result.back().makeArray();
					}
					else
					{
						if (fa.isOptional()) result.back().setOptional();
						if (fa.isMandatory()) result.back().setMandatory();
					}
				}
			}
			else
			{
				types::Variant val( fa.value());
				if (fa.isAttribute())
				{
					if (first == "_") throw std::runtime_error( "empty attribute name is illegal");
					result.addAttribute( first, val, fa.type());
				}
				else
				{
					if (first == "_")
					{
						result.addAtom( "", val, fa.type());
					}
					else
					{
						result.addAtom( first, val, fa.type());
					}
				}
				if (fa.isVector())
				{
					result.back().makeArray();
				}
				if (fa.isOptional())
				{
					result.back().setOptional();
				}
				if (fa.isMandatory())
				{
					result.back().setMandatory();
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
				else if (fa.isIndirection())
				{
					throw std::runtime_error( "Syntax error: Form self indirection declared as structure");
				}
				else if (fa.isFormReference())
				{
					throw std::runtime_error( "Syntax error: Form reference declared as structure");
				}
				if (fa.isAttribute())
				{
					throw std::runtime_error( "Syntax error: Form declared as attribute");
				}
				types::VariantStructDescriptionR substruct( new types::VariantStructDescription());
				compile_ptree( itr->second, *substruct, typemap, formmap, selfname);

				result.addStructure( first, *substruct);

				if (fa.isVector())
				{
					result.back().makeArray();
				}
				if (fa.isOptional())
				{
					result.back().setOptional();
				}
				if (fa.isMandatory())
				{
					result.back().setMandatory();
				}
				if (first == "_")
				{
					throw std::runtime_error( "Syntax error: Embedded substructure declared as untagged content element");
				}
			}
			else
			{
				types::VariantStructDescriptionR substruct( new types::VariantStructDescription());
				compile_ptree( itr->second, *substruct, typemap, formmap, selfname);
				result.addStructure( first, *substruct);
			}
		}
	}
}

static void compile_forms( const boost::property_tree::ptree& pt, std::vector<types::FormDescriptionR>& result, const types::NormalizeFunctionMap* typemap)
{
	types::keymap<types::FormDescriptionR> formmap;
	boost::property_tree::ptree::const_iterator itr=pt.begin(),end=pt.end();
	for (;itr != end; ++itr)
	{
		bool isForm = false;
		bool isStruct = false;
		if (boost::algorithm::iequals( itr->first, "FORM"))
		{
			isForm = true;
			if (!isIdentifier( itr->second.data())) throw std::runtime_error( "identifier expected after FORM");
		}
		else if (boost::algorithm::iequals( itr->first, "STRUCT"))
		{
			isStruct = true;
			if (!isIdentifier( itr->second.data())) throw std::runtime_error( "identifier expected after STRUCT");
		}
		if (isForm || isStruct)
		{
			try
			{
				types::FormDescriptionR form( new types::FormDescription( "simpleform", itr->second.data()));

				formmap.insert( form->name(), form);
				compile_ptree( itr->second, *form, typemap, formmap, itr->second.data());
				if (isForm) result.push_back( form);
			}
			catch (const std::runtime_error& e)
			{
				throw std::runtime_error( std::string( e.what()) + " in form '" + itr->second.data() +"'");
			}
		}
		else
		{
			throw std::runtime_error( "FORM or STRUCT expected as start of a structure definition");
		}
	}
}

std::vector<types::FormDescriptionR> SimpleFormCompiler::compile( const std::string& srcstring, const types::NormalizeFunctionMap* typemap) const
{
	std::vector<types::FormDescriptionR> rt;
	std::istringstream src( srcstring);
	boost::property_tree::ptree pt;
	boost::property_tree::info_parser::read_info( src, pt);
	compile_forms( pt, rt, typemap);
	return rt;
}

DDLCompiler* langbind::createSimpleFormCompilerFunc()
{
	return new SimpleFormCompiler();
}


