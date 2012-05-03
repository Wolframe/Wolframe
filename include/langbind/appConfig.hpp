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
///\file langbind/appConfig.hpp
///\brief Configuration of the processor environment
#ifndef _Wolframe_APPLICATION_ENVIRONMENT_CONFIGURATION_HPP_INCLUDED
#define _Wolframe_APPLICATION_ENVIRONMENT_CONFIGURATION_HPP_INCLUDED
#include <vector>
#include <string>
#include "cmdbind/commandHandler.hpp"
#include "langbind/appConfig_struct.hpp"
#include "config/descriptionBase.hpp"
#include "config/configurationBase.hpp"
#include "ddl/compilerInterface.hpp"

namespace _Wolframe {
namespace langbind {

///\brief application environment configuration
class ApplicationEnvironmentConfig : public config::ConfigurationBase
{
public:
	ApplicationEnvironmentConfig() : ConfigurationBase( "Application Environment", NULL, "Application environment configuration" )	{}
	~ApplicationEnvironmentConfig() {}

	bool parse( const config::ConfigurationTree& pt, const std::string& node, const module::ModulesDirectory* modules );
	bool check() const;
	void print( std::ostream& os, std::size_t indent) const;
	virtual void setCanonicalPathes( const std::string& referencePath );

	const EnvironmentConfigStruct& data() const				{return m_config;}
private:
	EnvironmentConfigStruct m_config;
};

}}//namespace
#endif


