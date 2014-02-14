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
///
/// \file moduleDirectory.hpp
///

#ifndef _MODULE_DIRECTORY_HPP_INCLUDED
#define _MODULE_DIRECTORY_HPP_INCLUDED

#include <string>
#include <list>
#include "processor/moduleInterface.hpp"

namespace _Wolframe {
namespace module {

/// The modules directory.
/// This directory is used by the constructors of the providers to build themselves.
class ModulesDirectory
{
public:
	ModulesDirectory()				{}
	~ModulesDirectory();

	/// Add a configured builder to the directory.
	bool addBuilder( ConfiguredBuilder* builder );

	/// Add a simple builder (builder for objects without configuration) to the directory.
	bool addBuilder( SimpleBuilder* builder );

	/// Get the builder for the configuration section, keyword pair.
	///\param section	the section (parent) of the configuration
	///\param keyword	the keyword in the section
	ConfiguredBuilder* getBuilder( const std::string& section, const std::string& keyword ) const;

	/// Get the builder for the specified object class name
	ConfiguredBuilder* getBuilder( const std::string& objectClassName ) const;

	/// Const iterator for the simple builders in the directory
	class simpleBuilder_iterator
	{
		friend class ModulesDirectory;
	public:
		simpleBuilder_iterator()		{}
		simpleBuilder_iterator( const simpleBuilder_iterator& it )
			: m_it( it.m_it )		{}

		SimpleBuilder* operator->() const	{ return *m_it; }
		SimpleBuilder* operator*() const	{ return *m_it; }
		simpleBuilder_iterator& operator++()	{ ++m_it; return *this; }
		simpleBuilder_iterator operator++( int ){ simpleBuilder_iterator rtrn( *this ); ++m_it; return rtrn; }
		bool operator == ( const simpleBuilder_iterator& rhs )
							{ return m_it == rhs.m_it; }
		bool operator != ( const simpleBuilder_iterator& rhs )
							{ return m_it != rhs.m_it; }

	private:
		std::list< SimpleBuilder* >::const_iterator	m_it;

		simpleBuilder_iterator( const std::list< SimpleBuilder* >::const_iterator& it )
			: m_it( it )			{}
	};

	simpleBuilder_iterator objectsBegin() const	{ return simpleBuilder_iterator( m_simpleBuilder.begin() ); }
	simpleBuilder_iterator objectsEnd() const	{ return simpleBuilder_iterator( m_simpleBuilder.end() ); }

	///\brief Get a selected list of configurable objects loaded as tuple (section,keyword)
	///\remark This function solves a henn and egg problem for the wolfilter program.
	///\param[in] objtype type of the configurable objects to select
	///\return The list of section,keyword tuples of configurable objects with the selected type
	std::vector<std::pair<std::string,std::string> > getConfigurableSectionKeywords( ObjectConstructorBase::ObjectType objtype ) const;

private:
	std::list< ConfiguredBuilder* >	m_cfgdBuilder;	///< list of configurable builders
	std::list< SimpleBuilder* >	m_simpleBuilder;///< list of simple builders
};


bool LoadModules( ModulesDirectory& modDir, const std::list< std::string >& modFiles,
		  const std::string& modFolder = "" );


}} // namespace _Wolframe::module

#endif // _MODULE_DIRECTORY_HPP_INCLUDED
