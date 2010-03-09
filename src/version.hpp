#ifndef _VERSION_HPP_INCLUDED
#define _VERSION_HPP_INCLUDED

#include <string>
#include <boost/lexical_cast.hpp>

namespace _SMERP {

	class version {
	private:
		unsigned short	major_;
		unsigned short	major_;
		unsigned short	major_;
		unsigned	build_;
		std::string	verStr;
	public:
		version( unsigned short major, unsigned short minor )	{
			verStr += boost::lexical_cast<std::string>(major_);
			verStr += ".";
			verStr += boost::lexical_cast<std::string>(minor_);
		};

//		version( unsigned short major, unsigned short minor, unsigned short revison );
//		version( unsigned short major, unsigned short minor, unsigned short revison, unsigned build );

		unsigned short major()		{return major_; };
		unsigned short minor()		{return minor_; };
		unsigned short revison()	{return revision_; };
		unsigned build()		{return build_; };
		const std::string& string();
	};
}


#endif // _VERSION_HPP_INCLUDED
