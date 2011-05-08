/************************************************************************
Copyright (C) 2011 Project Wolframe.
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
#include "implementation.hpp"
#include "logger.hpp"
#include "filters/char_isolatin1.hpp"
#include <boost/shared_ptr.hpp>
#include <new>

using namespace _Wolframe::mtproc;

//example mtprocHandler object implementation in C++ish form
struct Method::Data
{
	char buf;
	boost::shared_ptr<protocol::InputFilter> input;
	boost::shared_ptr<protocol::FormatOutput> output;

	Data() :buf(0)
	{
		input = boost::shared_ptr<protocol::InputFilter>( new filter::CharIsoLatin1::InputFilter());
		output = boost::shared_ptr<protocol::FormatOutput>( new filter::CharIsoLatin1::FormatOutput());
	}
};


Method::Data* Implementation::createData()
{
	return new (std::nothrow) Method::Data();
}

void Implementation::destroyData( Method::Data* data)
{
	delete data;
}

int Implementation::echo( Method::Context* ctx, unsigned int, const char**)
{
	LOG_DATA << "Method Call echo";

	if (!ctx->contentIterator)
	{
		ctx->contentIterator = ctx->data->input;
		ctx->output = ctx->data->output;
	}
	if (ctx->data->buf != 0)
	{
		if (!ctx->output->print( protocol::FormatOutput::Value, &ctx->data->buf, 1)) return 0;
		ctx->data->buf = 0;
	}
	protocol::InputFilter::ElementType type;
	protocol::InputFilter::size_type bp = 0;

	while (ctx->contentIterator->getNext( &type, &ctx->data->buf, 1, &bp))
	{
		if (!ctx->output->print( protocol::FormatOutput::Value, &ctx->data->buf, 1)) return 0;
		ctx->data->buf = 0;
		bp = 0;
	}
	return 0;
}

int Implementation::printarg( Method::Context* ctx, unsigned int, const char**)
{
	LOG_DATA << "Method Call printarg";

	if (!ctx->contentIterator)
	{
		ctx->contentIterator = ctx->data->input;
		ctx->output = ctx->data->output;
	}
	return 0;
}


