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
///\file utils/fileUtils.hpp
///\brief Utility functions for file handling

#ifndef _WOLFRAME_FILE_UTILS_HPP_INCLUDED
#define _WOLFRAME_FILE_UTILS_HPP_INCLUDED
#include <string>
#include <vector>
#include <boost/property_tree/ptree.hpp>

namespace _Wolframe {
namespace utils {

///\brief Remove .. and . from a path
///\param[in] path Path to process
std::string resolvePath( const std::string& path );

///\brief Get the extension of a file
///\param[in] path The path to the file to get the extension from
///\return The file extension including the extension marking dot '.'
std::string getFileExtension( const std::string& path);

///\brief Get the stem of a file
///\param[in] path The path to the file to get the stem from
///\return The file stem without the extension marking dot '.'
std::string getFileStem( const std::string& path);

///\brief Get the path 'path' relative to 'refpath' if it is not absolute
///\param[in] path relative part of the path or absolute path
///\param[in] refPath absolute part of the path if 'path' is relative
///\return the canonical path
std::string getCanonicalPath( const std::string& path, const std::string& refPath);

///\brief Get the parent path of 'path' (skipping /./ as parent path unlike boost::filesystem does)
///\param[in] path the path to get the parent path from
///\param[in] levels how many level (1 = parent, 2 = granparent, etc.)
///\return the parent path
std::string getParentPath( const std::string& path, unsigned int levels=1);

///\brief Join path with an item of the path to a combined path
///\param[in] path the main path 
///\param[in] item the sub path to join to path
///\return the joined path
std::string joinPath( const std::string& path, const std::string& item);

///\brief Returns true, if the file with path 'path' exists
bool fileExists( const std::string& path);

///\brief Reading the content of a source file as one string
///\remark Throws on error
std::string readSourceFileContent( const std::string& filename);

///\brief Reading the content of a source file as list of lines
///\remark Throws on error
std::vector<std::string> readSourceFileLines( const std::string& filename);

///\brief Reading the content of a source file as property tree (with property tree syntax)
///\remark Throws on error
boost::property_tree::ptree readPropertyTreeFile( const std::string& filename);

///\brief Writing a file
///\remark Throws on error
void writeFile( const std::string& filename, const std::string& content);

///\brief Get the file type
///\return the file type
struct FileType
{
	enum Encoding {Undefined,UCS1,UCS2BE,UCS2LE,UCS4BE,UCS4LE};
	enum Format {Unknown,XML,Info};

	Encoding encoding;
	Format format;

	FileType()
		:encoding(Undefined),format(Unknown){}
	FileType( const FileType& o)
		:encoding(o.encoding),format(o.format){}
	FileType( Encoding encoding_, Format format_)
		:encoding(encoding_),format(format_){}
};
FileType getFileType( const std::string& filename);

}} //namespace _Wolframe::utils

#endif // _MISC_UTILS_HPP_INCLUDED

