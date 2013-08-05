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
///\brief Program mapping wolframe elements to output (code generation)
#include <fstream>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <stdexcept>
#include "prgbind/programLibrary.hpp"
#include "wolfwizardCommandLine.hpp"
#include "processor/moduleInterface.hpp"
#include "processor/procProvider.hpp"
#include "types/variantStruct.hpp"
#include "types/variantStructDescription.hpp"
#include <boost/filesystem.hpp>

using namespace _Wolframe;

///\TODO Not to be defined here
static const unsigned short APP_MAJOR_VERSION = 0;
static const unsigned short APP_MINOR_VERSION = 0;
static const unsigned short APP_REVISION = 5;
static const unsigned short APP_BUILD = 0;

#define DO_STRINGIFY(x)	#x
#define STRINGIFY(x)	DO_STRINGIFY(x)
#define INDENTTAB	"  "

static void printStructXML( std::ostream& out, const char* name, const types::VariantStruct& st, bool isAttribute, std::string indent, bool optional, bool mandatory)
{
	const char* status = "";
	const char* addressing = isAttribute?" attribute='yes'":" attribute='no'";
	if (optional) status = " status='optional'";
	if (mandatory) status = " status='mandatory'";

	switch (st.type())
	{
		case types::VariantStruct::null_:
			break;
		case types::VariantStruct::unresolved_:
		{
			out << indent << "<element";
			if (name) out << " name='" << name << "'";
			out << " class='indirection'";
			out << " type='" << types::VariantStruct::typeName(st.type()) << "'";
			out << " symbol='" << st.unresolvedName() << "'/>" << std::endl;
			break;
		}
		case types::VariantStruct::bool_:
		case types::VariantStruct::double_:
		case types::VariantStruct::int_:
		case types::VariantStruct::uint_:
		case types::VariantStruct::string_:
		{
			out << indent << "<element";
			if (name) out << " name='" << name << "'";
			out << " class='atomic'";
			out << " type='" << types::VariantStruct::typeName(st.type()) << "'";
			out << " value='" << st.tostring() << "'";
			out << addressing << status << "/>" << std::endl;
			break;
		}
		case types::VariantStruct::array_:
		{
			out << indent << "<element";
			if (name) out << " name='" << name << "'";
			out << " class='vector'";
			out << addressing << status << ">" << std::endl;
			printStructXML( out, name, *st.prototype(), isAttribute, indent+INDENTTAB, false, false);
			out << indent << "</element>" << std::endl;
			break;
		}
		case types::VariantStruct::struct_:
		{
			if (name)
			{
				out << indent << "<element";
				out << " name='" << name << "'";
				out << " class='struct'";
				out << " size='" << st.nof_elements() << "'" << addressing << status << ">" << std::endl;
			}
			types::VariantStruct::const_iterator ii = st.begin(), ee = st.end();
			types::VariantStructDescription::const_iterator di = st.description()->begin();
			std::size_t idx;
			for (idx=0; ii != ee; ++di,++ii,++idx)
			{
				printStructXML( out, di->name, *ii, di->attribute(), indent+INDENTTAB, di->optional(), di->mandatory());
			}
			if (name) out << indent << "</element>" << std::endl;
			break;
		}
		case types::VariantStruct::indirection_:
			out << indent << "<element";
			if (name) out << " name='" << name << "'";
			out << " class='indirection'";
			out << " type='" << types::VariantStruct::typeName( st.prototype()->type()) << "'" << addressing << status << "/>" << std::endl;
			break;
	}
}

static void printFormXML( std::ostream& out, const types::Form& form)
{
	out << "<form name='" << form.description()->name() << "' " << "ddl='" << form.description()->ddlname() << "'";
	if (form.description()->xmlRoot()) out << " xmlroot='" << form.description()->xmlRoot() << "'";
	out << ">" << std::endl;
	printStructXML( out, 0, form, false, "", false, false);
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
			std::cerr << APP_MAJOR_VERSION << "." << APP_MINOR_VERSION << "." << APP_REVISION << "." << APP_BUILD << std::endl;
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
		if (!providerconf.parse( (const config::ConfigurationTree&)cmdline.providerconfig(), "", &modDir))
		{
			throw std::runtime_error( "Processor provider configuration could not be created from command line");
		}
		providerconf.setCanonicalPathes( cmdline.referencePath());
		if (!providerconf.check())
		{
			throw std::runtime_error( "error in command line. failed to setup a valid processor provider configuration");
		}

		prgbind::ProgramLibrary programLibrary;
		proc::ProcessorProvider processorProvider( &providerconf, &modDir, &programLibrary);

		if (!processorProvider.loadPrograms())
		{
			throw std::runtime_error( "Not all programs could be loaded. See log." );
		}

		// Output:
		printWizardXML( std::cout, programLibrary);
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

