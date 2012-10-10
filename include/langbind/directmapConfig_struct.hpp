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
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
///\file langbind/directmapConfig_struct.hpp
///\brief Data structures of the configuration of a directmap command handler
#ifndef _Wolframe_DIRECTMAP_CONFIGURATION_STRUCT_HPP_INCLUDED
#define _Wolframe_DIRECTMAP_CONFIGURATION_STRUCT_HPP_INCLUDED
#include "serialize/struct/filtermapBase.hpp"
#include "config/ConfigurationTree.hpp"
#include <vector>
#include <string>
#include <boost/property_tree/ptree.hpp>

namespace _Wolframe {
namespace langbind {

struct DirectmapCommandConfigStruct
{
	std::string name;						//< (optional) name of the command. default is the value of call
	std::string call;						//< name of the transaction or form function
	std::string filter;						//< name of the input/ouput filter
	std::string inputform;						//< name of the input form
	std::string outputform;						//< name of the output form

	///\brief Get the configuration structure description
	static const serialize::StructDescriptionBase* getStructDescription();
};


struct DirectmapConfigStruct
{
	std::vector<DirectmapCommandConfigStruct> command;		//< definitions of directmap commands

	///\brief Get the configuration structure description
	static const serialize::StructDescriptionBase* getStructDescription();

	void setCanonicalPathes( const std::string& referencePath);

	boost::property_tree::ptree toPropertyTree() const;
	void initFromPropertyTree( const boost::property_tree::ptree& pt);
	void initFromPropertyTree( const config::ConfigurationTree& pt);

	void print( std::ostream& os, size_t indent) const;

	bool check() const;
};

}}//namespace
#endif


