//
// Application Instance singleton
//


#include "singleton.hpp"
#include "version.hpp"

namespace _SMERP	{

class AppInstance : public Singleton<AppInstance>
{
protected:

public:
	AppInstance( unsigned short major, unsigned short minor, unsigned short revision = 0, unsigned build = 0 );
	Version& version()	{ return version_; }

private:
	Version	version_;
};

} // namespace _SMERP

