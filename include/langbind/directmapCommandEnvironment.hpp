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
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
///\file directmapCommandEnvironment.hpp
///\brief Static environment of a command as direct mapping from an input filter to a form, a transaction function and a mapping of the transaction output form through an output filter
#ifndef _langbind_DIRECTMAP_COMMAND_ENVIRONMENT_HPP_INCLUDED
#define _langbind_DIRECTMAP_COMMAND_ENVIRONMENT_HPP_INCLUDED
#include "ddl/structType.hpp"
#include "ddl/compilerInterface.hpp"
#include "protocol/commandHandler.hpp"
#include <string>
#include <vector>

namespace _Wolframe {
namespace langbind {

///\class Configuration
///\brief configuration object of a command as call to a lua script
class DirectmapCommandEnvironment :public protocol::CommandEnvironment
{
public:
	///\brief Constructor
	DirectmapCommandEnvironment( const ddl::CompilerInterface* ddlc, const std::string& in_path, const std::string& in_name, const std::string& out_path, const std::string& out_name)
		:m_inputformdescr(in_path,in_name,ddlc),m_outputformdescr(out_path,out_name,ddlc){}

	///\brief Interface implementation of ConfigurationBase::test() const
	virtual bool test() const;

	///\brief Interface implementation of ConfigurationBase::check() const
	virtual bool check() const;

	///\brief Interface implementation of ConfigurationBase::print(std::ostream& os, size_t indent) const
	virtual void print( std::ostream& out, size_t indent=0) const;

	const ddl::StructType& inputform() const	{return m_inputform;}
	const ddl::StructType& outputform() const	{return m_outputform;}

	///\brief Load all the environment
	///\return true on success
	bool load();

private:
	struct FormDescription
	{
		std::string m_path;
		std::string m_name;
		const ddl::CompilerInterface* m_compiler;

		FormDescription( const std::string& p, const std::string& n, const ddl::CompilerInterface* c)
			:m_path(p),m_name(n),m_compiler(c){}

		bool load( ddl::StructType& st) const;
	};

	FormDescription m_inputformdescr;		//< input form description
	FormDescription m_outputformdescr;		//< output form description
	ddl::StructType m_inputform;			//< input form prototype
	ddl::StructType m_outputform;			//< output form prototype
};

}}//namespace
#endif

