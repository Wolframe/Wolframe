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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
///\file fileUtils.cpp
///\brief Some utility functions for handling files

#include "utils/fileUtils.hpp"
#include "utils/parseUtils.hpp"
#include <cstring>
#include <cstdio>
#include <sstream>
#include <string>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/info_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/thread/thread.hpp>
#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

using namespace _Wolframe;
using namespace _Wolframe::utils;

std::string utils::joinPath( const std::string& path, const std::string& item)
{
	boost::filesystem::path rt( path);
	rt /= item;
	return rt.string();
}

std::string utils::resolvePath( const std::string& path )
{
	boost::filesystem::path result;
	boost::filesystem::path	p( path );

	for ( boost::filesystem::path::iterator it = p.begin(); it != p.end(); ++it )	{
		if ( *it == ".." )	{
			// /a/b/.. is not necessarily /a.. if b is a symbolic link
			if ( boost::filesystem::is_symlink( result ) )
				result /= *it;
			// /a/b/../.. is not /a/b/.. under most circumstances
			// We can end up with ..s in our result because of symbolic links
			else if( result.filename() == ".." )
				result /= *it;
			// Otherwise it should be safe to resolve the parent
			else
				result = result.parent_path();
		}
		else if( *it == "." )	{
			// Ignore
		}
		else {
			// Just cat other path entries
			result /= *it;
		}
	}
	return result.string();
}

std::string _Wolframe::utils::getFileExtension( const std::string& path)
{
	boost::filesystem::path p(path);
	std::string rt = p.extension().string();
	return rt;
}

std::string _Wolframe::utils::getFileStem( const std::string& path)
{
	boost::filesystem::path p(path);
	std::string rt = p.stem().string();
	return rt;
}

std::string _Wolframe::utils::getCanonicalPath( const std::string& path, const std::string& refPath )
{
	boost::filesystem::path pt( path );
	if ( pt.is_absolute() )
		return resolvePath( pt.string() );
	else
		return resolvePath( boost::filesystem::absolute( pt,
								 boost::filesystem::path( refPath ) ).string() );
}

bool _Wolframe::utils::fileExists( const std::string& path)
{
	try
	{
		boost::filesystem::path pt( path);
		return boost::filesystem::exists( pt);
	}
	catch (const std::exception&)
	{
		return false;
	}
}

std::string _Wolframe::utils::getParentPath( const std::string& path, unsigned int levels)
{
	boost::filesystem::path pt( path);
	if (!pt.is_absolute())
	{
		pt = boost::filesystem::absolute( pt, boost::filesystem::current_path()).string();
	}
	pt = boost::filesystem::path( resolvePath( pt.string()));

	while (levels > 0)
	{
		pt = pt.parent_path();
		--levels;
	}
	return pt.string();
}

static void readFileContent( const std::string& filename, std::string& res)
{
	unsigned char ch;
	FILE* fh = fopen( filename.c_str(), "r");
	if (!fh)
	{
		throw std::runtime_error( std::string( "failed (errno " + boost::lexical_cast<std::string>(errno) + ") to open file ") + filename + "' for reading");
	}
	boost::shared_ptr<FILE> fhr( fh, fclose);
	while (1 == fread( &ch, 1, 1, fh))
	{
		res.push_back( ch);
	}
	if (!feof( fh))
	{
		int ec = ferror( fh);
		if (ec) throw std::runtime_error( std::string( "failed to read (errno " + boost::lexical_cast<std::string>(ec) + ") from file ") + filename + "'");
	}
}

static void readSourceFileLines_( const std::string& filename, std::vector<std::string>& res)
{
	unsigned char ch;
	std::string ln;
	FILE* fh = fopen( filename.c_str(), "r");
	if (!fh)
	{
		throw std::runtime_error( std::string( "failed (errno " + boost::lexical_cast<std::string>(errno) + ") to open file ") + filename + "' for reading");
	}
	boost::shared_ptr<FILE> fhr( fh, fclose);
	while (1 == fread( &ch, 1, 1, fh))
	{
		if (ch == '\n')
		{
			res.push_back( ln);
			ln.clear();
		}
		else
		{
			ln.push_back( ch);
		}
	}
	if (!feof( fh))
	{
		int ec = ferror( fh);
		if (ec) throw std::runtime_error( std::string( "failed to read (errno " + boost::lexical_cast<std::string>(ec) + ") from file ") + filename + "'");
	}
}

void utils::writeFile( const std::string& filename, const std::string& content)
{
	unsigned char ch;
	FILE* fh = fopen( filename.c_str(), "w");
	if (!fh)
	{
		throw std::runtime_error( std::string( "failed (errno " + boost::lexical_cast<std::string>(errno) + ") to open file ") + filename + "' for reading");
	}
	boost::shared_ptr<FILE> fhr( fh, fclose);
	std::string::const_iterator fi = content.begin(), fe = content.end();
	for (; fi != fe; ++fi)
	{
		ch = *fi;
		if (1 > fwrite( &ch, 1, 1, fh))
		{
			int ec = ferror( fh);
			if (ec) throw std::runtime_error( std::string( "failed to read (errno " + boost::lexical_cast<std::string>(ec) + ") from file ") + filename + "'");
		}
	}
}

std::string utils::readSourceFileContent( const std::string& filename)
{
	std::string rt;
	readFileContent( filename, rt);
	return rt;
}

std::vector<std::string> utils::readSourceFileLines( const std::string& filename)
{
	std::vector<std::string> rt;
	readSourceFileLines_( filename, rt);
	return rt;
}


namespace {
struct FileTypeDetection
{
	static bool skipPattern( const unsigned char* pt, std::string::const_iterator& si, std::string::const_iterator se)
	{
		std::string::const_iterator start = si;
		std::size_t pi = 1;
		std::size_t pe = pt[0]+1;
		while (pi < pe && si != se && *si == (char)pt[pi])
		{
			++si;
			++pi;
		}
		if (pi < pe)
		{
			si = start;
			return false;
		}
		return true;
	}

	static char nextAsciiChar( std::string::const_iterator& si, std::string::const_iterator se, FileType::Encoding enc)
	{
		std::string::const_iterator start = si;
		char rt = 0;

		switch (enc)
		{
			case FileType::Undefined: goto FAILED;
			case FileType::UCS1:
			{
				if (si == se || (unsigned char)*si > 127) goto FAILED;
				return *si++;
			}
			case FileType::UCS2BE:
			{
				if (si == se || *si != 0) goto FAILED;
				++si;
				if (si == se || (unsigned char)*si > 127) goto FAILED;
				return *si++;
			}
			case FileType::UCS2LE:
			{
				if (si == se) goto FAILED;
				rt = *si;
				++si;
				if (si == se || (unsigned char)*si != 0) goto FAILED;
				++si;
				if ((unsigned char)rt > 127) goto FAILED;
				return rt;
			}
			case FileType::UCS4BE:
			{
				if (si == se || *si != 0) goto FAILED;
				++si;
				if (si == se || *si != 0) goto FAILED;
				++si;
				if (si == se || *si != 0) goto FAILED;
				++si;
				if (si == se || (unsigned char)*si > 127) goto FAILED;
				return *si++;
			}
			case FileType::UCS4LE:
			{
				if (si == se) goto FAILED;
				rt = *si;
				++si;
				if (si == se || (unsigned char)*si != 0) goto FAILED;
				++si;
				if (si == se || (unsigned char)*si != 0) goto FAILED;
				++si;
				if (si == se || (unsigned char)*si != 0) goto FAILED;
				++si;
				if ((unsigned char)rt > 127) goto FAILED;
				return rt;
			}
		}
	FAILED:
		si = start;
		return 0;
	}

	static char skipChar( std::string::const_iterator& si, std::string::const_iterator se, FileType::Encoding enc, const CharTable& chtab)
	{
		std::string::const_iterator start = si;
		char rt = 0;
		while ((rt = nextAsciiChar(si,se,enc)) != 0)
		{
			if (!chtab[rt]) return rt;
		}
		si = start;
		return 0;
	}

	static std::size_t countZeros( std::string::const_iterator si, std::string::const_iterator se)
	{
		std::size_t rt = 0;
		while (rt <= 4 && si != se && !*si)
		{
			++rt;
			++si;
		}
		return rt;
	}
};
}


FileType utils::getFileType( const std::string& filename)
{
	std::string source;
	readFileContent( filename, source);

	// Source: http://en.wikipedia.org/wiki/Byte_order_mark
	static const unsigned char pt_BOM_UTF8[]  = {3, 0xEF, 0xBB, 0xBF};
	static const unsigned char pt_BOM_UCS2BE[] = {2, 0xFE,0xFF};
	static const unsigned char pt_BOM_UCS2LE[] = {2, 0xFF,0xFE};
	static const unsigned char pt_BOM_UCS4BE[] = {4, 0x00,0x00,0xFE,0xFF};
	static const unsigned char pt_BOM_UCS4LE[] = {4, 0xFF,0xFE,0x00,0x00};
	static const unsigned char pt_UCS1_XML[]   = {1, '<'};
	static const unsigned char pt_UCS2BE_XML[] = {2, 0x00,'<'};
	static const unsigned char pt_UCS2LE_XML[] = {2, '<',0x00};
	static const unsigned char pt_UCS4BE_XML[] = {4, 0x00,0x00,0x00,'<'};
	static const unsigned char pt_UCS4LE_XML[] = {4, '<',0x00,0x00,0x00};
	static const CharTable xmlTagCharTab( "a..zA..Z0..9=_-\"\' ?!");

	std::string::const_iterator si = source.begin(), se = source.end();
	if (si == se) return FileType( FileType::Undefined, FileType::SourceText);

	FileType rt;
	if (FileTypeDetection::skipPattern( pt_BOM_UTF8, si, se)) rt.encoding = FileType::UCS1;
	else if (FileTypeDetection::skipPattern( pt_BOM_UCS2BE, si, se)) rt.encoding = FileType::UCS2BE;
	else if (FileTypeDetection::skipPattern( pt_BOM_UCS2LE, si, se)) rt.encoding = FileType::UCS2LE;
	else if (FileTypeDetection::skipPattern( pt_BOM_UCS4BE, si, se)) rt.encoding = FileType::UCS4BE;
	else if (FileTypeDetection::skipPattern( pt_BOM_UCS4LE, si, se)) rt.encoding = FileType::UCS4LE;

	if (rt.encoding == FileType::Undefined)
	{
		if (FileTypeDetection::skipPattern( pt_UCS1_XML, si, se))
		{
			rt.encoding = FileType::UCS1;
			rt.format = FileType::XML;
		}
		else if (FileTypeDetection::skipPattern( pt_UCS2BE_XML, si, se))
		{
			rt.encoding = FileType::UCS2BE;
			rt.format = FileType::XML;
		}
		else if (FileTypeDetection::skipPattern( pt_UCS2LE_XML, si, se))
		{
			rt.encoding = FileType::UCS2LE;
			rt.format = FileType::XML;
		}
		else if (FileTypeDetection::skipPattern( pt_UCS4BE_XML, si, se))
		{
			rt.encoding = FileType::UCS4BE;
			rt.format = FileType::XML;
		}
		else if (FileTypeDetection::skipPattern( pt_UCS4LE_XML, si, se))
		{
			rt.encoding = FileType::UCS4LE;
			rt.format = FileType::XML;
		}
	}

	if (rt.format == FileType::XML)
	{
		char ch = FileTypeDetection::skipChar( si, se, rt.encoding, xmlTagCharTab);
		if (ch != '>')
		{
			return FileType( rt.encoding, FileType::SourceText);
		}
		return rt;
	}
	if (rt.encoding == FileType::Undefined)
	{
		if (si != se && *si == 0)
		{
			++si;
			std::size_t zc = FileTypeDetection::countZeros( si, se);
			switch (zc)
			{
				case 0: return FileType( FileType::UCS2BE, FileType::SourceText);
				case 2: return FileType( FileType::UCS4BE, FileType::SourceText);
				default: break;
			}
		}
		else
		{
			++si;
			std::size_t zc = FileTypeDetection::countZeros( si, se);
			switch (zc)
			{
				case 0: return FileType( FileType::UCS1, FileType::SourceText);
				case 1: return FileType( FileType::UCS2LE, FileType::SourceText);
				case 3: return FileType( FileType::UCS4LE, FileType::SourceText);
				default: break;
			}
		}
	}
	return FileType( FileType::Undefined, FileType::SourceText);
}


boost::property_tree::ptree utils::readPropertyTreeFile( const std::string& filename)
{
	FileType filetype = getFileType( filename);
	boost::property_tree::ptree rt;

	switch (filetype.format)
	{
		case FileType::XML:
		{
			if (filetype.encoding == FileType::Undefined)
			{
				throw std::runtime_error( std::string( "cannot handle encoding of file as info file '") + filename + "' (encoding is unknown)");
			}
			namespace opt = boost::property_tree::xml_parser;
			read_xml( filename, rt, opt::no_comments | opt::trim_whitespace);
			break;
		}
		case FileType::SourceText:
		{
			if (filetype.encoding != FileType::UCS1)
			{
				throw std::runtime_error( std::string( "cannot handle encoding of file as info file '") + filename + "' (encoding is not UTF-8)");
			}
			read_info( filename, rt);
			break;
		}
		default:
		{
			throw std::runtime_error( std::string( "file type not recognized for '") + filename + "'");
		}
	}
	return rt;
}

