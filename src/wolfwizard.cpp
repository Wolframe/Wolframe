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
///\brief Program mapping wolframe elements to output (code generation)
#include "prgbind/programLibrary.hpp"
#include "wolframe.hpp"
#include "wolfwizardCommandLine.hpp"
#include "module/moduleInterface.hpp"
#include "processor/procProvider.hpp"
#include "types/variantStruct.hpp"
#include "types/variantStructDescription.hpp"
#include <fstream>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <stdexcept>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>

using namespace _Wolframe;

#define DO_STRINGIFY(x)	#x
#define STRINGIFY(x)	DO_STRINGIFY(x)

struct StackElement
{
	types::VariantStructDescription::const_iterator itr;
	types::VariantStructDescription::const_iterator end;

	StackElement( const StackElement& o)
		:itr(o.itr),end(o.end){}
	StackElement( const types::VariantStructDescription::const_iterator& itr_, const types::VariantStructDescription::const_iterator& end_)
		:itr(itr_),end(end_){}
};

struct Header
{
	Header(){}
	Header( const Header& o)
		:m_content(o.m_content){}

	void define( const std::string& name, const std::string& value)
	{
		m_content.push_back( std::pair<std::string,std::string>( name, value));
	}

	std::string openContent() const
	{
		std::string rt( attributeContent());
		rt.append( ">");
		return rt;
	}
	std::string openCloseContent() const
	{
		std::string rt( attributeContent());
		rt.append( "/>");
		return rt;
	}
	std::string closeContent() const
	{
		std::string rt( "</element>");
		return rt;
	}

private:
	std::string attributeContent() const
	{
		std::string rt( "<element");
		std::vector<std::pair<std::string,std::string> >::const_iterator ci = m_content.begin(), ce = m_content.end();
		for (; ci != ce; ++ci)
		{
			rt.append( " ");
			rt.append( ci->first);
			rt.append( "='");
			rt.append( ci->second);
			rt.append( "'");
		}
		return rt;
	}
private:
	std::vector<std::pair<std::string,std::string> > m_content;
};

static void setAtomicElemAttributes( Header& hdr, const types::VariantStruct& st)
{
	switch (st.type())
	{
		case types::VariantStruct::Null:
			break;
		case types::VariantStruct::Unresolved:
		{
			hdr.define( "value", st.unresolvedName());
			break;
		}
		case types::VariantStruct::Bool:
		case types::VariantStruct::Double:
		case types::VariantStruct::Int:
		case types::VariantStruct::UInt:
		case types::VariantStruct::String:
		case types::VariantStruct::Timestamp:
		case types::VariantStruct::BigNumber:
		case types::VariantStruct::Custom:
		{
			hdr.define( "value", ((const types::Variant)st).tostring());
			break;
		}
		case types::VariantStruct::Array:
		case types::VariantStruct::Struct:
			throw std::logic_error( "illegal state in print atomic element");

		case types::VariantStruct::Indirection:
			hdr.define( "value", ((const types::Variant)st).tostring());
			break;
	}
}


static const char* resolveIndirection( std::vector<StackElement> stk, const types::VariantStructDescription* ref)
{
	const char* rt = 0;
	if (!ref) throw std::logic_error( "internal: cannot resolve name of undefined (NULL) indirection pointer");
	std::vector<StackElement>::const_iterator si = stk.begin(), se = stk.end();
	for (; si != se; ++si)
	{
		if (si->itr->substruct == ref)
		{
			rt = si->itr->name;
		}
		if (si->itr->array() && si->itr->initvalue->prototype()->description() == ref)
		{
			rt = si->itr->name;
		}
	}
	return rt;
}

static std::string indent( std::size_t n)
{
	std::string rt( n, ' ');
	return rt + rt;
}

static void printStructXML( std::ostream& out, const types::VariantStructDescription::const_iterator& itr_, const types::VariantStructDescription::const_iterator& end_)
{
	std::vector<StackElement> stk;
	stk.push_back( StackElement( itr_, end_));

	while (!stk.empty())
	{
		if (stk.back().itr == stk.back().end)
		{
			stk.pop_back();
			if (stk.size())
			{
				Header hdr;
				out << indent( stk.size()) << hdr.closeContent() << std::endl;
			}
			++stk.back().itr;
			continue;
		}
		Header hdr;
		hdr.define( "name", stk.back().itr->name);
		if (!stk.back().itr->array())
		{
			if (stk.back().itr->optional())
			{
				hdr.define( "status", "optional");
			}
			else if (stk.back().itr->mandatory())
			{
				hdr.define( "status", "mandatory");
			}
		}
		hdr.define( "type", types::VariantStruct::typeName( (types::VariantStruct::Type)stk.back().itr->type()));

		switch ((types::VariantStruct::Type)stk.back().itr->type())
		{
			case types::VariantStruct::Null:
			case types::VariantStruct::Unresolved:
			case types::VariantStruct::Bool:
			case types::VariantStruct::Double:
			case types::VariantStruct::Int:
			case types::VariantStruct::UInt:
			case types::VariantStruct::String:
			case types::VariantStruct::Timestamp:
			case types::VariantStruct::BigNumber:
			case types::VariantStruct::Custom:
				hdr.define( "attribute", stk.back().itr->attribute()?"yes":"no");
				if (stk.back().itr->initvalue)
				{
					setAtomicElemAttributes( hdr, *stk.back().itr->initvalue);
					out << indent(stk.size()) << hdr.openCloseContent() << std::endl;
				}
				++stk.back().itr;
				break;

			case types::VariantStruct::Array:
			{
				const types::VariantStruct* elemst = stk.back().itr->initvalue->prototype();
				hdr.define( "elemtype", elemst->typeName());

				if (elemst->type() == types::VariantStruct::Indirection)
				{
					if (elemst->description())
					{
						const char* indname = resolveIndirection( stk, elemst->description());
						if (indname)
						{
							hdr.define( "backref", indname);
						}
					}
					out << indent(stk.size()) << hdr.openCloseContent() << std::endl;
					++stk.back().itr;
				}
				else if (elemst->description())
				{
					hdr.define( "size", boost::lexical_cast<std::string>( elemst->description()->size()));
					out << indent(stk.size()) << hdr.openContent() << std::endl;
					stk.push_back( StackElement( elemst->description()->begin(), elemst->description()->end()));
				}
				else
				{
					hdr.define( "attribute", stk.back().itr->attribute()?"yes":"no");
					setAtomicElemAttributes( hdr, *elemst);
					out << indent(stk.size()) << hdr.openCloseContent() << std::endl;
					++stk.back().itr;
				}
				break;
			}
			case types::VariantStruct::Struct:
			{
				const types::VariantStructDescription* descr = stk.back().itr->substruct;
				hdr.define( "size", boost::lexical_cast<std::string>( descr->size()));
				out << indent(stk.size()) << hdr.openContent() << std::endl;
				stk.push_back( StackElement( descr->begin(), descr->end()));
				break;
			}
			case types::VariantStruct::Indirection:
			{
				const char* indname = resolveIndirection( stk, stk.back().itr->initvalue->description());
				if (indname)
				{
					hdr.define( "backref", indname);
				}
				out << indent(stk.size()) << hdr.openCloseContent() << std::endl;
				++stk.back().itr;
				break;
			}
		}
	}
}

static void printFormXML( std::ostream& out, const types::Form& form)
{
	out << "<form name='" << form.description()->name() << "' " << "ddl='" << form.description()->ddlname() << "'";
	if (form.description()->xmlRoot()) out << " xmlroot='" << form.description()->xmlRoot() << "'";
	out << ">" << std::endl;
	printStructXML( out, form.description()->begin(), form.description()->end());
	out << "</form>" << std::endl;
}

static void printWizardXML( std::ostream& out, const prgbind::ProgramLibrary& programLibrary)
{
	out << "<?xml version='1.0' encoding='UTF-8' standalone='no'?>" << std::endl;
	out << "<!DOCTYPE forms SYSTEM \"Wolfwizard.simpleform\">" << std::endl;
	out << "<forms>" << std::endl;
	std::vector<std::string> formnames = programLibrary.getFormNames();
	std::vector<std::string>::const_iterator fi = formnames.begin(), fe = formnames.end();
	for (; fi != fe; ++fi)
	{
		const types::FormDescription* formdescription = programLibrary.getFormDescription( *fi);
		types::Form form( formdescription);
		printFormXML( out, form);
	}
	out << "</forms>" << std::endl;
}

int main( int argc, char **argv )
{
	bool doExit = false;
	bool doPrintHelpOnError = true;
	try
	{
		static boost::filesystem::path execdir = boost::filesystem::system_complete( argv[0]).parent_path();
		module::ModulesDirectory modDir;

#if defined( DEFAULT_MODULE_LOAD_DIR)
		config::WolfwizardCommandLine cmdline( argc, argv, execdir.string(), STRINGIFY( DEFAULT_MODULE_LOAD_DIR));
#else
		config::WolfwizardCommandLine cmdline( argc, argv, execdir.string(), execdir.string());
#endif
		doPrintHelpOnError = false;
		if (cmdline.printversion())
		{
			std::cerr << "wolfwizard version ";
			std::cerr << WOLFRAME_MAJOR_VERSION << "." << WOLFRAME_MINOR_VERSION << "." << WOLFRAME_REVISION << "." << WOLFRAME_BUILD << std::endl;
			doExit = true;
		}
		if (cmdline.printhelp() || cmdline.configfile().empty())
		{
			cmdline.print( std::cerr);
			doExit = true;
		}
		if (doExit) return 0;

		if (!LoadModules( modDir, cmdline.modules()))
		{
			throw std::runtime_error( "Modules could not be loaded");
		}
		proc::ProcProviderConfig providerconf;
		if (!providerconf.parse( cmdline.providerconfig(), "", &modDir))
		{
			throw std::runtime_error( "Processor provider configuration could not be created from command line");
		}
		providerconf.setCanonicalPathes( cmdline.referencePath());
		if (!providerconf.check())
		{
			throw std::runtime_error( "error in command line. failed to setup a valid processor provider configuration");
		}

		prgbind::ProgramLibrary* programLibrary = new prgbind::ProgramLibrary();
		proc::ProcessorProvider* processorProvider = new proc::ProcessorProvider( &providerconf, &modDir, programLibrary);

		if (!processorProvider->loadPrograms())
		{
			throw std::runtime_error( "Not all programs could be loaded. See log." );
		}

		// Output:
		printWizardXML( std::cout, *programLibrary);
		delete processorProvider;
		delete programLibrary;
	}
	catch (const std::bad_alloc& e)
	{
		std::cerr << "out of memory in wolfwizard" << std::endl;
		return 1;
	}
	catch (const std::runtime_error& e)
	{
		std::cerr << e.what() << std::endl;
		if (doPrintHelpOnError)
		{
			config::WolfwizardCommandLine::print( std::cerr);
		}
		return 2;
	}
	catch (const std::exception& e)
	{
		std::cerr << "exception " << e.what() << std::endl;
		return 2;
	}
	return 0;
}

