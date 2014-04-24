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
///\file audit.cpp
///\brief Implementation of auditing to file functions for testing only

#include "audit.hpp"
#include "serialize/struct/structDescription.hpp"
#include "utils/fileUtils.hpp"
#include <string>
#include <vector>
#include <sstream>
#include <boost/thread/thread.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/lexical_cast.hpp>

using namespace _Wolframe;
using namespace _Wolframe::audit;

namespace {

struct MutationDescription :public serialize::StructDescription<Mutation>
{
	MutationDescription( )
	{
		( *this )
		( "operation", &Mutation::operation )
		( "id", &Mutation::id)
		( "oldvalue", &Mutation::oldvalue)
		( "newvalue", &Mutation::newvalue)
		;
	}
};

} // anonymous namespace


const serialize::StructDescriptionBase* Mutation::getStructDescription( )
{
	static MutationDescription rt;
	return &rt;
}

static std::string g_auditfilename = "audit.log";
static int g_auditcnt = 0;

int audit::Audit::mutation( proc::ExecContext* ctx, serialize::EmptyStruct& res, const Mutation& param)
{
	std::string filename = utils::getCanonicalPath( g_auditfilename, ctx->provider()->referencePath());

	static boost::mutex mutex;
	boost::interprocess::scoped_lock<boost::mutex> lock(mutex);

	FILE* fh;
	if (g_auditcnt++ == 0)
	{
		fh = fopen( filename.c_str(), "w+");
	}
	else
	{
		fh = fopen( filename.c_str(), "a+");
	}
	if (!fh)
	{
		throw std::runtime_error( std::string( "audit failed (errno " + boost::lexical_cast<std::string>(errno) + ") to open file ") + filename + "' for appending");
	}
	fprintf( fh, "audit %s id %d old '%s' new '%s'\n", param.operation.c_str(), param.id, param.oldvalue.c_str(), param.newvalue.c_str());
	fclose( fh);
	return 0;
}

int audit::Audit::mutation1( proc::ExecContext* ctx, serialize::EmptyStruct& res, const Mutation& param)
{
	std::string filename = utils::getCanonicalPath( g_auditfilename, ctx->provider()->referencePath());

	static boost::mutex mutex;
	boost::interprocess::scoped_lock<boost::mutex> lock(mutex);

	FILE* fh;
	g_auditcnt = 0;
	fh = fopen( filename.c_str(), "w+");
	if (!fh)
	{
		throw std::runtime_error( std::string( "audit failed (errno " + boost::lexical_cast<std::string>(errno) + ") to open file ") + filename + "' for appending");
	}
	fprintf( fh, "audit %s id %d old '%s' new '%s'\n", param.operation.c_str(), param.id, param.oldvalue.c_str(), param.newvalue.c_str());
	fclose( fh);
	return 0;
}

