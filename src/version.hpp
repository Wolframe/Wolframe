#ifndef _VERSION_HPP_INCLUDED
#define _VERSION_HPP_INCLUDED

#include <string>
#include <boost/lexical_cast.hpp>

namespace _SMERP {

	class version {
	private:
		unsigned short	major_;
		unsigned short	minor_;
		unsigned short	revision_;
		unsigned	build_;
		std::string	verStr;
	public:
		version( unsigned short M, unsigned short m )	{
			major_ = M, minor_ = m;
			verStr += boost::lexical_cast<std::string>( major_ );
			verStr += ".";
			verStr += boost::lexical_cast<std::string>( minor_ );
		};

		version( unsigned short M, unsigned short m, unsigned short r )	{
			major_ = M, minor_ = m, revision_ = r;
			verStr += boost::lexical_cast<std::string>( major_ );
			verStr += ".";
			verStr += boost::lexical_cast<std::string>( minor_ );
			verStr += ".";
			verStr += boost::lexical_cast<std::string>( revision_ );
		};

		version( unsigned short M, unsigned short m, unsigned short r, unsigned b )	{
			major_ = M, minor_ = m, revision_ = r, build_ = b;
			verStr += boost::lexical_cast<std::string>( major_ );
			verStr += ".";
			verStr += boost::lexical_cast<std::string>( minor_ );
			verStr += ".";
			verStr += boost::lexical_cast<std::string>( revision_ );
			verStr += ".";
			verStr += boost::lexical_cast<std::string>( build_ );
		};

		unsigned short major()		{ return major_; };
		unsigned short minor()		{ return minor_; };
		unsigned short revision()	{ return revision_; };
		unsigned build()		{ return build_; };
		const std::string& string()	{ return verStr; };
	};
}


#endif // _VERSION_HPP_INCLUDED
