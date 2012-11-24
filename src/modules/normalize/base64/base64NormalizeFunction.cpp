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
///\file base64NormalizeFunction.cpp
#include "base64NormalizeFunction.hpp"
#include "types/base64.hpp"
#include <boost/scoped_ptr.hpp>
#include <cstdlib>

using namespace _Wolframe;
using namespace langbind;

std::string Base64DecodeFunction::execute( const std::string& str) const
{
	base64::Decoder decoder;
	int bufsize = str.size();
	char* buf = new char[ str.size()];
	boost::scoped_ptr<char> bufptr( buf);
	int decodedSize = decoder.decode( str.c_str(), str.size(), buf, bufsize);
	return std::string( buf, decodedSize);
}

std::string Base64EncodeFunction::execute( const std::string& str) const
{
	std::string rt;
	base64::Encoder encoder;
	int bufsize = str.size() * 2;
	char* buf = new char[ bufsize];
	boost::scoped_ptr<char> bufptr( buf);
	int encodedSize = encoder.encodeChunk( str.c_str(), str.size(), buf, bufsize);
	rt.append( buf, encodedSize);
	encodedSize = encoder.encodeEndChunk( buf, bufsize);
	rt.append( buf, encodedSize);
	return rt;
}

