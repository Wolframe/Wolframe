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
///\brief Interface for a form library
///\file langbind/formLibrary.hpp
#ifndef _LANGBIND_FORM_LIBRARY_HPP_INCLUDED
#define _LANGBIND_FORM_LIBRARY_HPP_INCLUDED
#include "langbind/formLibrary.hpp"
#include "types/keymap.hpp"
#include "ddl/compilerInterface.hpp"
#include "ddl/structType.hpp"
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace langbind {

class FormLibrary
	:public types::keymap<ddl::FormR>
{
public:
	typedef types::keymap<ddl::FormR> Parent;

	FormLibrary(){}
	FormLibrary( const FormLibrary& o)
		:Parent(o)
		,m_constructormap(o.m_constructormap){}
	~FormLibrary(){}

	void setTypeMap( const ddl::TypeMapR& typemap_)
	{
		m_typemap = typemap_;
	}
	void addConstructor( const ddl::DDLCompilerR& constructor);

	bool is_mine( const std::string& filename) const;
	void addProgram( const std::string& source);
	void loadProgram( const std::string& filename);

	const ddl::Form* get( const std::string& name) const
	{
		Parent::const_iterator rt = Parent::find( name);
		return (rt == Parent::end())?0:rt->second.get();
	}

private:
	types::keymap<ddl::DDLCompilerR> m_constructormap;
	ddl::TypeMapR m_typemap;
};

}}
#endif


