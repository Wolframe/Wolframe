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
///\file graphix.cpp
///\brief Fake implementation of graphix functions for testing

#include "graphix.hpp"
#include "serialize/struct/filtermapDescription.hpp"
#include <string>
#include <vector>
#include <sstream>

using namespace _Wolframe;
using namespace _Wolframe::graphix;

namespace {

struct ImageDescription : public serialize::StructDescription<Image>
{
	ImageDescription( )
	{
		( *this )
		( "data", &Image::data );
	}
};

struct ImageInfoDescription : public serialize::StructDescription<ImageInfo>
{
	ImageInfoDescription( )
	{
		( *this )
		( "width", &ImageInfo::width )
		( "height", &ImageInfo::height );
	}
};

struct ImageThumbDescription : public serialize::StructDescription<ImageThumb>
{
	ImageThumbDescription( )
	{
		( *this )
		( "image", &ImageThumb::image )
		( "size", &ImageThumb::size );
	}
};

struct ImageImplDescription : public serialize::StructDescription<ImageImpl>
{
	ImageImplDescription( )
	{
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

const serialize::StructDescriptionBase *ImageThumb::getStructDescription( )
{
	static ImageThumbDescription rt;
	return &rt;
}

const serialize::StructDescriptionBase *ImageImpl::getStructDescription( )
{
	static ImageImplDescription rt;
	return &rt;
}

int ImageImpl::info( ImageInfo &res, const Image &param )
{
	res.width = param.data.size();
	res.height = 1000 / param.data.size();
	return 0;
}

int ImageImpl::thumb( Image &res, const ImageThumb &param )
{
	std::size_t nn = (param.image.data.size() < (std::size_t)param.size)?param.image.data.size():param.size;
	res.data = std::string( param.image.data.c_str(), nn);
	return 0;
}

int _Wolframe::graphix::imageInfo( const proc::ProcessorProviderInterface*, void *res, const void *param )
{
	return ImageImpl::info( *(ImageInfo *)res, *(const Image *)param );
}

int _Wolframe::graphix::imageThumb( const proc::ProcessorProviderInterface*, void *res, const void *param )
{
	return ImageImpl::thumb( *(Image *)res, *(const ImageThumb *)param );
}
