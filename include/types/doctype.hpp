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
///\file types/doctype.hpp
///\brief Type for document type definition handling

#ifndef _Wolframe_TYPES_DOCTYPE_HPP_INCLUDED
#define _Wolframe_TYPES_DOCTYPE_HPP_INCLUDED
#include <string>
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace types {

///\class DocType
///\brief Document type representation.
///\remark Inspired by XML but acting as superset of all document type descriptions.
struct DocType
{
	///\class SchemaPath
	///\brief Elements needed besides document type id to build the URI of the document schema description
	struct SchemaPath
	{
		std::string dir;	//< location directory of the schema descriptions
		std::string ext;	//< file extension of the schema descriptions

		SchemaPath( const SchemaPath& o)
			:dir(o.dir),ext(o.ext){}
		SchemaPath( const std::string& dir_, const std::string& ext_)
			:dir(dir_),ext(ext_){}
		SchemaPath(){}

		void clear()
		{
			dir.clear();
			ext.clear();
		}
	};

	std::string id;			//< document type identifier
	std::string root;		//< document root element if defined
	SchemaPath schemapath;		//< schema validation path description

	DocType(){}
	DocType( const std::string& id_, const std::string root_, const SchemaPath& schemapath_)
		:id(id_),root(root_),schemapath(schemapath_){}
	DocType( const DocType& o)
		:id(o.id),root(o.root),schemapath(o.schemapath){}
	~DocType(){}

	void clear()
	{
		id.clear();
		root.clear();
		schemapath.clear();
	}

	void init( const std::string& id_, const std::string& root_, const SchemaPath& schemapath_)
	{
		id = id_;
		root = root_;
		schemapath = schemapath_;
	}

	bool defined() const		{return !id.empty() && !root.empty();}
	std::string schemaURL() const	{return schemapath.dir + id + schemapath.ext;}
};

}}//namespace
#endif


