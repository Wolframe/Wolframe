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
///\file fileUtils.cpp
///\brief Some utility functions for handling files

#include "utils/fileUtils.hpp"
#include "utils/parseUtils.hpp"
#include "types/string.hpp"
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
#if defined(_WIN32)
#define WIN32_MEAN_AND_LEAN
#include <windows.h>
#endif

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
#if defined(_WIN32)
	enum {BUFFERSIZE=8192};
	char readBuffer[ BUFFERSIZE+1];
	DWORD dwBytesRead = 0;
	BOOL success;
	struct Locals
	{
		HANDLE hFile;
		Locals()
		{
			hFile = INVALID_HANDLE_VALUE;
		}
		~Locals()
		{
			if (hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);
		}
	};
	Locals locals;

	locals.hFile = ::CreateFile( filename.c_str(), GENERIC_READ, FILE_SHARE_READ, 
				NULL/*default security*/, OPEN_EXISTING,
				FILE_ATTRIBUTE_NORMAL, NULL/*no attr. template*/);
	
	if (locals.hFile == INVALID_HANDLE_VALUE) 
	{
		unsigned int errcode = ::GetLastError();
		throw std::runtime_error( std::string("Failed to open file for reading [error code ") + boost::lexical_cast<std::string>(errcode) + "] file: " + filename);
	}

	while (TRUE == (success=::ReadFile( locals.hFile, readBuffer, BUFFERSIZE, &dwBytesRead, NULL)) && dwBytesRead > 0)
	{
		res.append( readBuffer, (std::size_t)dwBytesRead);
	}
	if (!success)
	{
		unsigned int errcode = ::GetLastError();
		throw std::runtime_error( std::string("Error reading from file [error code ") + boost::lexical_cast<std::string>(errcode) + "] file: " + filename);
	}
#else
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
#endif
}

void utils::writeFile( const std::string& filename, const std::string& content)
{
#if defined(_WIN32)
	DWORD dwBytesWritten;
	BOOL success;
	struct Locals
	{
		HANDLE hFile;
		Locals()
		{
			hFile = INVALID_HANDLE_VALUE;
		}
		~Locals()
		{
			if (hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);
		}
	};
	Locals locals;

	locals.hFile = ::CreateFile( filename.c_str(), GENERIC_WRITE, 0/*do not share*/, 
				NULL/*default security*/, CREATE_NEW,
				FILE_ATTRIBUTE_NORMAL, NULL/*no attr. template*/);
	
	if (locals.hFile == INVALID_HANDLE_VALUE) 
	{
		unsigned int errcode = ::GetLastError();
		throw std::runtime_error( std::string("Failed to open file for writing [error code ") + boost::lexical_cast<std::string>(errcode) + "] file: " + filename);
	}

	success = ::WriteFile( locals.hFile, content.c_str(), content.size(), &dwBytesWritten, NULL);
	if (!success)
	{
		unsigned int errcode = ::GetLastError();
		throw std::runtime_error( std::string("Error writing to file [error code ") + boost::lexical_cast<std::string>(errcode) + "] file: " + filename);
	}
	if (dwBytesWritten != content.size())
	{
		throw std::runtime_error( std::string("Error writing to file (incomplete) file: ") + filename);
	}
#else
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
			if (ec) throw std::runtime_error( std::string( "failed to write (errno " + boost::lexical_cast<std::string>(ec) + ") to file ") + filename + "'");
		}
	}
#endif
}

std::string utils::readSourceFileContent( const std::string& filename)
{
	std::string src;
	readFileContent( filename, src);
	switch (types::String::guessEncoding( src))
	{
		case types::String::EncodingClass::FAIL:
			throw std::runtime_error(std::string("failed to guess source file encoding of '") + filename + "'");
		case types::String::EncodingClass::NONE:
			return src;	//... empty file, assuming UTF-8 or Ascii
		case types::String::EncodingClass::UCS1:
			return src;	//... assuming UTF-8 or Ascii
		case types::String::EncodingClass::UCS2LE:
			return types::String( (const void*)src.c_str(), src.size(), types::String::UTF16LE).tostring();
		case types::String::EncodingClass::UCS2BE:
			return types::String( (const void*)src.c_str(), src.size(), types::String::UTF16BE).tostring();
		case types::String::EncodingClass::UCS4LE:
		case types::String::EncodingClass::UCS4BE:
			throw std::runtime_error(std::string("cannot parse source file in UCS4 type encoding: '") + filename + "'");
	}
	return src;
}


namespace {
struct FileTypeDetection
{
	static bool skipPattern( const unsigned char* pt, std::string::const_iterator& si, std::string::const_iterator se)
	{
		std::string::const_iterator start = si;
		std::size_t pi = 1;
		std::size_t pe = pt[0]+1;
		while (pi < pe && si != se && (*si == (char)pt[pi] || ((char)pt[pi] == ' ' && (*si == '\n' || *si == '\r' || *si == '\t'))))
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

	static std::size_t skipZeros( std::string::const_iterator& si, std::string::const_iterator se)
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
	static const unsigned char pt_BOM_UCS4BE[] = {4, 0x00,0x00,0xFE,0xFF};
	static const unsigned char pt_BOM_UCS4LE[] = {4, 0xFF,0xFE,0x00,0x00};
	static const unsigned char pt_BOM_UCS2BE[] = {2, 0xFE,0xFF};
	static const unsigned char pt_BOM_UCS2LE[] = {2, 0xFF,0xFE};

	static const CharTable xmlTagCharTab( "a..zA..Z0..9=_.-\"\' ?!");
	static const CharTable SpaceCharTab( " \t\r\n");

	std::string::const_iterator si = source.begin(), se = source.end();
	//[0] Handle special cases:
	if (si == se)
	{
		//... An empty file is assumed to be an info file
		return FileType( FileType::Undefined, FileType::Info);
	}

	FileType rt;
	char ch = 0;

	//[1] Try to recognize encoding by matching variants of BOM:
	if (FileTypeDetection::skipPattern( pt_BOM_UTF8, si, se)) rt.encoding = FileType::UCS1;
	else if (FileTypeDetection::skipPattern( pt_BOM_UCS4BE, si, se)) rt.encoding = FileType::UCS4BE;
	else if (FileTypeDetection::skipPattern( pt_BOM_UCS4LE, si, se)) rt.encoding = FileType::UCS4LE;
	else if (FileTypeDetection::skipPattern( pt_BOM_UCS2BE, si, se)) rt.encoding = FileType::UCS2BE;
	else if (FileTypeDetection::skipPattern( pt_BOM_UCS2LE, si, se)) rt.encoding = FileType::UCS2LE;

	//[2] Try to read first ASCII char and count the zeros to recognize encoding by the width of ASCII chars:
	if (rt.encoding == FileType::Undefined)
	{
		if (si != se && *si == 0)
		{
			std::size_t zc = FileTypeDetection::skipZeros( si, se);
			ch = *si++;
			switch (zc)
			{
				case 1: rt.encoding = FileType::UCS2BE; break;
				case 3: rt.encoding = FileType::UCS4BE; break;
				default: break;
			}
		}
		else
		{
			ch = *si++;
			std::size_t zc = FileTypeDetection::skipZeros( si, se);
			switch (zc)
			{
				case 0: rt.encoding = FileType::UCS1; break;
				case 1: rt.encoding = FileType::UCS2LE; break;
				case 3: rt.encoding = FileType::UCS4LE; break;
				default: break;
			}
		}
	}
	if (rt.encoding == FileType::Undefined)
	{
		//... encoding should be known now
		return rt;
	}
	//[3] Try to recognize the file format:
	try
	{
		if (ch == '\t' || ch == '\r' || ch == '\n' || ch == ' ')
		{
			ch = FileTypeDetection::skipChar( si, se, rt.encoding, SpaceCharTab);
		}
		if (ch == '<')
		{
			ch = FileTypeDetection::skipChar( si, se, rt.encoding, xmlTagCharTab);
			if (ch == '>')
			{
				return FileType( rt.encoding, FileType::XML);
			}
			//... recognition failed
			return rt;
		}
		if (rt.encoding == FileType::UCS1)
		{
			if (ch > 0 && ch <= 32)
			{
				ch = utils::gotoNextToken( si, se);
			}
			//... if encoding is UTF-8 check for info file format:
			while (ch == ';')
			{
				//... skip comments
				utils::parseLine( si, se);
				ch = utils::gotoNextToken( si, se);
			}
			std::string tok;
			ch = utils::parseNextToken( tok, si, se);
			if (ch && !tok.empty())
			{
				ch = utils::gotoNextToken( si, se);
				while (ch == ';')
				{
					//... skip comments
					++si;
					utils::parseLine( si, se);
					ch = utils::gotoNextToken( si, se);
				}
				if (ch == '{')
				{
					//... starts with an identifier or a string followed by a '{'
					rt.format = FileType::Info;
				}
				utils::parseNextToken( tok, si, se);
				if (!tok.empty())
				{
					//... starts with two subsequent identifiers or strings
					rt.format = FileType::Info;
				}
			}
		}
	}
	catch (std::bad_alloc& e)
	{
		throw e;
	}
	catch (std::runtime_error& e)
	{}
	return rt;
}


types::PropertyTree utils::readPropertyTreeFile( const std::string& filename)
{
	FileType filetype = getFileType( filename);
	types::PropertyTree rt;

	switch (filetype.format)
	{
		case FileType::XML:
		{
			if (filetype.encoding == FileType::Undefined)
			{
				throw std::runtime_error( std::string( "cannot handle encoding of file as info file '") + filename + "' (encoding is unknown)");
			}
			namespace opt = boost::property_tree::xml_parser;
			boost::property_tree::ptree xmlpt;
			read_xml( filename, xmlpt, opt::no_comments | opt::trim_whitespace);
			rt = types::PropertyTree( xmlpt, filename);
			break;
		}
		case FileType::Info:
		{
			if (filetype.encoding != FileType::UCS1)
			{
				throw std::runtime_error( std::string( "cannot handle encoding of file as info file '") + filename + "' (encoding is not UTF-8)");
			}
			boost::property_tree::ptree infpt;
			read_info( filename, infpt);
			rt = types::PropertyTree( infpt, filename);
			break;
		}
		case FileType::Unknown:
		{
			throw std::runtime_error( std::string( "type not recognized as 'Info' or 'XML' of file '") + filename + "'");
		}
	}
	return rt;
}

