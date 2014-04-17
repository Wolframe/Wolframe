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
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
///\file utils/fileLineInfo.hpp
///\brief Defines a data structure to hold positional info in a source file for error messsages etc.

#ifndef _WOLFRAME_FILE_LINE_INFO_HPP_INCLUDED
#define _WOLFRAME_FILE_LINE_INFO_HPP_INCLUDED
#include "utils/sourceLineInfo.hpp"
#include <string>
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace utils {

class FileLineInfo
	:public SourceLineInfo
{
public:
	typedef boost::shared_ptr<char> FileName;
	static FileName getFileName( const std::string& name);

	FileLineInfo(){}
	FileLineInfo( const std::string& filename_)
		:m_filename( getFileName( filename_)){}
	FileLineInfo( unsigned int line_, unsigned int column_)
		:SourceLineInfo(line_,column_){}
	FileLineInfo( const std::string& filename_, unsigned int line_, unsigned int column_)
		:SourceLineInfo(line_,column_),m_filename( getFileName( filename_)){}
	FileLineInfo( const FileLineInfo& o)
		:SourceLineInfo(o),m_filename(o.m_filename){}

	const char* filename() const
	{
		return m_filename.get();
	}
	void setFileName( const FileName& filename_)
	{
		m_filename = filename_;
	}
	std::string logtext() const;
	std::string logtext_short() const;

private:
	FileName m_filename;
};

}}
#endif
