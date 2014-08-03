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
#include "simpleFormLexer.hpp"
#include "types/keymap.hpp"
#include "utils/fileUtils.hpp"
#include <string>
#include <vector>
#include <cstring>
#include <cstddef>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>

using namespace _Wolframe;
using namespace _Wolframe::langbind;
using namespace _Wolframe::langbind::simpleform;

static void compile_structure( Lexer& lexer, types::VariantStructDescription& result, const types::NormalizeFunctionMap* typemap, const types::keymap<types::FormDescriptionR>& formmap, std::vector<std::string>& unresolvedSymbols)
{
	bool seenCloseStruct = false;
	Lexem lx;
	for (lx = lexer.next2(false); lx.id() != Lexem::CloseStruct; lx=lexer.next2(false))
	{
		if (lx.id() != Lexem::Identifier && lx.id() != Lexem::String)
		{
			throw std::runtime_error( std::string("unexpected ") + lexer.curtoken() + ", structure element name as identifier or string expected");
		}
		std::string elementname( lx.value());		//< name of the element
		bool isOptional = false;			//< element is optional (also in strict validation)
		bool isMandatory = false;			//< element is mandatory (also in relaxed validation)
		bool isIndirection = false;			//< element is an indirection (means it is expanded only when it exists)
		bool isArray = false;				//< element is an array
		bool isAttribute = false;			//< element is an attribute (for languages that know attributes like XML)
		bool isInherited = (elementname == "_");	//< element is inherited (means it has no name and it acts as content of the enclosing element)

		for (lx=lexer.next2(true);; lx=lexer.next2(true))
		{
			if (lx.id() == Lexem::Separator)
			{
				for (lx=lexer.next2(true); lx.id() == Lexem::Separator; lx=lexer.next2(true)){}
				if (lx.id() != Lexem::OpenStruct)
				{
					throw std::runtime_error("unexpected end of line in the middle of an atomic element definition");
				}
			}
			if (lx.id() == Lexem::OpenStruct)
			{
				types::VariantStructDescription substruct;
				compile_structure( lexer, substruct, typemap, formmap, unresolvedSymbols);
				int elementidx = result.addStructure( elementname, substruct);
				if (isArray) result.at( elementidx)->makeArray();
				if (isOptional) result.at( elementidx)->setOptional();
				if (isMandatory) result.at( elementidx)->setMandatory();
				if (isIndirection) throw std::runtime_error("cannot declare embedded substructure as indirection ('^')");
				if (isAttribute) throw std::runtime_error("cannot declare structure as attribute ('@')");
				if (isInherited) throw std::runtime_error("cannot declare embedded substructure with name \"_\" used to declare inheritance");
				lx = lexer.next2(true);
				if (lx.id() == Lexem::CloseStruct)
				{
					seenCloseStruct = true;
				}
				else if (lx.id() != Lexem::Separator)
				{
					throw std::runtime_error("expected close structure '}' or end of line or comma ',' as separator after close struct");
				}
				break;
			}
			else if (lx.id() == Lexem::Identifier)
			{
				std::string elementtype( lx.value());
				std::string defaultvalue;
				bool hasDefault = false;

				lx = lexer.next2(true);
				for (; lx.id() != Lexem::Separator && lx.id() != Lexem::CloseStruct; lx = lexer.next2(true))
				{
					if (lx.id() == Lexem::ArrayTag)
					{
						if (hasDefault) throw std::runtime_error("unexpected array marker after default value assignment");
						if (isArray) throw std::runtime_error("cannot handle duplicate array tag '[]'");
						isArray = true;
					}
					else if (lx.id() == Lexem::Assign)
					{
						if (isArray) throw std::runtime_error("cannot handle default value assignment for array");
						if (hasDefault) throw std::runtime_error("cannot handle duplicate default value assignment");
						lx = lexer.next2(true);
						if (lx.id() != Lexem::String && lx.id() != Lexem::Identifier)
						{
							throw std::runtime_error( "string or identifier expected for default value declaration after '='");
						}
						defaultvalue = lx.value();
						hasDefault = true;
					}
					else
					{
						throw std::runtime_error( std::string("unexpected token ") + lexer.curtoken() + ", default value assignment or array marker or following structure element declaration expected");
					}
				}
				if (lx.id() == Lexem::CloseStruct)
				{
					seenCloseStruct = true;
				}

				// Resolve type:
				types::keymap<types::FormDescriptionR>::const_iterator fmi;
				fmi = formmap.find( elementtype);
				if (fmi != formmap.end())
				{
					// ... type referenced is a sub structure
					if (isAttribute) throw std::runtime_error("cannot declare structure as attribute ('@')");

					// Sub structure referenced by 'elementtype':
					const types::FormDescription* substruct = fmi->second.get();
					if (isIndirection)
					{
						// ... resolved indirection
						int elementidx = result.addIndirection( elementname, substruct);
						if (isArray) result.at( elementidx)->makeArray();
					}
					else if (isInherited)
					{
						// ... resolved inheritance
						result.inherit( *substruct);
					}
					else
					{
						// ... resolved sub structure
						int elementidx = result.addStructure( elementname, *substruct);
						if (isArray) result.at( elementidx)->makeArray();
						if (isOptional) result.at( elementidx)->setOptional();
						if (isMandatory) result.at( elementidx)->setMandatory();
					}
				}
				else if (boost::algorithm::iequals( elementtype, "string"))
				{
					// ... type referenced is the reserved type string (no normalizer defined)
					if (isIndirection) throw std::runtime_error("cannot declare atomic string type as indirection ('^')");
					int elementidx; //< index of atomic element created
					if (isAttribute)
					{
						elementidx = result.addAttribute( elementname, hasDefault?types::Variant(defaultvalue):types::Variant(types::Variant::String));
						//... atomic attribute value
					}
					else if (isInherited)
					{
						elementidx = result.addAtom( "", hasDefault?types::Variant(defaultvalue):types::Variant(types::Variant::String));
						//... atomic unnamed (inherited) content value
					}
					else
					{
						elementidx = result.addAtom( elementname, hasDefault?types::Variant(defaultvalue):types::Variant(types::Variant::String));
						//... atomic named content value (non attribute)
					}
					if (isArray) result.at( elementidx)->makeArray();
					if (isOptional) result.at( elementidx)->setOptional();
					if (isMandatory) result.at( elementidx)->setMandatory();
				}
				else
				{
					// ... type referenced must be an atomic type defined in a .wnmp file or a custom data type
					const types::NormalizeFunction* atomictype;
					if (isIndirection)
					{
						// ... unresolved indirection
						int elementidx = result.addUnresolved( elementname, elementtype);
						if (isArray) result.at( elementidx)->makeArray();
						unresolvedSymbols.push_back( elementtype);
					}
					else if (0!=(atomictype=typemap->get( elementtype)))
					{
						// ... type referenced is an atomic type defined in a .wnmp file or a custom data type
						int elementidx; //< index of atomic element created
						if (isIndirection) throw std::runtime_error("cannot declare atomic type as indirection ('^')");
						if (isAttribute)
						{
							elementidx = result.addAttribute( elementname, atomictype->execute(hasDefault?types::Variant(defaultvalue):types::Variant()), atomictype);
							//... atomic attribute value
						}
						else if (isInherited)
						{
							elementidx = result.addAtom( "", atomictype->execute(hasDefault?types::Variant(defaultvalue):types::Variant()), atomictype);
							//... atomic unnamed (inherited) content value
						}
						else
						{
							elementidx = result.addAtom( elementname, atomictype->execute(hasDefault?types::Variant(defaultvalue):types::Variant()), atomictype);
							//... atomic named content value (non attribute)
						}
						if (isArray) result.at( elementidx)->makeArray();
						if (isOptional) result.at( elementidx)->setOptional();
						if (isMandatory) result.at( elementidx)->setMandatory();
					}
					else if (isInherited)
					{
						// ... unresolved inheritance = error
						throw std::runtime_error( std::string("undefined atomic data type or inherited structure '") + elementtype + "'");
					}
					else
					{
						throw std::runtime_error( std::string("cannot find type '") + elementtype + "'");
					}
				}
				break;
			}
			else if (lx.id() == Lexem::ArrayTag)
			{
				if (isInherited) throw std::runtime_error("contradicting array/inheritance tag '[]' and '_'");
				if (isArray) throw std::runtime_error("cannot handle duplicate array tag '[]'");
				isArray = true;
			}
			else if (lx.id() == Lexem::AttributeTag)
			{
				if (isInherited) throw std::runtime_error("contradicting attribute/inheritance tag '@' and '_'");
				if (isAttribute) throw std::runtime_error("cannot handle duplicate attribute tag '@'");
				isAttribute = true;
			}
			else if (lx.id() == Lexem::MandatoryTag)
			{
				if (isInherited) throw std::runtime_error("contradicting mandatory/inheritance tag '!' and '_'");
				if (isOptional) throw std::runtime_error("contradicting optional/mandatory tag '?' and '!'");
				if (isMandatory) throw std::runtime_error("cannot handle duplicate mandatory tag '!'");
				isMandatory = true;
			}
			else if (lx.id() == Lexem::OptionalTag)
			{
				if (isInherited) throw std::runtime_error("contradicting optional/inheritance tag '?' and '_'");
				if (isMandatory) throw std::runtime_error("contradicting optional/mandatory tag '?' and '!'");
				if (isOptional) throw std::runtime_error("cannot handle duplicate optional tag '?'");
				isOptional = true;
			}
			else if (lx.id() == Lexem::Indirection)
			{
				if (isInherited) throw std::runtime_error("contradicting indirection/inheritance tag '^' and '_'");
				if (isOptional) throw std::runtime_error("contradicting indirection/optional tag '^' and '?'");
				if (isMandatory) throw std::runtime_error("contradicting indirection/mandatory tag '^' and '!'");
				if (isIndirection) throw std::runtime_error("cannot handle duplicate indirection tag '^'");
				isIndirection = true;
			}
			else
			{
				throw std::runtime_error( std::string("unexpected token ") + lexer.curtoken() + ", attribute or type name or substructure expected");
			}
		}
		if (seenCloseStruct)
		{
			break;
		}
	}
}

static void compile_forms( const std::string& filename, std::vector<types::FormDescriptionR>& result, const types::NormalizeFunctionMap* typemap, const std::vector<std::string>& filenamestack)
{
	std::string content( utils::readSourceFileContent( filename));
	Lexer lexer( content.begin(), content.end());
	std::string includepath( boost::filesystem::system_complete( filename).parent_path().string());

	// Check circular include references:
	std::vector<std::string> filenamestack2 = filenamestack;
	filenamestack2.push_back( filename);
	std::vector<std::string>::const_iterator fi = filenamestack.begin(), fe = filenamestack.end();
	for (; fi != fe; ++fi)
	{
		if (filename == *fi) throw std::runtime_error( std::string("circular include file reference including file '") + filename + "'");
	}

	Lexem lx;
	try
	{
		std::vector<std::string> unresolvedSymbols;
		types::keymap<types::FormDescriptionR> formmap;
	
		// Compile all forms and structures:
		for (lx=lexer.next(false); lx.id() != Lexem::EndOfFile; lx=lexer.next(false))
		{
			if (lx.id() == Lexem::INCLUDE)
			{
				lx = lexer.next(false);
				if (lx.id() != Lexem::String) throw std::runtime_error( std::string("unexpected token ") + lexer.curtoken() + ", string expected");
				
				compile_forms( utils::getCanonicalPath( lx.value(), includepath), result, typemap, filenamestack2);
			}
			else if (lx.id() == Lexem::STRUCT)
			{
				lx = lexer.next(false);
				if (lx.id() != Lexem::Identifier && lx.id() != Lexem::String) throw std::runtime_error( std::string("unexpected token ") + lexer.curtoken() + ", string or identifier expected for name of structure");
				if (lx.value().empty()) throw std::runtime_error( "non empty structure name expected after STRUCT");
				std::string structname( lx.value());	//... name of the structure in the reference table for indirections and sub structures
				lx = lexer.next(false);
				if (lx.id() == Lexem::MetaDataDef) throw std::runtime_error("meta data definition (':') not possible in STRUCT definition");
				if (lx.id() != Lexem::OpenStruct) throw std::runtime_error("open structure operator '{' expected (start of the form or structure declaration)");

				types::FormDescriptionR form( new types::FormDescription( "simpleform"));
				compile_structure( lexer, *form, typemap, formmap, unresolvedSymbols);
				formmap.insert( structname, form);
			}
			else if (lx.id() == Lexem::FORM)
			{
				// Define form structure name (doctype):
				lx = lexer.next(false);
				if (lx.id() != Lexem::Identifier && lx.id() != Lexem::String) throw std::runtime_error( std::string("unexpected token ") + lexer.curtoken() + ", string or identifier expected for name of structure");
				if (lx.value().empty()) throw std::runtime_error( "non empty form name expected after FORM");
				std::string structname( lx.value());	//... name of the form

				types::DocMetaData metadata;
				lx = lexer.next(false);

				// Define meta data attributes:
				std::map<std::string,bool> defmap;
				while (lx.id() == Lexem::MetaDataDef)
				{
					lx = lexer.next(false);
					if (lx.id() != Lexem::Identifier && lx.id() != Lexem::String) throw std::runtime_error( std::string("unexpected token ") + lexer.curtoken() + ", string or identifier expected for meta data attribute name after ':'");
					if (lx.value().empty()) throw std::runtime_error( "non empty meta data attribute name name expected after ':'");
					std::string attrnam( lx.value());
					if (defmap.find( attrnam) != defmap.end())
					{
						throw std::runtime_error( "duplicate definition of meta data attribute in form");
					}
					defmap[ attrnam] = true;
					lx = lexer.next(false);
					if (lx.id() == Lexem::Identifier && lx.value() == "NULL")
					{
						metadata.deleteAttribute( attrnam);
					}
					else
					{
						if (lx.id() != Lexem::Identifier && lx.id() != Lexem::String)
						{
							throw std::runtime_error( std::string("unexpected token ") + lexer.curtoken() + ", string or identifier expected for meta data attribute value after ':' amd meta data name");
						}
						std::string attrval( lx.value());
						metadata.setAttribute( attrnam, attrval);
					}
					lx = lexer.next(true);
					if (lx.id() != Lexem::Separator)
					{
						if (lx.id() == Lexem::MetaDataDef)
						{
							throw std::runtime_error( "comma ',' or end of line expected as separator of meta data declarations");
						}
						break;
					}
					lx = lexer.next(false);
				}
				// Define form data structure:
				if (lx.id() != Lexem::OpenStruct) throw std::runtime_error("open structure operator '{' expected (start of the form or structure declaration)");

				types::FormDescriptionR form( new types::FormDescription( "simpleform", structname, metadata));
				compile_structure( lexer, *form, typemap, formmap, unresolvedSymbols);
				formmap.insert( structname, form);
				result.push_back( form);
			}
			else
			{
				throw std::runtime_error(std::string("unexpected token ") + lexer.curtoken() + ", FORM or STRUCT or INCLUDE declaration expected");
			}
		}

		// Do resolve unresolved form indirection references:
		types::FormDescription::ResolveMap resolvemap;

		std::vector<std::string>::const_iterator ui = unresolvedSymbols.begin(), ue = unresolvedSymbols.end();
		for (; ui != ue; ++ui)
		{
			types::keymap<types::FormDescriptionR>::const_iterator ri = formmap.find( *ui);
			if (ri == formmap.end())
			{
				throw std::runtime_error( std::string( "could not resolve reference to STRUCT or FORM '") + *ui + "'");
			}
			if (ri->second->name().empty())
			{
				//... push referenced private STRUCT declarations.
				// (public form declarations are already part of the result)
				result.push_back( ri->second);
			}
			resolvemap[ *ui] = ri->second.get();
		}

		types::keymap<types::FormDescriptionR>::const_iterator ri = formmap.begin(), re = formmap.end();
		for (; ri != re; ++ri)
		{
			ri->second.get()->resolve( resolvemap);
		}
	}
	catch (const std::runtime_error& e)
	{
		if (boost::algorithm::starts_with( e.what(), "error"))
		{
			if (filenamestack.size() >= 1)
			{
				throw std::runtime_error( std::string(e.what()) + "; in file included from '" + filenamestack.back() + "'"); 
			}
			else
			{
				throw e;
			}
		}
		else
		{
			throw std::runtime_error(std::string("error in file '") + filename + "' at line " + boost::lexical_cast<std::string>(lexer.position().line()) + " column " + boost::lexical_cast<std::string>(lexer.position().column()) + ": " + e.what());
		}
	}
}

std::vector<types::FormDescriptionR> SimpleFormCompiler::compile( const std::string& filename, const types::NormalizeFunctionMap* typemap) const
{
	std::vector<types::FormDescriptionR> rt;
	std::vector<std::string> filenamestack;
	compile_forms( filename, rt, typemap, filenamestack);
	return rt;
}



