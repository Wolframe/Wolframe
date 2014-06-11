/*
---------------------------------------------------------------------
    The template library textwolf implements an input iterator on
    a set of XML path expressions without backward references on an
    STL conforming input iterator as source. It does no buffering
    or read ahead and is dedicated for stream processing of XML
    for a small set of XML queries.
    Stream processing in this context refers to processing the
    document without buffering anything but the current result token
    processed with its tag hierarchy information.

    Copyright (C) 2010 Patrick Frey

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

--------------------------------------------------------------------

	The latest version of textwolf can be found at 'http://github.com/patrickfrey/textwolf'
	For documentation see 'http://patrickfrey.github.com/textwolf'

--------------------------------------------------------------------
*/

#ifndef __TEXTWOLF_HPP__
#define __TEXTWOLF_HPP__

#include "textwolf/char.hpp"
#include "textwolf/exception.hpp"
#include "textwolf/staticbuffer.hpp"
#include "textwolf/charset_interface.hpp"
#include "textwolf/charset.hpp"
#include "textwolf/textscanner.hpp"
#include "textwolf/xmlscanner.hpp"
#include "textwolf/cstringiterator.hpp"
#include "textwolf/sourceiterator.hpp"
#include "textwolf/endofchunk.hpp"
#include "textwolf/xmltagstack.hpp"
#include "textwolf/xmlprinter.hpp"
#include "textwolf/xmlhdrparser.hpp"
#include "textwolf/xmlpathselect.hpp"

#endif


