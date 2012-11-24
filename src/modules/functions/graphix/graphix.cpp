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
///\file src/modules/functions/graphix/graphix.hpp
///\brief Implementation of graphix functions

#include "serialize/struct/filtermapDescription.hpp"
#include "graphix.hpp"
#include <string>
#include <vector>

using namespace _Wolframe;
using namespace graphix;

namespace {

struct ImageDescription : public serialize::StructDescription<Image>
{
	ImageDescription( )
	{
		( *this )
		( "data", &Image::data )
	}
};

struct ImageInfoDescription : public serialize::StructDescription<ImageInfo>
{
	ImageInfoDescription( )
	{
		( *this )
		( "width", &ImageInfo::width )
		( "height", &ImageInfo::height )
	}
};

} // anonymous namespace

const serialize::StructDescriptionBase *Image::getStructDescription( )
{
	static ImageDescription rt;
	return &rt;
}

const serialize::StructDescriptionBase *ImageInfo::getStructDescription( )
{
	static ImageInfoDescription rt;
	return &rt;
}

const serialize::StructDescriptionBase *ImageImpl::getStructDescription( )
{
	static ImageImplDescription rt;
	return &rt;
}

int ImageImpl::get( ImageInfo &res, const Image &param )
{
	(void)param.data;
	res.width = 77;
	res.height = 78;
	
	return 0;
}

int imageInfo( void* res, const void* param )
{
	return ImageImpl::get( *(ImageInfo *)res, *(const Image *)param );
}

