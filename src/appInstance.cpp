//
// appInstance.cpp
//

#include "appInstance.hpp"

namespace _SMERP	{

	AppInstance::AppInstance( unsigned short major, unsigned short minor, unsigned short revision, unsigned build )
		: version_( major, minor, revision, build )
	{}

} // namespace _SMERP

