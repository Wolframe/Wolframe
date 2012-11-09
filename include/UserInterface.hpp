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
///
/// \file UserInterface.hpp
///

#ifndef _USER_INTERFACE_HPP_INCLUDED
#define _USER_INTERFACE_HPP_INCLUDED

#include <string>
#include <list>
#include "version.hpp"

namespace _Wolframe {
namespace UI {

/// User Interface form.
class UIform
{
public:
	/// Types of forms
	enum FormType	{
		FORM,
		MENU
	};

	/// User Interface form information.
	class Info
	{
	public:
		Info( FormType type_, const std::string& platform_,
		      const std::string& name_, const std::string& culture_,
		      unsigned long version_, const std::string& description_ )
			: m_type( type_ ), m_platform( platform_ ),
			  m_name( name_ ), m_culture( culture_ ),
			  m_version( version_ ), m_description( description_ )	{}

		FormType type() const			{ return m_type; }
		const std::string& name() const		{ return m_name; }
		const std::string& platform() const	{ return m_platform; }
		const std::string& culture() const	{ return m_culture; }
		const Version& version() const		{ return m_version; }
		const std::string& description() const	{ return m_description; }
	private:
		const FormType		m_type;		///< the type of the form
		const std::string	m_platform;	///< the platform of the form
		const std::string	m_name;		///< the name of the form
		const std::string	m_culture;	///< the culture of the form
		const Version		m_version;	///< the version of the form
		const std::string	m_description;	///< the description of the form
	};

	/// Constructor
	UIform( FormType type_, const std::string& platform_,
		const std::string& name_, const std::string& culture_,
		unsigned long version_, const std::string& description_,
		const std::string& body_ )
		: m_info( type_, platform_, name_, culture_, version_, description_ ),
		  m_body ( body_ )		{}

	const Info& header() const			{ return m_info; }
	const std::string& body() const			{ return m_body; }
private:
	const Info		m_info;			///< the header of the form
	const std::string	m_body;			///< the body of the form
};


/// Library of user interface elements.
/// This is an interface (base class).
class UserInterfaceLibrary
{
public:
	virtual ~UserInterfaceLibrary()	{}

	/// \brief Get a list of all form infos, latest versions
	/// \param platform	the platform for which to get the form info
	///			use an epmty string, "*" or "all"
	///			to get the form info for all platforms
	/// \param culture	the culture for which to get the form info
	///			use an epmty string, "*" or "all"
	///			to get the form info for all cultures
	virtual const std::list< UIform::Info > formInfos( const std::string& platform,
							   const std::string& culture = "" ) const = 0;

	/// \brief Get a list of all form infos, latest versions
	/// \param platform	the platform for which to get the form info
	/// \param role		a role for which to get the form infos
	/// \param culture	the culture for which to get the form info
	virtual const std::list< UIform::Info > formInfos( const std::string& platform,
							   const std::string& role,
							   const std::string& culture = "" ) const = 0;

	/// \brief Get a list of all form infos, latest versions
	/// \param platform	the platform for which to get the form info
	/// \param roles	a list of roles for which to get the form infos
	/// \param culture	the culture for which to get the form info
	virtual const std::list< UIform::Info > formInfos( const std::string& platform,
							   const std::list< std::string >& roles,
							   const std::string& culture = "" ) const = 0;

	/// \brief Get a list of form infos for all the versions of a form
	/// \param platform	the platform for which to get the form info
	/// \param name		the name of the form
	/// \param culture	the culture for which to get the form info
	virtual const std::list< UIform::Info > formVersions( const std::string& platform,
							      const std::string& name,
							      const std::string& culture = "" ) const = 0;

	/// \brief Get the form
	/// \param platform	the platform for which to get the form info
	/// \param name		the name of the form
	/// \param culture	the culture for which to get the form info
	virtual const UIform form( const std::string& platform,
				   const std::string& name,
				   const std::string& culture = "" ) const = 0;

	/// \brief Get the form
	/// \param platform	the platform for which to get the form info
	/// \param name		the name of the form
	/// \param version	the version of the form
	/// \param culture	the culture for which to get the form info
	virtual const UIform form( const std::string& platform,
				   const std::string& name, const Version& version,
				   const std::string& culture = "" ) const = 0;

	/// \brief Get the form
	/// \param info		the form info
	virtual const UIform form( const UIform::Info& info ) const = 0;

	/// \brief Add a form
	/// \param info		the form info
	virtual void addForm( const UIform& newForm ) const = 0;

	/// \brief Delete a form
	/// \param info		the form info
	virtual bool deleteForm( const UIform::Info& info ) const = 0;

	virtual void close()		{}
};

}} // namespace _Wolframe::UI

#endif // _USER_INTERFACE_HPP_INCLUDED
