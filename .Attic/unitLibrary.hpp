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
///
///\brief Unit function library
///\file unitLibrary.hpp
///

#ifndef _UNIT_FUNCTION_LIBRARY_HPP_INCLUDED
#define _UNIT_FUNCTION_LIBRARY_HPP_INCLUDED

#include <string>
#include <map>
#include <vector>
#include <boost/algorithm/string.hpp>

namespace _Wolframe {

///\brief Template for program unit mechs
template < typename Function >
class UnitLibrary
{
public:
	UnitLibrary()		{}
	~UnitLibrary()		{}

	/// \brief Add a new function to the directory.
	bool addFunction( const Function& func, const std::string& name )
	{
		std::string upperName = boost::algorithm::to_upper_copy( name );
		std::map< std::string, std::size_t >::const_iterator it = m_funcMap.find( upperName );
		if ( it != m_funcMap.end() )
			return false;

		m_funcMap[ upperName ] = m_functions.size();
		m_functions.push_back( func );
		return true;
	}

	/// \brief Get a function from the library. Function names are
	/// case insensitive.
	/// \param [in]	name	the name of the function
	/// \return	pointer to function or NULL if not found
	const Function* function( const std::string& name ) const
	{
		std::string upperName = boost::algorithm::to_upper_copy( name );
		std::map< std::string, std::size_t >::const_iterator it = m_funcMap.find( upperName );
		if ( it != m_funcMap.end() )
			return &m_functions[ it->second ];
		else
			return NULL;
	}

	/// \return The size (number of functions) in the library.
	std::size_t size() const			{ return m_funcMap.size(); }

	/// Const iterator for the functions map
	class const_iterator
	{
	public:
		const_iterator()			{}
		const_iterator( const const_iterator& it )
			: m_it( it.m_it )		{}

		const Function* operator->() const	{ return *m_it; }
		const Function* operator*() const	{ return *m_it; }
		const_iterator& operator++()		{ ++m_it; return *this; }
		const_iterator operator++( int )	{ const_iterator rtrn( *this );
							  ++m_it; return rtrn; }
		bool operator == ( const const_iterator& rhs )
							{ return m_it == rhs.m_it; }
		bool operator != ( const const_iterator& rhs )
							{ return m_it != rhs.m_it; }

	private:
		std::map< std::string, std::size_t >::const_iterator	m_it;

		const_iterator( const std::map< std::string, std::size_t >::const_iterator& it )
			: m_it( it )			{}
	};

	const const_iterator begin() const	{ return const_iterator( m_funcMap.begin() ); }
	const const_iterator end() const	{ return const_iterator( m_funcMap.end() ); }
private:
	std::vector< Function >			m_functions;
	std::map< std::string, std::size_t >	m_funcMap;
};

} // namespace _Wolframe

#endif // _UNIT_FUNCTION_LIBRARY_HPP_INCLUDED

