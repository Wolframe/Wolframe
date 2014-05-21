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
///\file types/docmetadata.hpp
///\brief Structure representing document meta data

#ifndef _Wolframe_TYPES_DOC_METADATA_HPP_INCLUDED
#define _Wolframe_TYPES_DOC_METADATA_HPP_INCLUDED
#include <string>
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace types {

///\class DocMetaData
///\brief Document metadata representation.
struct DocMetaData
{
	struct Attribute
	{
		enum Id
		{
			RootElement,
			XmlNamespace,
			Xsi,
			SchemaLocation,
			DOCTYPE_SYSTEM,
			DOCTYPE_PUBLIC,
			Identifier
		};
		static const char name( Id id)
		{
			static const char* ar[] = {"root","xmlns","xsi","schema","system","public","doctype"};
		}
		static Id firstid()	{return (Id)0;}
		static Id lastid()	{return Identifier;}
		static Id nextid( Id i)	{return (Id)((int)i + 1);}

		Id id;
		std::string value;

		Attribute( const Id& n, const std::string& v)
			:id(n),value(v){}
	};

	DocMetaData( const DocMetaData& o);
	explicit DocMetaData( const std::vector<Attribute>& attributes_);
	DocMetaData();

	void clear();

	void init( const std::vector<Attribute>& attributes_);
	void join( const std::vector<Attribute>& attributes_);

	void setDoctype( const std::string& id_, const std::string& root_);
	void setAttribute( const Attribute& attr);
	void setAttribute( Attribute::Id id, const std::string& value);

	void deleteAttribute( Attribute::Id id);
	const char* getAttribute( Attribute::Id id) const;

	const char* root() const;
	const char* doctype() const;

	const std::vector<Attribute>& attributes() const
	{
		return m_attributes;
	}

private:
	std::vector<Attribute> m_attributes;
};

typedef boost::shared_ptr<DocMetaData> DocMetaDataR;

}}//namespace
#endif


