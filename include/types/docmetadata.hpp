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
	struct Attribute
	{
		std::string name;
		std::string value;

		Attribute(){}
		Attribute( const std::string& n, const std::string& v)
			:name(n),value(v){}
		Attribute( const Attribute& o)
			:name(o.name),value(o.value){}
	};

	DocMetaData( const DocMetaData& o);
	DocMetaData( const std::string& doctype_, const std::vector<Attribute>& attributes_);
	DocMetaData();

	void clear();

	void join( const std::vector<Attribute>& attributes_);

	void setDoctype( const std::string& doctype_)
	{
		m_doctype = doctype_;
	}

	/// \brief Set a unique attribute by name (add or redefine)
	void setAttribute( const Attribute& attr);
	/// \brief Set a unique attribute by name (add or redefine)
	void setAttribute( const std::string& name_, const std::string& value_);

	/// \brief Define a unique attribute and throw if attribute with this name is already defined
	void defineAttribute( const std::string& name_, const std::string& value_);

	/// \brief Delete an attribute identified by name
	bool deleteAttribute( const std::string& name_);

	/// \brief Get an attribute identified by name or NULL if not defined
	const char* getAttribute( const std::string& name_) const;

	const std::string& doctype() const
	{
		return m_doctype;
	}
	const std::vector<Attribute>& attributes() const
	{
		return m_attributes;
	}

	static std::string replaceStem( const std::string& src, const std::string& newstem);
	static std::string extractStem( const std::string& src);

	std::string tostring() const;

private:
	std::vector<Attribute> m_attributes;
	std::string m_doctype;
};

typedef boost::shared_ptr<DocMetaData> DocMetaDataR;

}}//namespace
#endif


