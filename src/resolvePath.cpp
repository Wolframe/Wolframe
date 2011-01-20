//
// resolvePath.cpp
//

#include "miscUtils.hpp"

#include <string>

#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>


std::string resolvePath( const std::string& path )
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
