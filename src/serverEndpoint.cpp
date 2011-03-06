//
// serverEndpoint.cpp
//

#include <string>
#include "serverEndpoint.hpp"
#include "miscUtils.hpp"

#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>

namespace _Wolframe	{
	namespace Network	{

#ifdef WITH_SSL
/// set SSL files path to absolute pathes
void ServerSSLendpoint::setAbsolutePath( const std::string& refPath )
{
	if ( ! cert_.empty() )	{
		if ( ! boost::filesystem::path( cert_ ).is_absolute() )
			cert_ = resolvePath( boost::filesystem::absolute( cert_,
							boost::filesystem::path( refPath ).branch_path()).string());
		else
			cert_ = resolvePath( cert_ );
	}
	if ( ! key_.empty() )	{
		if ( ! boost::filesystem::path( key_ ).is_absolute() )
			key_ = resolvePath( boost::filesystem::absolute( key_,
							boost::filesystem::path( refPath ).branch_path()).string());
		else
			key_ = resolvePath( key_ );
	}
	if ( ! CAdir_.empty() )	{
		if ( ! boost::filesystem::path( CAdir_ ).is_absolute() )
			CAdir_ = resolvePath( boost::filesystem::absolute( CAdir_,
							boost::filesystem::path( refPath ).branch_path()).string());
		else
			CAdir_ = resolvePath( CAdir_ );
	}
	if ( ! CAchain_.empty() )	{
		if ( ! boost::filesystem::path( CAchain_ ).is_absolute() )
			CAchain_ = resolvePath( boost::filesystem::absolute( CAchain_,
							boost::filesystem::path( refPath ).branch_path()).string());
		else
			CAchain_ = resolvePath( CAchain_);
	}
}
#endif // WITH_SSL

	} // namespace Network
} // namespace _Wolframe

