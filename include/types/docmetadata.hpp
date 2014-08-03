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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
/// \file types/docmetadata.hpp
/// \brief Structure representing document meta data

#ifndef _Wolframe_TYPES_DOC_METADATA_HPP_INCLUDED
#define _Wolframe_TYPES_DOC_METADATA_HPP_INCLUDED
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace types {

/// \class DocMetaData
/// \brief Document meta data representation.
struct DocMetaData
{
	/// \class Attribute
	/// \brief One document meta data attribute.
	struct Attribute
	{
		std::string name;	///< name of the attribute
		std::string value;	///< value of the attribute

		/// \brief Default constructor
		Attribute(){}
		/// \brief Constructor
		Attribute( const std::string& n, const std::string& v)
			:name(n),value(v){}
		/// \brief Copy constructor
		Attribute( const Attribute& o)
			:name(o.name),value(o.value){}
	};

	/// \brief Copy constructor
	DocMetaData( const DocMetaData& o);
	/// \brief Constructor
	/// \param[in] doctype_ the value of the document type
	/// \param[in] attributes_ the attributes to insert
	DocMetaData( const std::string& doctype_, const std::vector<Attribute>& attributes_);
	/// \brief Default constructor
	DocMetaData();

	/// \brief Clear (reset) content
	void clear();

	/// \brief Update or insert all attributes passed
	/// \param[in] attributes_ the attributes to update or insert
	void join( const std::vector<Attribute>& attributes_);

	/// \brief Set the document type
	/// \param[in] doctype_ the new value of the document type
	void setDoctype( const std::string& doctype_)
	{
		m_doctype = doctype_;
	}

	/// \brief Set a unique attribute by name (add or redefine)
	/// \param[in] attr the attribute to define
	void setAttribute( const Attribute& attr);

	/// \brief Set a unique attribute by name (add or redefine)
	/// \param[in] name_ the name of the attribute to define
	/// \param[in] value_ the value of the attribute to define
	void setAttribute( const std::string& name_, const std::string& value_);

	/// \brief Define a unique attribute and throw if attribute with this name is already defined
	/// \param[in] name_ the name of the attribute to define
	/// \param[in] value_ the value of the attribute to define
	void defineAttribute( const std::string& name_, const std::string& value_);

	/// \brief Delete an attribute identified by name
	/// \param[in] name_ the name of the attribute to delete
	bool deleteAttribute( const std::string& name_);

	/// \brief Get an attribute identified by name or NULL if not defined
	/// \param[in] name_ the name of the attribute to retrieve
	/// \return the attribute value or NULL if not defined
	const char* getAttribute( const std::string& name_) const;

	/// \brief Get the document type
	/// \return the document type
	const std::string& doctype() const
	{
		return m_doctype;
	}
	/// \brief Get the meta data attributes
	/// \return the meta data attributes
	const std::vector<Attribute>& attributes() const
	{
		return m_attributes;
	}

	/// \brief Replace the value between the last '/' or start of the string and the last '.' or the end of the string with a new value
	/// \param[in] src the string to substitute in
	/// \param[in] newstem the new value to substitute with
	/// \return the result string of the substitution
	static std::string replaceStem( const std::string& src, const std::string& newstem);
	/// \brief Extract the value between the last '/' or start of the string and the last '.' or the end of the string
	/// \param[in] src the string to extract from in
	/// \return the exracted string of the pattern matched
	static std::string extractStem( const std::string& src);

	/// \brief Get the meta data as string to log
	std::string tostring() const;

private:
	std::vector<Attribute> m_attributes;		///< meta data attributes
	std::string m_doctype;				///< document type
};

/// \brief Shared meta data reference
typedef boost::shared_ptr<DocMetaData> DocMetaDataR;

}}//namespace
#endif


