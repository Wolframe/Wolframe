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
///\file graphix.hpp
///\brief Interface for graphix functions
#ifndef _Wolframe_TESTS_WOLFILTER_MODULE_FUNCTIONS_FAKE_GRAPHIX_HPP_INCLUDED
#define _Wolframe_TESTS_WOLFILTER_MODULE_FUNCTIONS_FAKE_GRAPHIX_HPP_INCLUDED
#include "serialize/struct/structDescriptionBase.hpp"
#include "processor/procProviderInterface.hpp"
#include "processor/execContext.hpp"
#include <string>
#include <vector>

namespace _Wolframe {
namespace graphix {

struct Image
{
	std::string data;

	static const serialize::StructDescriptionBase *getStructDescription( );
};

struct ImageInfo
{
	int width;
	int height;

	static const serialize::StructDescriptionBase *getStructDescription( );
};

struct ImageThumb
{
	Image image;
	int size;
	
	static const serialize::StructDescriptionBase *getStructDescription( );
};

class ImageImpl
{
	private:
		static std::string decode( const std::string &data );
		static std::string encode( const std::string &data );
		
	public:
		static const serialize::StructDescriptionBase *getStructDescription( );
		static int info( const proc::ProcessorProviderInterface* provider, ImageInfo &res, const Image &param );
		static int thumb( const proc::ProcessorProviderInterface* provider, Image &res, const ImageThumb &param );
};

}}
#endif

