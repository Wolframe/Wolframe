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
//\file processor/procProviderInterface.hpp
//\brief Interface to processor provider for language bindings and database
#ifndef _PROCESSOR_PROVIDER_INTERFACE_HPP_INCLUDED
#define _PROCESSOR_PROVIDER_INTERFACE_HPP_INCLUDED
#include "filter/filter.hpp"
#include "langbind/formFunction.hpp"
#include <string>
#include <vector>
namespace _Wolframe {
namespace db
{
//\brief Forward declaration
class Transaction;
//\brief Forward declaration
class Database;
}
namespace types
{
//\brief Forward declaration
class Form;
//\brief Forward declaration
class FormDescription;
//\brief Forward declaration
class NormalizeFunction;
//\brief Forward declaration
class NormalizeFunctionType;
}
namespace cmdbind
{
//\brief Forward declaration
class CommandHandler;
//\brief Forward declaration
class IOFilterCommandHandler;
}
namespace UI
{
class UserInterfaceLibrary;
}

namespace proc {

//\class ProcessorProviderInterface
//\brief Abstract class as processor provider interface
class ProcessorProviderInterface
{
public:
	virtual ~ProcessorProviderInterface(){};
	virtual const UI::UserInterfaceLibrary* UIlibrary() const=0;
	virtual cmdbind::CommandHandler* cmdhandler( const std::string& command) const=0;
	virtual cmdbind::IOFilterCommandHandler* iofilterhandler( const std::string& command) const=0;
	virtual std::string xmlDoctypeString( const std::string& formname, const std::string& ddlname, const std::string& xmlroot) const=0;
	virtual db::Database* transactionDatabase() const=0;
	///\brief return a database transaction object for the given name
	virtual db::Transaction* transaction( const std::string& name) const=0;
	virtual const types::NormalizeFunction* normalizeFunction( const std::string& name) const=0;
	virtual const types::NormalizeFunctionType* normalizeFunctionType( const std::string& name) const=0;
	virtual const langbind::FormFunction* formFunction( const std::string& name) const=0;
	virtual const types::FormDescription* formDescription( const std::string& name) const=0;
	virtual langbind::Filter* filter( const std::string& name, const std::vector<langbind::FilterArgument>& arg=std::vector<langbind::FilterArgument>()) const=0;
	virtual const types::CustomDataType* customDataType( const std::string& name) const=0;
};

}}//namespace
#endif

